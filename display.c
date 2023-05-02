#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <pthread.h>
#include "display.h"
#include "lightdips.h"

#define I2CDRV_LINUX_BUS0 "/dev/i2c-0"
#define I2CDRV_LINUX_BUS1 "/dev/i2c-1"
#define I2CDRV_LINUX_BUS2 "/dev/i2c-2"

#define I2C_DEVICE_ADDRESS 0x20

#define REG_DIRA 0x00
#define REG_DIRB 0x01
#define REG_OUTA 0x14 //bottom
#define REG_OUTB 0x15

#define config1 "config-pin P9_17 i2c"
#define config2 "config-pin P9_18 i2c"

#define leftside  "/sys/class/gpio/gpio61/value"  //leftside is the side closest to the mircousb port
#define rightside "/sys/class/gpio/gpio44/value"

static int i2cFileDesc;
static int numtobewriten;
static pthread_t thread;
static pthread_t getvaluethread;
static bool run = true;

static void writetofile(char* filename, char* data){
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("ERROR OPENING %s.", filename);
        exit(1);
    }
    int charWritten = fprintf(file, data);
    if (charWritten <= 0) {
        printf("ERROR WRITING DATA");
        exit(1);
    }
    fclose(file);
}


static void sleepForMs(long long delayInMs)
{
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}




static void runCommand(char* command){
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");
    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
            break;
    // printf("--> %s", buffer); // Uncomment for debugging
        }
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0) {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}


 
static void writeI2cReg(int i2cFileDes, unsigned char regAddr, unsigned char value)
{
	unsigned char buff[2];
	buff[0] = regAddr;
	buff[1] = value;
	int res = write(i2cFileDesc, buff, 2);
	if (res != 2) {
		perror("Unable to write i2c register");
		exit(-1);
	}
}
//returns the correct bits to produce the number of the top half 
static int getTopValue(int num){
	int ret[10] = {0x86,0x2,0xE,0xE,0x8A,0x8C, 0x8C, 0x6, 0x8E, 0x8E};
	for(int i = 0; i < 9; i++){
		return ret[num];
	}
	return 0;
}
//returns the correct bits to produce the number of the bottom half 
static int GetBottomValue(int num){
	int ret[10] = {0xA1,0x80,0x31, 0xB0, 0x90, 0xB0, 0xB1, 0x80, 0xB1, 0xB0};
	for(int i = 0; i < 9; i++){
		return ret[num];
	}
	return 0;
}
//write number to the I2cReg 
static void wirteNumI2cReg(int num){
	int top;
	int bottom;
	if(num < 10){
		top = getTopValue(0);
		bottom = GetBottomValue(0);
		writetofile(leftside,"0");
		writetofile(rightside,"0");
		writeI2cReg(i2cFileDesc, REG_OUTA, bottom);
		writeI2cReg(i2cFileDesc, REG_OUTB, top);
		writetofile(leftside,"1");
		sleepForMs(5);
		top = getTopValue(num);
		bottom = GetBottomValue(num);
		writetofile(leftside,"0");
		writetofile(rightside,"0");
		writeI2cReg(i2cFileDesc, REG_OUTA, bottom);
		writeI2cReg(i2cFileDesc, REG_OUTB, top);
		writetofile(rightside,"1");
		sleepForMs(5);
	}
	else{
		int leftnum = num/10;
		top = getTopValue(leftnum);
		bottom = GetBottomValue(leftnum);
		writetofile(leftside,"0");
		writetofile(rightside,"0");
		writeI2cReg(i2cFileDesc, REG_OUTA, bottom);
		writeI2cReg(i2cFileDesc, REG_OUTB, top);
		writetofile(leftside,"1");
		sleepForMs(5);
		int rightnum = num%10;
		top = getTopValue(rightnum);
		bottom = GetBottomValue(rightnum);
		writetofile(leftside,"0");
		writetofile(rightside,"0");
		writeI2cReg(i2cFileDesc, REG_OUTA, bottom);
		writeI2cReg(i2cFileDesc, REG_OUTB, top);
		writetofile(rightside,"1");
		sleepForMs(5);
	}
}
//thread that controls display 
static void* writenum(){
	while(run){
		
		wirteNumI2cReg(numtobewriten);
	}
	return NULL;
}
//thread that changes the number
static void* changeNumI2cBus(){
	int num;
	while(run){
		num = getlightdips();
		if(num > 99){
			numtobewriten = 99;
		}
		else{
			numtobewriten = num;
		}
		sleepForMs(100);
	}
	return NULL;

}



//start of the public methods 
//intialize the I2cBus 
void initI2cBus()
{
	
	runCommand("echo out > /sys/class/gpio/gpio61/direction");
	runCommand("echo out > /sys/class/gpio/gpio44/direction");

	writetofile(leftside,"1");
	writetofile(rightside,"1");
	runCommand(config1);
	runCommand(config2);
	runCommand("i2cset -y 1 0x20 0x00 0x00");
	runCommand("i2cset -y 1 0x20 0x01 0x00");
	numtobewriten = 0;
	i2cFileDesc = open(I2CDRV_LINUX_BUS1, O_RDWR);

	if (i2cFileDesc < 0) {
		printf("I2C DRV: Unable to open bus for read/write (%s)\n", I2CDRV_LINUX_BUS1);
		perror("Error is:");
		exit(-1);
	}

	int result = ioctl(i2cFileDesc, I2C_SLAVE, I2C_DEVICE_ADDRESS);
	if (result < 0) {
		perror("Unable to set I2C device to slave address.");
		exit(-1);
	}

	writeI2cReg(i2cFileDesc, REG_DIRA, 0x00);
	writeI2cReg(i2cFileDesc, REG_DIRB, 0x00);
	pthread_create(&thread,NULL,writenum, NULL);
	pthread_create(&getvaluethread,NULL,changeNumI2cBus,NULL);
	return; 
}

void exitI2cBus(){
	run =false;
	pthread_join(thread, NULL);
	pthread_join(getvaluethread, NULL);
	writeI2cReg(i2cFileDesc, REG_OUTA, 0x00);
	writeI2cReg(i2cFileDesc, REG_OUTB, 0x00);
	writetofile(leftside,"0");
	writetofile(rightside,"0");

	close(i2cFileDesc);
}


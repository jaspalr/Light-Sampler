#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include "potDriver.h"
#include "sampler.h"

#define A2D_FILE_VOLTAGE0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095
static int potval = 0;
static pthread_t valchecker;
static pthread_t adjusterthread;
static bool run;


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

//thread that reads the pot on the zen cape
static void* getVoltage0Reading()
{
    
    while(run){
        // Open file
        FILE *f = fopen(A2D_FILE_VOLTAGE0, "r");
        if (!f) {
            printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
            printf(" Check /boot/uEnv.txt for correct options.\n");
            exit(-1);
        }
        // Get reading
        int a2dReading = 0;
        int itemsRead = fscanf(f, "%d", &a2dReading);
        if (itemsRead <= 0) {
                printf("ERROR: Unable to read values from voltage input file.\n");
                exit(-1);
        }
        // Close file   
        fclose(f);

        potval = a2dReading;
        sleepForMs(1000);
    }

    return NULL;
}
//adjusts the size of the buffer according to the potentiometer 
static void *adjuster(){
    int currentval = 0;
    int change = 0;
    while(run){
        change = potval;
        if(Sampler_getNumSamplesTaken() != 0 && currentval!=change){
            if(change == 0){
                change = 1;
            }
            currentval = change;
            Sampler_changeSize(change);
            
        }
    }
    return NULL;
}
//starts two threads one to read the potentiometer and one to adjust the buffer as needed 
void startpot(){
    run = true;
    pthread_create(&valchecker, NULL, getVoltage0Reading, NULL);
    pthread_create(&adjusterthread, NULL, adjuster, NULL);
}
//stops both threads 
void stoppot(){
    run = false;
    pthread_join(valchecker,NULL);
    pthread_join(adjusterthread,NULL);
}

int getpotval(){
    return potval;
}

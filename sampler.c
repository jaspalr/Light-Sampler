#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include "sampler.h"
#include "circlebuffer.h"
#include "periodTimer.h"
#define lightSensor "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"
#define alpha 0.999
static Circlebuffer* buffer;
static bool stop = false;
static long long totalsamples = 0;
static bool go = false;
static pthread_mutex_t lock;
static pthread_t starter;
static double prevavg = 0;




static int readFromFileToScreen(char *fileName){
    FILE *pFile = fopen(fileName, "r");
    if (pFile == NULL) {
        printf("ERROR: Unable to open file (%s) for read\n", fileName);
        exit(-1);
    }
    // Read string (line)
    const int MAX_LENGTH = 1024;
    char buff[MAX_LENGTH];
    fgets(buff, MAX_LENGTH, pFile);
    // Close
    fclose(pFile);
    int ret = atoi(buff);
    return ret;
  
    
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


//creates the buffer and adds samples to it 
void* Sampler_startSamplingThread(void){
    buffer = createBuffer(1000);
    totalsamples = 0;
    int val;
    go = true;
    double voltage = 0;
    while(!stop){
        Period_markEvent(PERIOD_EVENT_SAMPLE_LIGHT);
        val = readFromFileToScreen(lightSensor);
        voltage = ((double)val/4095) * 1.8;
        //printf("%f\n",voltage);
        if(prevavg == 0){
            prevavg = voltage;
        }
        else{
            prevavg = (1-alpha)*voltage + (alpha)*prevavg;
        }
        pthread_mutex_lock(&lock);
        addBuffer(voltage, buffer);
        totalsamples++;
        pthread_mutex_unlock(&lock);
        sleepForMs(1);
    }
    deleteBuffer(buffer);    
    return NULL;
}
//starts the sampling thread 
void Sampler_startSampling(void){
    pthread_mutex_init(&lock, NULL);
    pthread_create(&starter,NULL, (void*)Sampler_startSamplingThread, NULL);
}

bool Sampler_isReady(){
    return go;
}

void Sampler_stopSampling(void){
    stop = true;
    pthread_join(starter,NULL);
    pthread_mutex_destroy(&lock);
    buffer = NULL;
}

void Sampler_setHistorySize(int newSize){
    pthread_mutex_lock(&lock);
    changeBuffer(newSize,buffer);
    pthread_mutex_unlock(&lock);
}

int Sampler_getHistorySize(void){
    return getsize(buffer);
}

double* Sampler_getHistory(int *length){
    pthread_mutex_lock(&lock);
    *length = getlength(buffer);
    double* i = copyBuffer(buffer);
    pthread_mutex_unlock(&lock);
    return i;
}

int Sampler_getNumSamplesInHistory(){
    return getlength(buffer);
}


long long Sampler_getNumSamplesTaken(void){
    return totalsamples; 
}


void Sampler_changeSize(int val){
    pthread_mutex_lock(&lock);
    changeBuffer(val,buffer);
    pthread_mutex_unlock(&lock);
    return;
}

double getavg(){
    return prevavg;
}
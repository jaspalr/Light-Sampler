#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include "lightdips.h"
#include "terminaloutput.h"
#include "potDriver.h"
#include "sampler.h"
#include "periodTimer.h"

static pthread_t writerthread;
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

//thread for writing output to the terminal 
static void* writer(){
    int samplestaken = 0;
    while(run){
        int samplesinhistory = Sampler_getNumSamplesInHistory();
        int newsamplestaken = Sampler_getNumSamplesTaken();
        int newsamples;
        if(samplestaken == 0){
            newsamples = newsamplestaken;
        }
        else{
            newsamples = samplestaken - newsamplestaken;
            samplestaken = newsamples;
        }
        Period_statistics_t event; 
        Period_getStatisticsAndClear(0,&event);
       
        double avglightlvl = getavg();
        int lightdips = getlightdips();
        int potval = getpotval();
        int length;
        double* history= Sampler_getHistory(&length);

        if(samplesinhistory != 0){
            printf("Samples/s = %d  Pot Value = %d  History Size = %d   avg =  %0.3f   dips = %d   ", newsamples, potval,samplesinhistory, avglightlvl, lightdips);
            printf("Sampling[ %0.3f, %0.3f] avg %0.3f/%d\n",event.minPeriodInMs, event.maxPeriodInMs, event.avgPeriodInMs, event.numSamples);
            
            for(int i = 0; i< length; i++){
                if(i % 200 == 0){
                    printf("%.3f ",history[i]);
                }
            }
            printf("\n");
            free(history);
        }
        sleepForMs(1000);
    }
    return NULL;
}

//starts the thread to write to the terimal 
void startwriterthread(){
    run = true;
    pthread_create(&writerthread,NULL, &writer, NULL);


}
//ends that thread 
void endwriterthread(){
    run = false;
    pthread_join(writerthread,NULL);

}
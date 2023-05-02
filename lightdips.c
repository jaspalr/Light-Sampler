#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "sampler.h"
#include "lightdips.h"
#include <time.h>
#include <pthread.h>

static int counter;
static bool run;
static pthread_mutex_t lock;
static pthread_t counterthread;



static void* ComputeLightDips(){
    double diff =0;
    double val;
    int length;
    bool dip;
    double avg;
    while(run){
        pthread_mutex_lock(&lock);
        double* history = Sampler_getHistory(&length);
        diff =0;
        dip  = false;
        counter = 0;
        avg = getavg();
        for(int i = 0; i < length; i++){

            val = history[i];
         
                
            diff = val - avg;
            

            if(diff <= -0.1){
                if(!dip){
                    counter++;
                    dip = true;
                }
            }
            else if (val >= avg){
                dip = false;

            }                
        }

    
        pthread_mutex_unlock(&lock);
        free(history);
    }
    return NULL;

}

void startlightthread(){
    run = true;
    pthread_mutex_init(&lock, NULL);
    pthread_create(&counterthread,NULL,(void*)ComputeLightDips,NULL);
}

void stoplightthread(){
    run = false;
    pthread_join(counterthread,NULL);
    pthread_mutex_destroy(&lock);
}


int getlightdips(){
    pthread_mutex_lock(&lock);
    int ret = counter;
    pthread_mutex_unlock(&lock);
    return ret;
}

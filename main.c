#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>
#include "sampler.h"
#include "circlebuffer.h"
#include "potDriver.h"
#include "display.h"
#include "sampler.h"
#include "terminaloutput.h"
#include "lightdips.h"
#include "udpserver.h"
#include "periodTimer.h"

int main(){
    Period_init();
    Sampler_startSampling();
    while(!Sampler_isReady());
    startpot();
    startlightthread();
    startwriterthread();
    initI2cBus();
    server();
}
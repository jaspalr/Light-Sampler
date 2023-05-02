//shutdowns the light dips progarm singaled by udpserver

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "potDriver.h"
#include "periodTimer.h"
#include "display.h"
#include "terminaloutput.h"

void shutdownlightdips(){
    stoppot();
    exitI2cBus();
    endwriterthread();
    stoplightthread();
    Sampler_stopSampling();
    Period_cleanup();
}
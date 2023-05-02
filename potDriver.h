//reads the potentiometer and adjusts the biffer according 
#ifndef potDriver_h
#define potDriver_h
//starts two threads one to read the potentiometer and one to adjust the buffer as needed 
void startpot();
//stops both threads 
void stoppot();
//returns potentiometer value
int getpotval();

#endif
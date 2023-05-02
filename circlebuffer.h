//circlebuffer interface, allows size to be changed 
#ifndef circlebuffer_h
#define circlebuffer_h

typedef struct{
    double* buffer;
    int size;
    int index;
    int numobjects;
}Circlebuffer;
//creates a buffer size of n
Circlebuffer* createBuffer(int n);

int getsize(Circlebuffer* buf);

int getlength(Circlebuffer* buf);

int getindex(Circlebuffer* buf);
//returns copy of the buffer reciever must free
double* copyBuffer(Circlebuffer* buf);

void addBuffer(double val, Circlebuffer* buf);

void changeBuffer(int newsize,Circlebuffer* buf);

void deleteBuffer(Circlebuffer* buf);

#endif

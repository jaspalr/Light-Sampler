#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "circlebuffer.h"



//creates a buffer size of n
Circlebuffer* createBuffer(int n){
    Circlebuffer* ret = malloc(sizeof(*ret));
    if(!ret){
        exit(EXIT_FAILURE);
    }
    ret->buffer = malloc(sizeof(*ret->buffer)* n);
    if(!ret->buffer){
        exit(EXIT_FAILURE);
    }
    ret->index = -1;
    ret->size = n;
    ret->numobjects = 0;
    return ret;
}

int getsize(Circlebuffer* buf){
    return buf->size;
}

int getlength(Circlebuffer* buf){
    return buf->numobjects;
}

int getindex(Circlebuffer* buf){
    return buf->index;
}

//returns copy of the buffer reciever must free
double* copyBuffer(Circlebuffer* buf){
    double* newbuf = malloc(sizeof(*newbuf) * buf->numobjects);
    int index = buf->index;
    for(int i = buf->numobjects - 1; i > -1; i--){
        newbuf[i] = buf->buffer[index];
        index--;
        if(index < 0){
            index = buf->size - 1;
        }
    }


    return newbuf;

}

void addBuffer(double val, Circlebuffer* buf){
    if(((buf->index + 1) == (buf->size))){
        buf->index = -1;
    }
    buf->index++;
    buf->buffer[buf->index] = val;
    buf->numobjects++;
    if(buf->numobjects > buf->size){
        buf->numobjects = buf->size;
    }
}


void changeBuffer(int newsize,Circlebuffer* buf){
    double* newbuf = malloc(sizeof(*newbuf)*newsize);
    if(!newbuf){
        exit(EXIT_FAILURE);
    }
    if(newsize > buf->numobjects){//larger buffer 
        for(int i = buf->numobjects - 1; i > -1; i--){
            newbuf[i] = buf->buffer[buf->index];
            buf->index--;
            if(buf->index == -1){
                buf->index = buf->size - 1;
            }   
        }
        buf->index = buf->numobjects - 1;
    }
    else{//smaller buffer 
        for(int i = newsize-1; i > -1; i--){
            newbuf[i] = buf->buffer[buf->index];
            buf->index--;
            if(buf->index == -1){
                buf->index = buf->size - 1;
            }
        }
        buf->index = newsize-1;
        buf->numobjects = newsize;
    }
    buf->size = newsize;
    free(buf->buffer);
    buf->buffer = newbuf;
    newbuf = NULL;
 }


void deleteBuffer(Circlebuffer* buf){
    free(buf->buffer);
    buf->buffer = NULL;
    free(buf);
    buf = NULL;
}
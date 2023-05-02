#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include "udpserver.h"
#include "sampler.h"
#include "lightdips.h"
#include "shutdown.h"
#include <pthread.h>
#define MAX_LEN 1500
#define PORT 12345
static unsigned int sin_len;
static struct sockaddr_in sinRemote;
static int socketDescriptor;
static pthread_t udpserver;


static bool command(char* messageRx){
    char messageTx[MAX_LEN];
    bool ret = true;

    if(strcmp("help\n",messageRx) == 0){ //case help
        snprintf(messageTx, MAX_LEN, "Accepted command examples:\ncount -- display total number of samples taken.\nlength -- display number of samples in history (both max, and current).\nhistory -- display the full sample history being saved.\nget 10 -- display the 10 most recent history values.\ndips -- display number of .\nstop -- cause the server program to end\n<enter> -- repeat last command.\n");
    }
    else if(strcmp("count\n",messageRx) == 0){ //case count 
        snprintf(messageTx, MAX_LEN, "Number of samples taken = %lld\n",Sampler_getNumSamplesTaken());
    }
    else if(strcmp("history\n",messageRx) == 0){ //case history 
        int length;
        double* history = Sampler_getHistory(&length);
        char str[8] = "\0";
        messageTx[0] = 0;
        for(int ind = 0; ind < length; ind++){        
            snprintf(str,8,"%.3f, ",history[ind]);
            strncat(messageTx,str,MAX_LEN);
            str[0] = 0;
            if(ind % 213 == 0 && ind != 0){
                sin_len = sizeof(sinRemote);
                sendto( socketDescriptor,
                messageTx, strlen(messageTx),
                0,
                (struct sockaddr *) &sinRemote, sin_len);
                messageTx[0] = '\0';
            }
        }
        strcat(messageTx, "\n");
        free(history);
    }
    else if(strcmp("dips\n",messageRx) == 0){ //case dips 
        snprintf(messageTx, MAX_LEN, "# Dips = %d.\n",getlightdips());
    }
    else if(strcmp("stop\n",messageRx) == 0){ //case stop
        snprintf(messageTx, MAX_LEN, "Program terminating.\n");
        shutdownlightdips();
        ret = false;
    }
    else if(strcmp("length\n",messageRx) == 0){ //case length
        snprintf(messageTx, MAX_LEN, "History can hold  %d.\nCurrently holding %d.\n",Sampler_getHistorySize(), Sampler_getNumSamplesInHistory());
           
    }
    else if(messageRx[0] == 'g' && messageRx[1] == 'e' &&  messageRx[2] == 't'){ //get N
        char size[1500];
        int i = 3;
        int x = 0;
        while(messageRx[i] != '\n'){
            size[x] = messageRx[i];
            x++;
            i++;
        }
        size[x] = 0;
        int n = atoi(size);
        int length;
        double* history = Sampler_getHistory(&length);
        if(length < n){
            snprintf(messageTx, MAX_LEN, "Error only %d in history\n", length);
        }
        else{
            char str[8] = "\0";
            messageTx[0] = 0;
            for(int ind = 0; ind < n; ind++){
                
                snprintf(str,8,"%.3f, ",history[ind]);
                strncat(messageTx,str,MAX_LEN);
                str[0] = 0;
              
                if(ind % 213 == 0 && ind != 0){
                    sin_len = sizeof(sinRemote);
                    sendto( socketDescriptor,
                    messageTx, strlen(messageTx),
                    0,
                    (struct sockaddr *) &sinRemote, sin_len);
                    messageTx[0] = '\0';
                }
            }
            free(history);
            strcat(messageTx,"\n");
        }
        }
        else{ //default
            snprintf(messageTx, MAX_LEN, "Help me now!\n");
        }
        
        sin_len = sizeof(sinRemote);
        sendto( socketDescriptor,
        messageTx, strlen(messageTx),
        0,
        (struct sockaddr *) &sinRemote, sin_len);
        
        return ret;
}


static void* serverthread(){
    struct sockaddr_in sin; 
    memset(&sin, 0, sizeof(sin)); 
    sin.sin_family = AF_INET; 
    sin.sin_addr.s_addr = htonl(INADDR_ANY); 
    sin.sin_port = htons(PORT);
    socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0); 
    bind (socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));
    char prev[1024];
    bool run = true;
    sin_len = sizeof(sinRemote);
    char messageRx[MAX_LEN];
    while(run){
        int bytesRx = recvfrom(socketDescriptor,
        messageRx, MAX_LEN - 1, 0,
        (struct sockaddr *) &sinRemote, &sin_len);
        messageRx[bytesRx] = 0;
        if(messageRx[0] =='\n'){
           run = command(prev);
        }
        else{
            run = command(messageRx);
            strcpy(prev,messageRx);
        }
        
    }
    close(socketDescriptor);
    return NULL;
}


void server(){
    pthread_create(&udpserver,NULL,serverthread,NULL);
    pthread_join(udpserver,NULL);
}
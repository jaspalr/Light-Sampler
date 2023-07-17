# Light-Sampler
Uses a photoresitstor to measure the number of light dips and displays them using a 14-segment display and a UDP client. Also 
uses a potentiometer to determine the size of the buffer. 

## To Run:
&nbsp; &nbsp; &nbsp; Type "Make" into the host's terminal  \
&nbsp; &nbsp; &nbsp; On the BeagleBone type “cd /mnt/remote/myApps/” \
&nbsp; &nbsp; &nbsp; Then "./light_sampler"; 
 
## To Use UDP Client:
&nbsp; &nbsp; &nbsp; Type "netcat -u 192.168.7.2 12345" on the host's terminal \
&nbsp; &nbsp; &nbsp; Supports: \
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; "help" - shows accpeted commands \
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; "count - shows the number of samples taken \
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; "length" - shows size of the buffer and number of samples in the buffer \
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; "history" - shows the samples in the buffer \
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; get \<x> - shows the last x samples \
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; "dips" - shows the number of dips in the buffer \
&nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; "stop" - stops the program 

## Video Demo


https://user-images.githubusercontent.com/105681721/235582658-187918c7-6819-4295-9b88-926ea4928fa4.MOV







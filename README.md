# Project 2 - many-to-many Chatroom
## How to run my program?
### compile
* use makefile
```
make
```
![image](https://github.com/YeeHsun/chatroom/blob/main/image/makefile.PNG?raw=true)
### execution
* bulid a server
```
./server
```
![image](https://github.com/YeeHsun/chatroom/blob/main/image/server.PNG?raw=true)
* build a client
```
./client [username] 127.0.0.x
```
![image](https://github.com/YeeHsun/chatroom/blob/main/image/client.PNG?raw=true)
### result
(1) Kevin and Amy connect to the server

![image](https://github.com/YeeHsun/chatroom/blob/main/image/jump%20in%20server2.PNG?raw=true)

(2) Kevin says "hello~Amy"

![image](https://github.com/YeeHsun/chatroom/blob/main/image/Kevin%20said.PNG?raw=true)

(3) Amy will receive the message

![image](https://github.com/YeeHsun/chatroom/blob/main/image/Amy%20recv%20message.PNG?raw=true)

(4) Amy responses "hi~Kevin!"

![image](https://github.com/YeeHsun/chatroom/blob/main/image/Amy%20response.PNG?raw=true)

(5) Kevin will receive the respond

![image](https://github.com/YeeHsun/chatroom/blob/main/image/Kevin%20recv%20message.PNG?raw=true)

(6) server receives all the messages

![image](https://github.com/YeeHsun/chatroom/blob/main/image/server%20chatroom2.PNG?raw=true)

 * this chatroom accommodates max to 10 clients
 
## Briefly describe the workflow and architecture of the system
* architecture

![image](https://github.com/YeeHsun/chatroom/blob/main/image/block%20diagram.PNG?raw=true)

* workflow

![image](https://github.com/YeeHsun/chatroom/blob/main/image/workflow.PNG?raw=true)
## Explain the solution of synchronization problems in this project
### synchronization problems :
message in shared memory might be changed by new message while not all the clients receive the old message
```c
sem_wait(&mutex); //semaphore wait
while(change); // wait until all the threads receive the message, then we can change the content in shared memory
"critical section"
change=1;
sem_post(&mutex); //semaphore signal
```
* mutual exclusive : only one thread (client) can change the message content in shared memory at one time
* progress : thread can access to shared momory if all the clients receive the old message (change=0)
* bounded waiting
## Discuss anything as possible
(1) user interface
(2) have read notice
(3) record chatting history

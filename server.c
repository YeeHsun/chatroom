#include <stdio.h> 
#include <netdb.h> 
#include <netinet/in.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h>
#include <arpa/inet.h>
#define MAX 100
#define max_users 10

sem_t mutex; //semophore
char ShareMemory[MAX]; // Share memory
int num_client=0; // total number of clients
int access_SM_time=0; // number of times that the share memory has been accessed
int change = 0; // set to 1 when the content in share memory is "not" able to be changed (new message)
int sent_clientfd; //the clientfd who sent the message
int exit_clientfd=-1; //the clientfd who left the chatroom

void* send_message(void* arg) 
{
    int socketfd = *(int*)arg;
	int recv_flag=0; // set to one when this thread has already received the new message in share memory
    int i;
	for (;;) { 
		if(exit_clientfd == socketfd) //fail to create a socket
        {
			exit_clientfd=-1;
			break; 
		}	
		if(change==0 && recv_flag==1)	//this thread has received the message in SM and no other new message
            recv_flag=0;	
		else if(change==1 && recv_flag==0) //this thread hasn't received the message in SM
        {   
            if(sent_clientfd!=socketfd)//the client that sent the message will not receive the message it sent 
            {  
                send(socketfd, ShareMemory, sizeof(ShareMemory), 0); 
                access_SM_time++;
                recv_flag=1;	
                if(access_SM_time == num_client-1)  //when the last thread receive the message in shared memory, then clean it up!
                {   
                    bzero(ShareMemory, MAX); // reset Share memory
                    access_SM_time=0;
                    change=0;
                }
            }  
		}
		else if(sent_clientfd==socketfd && num_client==1 && change==1)
        { 
		    //reset when there is only one client
			bzero(ShareMemory, MAX);
			access_SM_time=0;
			change=0;		
		}
	}
   
}

void* recv_message(void* arg) 
{
    int socketfd = *(int*)arg;
	char buff[MAX];
	char name[MAX];
	bzero(name,MAX);
    int receive_name=1; 

	for(;;)
    {		
		bzero(buff, MAX);
		recv(socketfd, buff, sizeof(buff), 0); 
		if(buff[0]=='\0')
        {   //even no user name
			exit_clientfd= socketfd; // record exited clientfd
			printf("\n%s離開了聊天室QwQ\n",name);			
			break;
		}
		if(receive_name)
        {   //fisrt message is the name of client
            
			strcpy(name,buff);	
			printf("\n%s 跳進了伺服器! \n",name); 
			receive_name=0;
            strcpy(buff, "歡迎");
			strcat(buff, name);
			strcat(buff, "進入聊天室!!\n");		
		}
		else
			printf("\n%s",buff);

        //synchronization
		sem_wait(&mutex); //semaphore wait
		while(change); // wait until all the threads receive the message, then we can change the content in SM
		sent_clientfd= socketfd; // remember the client that sent the message
		strcpy(ShareMemory, buff); // Put the received message into SM (change the content in SM)
		change=1;  
		sem_post(&mutex); //semaphore signal
	}

	num_client--;	

}
int main(int argc , char *argv[])
{

	int socketfd = 0;
	//建立socket : int socket(int domain, int type, int protocol);
	//domain = AF_INET : 讓兩台主機透過網路進行資料傳輸
	//type = SOCK_STREAM : use TCP protocal
	//protocal = 0 : 讓OS選擇相對應的protocal
	socketfd = socket(AF_INET , SOCK_STREAM , 0);
	
	if(socketfd == -1)
    {
		printf("fail to create a socket!\n");
        exit(0);
    }
	else
		printf("create a socket successfully!\n");
	
	//socket連線
	struct sockaddr_in serverInfo,clientInfo;
	//初始化，將struct涵蓋的bits設為0
	bzero(&serverInfo,sizeof(serverInfo));
	serverInfo.sin_family = AF_INET;
	//IP位置
	serverInfo.sin_addr.s_addr = htonl(INADDR_ANY); 
	//htons : 將本機端的endian轉換成了網路端的endian
	serverInfo.sin_port = htons(8787);
	
	//int bind(int sockfd, struct sockaddr* addr, int addrlen);
    int bind_err = 0;
	bind_err = bind(socketfd,(struct sockaddr *)&serverInfo,sizeof(serverInfo));
	if(bind_err == -1)
    {
        printf("fail to bind to a socket!\n");
        exit(0);
    }
    else
        printf("bind to a socket successfully!\n");
    
	//int listen(int sockfd, int backlog);
	//backlog : maximum client number
    int listen_err = 0;
	listen_err = listen(socketfd,max_users);
    if(listen_err == -1)
    {
        printf("listen failed!\n");
        exit(0);
    }
    else
        printf("listening to port 8787......\n");
	pthread_t thread_send,thread_recv;
    int *forClientfd;
    int client_len = sizeof(clientInfo);
    int clientfd_array[max_users];

    //int sem_init(sem_t *sem, int pshared, unsigned int value);
    //If pshared has the value 0, then the semaphore is shared between the threads of a process
    sem_init(&mutex, 0, 1); 
	while(1)
	{
        //access address
        forClientfd = &clientfd_array[num_client];
        *forClientfd = accept(socketfd,(struct sockaddr*)&clientInfo,&client_len);
        if(*forClientfd == -1)
        {
            printf("fail to accept\n");
            exit(0);
        }
        else //accept successfully
        {
            //allow to enter server
            num_client++;
            pthread_create(&thread_send, NULL, send_message, (void*) forClientfd);
            pthread_create(&thread_recv, NULL, recv_message, (void*) forClientfd);
          
        }
	}
    bzero(ShareMemory, MAX);
	sem_destroy(&mutex);
    pthread_join(thread_send,NULL);
    pthread_join(thread_recv,NULL);
    close(socketfd);
	return 0;
}
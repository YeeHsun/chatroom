#include"lib.h"

//global variable
char user_name[MAX];

void*send_message(void*arg)
{
    int socketfd = *(int*)arg;
    int send_name_to_server = 1;
    char messagebuffer[MAX];
    int message_length = 0;
    for(;;)
    {
        strcpy(messagebuffer,user_name);
        //send user name to server first
        if(send_name_to_server)
        { 
            send(socketfd,messagebuffer,sizeof(messagebuffer),0);
            send_name_to_server = 0;
        }
        //then the message
        else
        {
            strcat(messagebuffer,"說 : ");
            message_length = strlen(messagebuffer);
            //input until press ENTER
            while((messagebuffer[message_length++] = getchar())!='\n');
            send(socketfd,messagebuffer,sizeof(messagebuffer),0);
            bzero(messagebuffer,sizeof(messagebuffer));
        }
    }
    return NULL;
}
void*recv_message(void*arg)
{
    int socketfd = *(int*)arg;
    char messagebuffer[MAX]; 
    for(;;)
    {
        recv(socketfd,messagebuffer,sizeof(messagebuffer),0);
        printf("\n%s",messagebuffer);
        bzero(messagebuffer,sizeof(messagebuffer));
    }

    return NULL;
}
int main(int argc , char *argv[])
{
    //get user name
    bzero(user_name,sizeof(user_name));
    //first input is user name
    strcpy(user_name,argv[1]);


	//讓Socket能把接收到的資料塞進inputBuffer裡頭
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
	struct sockaddr_in clientInfo;
	//初始化，將struct涵蓋的bits設為0
	bzero(&clientInfo,sizeof(clientInfo));
	clientInfo.sin_family = AF_INET;
	//IP位置 , second input
	inet_pton(AF_INET,argv[2],&clientInfo.sin_addr);
	//htons : 將本機端的endian轉換成了網路端的endian
	clientInfo.sin_port = htons(8787);
	
	//int connect(int sd, struct sockaddr *server, int addr_len);
	int connect_error = connect(socketfd,(struct sockaddr *)&clientInfo,sizeof(clientInfo));
	if(connect_error == -1)
    {
		printf("fail to connect!\n");
        exit(0);
    }
	else
		printf("connect successfully!\n");
	

    //thread programming
	pthread_t thread_send, thread_recv;

	pthread_create(&thread_send, NULL, send_message, (void*) &socketfd);
	pthread_create(&thread_recv, NULL, recv_message, (void*) &socketfd);

	pthread_join(thread_send, NULL);
	pthread_join(thread_recv, NULL);

	// close the socket 
	close(socketfd); 
	return 0;
}
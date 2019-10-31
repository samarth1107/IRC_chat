//Samarth Chauhan 
//2018410 CSB

//header file
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

//defing max limit of length of message and name
#define MAX_CLIENT 20
#define MAX__MESSAGE_LIMIT 500

//defing global variable for thread client list and client number
pthread_mutex_t thread_mutex;
int clients[MAX_CLIENT],client_number=0;
char Message[MAX__MESSAGE_LIMIT];


//this function will be used in the thread to receive message from user and then send the message to all user 
void *receive_message(void *client_sock)
{
    int len,C_sock=*((int *)client_sock);
    
    //to enter in the loop when len of the received message is more than 0
    while((len=recv(C_sock,Message,MAX__MESSAGE_LIMIT,0))>0) 
    {
        //defining end of the message
        Message[len] = '\0';
        //send to all user
        send_to_all_user(Message,C_sock);
    }    
}


//this is helper function of receive_message which will send message to all user
void send_to_all_user(char *Message,int current)
{
    int i;
    pthread_mutex_lock(&thread_mutex);
    for(i = 0; i < client_number; i++) 
    {
        if(clients[i] != current) 
        {            
            if(send(clients[i],Message,strlen(Message),0) < 0) 
            {
                perror("[-] Error in sending message\nPlease try again\n");
                continue;
            }
        }
    }
    pthread_mutex_unlock(&thread_mutex);
}


//main function
int main()
{
    //variable defination
    struct sockaddr_in Server_IP;
    pthread_t message_thread;
    int Socket=0,Client_sock=0;

    //defining port and IP address for communication
    Server_IP.sin_family = AF_INET;
    Server_IP.sin_port = htons(1234);
    Server_IP.sin_addr.s_addr = inet_addr("127.0.0.1");

    //socket creation
    Socket=socket( AF_INET , SOCK_STREAM, 0 );
    // this willl bind socket and give error if their is any error in listening or binding
    if((bind(Socket,(struct sockaddr *)&Server_IP,sizeof(Server_IP))==-1) || (listen(Socket,20)==-1))
    {
        perror("\n[-] Server cannot be started due to binding error!! \n");
        return 0;
        exit(1);
    } 
    else printf("\n[+] Server Started\n");

    //this will show info to user
    printf("To open client side program follow these steps\n1. Open another terminal.\n2. Type ./client -pthread\n3. Repeat step 2 to add more user");
    
    //this is infinite loop which will send chat to all user 
    while(1)
    {
        //if there is error in the accepting message then closing the server
        if((Client_sock=accept(Socket,(struct sockaddr *)NULL,NULL))<0)
        {
            perror("[-] Message accept failiure \n[-] Closing program \n");
            break;
        }

        pthread_mutex_lock(&thread_mutex);
        
        clients[client_number]= Client_sock;
        client_number++;
        
        // creating a thread for each client 
        pthread_create(&message_thread,NULL,(void *)receive_message,&Client_sock);
        pthread_mutex_unlock(&thread_mutex);
    }
    return 0; 
    
}
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


//defing max limit of lenth of message and name
#define MAX__NAME_LIMIT 100
#define MAX__MESSAGE_LIMIT 500

//defing global variable for message and name
char user_message[MAX__MESSAGE_LIMIT],receive_message[MAX__MESSAGE_LIMIT],send_message[MAX__MESSAGE_LIMIT];
char Name_const[MAX__NAME_LIMIT],client_name[MAX__NAME_LIMIT];

void *receive_chat(void *User_Socket)
{
    int Socket = *((int *)User_Socket),length;

    // client thread always ready to receive message
    while((length=recv(Socket,receive_message,MAX__MESSAGE_LIMIT,0))>0) 
    {
        receive_message[length]='\0';

        // to ckeck if the receive_message was indented for the user or not
        if (strstr(receive_message,":@"))
        {
            if (strstr(receive_message,Name_const)) 
            {
                char *substring,*chat;
                chat=strdup(receive_message);

                // this loop will delete :@name substring and print the receive_message
                while ((substring=strsep(&chat," "))!=NULL)
                {
                    if(strstr(substring,":@")==NULL){printf("%s ",substring);}                    
                }
                printf("\n");
            }         
        }

        //if message was intended for everyone
        else{fputs(receive_message,stdout);}
    }
}

int main(int argc,char *argv[])
{    
    int Socket;    
    struct sockaddr_in Server_IP;
    pthread_t thread;

    //for timing 
    time_t rawtime;
    struct tm *timeinfo;   
    
    
    //To take name of the user or client which will be used as clientID
    printf("Please enter your name :- ");
    fgets(client_name,MAX__NAME_LIMIT,stdin);
    //client_name[strlen(client_name)-1]=" ";
    size_t ln = strlen(client_name) - 1;
    if (*client_name && client_name[ln] == '\n')client_name[ln] = '\0';



    //constant initilisation
    strcat(Name_const,":@");
    strcat(Name_const,client_name);
    strcat(Name_const,"\0");


    //socker creation 
    Socket=socket(AF_INET,SOCK_STREAM,0);
    if (Socket < 0)
    {
        perror("[-] Error in socket creation\n");
        return -1;
    }    
    else{printf("[+] Socket created\n");}

    //defining ports and IP address
    Server_IP.sin_port = htons(1234);
    Server_IP.sin_family= AF_INET;
    Server_IP.sin_addr.s_addr = inet_addr("127.0.0.1");

    // connection to socket which is connecting server
    if((connect(Socket,(struct sockaddr *)&Server_IP,sizeof(Server_IP)))==-1)
    {
        perror("\n[-] Connection Failed \n"); 
        return -1;
    }
    else{printf("[+] Connection successfull\n\n");}


    //creating a client thread which is always waiting for a message
    pthread_create(&thread,NULL,(void *)receive_chat,&Socket);
    
    //ready to read a message from console
    while(fgets(user_message,MAX__MESSAGE_LIMIT,stdin)>0) 
    {
        //updating time from local or computer time
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        if(strcmp(user_message,"\n")==0)continue;

        // making user_message which will be sent through socket
        strcpy(send_message," - ");
        strcat(send_message,client_name);
        strcat(send_message,"       | ");
        strcat(send_message,asctime(timeinfo));
        strcat(send_message,"------------------------------  ");
        strcat(send_message,user_message); 

        //if user want to exit
        if (strstr(user_message,":exit"))
        {
            printf("[+] Closing program \n");
            exit(1);
            break;
        }

        //if there is error in send message to server
        if(write(Socket,send_message,strlen(send_message)) < 0){perror("[-] Error in sending message\n");}  
    }
    
    //thread is closed
    pthread_join(thread,NULL);
    close(Socket);
    return 0;
}
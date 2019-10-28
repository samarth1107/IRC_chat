#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

char msg[500];
char Name_const[100];

void slice_str(const char * str, char * buffer, size_t start, size_t end)
{
    size_t j = 0;
    for ( size_t i = start; i <= end; ++i ) {
        buffer[j++] = str[i];
    }
    buffer[j] = 0;
}

void *recvmg(void *my_sock)
{
    int sock = *((int *)my_sock);
    int len;

    // client thread always ready to receive message
    while((len = recv(sock,msg,500,0))>0) 
    {
        msg[len] = '\0';

        // to ckeck if the message was indented for the user or not
        if (strstr(msg,":@"))
        {
            if (strstr(msg,Name_const)) 
            {
                char buffer[501];
                printf("this is the real message %s\n",msg);
                slice_str(msg,buffer,0,strlen(msg)-strlen(Name_const)-1);
                fputs(buffer,stdout);
            }         
        }

        //if message was intended for everyone
        else fputs(msg,stdout);        
        
    }
}

int main(int argc,char *argv[])
{
    pthread_t recvt;
    int len;
    int sock;
    char send_msg[500];
    struct sockaddr_in ServerIp;
    
    // to copy name which is taken while command which is used for running out file
    char client_name[100];
    strcpy(client_name, argv[1]);

    //constant initilisation
    strcat(Name_const,":@");
    strcat(Name_const,client_name);
    strcat(Name_const,"\0");

    //socker creation 
    sock = socket( AF_INET, SOCK_STREAM,0);
    if (sock < 0)
    {
        printf("[-] Error in socket creation\n");
        return -1;
    }    
    else 
    {
        printf("[+] Socket created\n");
    }


    ServerIp.sin_port = htons(1234);
    ServerIp.sin_family= AF_INET;
    ServerIp.sin_addr.s_addr = inet_addr("127.0.0.1");

    // connection to socket which is connecting server
    if((connect( sock ,(struct sockaddr *)&ServerIp,sizeof(ServerIp)))==-1)
    {
        printf("\n[-] Connection Failed \n"); 
        return -1;
    }
    else 
    {
        printf("[+] Connection successfull\n\n");
    }


    //creating a client thread which is always waiting for a message
    pthread_create(&recvt,NULL,(void *)recvmg,&sock);
    
    //ready to read a message from console
    while(fgets(msg,500,stdin) > 0) 
    {
        // making message which will be sent through socket
        strcpy(send_msg,client_name);
        strcat(send_msg," :- ");
        strcat(send_msg,msg); 

        //if user want to exit
        if (strstr(msg,"exit:-"))
        {
            printf("[+] Closing the program \n");
            exit(1);
            break;
        }

        if (strstr(msg,":@"))
        {
            len = write(sock,send_msg,strlen(send_msg));
            if(len < 0)
            { 
                printf("[-] Error in sending message\n");
            }
        }

        else
        {
            len = write(sock,send_msg,strlen(send_msg));
            if(len < 0)
            { 
                printf("[-] Error in sending message\n");
            }
        }
                
    }
    
    //thread is closed
    pthread_join(recvt,NULL);
    close(sock);
    return 0;
}
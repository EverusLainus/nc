//i'm a client.
//i send and receive messages from from the server


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define MAX 500
static int keepRunning = 1;

void intHandler(int a){
    keepRunning = 0;
}

int client(char *s, char *ip, char *port){
    struct sigaction act;
    act.sa_handler = intHandler;
    sigaction(SIGINT, &act, NULL);

    //addrinfo
    int rv;
    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    rv = getaddrinfo(ip, port, &hints, &servinfo);
    if(rv == -1){
        perror("addrinfo \n");
        return 0;
    }
//socket and connect
    int sock_res;
    int connect_res;
    int got_connected=0;

    for(p=servinfo; p!=NULL; p=p->ai_next){
       //printf("into socket \n"); 
        sock_res = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        //printf("sock_res is %d \n", sock_res);
        if(sock_res == -1){
            perror("socket");
            continue;
        }
        connect_res = connect(sock_res, p->ai_addr, p->ai_addrlen);
        if(connect_res == -1){
            perror("connect");
            continue;
        }
        //i never reach so bar for any p
        got_connected=1;
        break;
    }

//send and receive continuously   
    int recv_res;
    int send_res;
    char sbuff[MAX];
 
    while(got_connected && keepRunning){
    //send
        
        memset(&sbuff, 0, MAX);

        //read message from console
        if(fgets(sbuff,sizeof sbuff, stdin) == NULL){
            printf("true");
            continue;
        }

        int length = strlen(sbuff)-1;
        //add extra new lines for web server
        sbuff[length+1]='\r';
        sbuff[length+2]='\n';
        sbuff[length+3]='\r';
        sbuff[length+4]='\n';

        //printing what i'm gonna send
        for(int i=0; i< strlen(sbuff)-4 ; i++){
            printf("%c", sbuff[i]);
        }

        //printf("strlen(sbuff) is: %lu \n", strlen(sbuff));
        if(s == NULL){//not a web server
            send_res= send(sock_res, sbuff, strlen(sbuff)-4, 0);
        }
        else{//a webserver so send with newlines
            send_res= send(sock_res, sbuff, strlen(sbuff), 0);
        }
        
        if(send_res <=0){
            perror("send");
            continue;
        }
        else{
            printf("msg send");
        }
        sbuff[send_res]='\0';//clean up
        //sleep(6);

    //recv
        char buff[4098];
        recv_res = recv(sock_res, buff, 4098 ,0);       
        if(recv_res ==0){
            perror("recv");
            break;
        }
        if(recv_res <0){
            perror("recv");
            continue;
        }
        //printf("sizeof buff is  %lu\n", strlen(buff)); 
        printf("\ns:");      
        for(int i=0; i< recv_res -1 ; i++){
            printf("%c", buff[i]);
        }
       printf("\n");
        buff[recv_res]='\0';//clean up
    }     

//close
    close(sock_res);

    return 0;
}

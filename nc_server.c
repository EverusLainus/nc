// i'm a server
//i send a message to clients


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

static int keepRunning = 1;

void intHandler(int );

void add_to_pfds(struct pollfd *pfds[], int newfd, int *p_count, int *p_size){   

    if(*p_count == *p_size){
        *p_size *=2;
        *pfds = realloc(*pfds, sizeof (* *pfds) * (*p_size));
    }
    
    (*pfds)[*p_count].fd = newfd;
    (*pfds)[*p_count].events=(POLLOUT | POLLIN);
    (*p_count)++;
    //printf("incremented *p_count is %d \n", *p_count);
}

void del_to_pfds(struct pollfd pfds[], int i, int *p_count){

    (pfds)[i] = (pfds) [*p_count];
    (*p_count)=(*p_count)-1;
    //printf("decremented *p_count is %d \n", *p_count);
}

int server(char *port){

    struct sigaction act;
    act.sa_handler = intHandler;
    sigaction(SIGINT, &act, NULL);
//addrinfo
    int rv;
    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof hints);
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    rv = getaddrinfo(NULL, port, &hints, &servinfo);
    if(rv == -1){
        perror("addrinfo \n");
        return 0;
    }

//socket and bind
    int sock_res;
    int bind_res;
    //printf("into socket and connect \n");
    for(p=servinfo; p!=NULL; p=p->ai_next){
       //printf("into socket \n"); 
        sock_res = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        //printf("sock_res is %d \n", sock_res);
        if(sock_res == -1){
            perror("socket \n");
            continue;
        }
       
        // to avoid "address already in use" error message
        int yes =1;
        setsockopt(sock_res, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        //printf("into bind \n"); 
        bind_res = bind(sock_res, p->ai_addr, p->ai_addrlen);
        if(bind_res == -1){
            perror("bind ");
            continue;
        }
        break;
    }

//listen
    int listen_res;
    listen_res = listen(sock_res,10);
    if(listen_res ==-1){
        perror("listen");
        return 0;
    }

    //printf("listening \n"); 
    int p_count=0;   
    int p_size=5;
    struct pollfd *pfds = malloc(sizeof(*pfds)* p_size);
       
    pfds[0].fd = sock_res;
    pfds[0].events =(POLLOUT | POLLIN);//get messages
    p_count=1;

    struct sockaddr_storage their_addr;
    socklen_t len = sizeof their_addr;

    char *message ="something magical is gonna happen you";
    int len_message = strlen(message);

//accept and send
    while(keepRunning){

       // printf("into big for \n");
        int poll_res = poll(pfds, p_count, -1);
        //printf("read one is %d \n", poll_res);
        if(poll_res < 0){
            perror("poll");
            return 1;
        }
        for(int i=0; i<p_count; i++){

            //printf(" for i = %d  \n", i);
            if(pfds[i].revents & (POLLOUT | POLLIN)){
                
                if(pfds[i].fd== sock_res){
                    int accept_res;
                    accept_res = accept(sock_res,(struct sockaddr *)&their_addr, &len);
                    if(accept_res == -1){
                        perror("accept");
                        continue;
                    }
                    //add new fd to pfds
                    add_to_pfds(&pfds, accept_res, &p_count, &p_size);
                }//its the listener
                else{
                    int send_res;
                    //printf("read to send \n");
                    send_res = send(pfds[i].fd, message, len_message,0);
                    if(send_res <=0){
                        perror("send");                        
                    }
                    printf("message sent successfully \n");
                    close(pfds[i].fd);
                    del_to_pfds(pfds, i, &p_count);
                }//not a listener
            }//i is ready to receive
        }//end counting on i.
    }//end empty for loop

    return 0;
}

//problem when listener stops listening in unterminating while loop is solved
//by <=0 as result for send.
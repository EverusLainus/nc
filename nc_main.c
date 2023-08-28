
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

void intHandler(int );
int client(char *, char *, char *);
void add_to_pfds(struct pollfd *[], int , int *, int *);
void del_to_pfds(struct pollfd[] , int , int *);
int server(char *);

int main(int argv, char *argc[]){
    char flag1[]="-c";
    int res1 = strcmp(argc[1], flag1);
    char flag2[]="-s";
    int res2 = strcmp(argc[1], flag2);
    char flag3[]="-w";
    int res3 = strcmp(argc[2], flag3);

    if(argv == 4){                       
        //printf("argc is %s \n", flag);      
        if(!res1){
            printf("i'm a client\n");
            client(0, argc[2], argc[3]); //connect 
        }
        else if(!res2){ 
            printf("i'm a server\n");         
            server(argc[3]); //bind listen
        }
        else{           
            printf("no appropriate flag");
            return 0;
        }        
    }
    else if (argv==5){
        if((!res1) && (!res3)){ 
            printf("i'm a client to a web server\n");         
            client(argc[2], argc[3], argc[4]); //connect 
        }
        else{
           printf("no appropriate client flag");
            return 0; 
        }
       
    }
    else{
        perror("not enough arguments \n");
    }
        return 0;
}
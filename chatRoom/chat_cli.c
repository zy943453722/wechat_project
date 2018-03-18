/*************************************************************************
	> File Name: chat_cli.c
	> Author: 
	> Mail: 
	> Created Time: 2017年11月29日 星期三 09时48分36秒
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<sys/epoll.h>
#include<fcntl.h>

#define MAX 1024
#define PORT 12345
typedef struct info
{
   int fd;
   char name[20];
   int flag;
   int ffd;//朋友的文件描述符
   char fname[20];
   char message[MAX];
}Info;
Info my;
char a[4] = "exit";
char b[2] = "ok";
char buff[MAX];

char Menu()
{
    char choice;
    printf("\t***********欢迎进入聊天系统************\n");
    printf("\t***********1.单人聊天******************\n");
    printf("\t***********2.群组聊天******************\n");
    printf("\t***********3.退出**********************\n");
    printf("\t***************************************\n");
    while(1)
    {
      printf("\n\t请输入您的选择:\n");
      scanf("%c",&choice);
      char c_tmp;
      while((c_tmp = getchar()) != '\n' && c_tmp !=EOF);
      if(choice == '1' || choice == '2' ||choice == '3')
      {
         return choice;
      }
      else
      {
         printf("\t您输入的选择有误，请重新输入！\n");
         continue;
      }
    }
}
void Select(int sockfd)
{
   char fname[20] = "";
   char choice = Menu();
   switch(choice)
   {
       case '1':
         my.flag = 1;
         printf("\t欢迎进入一对一聊天\n");
         printf("\t请输入好友昵称:\n");
         scanf("%s",my.fname);
         char c_tmp;
         while((c_tmp = getchar()) != '\n' && c_tmp !=EOF);
         break;
       case '2':
         my.flag = 2;
         printf("\t欢迎进入多人群组聊天\n");
         break;
       case '3':
         my.flag = 3;
         exit(1);
       default:
         exit(0);
   }
}
int init()
{
    int sock = 0;
    if((sock = socket(PF_INET,SOCK_STREAM,0)) < 0)
      perror("socket");
    struct sockaddr_in serveraddr;
    bzero(&serveraddr,0);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serveraddr.sin_port = htons(PORT);
    if(connect(sock,(struct sockaddr*)&serveraddr,sizeof(serveraddr)) < 0)
      perror("connect");
    return sock;
}
int setnonblocking(int fd)
{
    int old_option;
    if((old_option = fcntl(fd,F_GETFL)) < 0)
     perror("fcntl(fd,F_GETFL)");
    int new_option = old_option | O_NONBLOCK;
    if(fcntl(fd,F_SETFL,new_option) < 0)
     perror("fcntl(fd,F_SET,new_option)");
    return old_option;
}
void handleSession(int epollfd,int ret,struct epoll_event *triggerEvents,int sock,int std)
{
    char recvbuff[MAX+20];
    char sendbuff[MAX];
   for(int i = 0; i < ret; i++)
    {
        int sockfd = triggerEvents[i].data.fd;
        if(sockfd == sock && (triggerEvents[i].events & EPOLLIN))
        {
           //printf("read\n");
           memset(recvbuff,0,sizeof(recvbuff));
           int ret = recv(sock,recvbuff,sizeof(recvbuff),0);
            if(ret < 0)
               perror("recv");
            else if(ret == 0)
            {
                printf("server close\n");
                exit(EXIT_SUCCESS);
            }
            fputs(recvbuff,stdout);
            memset(recvbuff,0,sizeof(recvbuff));
        }
        else if((sockfd == std)&&(triggerEvents[i].events & EPOLLIN))
        {
            //printf("write\n");
           //memset(my.message,0,sizeof(my.message));
           memset(sendbuff,0,sizeof(sendbuff));
           if(fgets(sendbuff,sizeof(sendbuff),stdin) == NULL)
              break;
            int len = strlen(sendbuff);
            strncpy(my.message,sendbuff,len);
            my.message[len] = '\0';
            //printf("%s\n",my.message);
            send(sock,&my,sizeof(my),0);
            memset(sendbuff,0,sizeof(sendbuff));
        }
        else
           continue;    
    }
}
void Verify(int sockfd)
{
    char name[20];
    while(1)//验证身份
    {
        printf("\t请输入您的用户名：\n");
        scanf("%s",name);
        getchar();
        send(sockfd,name,strlen(name),0);
        memset(buff,0,sizeof(buff));
        recv(sockfd,buff,sizeof(buff),0);
        if(strncmp(buff,a,4) == 0)//与exit匹配
        {
            printf("\t当前用户已存在!请重新输入\n");
        }
        if(strncmp(buff,b,2) == 0)//与ok匹配
        {
            strcpy(my.name,name);
            break;
        }
    }
}
void Clear()
{
   my.ffd = -1;
   memset(my.message,0,sizeof(my.message));
}
int main()
{
    char name[20];
    my.fd = -1;
    my.flag = -1;
    memset(my.fname,0,sizeof(my.fname));
    Clear();
    int epollfd = epoll_create(MAX);
    struct epoll_event registerEvents;
    struct epoll_event triggerEvents[2];
    int sockfd = init();
    my.fd = sockfd;
    //printf("请输入您的用户名:\n");
    //scanf("%s",name);
    //getchar();
    strcpy(my.name,name);
    Verify(sockfd);//验证并填入用户名
    Select(sockfd);//进行功能选择
    registerEvents.data.fd = sockfd;
    registerEvents.events = EPOLLIN;
    epoll_ctl(epollfd,EPOLL_CTL_ADD,registerEvents.data.fd,&registerEvents);//注册读取连接事件
    //setnonblocking(sockfd);
    int std = fileno(stdin);
    registerEvents.data.fd = std;
    registerEvents.events = EPOLLIN;
    //注册从标准输入读入事件
    epoll_ctl(epollfd,EPOLL_CTL_ADD,registerEvents.data.fd,&registerEvents);
    //setnonblocking(std);
    while(1)
    {
        int ret = epoll_wait(epollfd,triggerEvents,MAX,-1);
        //printf("ret = %d\n",ret);
        if(ret < 0)
        {
            printf("epoll fail\n");
            break;
        }
        handleSession(epollfd,ret,triggerEvents,sockfd,std);
        Clear();
        //printf("exit\n");
    }
    close(sockfd);
    return 0;
}

/*************************************************************************
	> File Name: chat_ser.c
	> Author: 
	> Mail: 
	> Created Time: 2017年11月29日 星期三 09时48分30秒
 ************************************************************************/

#include<stdio.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include<sys/epoll.h>
#include <fcntl.h>

#define PORT 12345
#define MaxMember 300
#define MAX 1024

typedef struct client{
  int fd;
  char name[20];
  int flag;
  int ffd;
  char fname[20];
  char message[MAX];
}Client;

int total = 0;//当前的连接人数
struct sockaddr_in server_addr;
struct sockaddr_in client_addr;
Client cli[MaxMember];

int init()
{
    int listenfd;
    int on = 1;
    if((listenfd = socket(PF_INET,SOCK_STREAM,0)) < 0)
      perror("socket");
    bzero(&server_addr,sizeof(server_addr));//清零
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)) < 0)
      perror("setsockopt");//地址复用
    if(bind(listenfd,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0)
      perror("bind");
    if(listen(listenfd,SOMAXCONN) < 0)
      perror("listen");
    return listenfd;
}
int setnonblocking(int fd)//设置非阻塞
{
    int old_option;
    if((old_option = fcntl(fd,F_GETFL)) < 0)
      perror("fcntl(fd,F_GETFL)");
    int new_option = old_option | O_NONBLOCK;
    if(fcntl(fd,F_SETFL,new_option) < 0)
      perror("fcntl(fd,F_SET,new_option)");
    return old_option;
}
void registerConnect(int epollfd,int fd,struct epoll_event registerEvents)
{
   registerEvents.data.fd = fd;
   registerEvents.events = EPOLLIN;
   epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&registerEvents);
   //setnonblocking(fd);
}
void Clear(int i)//清空结构体
{
            //printf("%s\n",sendbuff);
    cli[i].ffd = -1;
    cli[i].flag = -1;
    cli[i].fd = -1;
    memset(cli[i].name,0,sizeof(cli[i].name));
    memset(cli[i].message,0,sizeof(cli[i].message));
    memset(cli[i].fname,0,sizeof(cli[i].fname));
}
void handleConnect(int epollfd,int ret,struct epoll_event *triggerEvents,int listenfd,struct epoll_event registerEvents)
{
    int len = sizeof(Client);
    char buff[MAX+20];//用于接收用户信息
    char recvbuff[len];//用来接收用户消息
    char name[20];
    int connfd;
    for(int i = 0; i < ret; i++)
    {
        int sockfd = triggerEvents[i].data.fd;
        if(sockfd == listenfd && (triggerEvents[i].events & EPOLLIN))//表明某个listenfd事件发生了
        {
            socklen_t client_addrlength = sizeof(client_addr);
            if((connfd = accept(listenfd,(struct sockaddr*)&client_addr,&client_addrlength)) < 0)
               perror("connfd");
            while(1)//验证用户名是否重复
            {
                memset(name,0,sizeof(name));
                recv(connfd,name,sizeof(name),0);
                int b = 0;
                for(b = 0; b < MaxMember; b++)
                {
                    if(strcmp(name,cli[b].name) == 0)
                    {
                        memset(buff,0,sizeof(buff));
                        sprintf(buff,"exit");//填入buff
                        send(connfd,buff,sizeof(buff),0);
                        break;
                    }
                }
                if(b == MaxMember)
                {
                    break;
                }
            }
            for(int j = 0;j < MaxMember;j++)
            {
               if(cli[j].fd == -1)
                {
                    cli[j].fd = connfd;//放的时候i和j不一定一样，因此事件号和连接号无法一一对应
                    memset(buff,0,sizeof(buff));
                    sprintf(buff,"ok");
                    send(connfd,buff,sizeof(buff),0);
                    strcpy(cli[j].name,name);
                    printf("%s上线了\n",cli[j].name);
                    registerConnect(epollfd,connfd,registerEvents);
                    break;
                }
            }
            printf("IP = %s PORT = %d\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
            total++;
            printf("当前的链接人数为：%d\n",total);
        }
        else if(triggerEvents[i].events & EPOLLIN)//发生读事件
        {
            int k = 0;
            //printf("%d,%d\n",triggerEvents[i].data.fd,cli[0].fd);
            for(k = 0; k < MaxMember; k++)
            {
                if(cli[k].fd == triggerEvents[i].data.fd)//找到了某个用户
                {
                    //printf("k = %d\n",k);
                    Clear(k);
                    break;
                }
            }
            //printf("k = %d\n",k);
            memset(recvbuff,0,sizeof(recvbuff));
            memset(buff,0,sizeof(buff));
            int res = recv(triggerEvents[i].data.fd,recvbuff,sizeof(recvbuff),0);
            //printf("%s\n",recvbuff);
            if(res == -1)
            {
               //perror("recv");
                exit(0);
            }
            else if(res == 0)//客户端被动退出
            {
                printf("%s close\n",cli[k].name);
                printf("当前连接人数:%d\n",--total);
                cli[k].fd = -1;
                close(triggerEvents[i].data.fd); //一定是先关闭后置数
                triggerEvents[i].data.fd = -1;
            }
            else
            {
                //printf("%d,%d,%d\n",cli[k].fd,cli[k].ffd,cli[k].flag);
                memcpy(&cli[k],recvbuff,sizeof(cli[k]));   
                cli[k].fd = triggerEvents[i].data.fd;
                //printf("%s\n",cli[k].message);
                if(cli[k].flag == 1)
                {
                    //printf("1\n");
                    sprintf(buff,"%s:%s",cli[k].name,cli[k].message);
                    printf("%s\n",buff);
                    for(int a = 0; a < MaxMember; a++)
                    {
                        if(strcmp(cli[a].name,cli[k].fname) == 0)
                        {
                            cli[k].ffd = cli[a].fd;
                            //printf("cli[%d].ffd = %d\n",k,cli[k].ffd);
                            break;
                        }
                    }
                    send(cli[k].ffd,buff,sizeof(buff),0);
                }
                else if(cli[k].flag == 2)
                {
                    sprintf(buff,"%s:%s",cli[k].name,cli[k].message);
                    printf("%s\n",buff);
                    for(int a = 0; a < MaxMember; a++)
                    {
                       if(cli[a].fd >=0 && k != a)//群发
                       {
                          send(cli[a].fd,buff,sizeof(buff),0);
                       }
                    }

                }
                else//客户端主动关闭
                {
                    printf("%s close\n",cli[k].name);
                    printf("当前连接人数:%d\n",--total);
                    cli[k].fd = -1;
                    close(triggerEvents[i].data.fd);
                    triggerEvents[i].data.fd = -1;
                }
            }
        }
        else
        {
            continue;
        }
    }
}
int main()
{
    int epollfd = epoll_create(MaxMember);//设置内核事件表的fd
    struct epoll_event registerEvents;//设置注册新事件
    struct epoll_event triggerEvents[MaxMember];//设置触发返回的事件
    for(int i = 0; i < MaxMember; i++)//初始化
    {
        triggerEvents[i].data.fd = -1;//感兴趣事件的文件描述符
        cli[i].fd = -1;
    }
    int listenfd = init();
    registerEvents.data.fd = listenfd;
    registerEvents.events = EPOLLIN;//注册事件设置为读事件
    epoll_ctl(epollfd,EPOLL_CTL_ADD,registerEvents.data.fd,&registerEvents);//将事件注册到内核事件表中
    //setnonblocking(registerEvents.data.fd);//将文件描述符设置成非阻塞的
    while(1)//接受连接所用循环
    {
        if(total < MaxMember)
        {
           int ret = epoll_wait(epollfd,triggerEvents,MaxMember,-1);//设置仅有一个事件触发时才返回
           if(ret < 0)
           {
              printf("epoll fail\n");
              break;
           }
           handleConnect(epollfd,ret,triggerEvents,listenfd,registerEvents);        
        }
        else
        {
            printf("连接人数已达上限！\n");
            break;
        }
    }
    close(listenfd);
    return 0;
}

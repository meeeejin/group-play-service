#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include <wifi.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <tizen_error.h>

#include <dlog.h>
#include <udp_test.h>

#include <vconf.h>
#include <aul.h>
#include <app.h>

#include <time.h>
#undef LOG_TAG
#define LOG_TAG "GROUP_PLAY_SVC"

#define UDP_PORT    10122
#define MAXLINE     1024

#define SOUND_PLAYER_PKG_NAME "org.tizen.sound-player"

int socket_fd;

static void playMusic()
{
    ALOGI("Play music...\n");

    service_h service;

    service_create(&service);
    service_set_operation(service, SERVICE_OPERATION_VIEW);
    service_set_package(service, SOUND_PLAYER_PKG_NAME);
    service_set_uri(service, "file:///opt/storage/sdcard/Sounds/1.mp3");
    if (service_send_launch_request(service, NULL, NULL) == SERVICE_ERROR_NONE) {
        ALOGI("Success\n");
    } else {
        ALOGI("Fail\n");
    }
    service_destroy(service);
}

int msg_send_func(unsigned int ulMsgId)
{
    struct stMsg stSendBuf;
    struct sockaddr_in destaddr;
    int addrlen = sizeof(destaddr); 
    char ip_addr[16] = {0,};
    char *temp = "115.145.173.197";

    strcpy(ip_addr, temp);

    ALOGI("Dest Ip Addres is %s.\n", ip_addr);

    memset(&stSendBuf, 0x0, sizeof(stSendBuf));

    stSendBuf.ulMsgId = htonl(ulMsgId);

    // Sender
    destaddr.sin_family = AF_INET;
    destaddr.sin_addr.s_addr = inet_addr((const char*)ip_addr); 
    destaddr.sin_port = htons(UDP_PORT);

    if((sendto(socket_fd, &stSendBuf, sizeof(struct stMsg), 0, (struct sockaddr *)&destaddr, addrlen)) < 0) 
    {
        perror("sendto fail");
        exit(0);
    }
}

void global_socket_init()
{
    struct sockaddr_in srcaddr;
    int addrlen = sizeof(srcaddr);
    int iMode;

    close(socket_fd);
   
    if((socket_fd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket fail");
        exit(0);
    }

    // Receiver 
    srcaddr.sin_family = AF_INET;
    srcaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    srcaddr.sin_port = htons(UDP_PORT);

    #if 1
    iMode = 1;
    if (ioctl(socket_fd, FIONBIO, &iMode))
    {
        perror("ioctl failed");
        exit(0);
    }
    #endif

    if(bind(socket_fd, (struct sockaddr *)&srcaddr, addrlen) < 0) {
        perror("bind fail");
        exit(0);
    }

    msg_send_func(INITIAL_CONNECT_REQ);
}


void *udp_thread_start(void*)
{
    char RecvBuf[MAXLINE];
    struct stMsg *stUdpMsg;
    int nbyte;
    struct sockaddr_in cliaddr;
    int addrlen = sizeof(cliaddr);

    global_socket_init();

    while(1)
    {
        ALOGI("\n\rWaiting...\n\r");
       
        memset(RecvBuf, 0, sizeof(RecvBuf));
        nbyte = recvfrom(socket_fd, RecvBuf, MAXLINE , 0, (struct sockaddr *)&cliaddr, (socklen_t *)&addrlen);
        ALOGI("nbyte = %d\n", nbyte);

        if(nbyte > 0)
        {
            stUdpMsg = (struct stMsg *)RecvBuf;

            ALOGI("\n\r");
            ALOGI("Message ID : 0x%08X \n\r", ntohl(stUdpMsg->ulMsgId));

            switch(ntohl(stUdpMsg->ulMsgId))
            {
                case NTP_REQ:
                //시간 동기화 과정  필요!
                time_t raw_time;
                struct tm* time_info;
                time(&raw_time);
                time_info = localtime(&raw_time);

                break;

                case PLAY_TIME_REQ:
                ALOGI("time = %d\n", ntohl(stUdpMsg->Time));
                /*time_t timer;
                while(1)
                {
                    
                    timer = time(NULL);
                    if(ntohl(stUdpMsg->Time) == timer)
                    {
                        playMusic();
                        break;
                    }
                }*/
                playMusic();
               
                break;

                default :
                break;
            }
        }
        else
        {
            ALOGI("Receive Error! errno = %d, %s\n", errno, strerror(errno));
        }


        sleep(1);
    }

    close(socket_fd);
}
   

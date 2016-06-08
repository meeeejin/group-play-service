#define NTP_REQ 0x74160004
#define PLAY_TIME_REQ 0x7416004 + 0x1000
#define INITIAL_CONNECT_REQ 0x7416004 + 0x2000

struct stMsg{
   unsigned int ulMsgId;
   unsigned int Time;
};

int msg_send_func(unsigned int ulMsgId);
void *udp_thread_start(void*);

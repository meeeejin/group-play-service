#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <vconf.h>

#include "common.h"
#include <dlog.h>
#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_Evas.h>

#include <wifi.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <tizen_error.h>

#include "udp_test.h"

#include <pthread.h>
//#include <FBase.h> 
//#include <FApp.h>

#include <aul.h>
#include <app_manager.h>

#undef LOG_TAG
#define LOG_TAG "GROUP_PLAY_SVC"

#define VCONFKEY_GROUP_PLAY  "db/private/org.tizen.menu-screen/group_play"
#define MUSIC_PLAYER_PKG_NAME "org.tizen.music-player"

static GMainLoop* gMainLoop = NULL;
static int gSocketFd = -1;
static const char uuid[] = "00001101-0000-1000-8000-00805F9B34FB";


void _vconf_noti_callback(keynode_t *node, void* data)
{
    printf("%s:+++\n", __func__);

//    struct appdata *ad = (struct appdata *)data;
    char *keyname = vconf_keynode_get_name(node);

    if (strcmp(keyname, VCONFKEY_GROUP_PLAY) == 0)
    {
        msg_send_func(PLAY_TIME_REQ);
    }
}

bool initEcore()
{
    printf("initEcore()\n");

    int ret, type;
    Eina_Bool did = EINA_FALSE;
    Ecore_Event_Handler *mouse_down = NULL;
    Ecore_Event_Handler *handler = NULL;
    Ecore_Event *event;

    ret = ecore_init();
    if (ret != 1)
        printf("ecore_init fail\n");

    ecore_event_init();
    type = ecore_event_type_new();
    if (type < 1) 
        printf("type fail\n");

    handler = ecore_event_handler_add(type, mp_app_mouse_event_cb, &did);
    if (!handler) 
        printf("Regi fail 1\n");

    event = ecore_event_add(type, NULL, NULL, NULL);
    if (!event)
        printf("add fail\n");


    mouse_down = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_DOWN, mp_app_mouse_event_cb, NULL);
    if (!mouse_down)
        printf("Regi fail 2\n");

    printf("%d %d\n", type, ECORE_EVENT_MOUSE_BUTTON_DOWN);

    printf("main_loop_bengin()\n");
    ecore_main_loop_begin();

    ret = ecore_shutdown();
    printf("unreached main_loop_bengin()\n");
}

bool init_vconf()
{
    bool res = TRUE;

    printf("%s:+++\n", __func__);

    if (vconf_notify_key_changed(VCONFKEY_GROUP_PLAY, _vconf_noti_callback, NULL) < 0)
    {
            printf("Error when register callback\n");
            res = FALSE;
    }

    printf("%s:---:res=%d\n", __func__, res);
    
    return res;
}

bool deinit_vconf()
{
    bool res = TRUE;
        
    printf("%s:+++\n", __func__);
                
    //vconf_ignore_key_changed(VCONF_PLAYER_SHUFFLE, _vconf_noti_callback);
                        
    printf("%s:---:res=%d\n", __func__, res);
                                
    return res;
}



void initIPC(void)
{
    pthread_attr_t attr;
    pthread_t thread_t;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&thread_t, &attr, &udp_thread_start, NULL);
}

int main(int argc, char *argv[])
{
	int error, ret = 0;

	// Initialize a GTK main loop
	gMainLoop = g_main_loop_new(NULL, FALSE);
    //dlog_print(DLOG_INFO, LOG_TAG, "Group Play Sevice started.");

    // Initialize a udp communication
    initIPC();

    // Initialize the vconf file
    init_vconf();

    // Initialize Ecore
    initEcore();

	// Start the main loop of service
	g_main_loop_run(gMainLoop);

    // Deinitializing vconf
    deinit_vconf();

    //dlog_print(DLOG_INFO, LOG_TAG, "Group Play Sevice terminated.");

	return ret;
}


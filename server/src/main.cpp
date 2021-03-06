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
#include <FBase.h> 
#include <FApp.h>

#include <aul.h>
#include <app_manager.h>

#undef LOG_TAG
#define LOG_TAG "GROUP_PLAY_SVC"

static GMainLoop* gMainLoop = NULL;

Eina_Bool mp_app_mouse_event_cb(void *data, int type, void *event)
{
    ALOGI("TEST\n");
    if (type == ECORE_EVENT_MOUSE_BUTTON_DOWN) {
        ALOGI("ECORE_EVENT_MOUSE_BUTTON_DOWN\n");
    }
    else if (type == ECORE_EVENT_MOUSE_BUTTON_UP) {
        ALOGI("ECORE_EVENT_MOUSE_BUTTON_UP\n");
    }
    return 0;
}

bool initEcore()
{
    ALOGI("initEcore()\n");

    int ret, type;
    Eina_Bool did = EINA_FALSE;
    Ecore_Event_Handler *mouse_down = NULL;
    Ecore_Event_Handler *handler = NULL;
    Ecore_Event *event;

    ret = ecore_init();
    if (ret != 1)
        ALOGI("ecore_init fail\n");

    ecore_event_init();
    type = ecore_event_type_new();
    if (type < 1) 
        ALOGI("type fail\n");

    handler = ecore_event_handler_add(type, mp_app_mouse_event_cb, &did);
    if (!handler) 
        ALOGI("Regi fail 1\n");

    event = ecore_event_add(type, NULL, NULL, NULL);
    if (!event)
        ALOGI("add fail\n");


    mouse_down = ecore_event_handler_add(ECORE_EVENT_MOUSE_BUTTON_DOWN, mp_app_mouse_event_cb, NULL);
    if (!mouse_down)
        ALOGI("Regi fail 2\n");

    ALOGI("%d %d\n", type, ECORE_EVENT_MOUSE_BUTTON_DOWN);

    ALOGI("main_loop_begin()\n");
    ecore_main_loop_begin();

    ret = ecore_shutdown();
    ALOGI("unreached main_loop_begin()\n");
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
    ALOGI("Group Play Sevice started.");

    // Initialize a udp communication
    initIPC();

    // Initialize Ecore
    initEcore();

	// Start the main loop of service
	g_main_loop_run(gMainLoop);

    ALOGI("Group Play Sevice terminated.");

	return ret;
}


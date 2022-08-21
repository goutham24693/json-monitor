#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <sys/select.h>
#include <unistd.h>

#define SUCCESS 0
#define FAILURE 1

#define PROGRAM_NAME "run.exe"

typedef enum _Event_ {
	READY_TO_WAIT,
	STARTING,
	COMPLETE
}Event;

typedef struct EventData {
	Event event;
	char data[128];
} EventData;

int get_value_str (char * out, ssize_t l);

Event get_event ();
int handler_init();
int handler_exit();
void * monitor_thread (void * arg);
int set_event_value (char * event, char * value);
void wait_for_monitor_ready();
void monitor_signal_ready();

#endif // _GLOBAL_H_

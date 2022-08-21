#include "global.h"

#define READY_EVENT_STR    "Ready To Wait"
#define STARTING_EVENT_STR "Starting"
#define COMPLETE_EVENT_STR "Complete"

// Shared objects between main-handler & helper-monitor thread
static EventData gEventData;			// Event and value saved here	
static pthread_mutex_t EDmutex;			// lock to operate on gEventData
static pthread_t monitor_thread_id;		// monitor thread id
static pthread_cond_t ready_condition;	// conditional variable for thread sync

// init part for the program
int handler_init(char * path)
{
	// init mutex
	if (pthread_mutex_init(&EDmutex, NULL) != 0) 
	{
		perror("pthread_mutex_init:");
		return FAILURE;
	}

	// init conditional variable
	if (pthread_cond_init (&ready_condition, NULL) != 0)
	{
		pthread_mutex_destroy(&EDmutex);
		perror("pthread_cond_init:");
		return FAILURE;
	}

	char * filepath = malloc(strlen(path) + 1);
	if (filepath == NULL)
	{
		pthread_mutex_destroy(&EDmutex);
		pthread_cond_destroy(&ready_condition);
		perror("malloc:");
		return FAILURE;
	}
	strcpy(filepath, path);

	// init monitor thread
	if (pthread_create(&monitor_thread_id, NULL, &monitor_thread, (void *)filepath) != 0)
	{
		perror("pthread_create:");
		pthread_mutex_destroy(&EDmutex);
		pthread_cond_destroy(&ready_condition);
		free(filepath);
		return FAILURE;
	}

	return SUCCESS;
}

// exit part for the program
int handler_exit()
{
	int ret = SUCCESS;

	// collect thread exit	
	if (pthread_join(monitor_thread_id, NULL) != 0)
	{
		perror("pthread_join:");
		ret = FAILURE;
	}

	// clear conditional variable
	if (pthread_cond_destroy(&ready_condition) != 0) 
	{
		perror("pthread_mutex_destroy:");
		ret = FAILURE;
	}

	// clear mutex
	if (pthread_mutex_destroy(&EDmutex) != 0) 
	{
		perror("pthread_mutex_destroy:");
		ret = FAILURE;
	}

	return ret;
}

// called by handler-main thread - Consumer waits for producer
void wait_for_monitor_ready()
{
	pthread_mutex_lock(&EDmutex);
	//printf("Waiting for monitor thread\n");
	pthread_cond_wait(&ready_condition, &EDmutex);
	//printf("monitor thread unlocked the handler thread\n");
	pthread_mutex_unlock(&EDmutex);
}

// called by monitor-helper thread - producer signals consumer that its ready with data
void monitor_signal_ready()
{
	pthread_mutex_lock(&EDmutex);
	//printf("signaling handler thread\n");
	pthread_cond_signal(&ready_condition);
	pthread_mutex_unlock(&EDmutex);
}

// called by handler-main thread
Event get_event ()
{
	Event ret;
	pthread_mutex_lock(&EDmutex);
	ret = gEventData.event;
	pthread_mutex_unlock(&EDmutex);
	return ret;
}

// called by handler-main thread
int get_value_str (char * out, ssize_t l)
{
	if (out == NULL || l < 0)
	{
		printf("%s %d: Invalid args\n", __func__, __LINE__);
		return FAILURE;
	}

	pthread_mutex_lock(&EDmutex);
	strcpy(out, gEventData.data);
	pthread_mutex_unlock(&EDmutex);
	return SUCCESS;
}

// called by monitor-helper thread
int set_event_value (char * event, char * value)
{

	if ((event == NULL) || (value == NULL) || (strlen(value) > sizeof(gEventData.data) - 1))
	{
		printf("%s %d: Invalid args\n", __func__, __LINE__);
		return FAILURE;
	}

	int ret = FAILURE;

	pthread_mutex_lock(&EDmutex);
	strcpy(gEventData.data, value);
	if (strcmp(event, READY_EVENT_STR)	== 0)
	{
		gEventData.event = READY_TO_WAIT;
		ret = SUCCESS;
	}
	else if (strcmp(event, STARTING_EVENT_STR) == 0)
	{
		gEventData.event = STARTING;
		ret = SUCCESS;
	}
	else if (strcmp(event, COMPLETE_EVENT_STR) == 0)
	{
		gEventData.event = COMPLETE;
		ret = SUCCESS;
	}
	pthread_mutex_unlock(&EDmutex);


	return ret;
}

#include "utils.h"

static bool is_monitor_ready;

// read file and update data structure
static void update_event_data (char * file)
{
	char event[128];
	char value[128];

	memset (event, 0, sizeof(event));
	memset (value, 0, sizeof(value));

	if ((get_event_str_from_file(file, event, sizeof(event)) == SUCCESS) &&
			(get_value_str_from_file(file, value, sizeof(value)) == SUCCESS))
	{
		set_event_value(event, value);
		//printf("set event: %s value: %s\n", event, value);
	}

	if (strcmp(event, "Complete") == 0)
	{
		if (is_monitor_ready == false)
		{
			monitor_signal_ready();
			is_monitor_ready = true;
		}
		free(file);
		pthread_exit(NULL);
	}
} 

// thread that waits on a loops and reads the file
void * monitor_thread (void * arg)
{

	if (arg == NULL)
	{
		// no input json file arg, exit
		pthread_exit(NULL);
	}

	int n = 0;
	char * file = (char *) arg;
	struct timeval tv;

	is_monitor_ready = false;

	update_event_data(file);

	monitor_signal_ready();
	is_monitor_ready = true;

	while (1)
	{
		tv.tv_sec = 0;
		tv.tv_usec = 5000;
		n = select(0, NULL, NULL, NULL, &tv);
		if (n < 0)
		{
			continue;
		}

		update_event_data(file);

	}
	return NULL;
}

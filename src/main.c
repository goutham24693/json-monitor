#include "handler.h"
#include "utils.h"

/* State Functions */
static State state_idle (SmData * psm_data);
static State state_wait(SmData * psm_data);
static State state_in_progress(SmData * psm_data);
static State state_done(SmData * psm_data);

/* Transition Functions */
static State transition_init (SmData * psm_data);
static State transition_waiting (SmData * psm_data);
static State transition_start (SmData * psm_data);
static State transition_complete (SmData * psm_data);

int fetch_sm_data (SmData * psm_data)
{
	if (psm_data == NULL)
	{
		printf("%s %d: invalid args\n", __func__, __LINE__);
		return FAILURE;
	}

	psm_data->event = get_event();
	get_value_str(psm_data->data, sizeof(psm_data->data));
	//printf("get event %d value %s\n", psm_data->event, psm_data->data);
	psm_data->sm_running = true;
	return SUCCESS;
	
}

static State transition_init (SmData * psm_data)
{
	if (psm_data == NULL)
	{
		printf("%s %d: invalid args\n", __func__, __LINE__);
		return HANDLER_INVALID_STATE;
	}
	if (fetch_sm_data(psm_data) != SUCCESS)
		return HANDLER_INVALID_STATE;

	// ideally initial transition should return IDEL state,
	// considering the program started later and we have received 
	// events such as Starting, Complete etc.
	if (psm_data->event == READY_TO_WAIT)
		return HANDLER_IDLE_STATE;
	else if (psm_data->event == STARTING)
		return HANDLER_WAIT_STATE;
	else if (psm_data->event == COMPLETE)
		return HANDLER_IN_PROGRESS_STATE;
	else
		return HANDLER_INVALID_STATE;
}

static State transition_waiting (SmData * psm_data)
{
	if (psm_data == NULL)
	{
		printf("%s %d: invalid args\n", __func__, __LINE__);
		return HANDLER_INVALID_STATE;
	}

	char buff[128];
	memset (buff, 0, sizeof(buff));

	get_value_str(buff, sizeof(buff));

	printf("State: Waiting Value:%s\n", buff);

	return HANDLER_WAIT_STATE;
}

static State transition_start (SmData * psm_data)
{
	if (psm_data == NULL)
	{
		printf("%s %d: invalid args\n", __func__, __LINE__);
		return HANDLER_INVALID_STATE;
	}

	char buff[128];
	memset (buff, 0, sizeof(buff));

	get_value_str(buff, sizeof(buff));

	printf("State: In Progress Value:%s\n", buff);

	return HANDLER_IN_PROGRESS_STATE;
}

static State transition_complete (SmData * psm_data)
{
	if (psm_data == NULL)
	{
		printf("%s %d: invalid args\n", __func__, __LINE__);
		return HANDLER_INVALID_STATE;
	}

	char buff[128];
	memset (buff, 0, sizeof(buff));

	get_value_str(buff, sizeof(buff));

	printf("State: Done Value:%s\n", buff);

	return HANDLER_DONE_STATE;
}

static State state_idle (SmData * psm_data)
{
	if (psm_data == NULL)
	{
		printf("%s %d: invalid args\n", __func__, __LINE__);
		return HANDLER_INVALID_STATE;
	}

	if (psm_data->event == READY_TO_WAIT)
	{
		// we have received READY event
		return transition_waiting(psm_data);	
	}

	// no READY event yet, wait in this state
	return HANDLER_IDLE_STATE;

}

static State state_wait(SmData * psm_data)
{
	if (psm_data == NULL)
	{
		printf("%s %d: invalid args\n", __func__, __LINE__);
		return HANDLER_INVALID_STATE;
	}

	if (psm_data->event == STARTING)
	{
		// we have received START event
		return transition_start(psm_data);	
	}

	// no READY event yet, wait in this state
	return HANDLER_WAIT_STATE;

}

static State state_in_progress(SmData * psm_data)
{
	if (psm_data == NULL)
	{
		printf("%s %d: invalid args\n", __func__, __LINE__);
		return HANDLER_INVALID_STATE;
	}

	if (psm_data->event == COMPLETE)
	{
		// we have received COMPLETE event
		return transition_complete(psm_data);	
	}

	// no COMPLETE event yet, wait in this state
	return HANDLER_IN_PROGRESS_STATE;

}

static State state_done(SmData * psm_data)
{
	if (psm_data == NULL)
	{
		printf("%s %d: invalid args\n", __func__, __LINE__);
		return HANDLER_INVALID_STATE;
	}

	// exit from state machine
	psm_data->sm_running = false;

	return HANDLER_DONE_STATE;

}

static int start_handler_state_machine()
{
	State curr_state;
	SmData sm_data;
    int n = 0;
    struct timeval tv;

	// start state machine 
	curr_state = transition_init(&sm_data);

	while (sm_data.sm_running)
	{
        tv.tv_sec = 0;
        tv.tv_usec = 5000;
		// Adding delay to improve CPU usage
        n = select(0, NULL, NULL, NULL, &tv);
        if (n < 0)
        {
            continue;
        }

		// update event data every cycle
		fetch_sm_data(&sm_data);
		switch(curr_state)
		{
			case HANDLER_IDLE_STATE:
				curr_state = state_idle(&sm_data);
				break;
			case HANDLER_WAIT_STATE:
				curr_state = state_wait(&sm_data);
				break;
			case HANDLER_IN_PROGRESS_STATE:
				curr_state = state_in_progress(&sm_data);
				break;
			case HANDLER_DONE_STATE:
				curr_state = state_done(&sm_data);
				break;
			case HANDLER_INVALID_STATE:
			default:
				printf("%s %d: state machine moved to invalid state\n",__func__, __LINE__);
				return FAILURE;
		}
	}
	// received COMPLETED event
	return SUCCESS;
}

static void print_help()
{
	printf("%s: incorrect option\n", PROGRAM_NAME);
	printf("usage: %s <path/to/json/file>\n", PROGRAM_NAME);
}

int main(int argc, char * argv[])
{

	//check param
	if (argc != 2 || access(argv[1], F_OK) != 0)
	{
		print_help();
		return FAILURE;
	}

	// program constructor
	if (handler_init(argv[1]) != SUCCESS)
	{
		printf("%s %d: thread creation failed\n", __func__, __LINE__);
		return FAILURE;
	}

	// waits for event monitor thread to get read the file
	wait_for_monitor_ready();

	// state handling
	if (start_handler_state_machine() != SUCCESS)
	{
		printf("unexpected error. Exiting program\n");
	}

	// program destructor
	if (handler_exit() != SUCCESS)
	{
		printf("%s %d: exit failed\n", __func__, __LINE__);
		return FAILURE;
	}

	return SUCCESS;
}


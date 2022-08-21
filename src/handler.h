#include "global.h"

typedef enum _States_ {
	HANDLER_IDLE_STATE,
	HANDLER_WAIT_STATE,
	HANDLER_IN_PROGRESS_STATE,
	HANDLER_DONE_STATE,
	HANDLER_INVALID_STATE
} State;

typedef struct _sm_data_ {
	Event event;
	char data[128];
	bool sm_running;
}SmData;

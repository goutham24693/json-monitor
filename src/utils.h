#include "global.h"
#define EVENT_PATTERN  "\"event\":"
#define VALUE_PATTERN  "\"value\":"


int get_event_str_from_file(char * file, char * out, ssize_t l);
int get_value_str_from_file(char * file, char * out, ssize_t l);

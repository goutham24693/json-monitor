#include "utils.h"

// called by monitor-helper thread
static int get_pattern(char * file, char * out, ssize_t l, char * pattern)
{
	if (file == NULL || out == NULL || l < 0 || pattern == NULL)
	{
		printf("%s %d: Invalid args\n", __func__, __LINE__);
		return FAILURE;
	}
	FILE * fp = NULL;

	fp = fopen(file, "r");
	if (fp == NULL)
	{
		perror("fopen:");
		return FAILURE;
	}

	ssize_t read;
	char * line = NULL;
	size_t len = 0;
	char * s = NULL;
	int ret = FAILURE;	

	while ((read = getline(&line, &len, fp)) != -1) 
	{
		s = strstr(line, pattern);
		if (s != NULL)
		{
			s = s + strlen(pattern);
			if (strlen(s) > 0 )
			{
				// remove leading white space
				while (*s == ' ')  s++;
				if (strlen(s) > l || strlen(s) <= 4)
				{
					printf("object param %s has unexpected size\n", pattern);
					break;
				}
				if (sscanf(s, "\"%[^\",]\"", out) == 1)
				{
					//printf("%s\n", out);
					ret = SUCCESS;
				}
				break;
			}

		}
	}

	fclose(fp);
	if (line)
		free(line);

	return ret;
}

// called by monitor-helper thread
int get_event_str_from_file(char * file, char * out, ssize_t l)
{
	return get_pattern(file, out, l, EVENT_PATTERN);
}

// called by monitor-helper thread
int get_value_str_from_file(char * file, char * out, ssize_t l)
{
	return get_pattern(file, out, l, VALUE_PATTERN);
}


SUBDIRS = src/

CC = gcc
CFLAGS = -g -Wall -pthread

$(SUBDIRS):
	$(MAKE) -C $@

clean:
	$(MAKE) -C $(SUBDIRS) $@

.PHONY: $(SUBDIRS) clean

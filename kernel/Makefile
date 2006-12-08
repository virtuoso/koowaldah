SOURCES := \
	init.c \
	textio.c \
	thread.c \
	fork.c \
	thread_queue.c \
	scheduler.c \
	sched0.c \
	timecore.c \
	mm_zone.c \
	page_alloc.c \
	slice.c \
	galloc.c

SUBDIRS := fs init lib

include $(PRJROOT)/konfig.mk
ifeq ($(MK_INSTRUMENT_PROFILER),Y)
 SOURCES += debug/profile.c debug/triggers.c
endif

include $(PRJROOT)/tools/main.mk


SOURCES := \
	init.c \
	textio.c \
	thread.c \
	fork.c \
	exec.c \
	exit.c \
	thread_queue.c \
	scheduler.c \
	sched0.c \
	timecore.c \
	symtab.c \
	mm_zone.c \
	mem_area.c \
	brk.c \
	page_alloc.c \
	slice.c \
	galloc.c

SUBDIRS := fs init lib

-include $(PRJROOT)/konfig.mk
ifeq ($(MK_INSTRUMENT_PROFILER),Y)
 SOURCES += debug/profile.c debug/triggers.c
endif

ifeq ($(MK_SYSRQ_DEBUG),Y)
 SOURCES += debug/sysrq.c
endif

include $(PRJROOT)/tools/main.mk


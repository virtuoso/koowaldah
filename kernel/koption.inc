KOPTIONS_START(KERNEL, "Main kernel options", 2)
KOPTION(
	"MM_ANAL_DEBUG",
	"Memory management debugging",
	"Turn this on if you want to debug page allocation "
	"and related functionality. Produces A LOT of output.",
	KOPT_BOOL,
	'N'
),
KOPTION(
	"KPRINTF_BUFSIZE",
	"Tune the kernel output buffer size",
	"If unsure, leave intact.",
	KOPT_INT,
	1024
),
KOPTIONS_END
#include "kernel/init/koption.inc"
#include "kernel/fs/koption.inc"
#include "kernel/debug/koption.inc"


KOPTIONS_START(MAIN, "General kernel configuration", 2)
KOPTION(
	"BUILD_KERNEL",
	"Do build the kernel?",
	"Turn this on if unsure",
	KOPT_BOOL,
	'Y'
),
KOPTION(
	"VERSION_APPEND",
	"Custom version suffix",
	"Append a custom string to the kernel version.",
	KOPT_STRING,
	"-ash"
),
KOPTIONS_END
#include "kernel/init/koption.inc"


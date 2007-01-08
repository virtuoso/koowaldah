KOPTIONS_START(MAIN, "General kernel configuration", 7)
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
KOPTION(
	"GLOBAL_DEBUG",
	"Turn on debugging output",
	"This option enables debugging outout in the "
	"entire kernel.",
	KOPT_BOOL,
	'N'
),
KOPTION(
	"WERROR",
	"Issue error for each warning",
	"This is maintainer's option, it enables -Werror "
	"to the C compiler.",
	KOPT_BOOL,
	'N'
),
KOPTION_FULL(
	"CPU_ARCH_I386",
	"Target CPU architecture is i386",
	"Select this if you want to build a kernel for "
	"i386 target.",
	KOPT_BOOL,
	CONFLICTS("CPU_ARCH_POWERPC", "CPU_ARCH_ARM"),
	'Y'
),
KOPTION_FULL(
	"CPU_ARCH_POWERPC",
	"Target CPU architecture is powerpc",
	"Select this if you want to build a kernel for "
	"POWER CPU family target.",
	KOPT_BOOL,
	CONFLICTS("CPU_ARCH_I386", "CPU_ARCH_ARM"),
	'Y'
),
KOPTION_FULL(
	"CPU_ARCH_ARM",
	"Target CPU architecture is ARM",
	"Select this if you want to build a kernel for "
	"ARM CPU family target.",
	KOPT_BOOL,
	CONFLICTS("CPU_ARCH_I386", "CPU_ARCH_POWERPC"),
	'Y'
),
KOPTIONS_END
#include "kernel/koption.inc"


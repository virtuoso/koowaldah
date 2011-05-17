KOPTIONS_START(FS, "FS layer configuration", 2)
KOPTION(
	"DEBUG_CPIO",
	"Debug cpio driver",
	"Turns on debug output of what is there on the rootfs.",
	KOPT_BOOL,
	'N'
),
KOPTION_FULL(
	"AOUT_SUPPORT",
	"Support a.out executable format",
	"Only non-relocatable omagic aouts are supported.",
	KOPT_BOOL,
	CONFLICTS("CPU_ARCH_DUMMY"),
	'N'
),
KOPTIONS_END


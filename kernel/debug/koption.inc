KOPTIONS_START(DEBUG, "Kernel debugging options", 1)
KOPTION(
	"INSTRUMENT_PROFILER",
	"Simple static instrumentation profiler",
	"Makes use of gcc's instrument functions to keep "
	"track of what's going on in the kernel.",
	KOPT_BOOL,
	'N'
),
KOPTIONS_END


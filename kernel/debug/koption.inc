KOPTIONS_START(DEBUG, "Kernel debugging options", 3)
KOPTION(
	"INSTRUMENT_PROFILER",
	"Simple static instrumentation profiler",
	"Makes use of gcc's instrument functions to keep "
	"track of what's going on in the kernel.",
	KOPT_BOOL,
	'N'
),
KOPTION(
	"EVENTLOG_SIZE",
	"Size of profiler's event log",
	"This is the length of profiler's internal event "
	"log buffer. Change this to a bigger value if you "
	"want to trace longer codepaths. Shortening this "
	"value hardly ever makes sence",
	KOPT_INT,
	2048
),
KOPTION(
	"DEBUG_SPINLOCKS",
	"Check spin locks and associated counters",
	"",
	KOPT_BOOL,
	'Y'
),
KOPTIONS_END


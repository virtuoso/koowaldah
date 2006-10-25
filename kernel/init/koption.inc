KOPTIONS_START(KERNEL_INIT, "Initialization options", 10)
KOPTION(
	"TEST_IRQS",
	"Run irq serving test",
	"This simple test installs a keyboard irq handler "
	"and does various simple things.",
	KOPT_BOOL,
	'N'
       ),
KOPTION(
	"TEST_TIMER",
	"Run core timers test",
	"Install an alarm function which prints a message "
	"after a certain amount of ticks.",
	KOPT_BOOL,
	'N'
       ),
KOPTION(
	"TEST_THREADS",
	"Run a couple of concurrent threads",
	"Install two threads printing 'A' and 'B' with a delay.",
	KOPT_BOOL,
	'N'
       ),
KOPTION(
	"TEST_KLIST",
	"Run klist functionality tests",
	"Test if klist.c strill works.",
	KOPT_BOOL,
	'N'
       ),
KOPTION(
	"PAGE_ALLOCATOR_TEST",
	"Run page allocator tests",
	"Test page allocator and produce a lot of semiinteresting output.",
	KOPT_BOOL,
	'N'
       ),
KOPTION(
	"TEST_PCKBD",
	"Test pc keyboard driver",
	"Install a thread which reads keyboard input via "
	"pc-kbd driver and prints it right away.",
	KOPT_BOOL,
	'N'
       ),
KOPTION(
	"TEST_FSLOOKUP",
	"Test FS path lookup facility",
	"Try to find /dev/console on the rootfs.",
	KOPT_BOOL,
	'N'
       ),
KOPTION(
	"TEST_ROOTFS",
	"Test root filesystem",
	"Perform various tests on the rootfs.",
	KOPT_BOOL,
	'N'
       ),
KOPTION(
	"TEST_BUG",
	"Test bug() function",
	"Call bug() in the course of main kernel "
	"thread execution, i.e. kill the kernel.",
	KOPT_BOOL,
	'N'
       ),
KOPTION(
	"TEST_SLICE",
	"Test the slice allocator",
	"",
	KOPT_BOOL,
	'N'
       )
KOPTIONS_END

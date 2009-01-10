KOPTIONS_START(DRIVERS, "In-kernel drivers", 2)
KOPTION(
	"SERIAL_8250",
	"8250 serial driver",
	"Turn this on for i386 architecture.",
	KOPT_BOOL,
	'N'
),
KOPTION(
	"PCKBD",
	"PC keyboard driver",
	"Turn this on for i386 architecture.",
	KOPT_BOOL,
	'N'
),
KOPTIONS_END


KOPTIONS_START(ARCH_I386, "i386 architecture configuration", 2)
KOPTION(
	"EARLY_SERIAL_CONSOLE",
	"Initialize a serial consoe for early debug messages.",
	"",
	KOPT_BOOL,
	'Y'
)
,
KOPTION(
	"EARLY_VGA_CONSOLE",
	"Initialize a vga consoe for early debug messages.",
	"",
	KOPT_BOOL,
	'Y'
),
KOPTIONS_END

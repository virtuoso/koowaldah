KOPTIONS_START(ARCH_DUMMY, "Dummy architecture configuration", 3)
KOPTION(
        "UCONTEXT_SIZE",
        "ucontext_t size",
        "By default, the build system will attempt to guess "
        "sizeof(ucontext_t) by compiling and executing a native binary. "
	"Setting this to non-zero will omit the automated guess and use "
	"specified value instead. Set to zero if unsure.",
        KOPT_INT,
        0
),
KOPTION(
        "PAGE_SHIFT",
        "Order of desired page size",
        "By default, the build system will attempt to guess "
        "PAGE_SIZE constant by compiling and executing a native binary. "
	"Setting this to non-zero will omit the automated guess and use "
	"specified value instead. Set to zero if unsure.",
        KOPT_INT,
        0
),
KOPTION(
        "DUMMY_USE_DLOPEN",
        "Use host libdl for userspace",
        "The easiest way to have userspace in dummy is to use host libdl.",
        KOPT_BOOL,
        'Y'
),
KOPTIONS_END

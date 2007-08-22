KOPTIONS_START(ARCH, "Dummy architecture configuration", 2)
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
KOPTIONS_END

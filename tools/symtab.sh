#!/bin/sh
#
# tools/symtab.sh
#
# Copyright (C) 2007 Alexander Shishkin
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. Neither the name of the Koowaldah developers nor the names of their
#    contributors may be used to endorse or promote products derived from
#    this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE DEVELOPERS AND CONTRIBUTORS ``AS IS'' AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE DEVELOPERS OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
# OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
# OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.

# This script generates a list of all in-kernel function symbols
# so that we can better decipher the stack traces.

usage() {
	echo "Usage: $0 <input file> <output file>"
	echo "  <input file>  must be elf with nitty functions inside"
	echo "  <output file> is where to write the resulting C code."
	exit 1
}

ELFFILE="$1"
if test -z "$ELFFILE"; then
	usage
fi

OUTFILE="$2"
if test -z "$OUTFILE"; then
	usage
fi

objdump -t "$ELFFILE" | sort -k1 | gawk -v myname="$0" '
	BEGIN {
		printf "/*\n * This file is automatically generated by %s,\n", myname;
		printf " * do not edit.\n */\n\n";
		printf "#define __GENERIC_SYMTAB__\n";
		printf "#ifndef LO_CORE\n";
		printf "#include <symtab.h>\n\n";
		printf "struct symbol symbol_table[] = {\n";
		start_addr = 0;
		end_addr = 0;
		symcount = 0;
		kos_entry = 0;
	}
	END {
		printf "\n};\n\n";
		printf "unsigned long start_addr = 0x%08x;\n", start_addr;
		printf "unsigned long end_addr   = 0x%08x;\n", end_addr;
		printf "unsigned long total_symbols = %d;\n\n", symcount;
		printf "#endif /* LO_CORE */\n\n";
		printf "#define KOS_ENTRY 0x%08x\n\n", kos_entry
	}
	/ F / {
		if ($1 != 0 && $5 != 0) {
			addr = strtonum(sprintf("0x%s", $1));
			size = strtonum(sprintf("0x%s", $5));
			section = $4;
			name = $6;

			start_addr = start_addr ? start_addr : addr;
			end_addr = (addr + size);
			symcount++;

			printf "\t{\n\t\t.addr = 0x%08x,\n", addr;
			printf "\t\t.size = 0x%08x,\n", size;
			printf "\t\t.section = \"%s\",\n", section;
			printf "\t\t.name = \"%s\"\n\t},\n", name;

			if (name == "multiboot_start")
				kos_entry = addr;
		}
	}
' > "$OUTFILE"


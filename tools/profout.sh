#!/bin/sh

if [ -z "$1" ]; then
	echo "expected text file input"
	exit 1
fi

IN="$1"
CWD=`pwd`/

echo "# $IN $CWD"
grep '#KM:.*enters' $IN | while read line; do
	time=`echo $line|cut -d' ' -f1|cut -d: -f2`
	entry=`echo $line|cut -d' ' -f2`
	func=`echo $line|cut -d' ' -f4`
	lentry=`addr2line -e kos-elf $entry|sed -e "s,$CWD,,"`
	lfunc=`addr2line -e kos-elf $func|sed -e "s,$CWD,,"`
	echo "$time: $lentry [$entry] => $lfunc [$func]"
done

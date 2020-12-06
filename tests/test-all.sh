#!/usr/bin/env bash

error=0

for program_test in */test.sh; do
	(cd "$(dirname "$program_test")" && ./"$(basename "$program_test")")
	result=$?
	if [ $result -eq 0 ]; then
		printf "Test %s successful\n" "$(dirname "$program_test")"
	fi
	[ $result -eq 0 ] || error=1
done

exit $error

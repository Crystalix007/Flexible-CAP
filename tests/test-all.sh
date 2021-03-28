#!/usr/bin/env bash

error=0

for program_test in */test.sh; do
	test_output="$(cd "$(dirname "$program_test")" && ./"$(basename "$program_test" 2>&1)")"
	result=$?
	if [ $result -eq 0 ]; then
		printf "Test %s successful\n" "$(dirname "$program_test")"
	else
		printf "Test %s failed:\n%s\n" "$(dirname "$program_test")" "$(awk '{ printf "  %s\n", $0 }' <<<"${test_output}")"
	fi
	[ $result -eq 0 ] || error=1
done

exit $error

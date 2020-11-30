#!/usr/bin/env bash

CXX=${CXX:-clang++}
GENERATOR=${GENERATOR:-../../../generator/fcap}

argspec() {
	(mkdir _build && cd _build && ${GENERATOR} ../program.argspec)
}

cleanup() {
	rm -rf _build
}

compile() {
	for parser in *.yy; do
		bison $parser 2>/dev/null
	done

	for lexer in *.ll; do
		flex $lexer 2>/dev/null
	done

	$CXX -std=c++17 -pedantic *.cc *.cpp -o program
	return $?
}

setup() {
	argspec
	res=$?
	if [ $res -ne 0 ]; then
		printf "Failed to parse argspec\n"
		return $res
	fi

	cp main.cpp _build/

	(cd _build && compile)
	res=$?
	if [ $res -ne 0 ]; then
		printf "Failed to compile argspec\n"
		return $res
	fi
}

run_tests() {
	for _test in testcase/*.json; do
		test_output="$(jq -r '.output' $_test)"
		test_arguments=$(jq -r '.args // ""' $_test)
		run_output="$(jq -r ".input" $_test | ./_build/program $test_arguments)"
		res=$?

		if [ $res -ne 0 ]; then
			printf "Failed to run test: %s, error code: %d\n" "$_test" "$res"
			return $res
		fi

		if [ "$test_output" != "$run_output" ]; then
			printf "Failed test %s (bad output):\n" "$_test"
			diff -W 80 <(printf "%s\n" "$test_output") <(printf "%s\n" "$run_output")
			return 1
		fi
	done

	return 0
}

if setup; then
	run_tests
	result=$?
else
	result=1
fi

cleanup

exit $result

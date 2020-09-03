#!/usr/bin/env sh

TEMPLATE_FILES=($3/*)
OUTPUT_FILES=()

for file in ${TEMPLATE_FILES[@]}; do
	OUTPUT_FILES+=("$1/$2$(basename $file)")
done

printf ': &(ARGSPEC) | $(BASE_DIR)/$(PROG) |> cd $(BASE_DIR) && ./$(PROG) ../&(ARGSPEC) |> %s {CLI}\n' "${OUTPUT_FILES[*]}"


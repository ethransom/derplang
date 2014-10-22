#!/bin/bash

for file in examples/*.cream; do
	expected="examples/$(basename $file).out"
	if test -e $expected; then
		if (./main $file | diff - $expected); then
			echo "[SUCCESS] $file"
		else
			echo "[FAILURE] $file did not match $expected"
			exit 1
		fi
	else
		echo "[SKIP] no out file for $file"
	fi
done

echo "All Tests Succeeded!"
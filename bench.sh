#!/bin/bash

for i in {1..100}; do
	perf stat $@ 2>&1 >/dev/null | grep 'task-clock' | grep -v "stalled" | awk '{print $1;}'
done


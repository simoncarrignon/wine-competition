#!/bin/sh

for dir in $1/agent_*/run_*
do
	echo $dir;
	mkdir -p $dir/csv;
	./analysis.bin $dir/output/model.h5 $dir/csv > /dev/null;
done

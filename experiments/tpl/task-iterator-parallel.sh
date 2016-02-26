#!/bin/bash

set -e

for ((i=1;i<=${nlines};i++)); do
		while [ `jobs | wc -l` -ge ${ncores} ] # check the number of instance currently running
		do
			jobs >/dev/null
			sleep 5
		done

    ${runtask} $$i &
		sleep 1
done

while [ `jobs | wc -l` -ge ${ncores} ] # wait that all jobs are finished
do
	jobs >/dev/null
	sleep 5
done

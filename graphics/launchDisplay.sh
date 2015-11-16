#!/bin/sh

for dir in $1"/consumption_"*
do
	base=`basename $dir`
	echo $dir
	echo $base
	./displayNumAgents.R $dir $1"/numAgents-"$base".eps" > /dev/null
	./displayResources.R $dir $1"/resources-"$base".eps" > /dev/null
done

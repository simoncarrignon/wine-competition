#!/bin/sh

for dir in $1"/agent_"*
do
	base=`basename $dir`
	echo $base
	type=${base#agent_}
	#type=${type%.autocorrelation*}
	type=${type%consumption*}
	echo $type
	./displayNumAgents.R $dir $type $dir"/numAgents"$type".eps" > /dev/null
	./displayResources.R $dir $type $dir"/resources"$type".eps" > /dev/null
done

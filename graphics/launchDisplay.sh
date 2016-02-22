#!/bin/sh

for dir in $1"/agent_"*
do
	base=`basename $dir`
	echo $base
	ending=${base#agent_}
	#type=${type%.autocorrelation*}
	type=${ending%__alpha*}
	echo $type
	./displayNumAgents.R $dir $type $1"/numAgents"$ending".eps" > /dev/null
	./displayResources.R $dir $type $1"/resources"$ending".eps" > /dev/null
done

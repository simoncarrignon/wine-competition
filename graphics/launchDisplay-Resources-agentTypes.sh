#!/bin/sh

autocorrelation=10
consumption=2
map=1

#for consumption in 2 3 
#do
#	for autocorrelation in 10 
#	do
#		for map in 1 2 3 4 5
#		do
#			echo $consumption" "$autocorrelation" "$map
#			./displayResources-agentTypes.R 2 $1"/agent_random__autocorrelation_"$autocorrelation"__consumption_"$consumption"__map_"$map"__run_10__" random $1"/agent_sarsa__autocorrelation_"$autocorrelation"__consumption_"$consumption"__map_"$map"__run_10__" sarsa $1"/resources_autocorrelation_"$autocorrelation"_consumption_"$consumption"_map_"$map".eps"
#		done
#	done
#done


	for proba in `seq 1 9`
	do
		for map in 1 2
		do
			echo $proba" "$map
			./displayResources-agentTypes.R 3 $1"/agent_random__mapInst_"$map"__probaObs_"$proba"__" random $1"/agent_sarsa__mapInst_"$map"__probaObs_"$proba"__" sarsa $1"/agent_evo__mapInst_"$map"__probaObs_"$proba"__" evo $1"/compare_mapInst_"$map"_probaObs_"$proba".eps"
		done
	done

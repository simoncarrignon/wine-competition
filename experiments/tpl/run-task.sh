#!/bin/bash

SIMULPAST_DIR=${simulpast_dir}
SIMULPAST_BIN=${simulpast_bin}


TASK=$${1:-$$SGE_TASK_ID}
case $${TASK} in

${task_options}
*) echo error in SGE_TASK_ID:$${TASK} ;;

esac

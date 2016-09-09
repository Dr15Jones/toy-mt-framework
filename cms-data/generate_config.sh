#!/bin/bash

declare -i stepsize
stepsize=64
declare -i nsteps
nsteps=20
declare -i iterations
iterations=1000
declare -i ncore
ncore=4
declare -i nSimulEvt
nSimulEvt=ncore
declare -i nThreads
nThreads=64
declare -i busyWaitScaleFactor
busyWaitScaleFactor=2080000

for ((x=1;x<=${nsteps};x+=1)); do
     echo $x
     sed -e "s/\$iterations/$iterations/" -e "s/\$nSimulEvt/$nSimulEvt/" -e "s/\$nThreads/$nThreads/" -e "s/\$busyWaitScaleFactor/$busyWaitScaleFactor/"  reco_hipileup_5_2_0_busywait_perfectIO.config.tt >reco_hipileup_5_2_0_busywait_perfectIO.${nSimulEvt}s.${nThreads}t.${iterations}i.config

    iterations+=${step}
    nSimulEvt+=${ncore}
    nThreads+=${ncore}
done

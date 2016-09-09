#!/bin/bash

declare -i stepsize
stepsize=64
declare -i nsteps
nsteps=256
declare -i iterations
iterations=1000
declare -i ncore
ncore=8
declare -i nSimulEvt
nSimulEvt=0
declare -i nThreads
nThreads=0
declare -i busyWaitScaleFactor
busyWaitScaleFactor=9700000

echo "#!/bin/sh" >driver.sh



for ((x=8;x<=256;x+=8)); do
    nSimulEvt=${x}
    nSimulEvtP=`printf "%03d\n" $nSimulEvt`
    echo $nSimulEvtP
    scale=1
    nThreads=`echo "($scale * $nSimulEvt)/1" | bc`
    nThreadsP=`printf "%03d\n" $nThreads`
    echo $nThreadsP
    echo qsub -q knl -l nodes=1:knl,walltime=12:00:00 -A usertest ~/toy-mt-framework/cms-data/driver-${nSimulEvtP}s-${nThreadsP}t.sh >>driver.sh

    echo "#!/bin/sh" >driver-${nSimulEvt}s-${nThreadsP}t.sh
    echo source /opt/intel/parallel_studio_xe_2016.3.067/psxevars.sh intel64 >>driver-${nSimulEvtP}s-${nThreadsP}t.sh
    echo ~/build/BuildProducts/bin/TBBDemo ~/toy-mt-framework/cms-data/reco_hipileup_5_2_0_busywait_perfectIO.${nSimulEvtP}s.${nThreadsP}t.${iterations}i.config \> job-${iterations}i-${nSimulEvtP}s-${nThreadsP}t.log.txt 2\>\&1  >>driver-${nSimulEvtP}s-${nThreadsP}t.sh

#    echo "#!/bin/sh" >driver-$x.sh
#    echo ids[$x]=\$\! >>driver-$x.sh
#    cat >> driver-$x.sh << EOF
#tim=0
#live=1
#while [ \$live -eq 1 ]; do
#   let tim=\$tim+100
#   live=0
#   sleep 100
#   for ((x=1;x<=$y;x+=1)); do
#      if ps -p \${ids[\$x]}> /dev/null 2>&1
#      then
#         if [ \$tim -gt 720000 ] ; then
#            kill \${ids[\$x]}
#            echo "APPLICATION RAN OUT OF TIME"
#         else
#            live=1
#         fi
#      fi
#   done
#done
#EOF

     sed -e "s/\$iterations/$iterations/" -e "s/\$nSimulEvt/$nSimulEvt/" -e "s/\$nThreads/$nThreads/" -e "s/\$busyWaitScaleFactor/$busyWaitScaleFactor/"  reco_hipileup_5_2_0_busywait_perfectIO.config.tt >reco_hipileup_5_2_0_busywait_perfectIO.${nSimulEvtP}s.${nThreadsP}t.${iterations}i.config

done

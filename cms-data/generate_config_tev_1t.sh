#!/bin/bash

declare -i stepsize
stepsize=64
declare -i nsteps
nsteps=256
declare -i iterations
iterations=100
declare -i ncore
ncore=8
declare -i nSimulEvt
nSimulEvt=1
declare -i nThreads
nThreads=1
declare -i busyWaitScaleFactor
busyWaitScaleFactor=9700000

sed -e "s/\$iterations/$iterations/" -e "s/\$nSimulEvt/$nSimulEvt/" -e "s/\$nThreads/$nThreads/" -e "s/\$busyWaitScaleFactor/$busyWaitScaleFactor/"  reco_hipileup_5_2_0_busywait_perfectIO.config.tt >reco_hipileup_5_2_0_busywait_perfectIO.${nSimulEvtP}s.${nThreadsP}t.${iterations}i.config

echo "#!/bin/sh" >driver.sh
for ((x=8;x<=256;x+=8)); do
    echo $x
    echo qsub -q knl -l nodes=1:knl,walltime=12:00:00 -A usertest ~/toy-mt-framework/cms-data/driver-${x}.sh >>driver.sh
    
    echo "#!/bin/sh" >driver-${x}.sh
    echo source /opt/intel/parallel_studio_xe_2016.3.067/psxevars.sh intel64 >>driver-${x}.sh

for ((y=1;y<=$x;y+=1));do
    echo ~/build/BuildProducts/bin/TBBDemo ~/toy-mt-framework/cms-data/reco_hipileup_5_2_0_busywait_perfectIO.${nSimulEvt}s.${nThreads}t.${iterations}i.config \> job-${x}-${y}.log.txt 2\>\&1\&  >>driver-$x.sh
    echo ids[$x]=\$\! >>driver-$x.sh
done
    cat >> driver-$x.sh << EOF
tim=0
live=1
while [ \$live -eq 1 ]; do
   let tim=\$tim+100
   live=0
   sleep 100
   for ((x=1;x<$y;x+=1)); do
      if ps -p \${ids[\$x]}> /dev/null 2>&1
      then
         if [ \$tim -gt 720000 ] ; then
            kill \${ids[\$x]}
            echo "APPLICATION RAN OUT OF TIME"
         else
            live=1
         fi
      fi
   done
done
EOF


done

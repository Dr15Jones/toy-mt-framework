#!/bin/bash

declare -i iterations
iterations=200
declare -i nStreams
nStreams=1
declare -i nThreads
nThreads=1
declare -i busyWaitScaleFactor
busyWaitScaleFactor=9700000
declare -i nProc
nProc=16
nStreams=256/${nProc}
nProc=${nProc}-4
for ((z=${nStreams};z<=${nStreams}+7;z+=1));do
   nThreads=$z
   sed -e "s/\$iterations/$iterations/" -e "s/\$nSimulEvt/$nStreams/" -e "s/\$nThreads/$nThreads/" -e "s/\$busyWaitScaleFactor/$busyWaitScaleFactor/"  reco_hipileup_5_2_0_busywait_perfectIO.config.tt >reco_hipileup_5_2_0_busywait_perfectIO.${nStreams}s.${nThreads}t.${iterations}i.config
done

echo "#!/bin/sh" >driver.sh

for ((x=${nStreams};x<=${nStreams}+7;x+=1)); do
    nThreads=$x
    echo qsub -q knl -l nodes=1:knl,walltime=12:00:00 -A usertest ~/toy-mt-framework/cms-data/driver-${nThreads}t-${nStreams}s-${nProc}p.sh >>driver.sh
    echo "#!/bin/sh" >driver-${nThreads}t-${nStreams}s-${nProc}p.sh
    echo source /opt/intel/parallel_studio_xe_2016.3.067/psxevars.sh intel64 >>driver-${nThreads}t-${nStreams}s-${nProc}p.sh

for ((y=1;y<=${nProc};y+=1));do
    echo ~/build/BuildProducts/bin/TBBDemo ~/toy-mt-framework/cms-data/reco_hipileup_5_2_0_busywait_perfectIO.${nStreams}s.${nThreads}t.${iterations}i.config \> job-${nThreads}t-${nStreams}s-${nProc}p-$y.log.txt 2\>\&1\&  >>driver-${nThreads}t-${nStreams}s-${nProc}p.sh
    echo ids[$y]=\$\! >>driver-${nThreads}t-${nStreams}s-${nProc}p.sh
done
    cat >> driver-${nThreads}t-${nStreams}s-${nProc}p.sh << EOF
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

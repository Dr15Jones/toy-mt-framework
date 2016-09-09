#!/bin/bash

declare -i stepsize
stepsize=64
declare -i nsteps
nsteps=32
declare -i iterations
iterations=100
declare -i ncore
ncore=64
declare -i nSimulEvt
nSimulEvt=1
declare -i nThreads
nThreads=1
#nThreads=64
busyWaitScaleFactor=4.23729e+08


nSimulEvtP=`printf "%02d" $nSimulEvt`
nThreadsP=`printf "%02d" $nThreads`

sed -e "s/\$iterations/$iterations/" -e "s/\$nSimulEvt/$nSimulEvt/" -e "s/\$nThreads/$nThreads/" -e "s/\$busyWaitScaleFactor/$busyWaitScaleFactor/"  reco_hipileup_5_2_0_busywait_perfectIO.config.tt >reco_hipileup_5_2_0_busywait_perfectIO.${nSimulEvtP}s.${nThreadsP}t.${iterations}i.config

echo "#" >srun-driver.sh
for ((x=2;x<=64;x+=2)); do
   echo "sbatch -N1 srun-driver-${x}.sh" >>srun-driver.sh
   echo "#!/bin/bash -l     "  >srun-driver-${x}.sh  
   echo "                   " >>srun-driver-${x}.sh
   echo "#SBATCH -p regular " >>srun-driver-${x}.sh   
   echo "#SBATCH -N 1      "  >>srun-driver-${x}.sh   
   echo "#SBATCH -t 24:00:00" >>srun-driver-${x}.sh  
   echo "#SBATCH -J my_job  " >>srun-driver-${x}.sh   
   echo "#SBATCH -L SCRATCH " >>srun-driver-${x}.sh
   for ((y=1;y<=${x};y+=1));do
    echo srun -n1 -c1 /global/homes/g/gartung/build/BuildProducts/bin/TBBDemo /global/homes/g/gartung/toy-mt-framework/cms-data/reco_hipileup_5_2_0_busywait_perfectIO.${nSimulEvtP}s.${nThreadsP}t.${iterations}i.config\& >>srun-driver-$x.sh
    done
echo "wait">>srun-driver-${x}.sh
done

import os
import sys
import subprocess

#find all config files
configs = []
for f in os.listdir(os.getcwd()):
    if 'config' == f.split(".")[-1]:
        configs.append(f)

executablePath = sys.argv[1]

#run the jobs
for c in configs:
    print 'config ',c
    ompFileName ="openmp_"+c.split('.')[0]+".log" 
    outF = open(ompFileName,'w')
    print '  starting openmp'
    subprocess.call([executablePath+'/OpenMPProcessingDemo/OpenMPDemo', c], stdout=outF)
    outF.close()
    subprocess.call(['tail','-1',ompFileName])
    
    tbbFileName = "tbb_"+c.split('.')[0]+".log" 
    outF = open(tbbFileName,'w')
    print 'starting tbb'
    subprocess.call([executablePath+'/TBBProcessingDemo/TBBDemo', c], stdout=outF)
    outF.close()
    subprocess.call(['tail','-1',tbbFileName])


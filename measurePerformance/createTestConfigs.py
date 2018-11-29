import json
import sys
import os

configFile = file(sys.argv[1],"r")

configTemplate = json.load(configFile)

testsFile = file(sys.argv[2],'r')
testParams = json.load(testsFile)

process = configTemplate['process']

fileName = sys.argv[1].split(os.sep)[-1]
fileNameParts = fileName.split(".")

if 'busyWaitScaleFactor' in testParams:
    process['options']['busyWaitScaleFactor'] = testParams['busyWaitScaleFactor']

for nThreads,nStreams,nEvents in testParams['tests']:
    process['options']["nThreads"]=nThreads
    process['options']["nSimultaneousEvents"]=nStreams
    process['source']["iterations"] = nEvents
    outF = file(fileNameParts[0]+"_"+str(nThreads)+"_"+str(nStreams)+"_"+str(nEvents)+"."+fileNameParts[1],'w')
    json.dump(configTemplate,outF, indent = 2, sort_keys=True)


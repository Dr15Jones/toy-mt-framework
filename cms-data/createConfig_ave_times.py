#!/usr/bin/env python
import sys

kCPUTimeIndex = 0
kRealTimeIndex = 1
kTimeIndex = kCPUTimeIndex
#indent = " "
    
if len(sys.argv) != 4:
    print "requires three arguments"
    sys.exit(1)

f = file(sys.argv[1])
if not f:
    print "unable to open file %s",sys.argv[1]

fTime = file(sys.argv[2])
if not fTime:
    print "unable to open file %s",sys.argv[2]

fStorage = file(sys.argv[3])
if not fStorage:
    print "unable to open file %s",sys.argv[3]

moduleRelations = dict()
for l in f:
    values = l.split()
    processName = values[0].split('/')[2]
    if processName == 'RECO':
        moduleRelations[values[0]] = values[1:]

moduleTime = dict()
for l in fTime:
    values = l.split()
    if len(values) == 8:
        #print values
        moduleTime[values[-1]] = values[1:-2]
    else:
        print values

productsFromOtherJobs = set()
storageToGet = list()
for l in fStorage:
    values = l.split()
    if(values[1]=="RECO"):
        storageToGet.append({"label":values[0], "product":""})
    else:
        storageToGet.append({"label":values[0]+"/"+values[1], "product":""})
        productsFromOtherJobs.add(values[0]+"/"+values[1])

config = {
 "process" :
 {
   "label" : "TEST",
   "options" :
   {
     "nSimultaneousEvents" : 1,
     "busyWaitScaleFactor" : 2.2e+07
   },
   "source" :
   {
     "@type" : "demo::SimpleSource",
     "iterations" : 100
   },
   "filters" :
   [
     { "@label" : "output",
       "@type" : "demo::BusyWaitPassFilter",
       "threadType" : "ThreadSafeBetweenModules",
       "averageTime": 0.140279,
       "toGet" : storageToGet
     }
   ],
   "producers" :
   [
   ],
   "paths" :
   {
     "o":
      [ "output"]
   }
 }
}

#add producers
producers = config["process"]["producers"]
for mod,dependents in moduleRelations.items():
    pieces = mod.split("/")
    moduleLabel = pieces[1]
    time = moduleTime.get(moduleLabel,-1)[kTimeIndex]
    toGet = list()
    for d in dependents:
        dPieces = d.split("/")
        dLabel = dPieces[1]
        if dPieces[2] != "RECO":
            dLabel = dLabel +"/"+dPieces[2]
            productsFromOtherJobs.add(dLabel)
        toGet.append({"label":dLabel, "product":""})
    c = { "@label" : moduleLabel,
      "@type" : "demo::BusyWaitProducer",
      "threadType" : "ThreadSafeBetweenInstances",
      "averageTime":time,
      "toGet" :toGet
    }
    producers.append(c)
#the products from other processes really come from the source
for p in productsFromOtherJobs:
    c = { "@label" : p,
      "@type" : "demo::BusyWaitProducer",
      "threadType" : "ThreadUnsafe",
      "averageTime":0.,
      "toGet" :[]
    }
    producers.append(c)
    
#have to convert single quotes to double quotes
configString = str(config)
configString = configString.replace("'",'"')
print configString

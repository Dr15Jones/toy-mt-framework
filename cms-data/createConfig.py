#!/usr/bin/env python
import sys

nEvents = 100
    
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

moduleTimes = dict()
for l in fTime:
    values = l.split()
    #print values
    if nEvents < len(values)-1:
        moduleTimes[values[0]] = [ float(x) for x in values[1:nEvents+1]]
    else:
        moduleTimes[values[0]] = [ float(x) for x in values[1:]]

productsFromOtherJobs = set()
storageToGet = list()
for l in fStorage:
    values = l.split()
    if(values[1]=="RECO"):
        storageToGet.append({"label":values[0], "product":""})
    else:
        storageToGet.append({"label":values[0]+"/"+values[1], "product":""})
        productsFromOtherJobs.add(values[0]+"/"+values[1])
#print storageToGet
#exit(0)

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
     "iterations" : nEvents
   },
   "filters" :
   [
     { "@label" : "output",
       "@type" : "demo::EventTimesSleepingPassFilter",
       "threadType" : "ThreadSafeBetweenModules",
       "eventTimes": moduleTimes['RECOoutput'],
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
    time = moduleTimes.get(moduleLabel,[0.])
    toGet = list()
    for d in dependents:
        dPieces = d.split("/")
        dLabel = dPieces[1]
        if dPieces[2] != "RECO":
            dLabel = dLabel +"/"+dPieces[2]
            productsFromOtherJobs.add(dLabel)
        toGet.append({"label":dLabel, "product":""})
    c = { "@label" : moduleLabel,
      "@type" : "demo::EventTimesSleepingProducer",
      "threadType" : "ThreadSafeBetweenInstances",
      "eventTimes":time,
      "toGet" :toGet
    }
    producers.append(c)
#the products from other processes really come from the source
for p in productsFromOtherJobs:
    label = p.split('/')[0]
    label = 'fetch'+label
    #print label
    times = moduleTimes.get(label,[0.])
    c = { "@label" : p,
      "@type" : "demo::EventTimesSleepingProducer",
      "threadType" : "ThreadUnsafe",
      "eventTimes":times,
      "toGet" :[]
    }
    producers.append(c)
#exit(1)
import pprint
import StringIO
output = StringIO.StringIO()
#pp = pprint.PrettyPrinter(indent=3)
pprint.pprint(config,stream=output)

#have to convert single quotes to double quotes
configString = output.getvalue()
configString = configString.replace("'",'"')

print configString


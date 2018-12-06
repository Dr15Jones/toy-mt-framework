#!/bin/env python
import sys
import re 
from collections import defaultdict
import json

if len(sys.argv) != 2:
    print "requires input with module relations from tracer log"
    sys.exit(1)

f = file(sys.argv[1])
if not f:
    print "unable to open file %s",sys.argv[1]

moduleRelations = defaultdict(list)
moduleConsumes = defaultdict(list)
moduleTimings = defaultdict(list)
consumedBy = defaultdict(list)
processName = None
module2space=re.compile("^\s\s(\S*)/'(\S*)'\n")
module4space=re.compile("^\s\s\s\s(\S*)/'(\S*)'\n")
fourspace=re.compile("^\s\s\s\s(\S*)\s'(\S*)'\s'(|\S*)'\s'(|\S*)'")
modules=re.compile("^\s\s(\S*)/'(\S*)' consumes products from these modules:$")
consumes=re.compile("^\s\s(\S*)/'(\S*)' consumes:$")
tickspace=re.compile("' '")
constructing=re.compile("\+\+\+\+ starting: constructing module with label '(\S*)' id = (\S*)")


constructed=set()
for l in f:
    if constructing.match(l):
        labels = re.match(constructing,l).groups()
        constructed.add(labels[0])
    if module4space.match(l):
        labels=re.match(module4space,l).groups()
        if len(labels) > 1:
            if not labels[1] == '':
                moduleRelations[processName].append(labels[1])
#    if module2space.match(l):
#        values=re.match(module2space,l).groups()
#        processName = values[1]
#        moduleRelations[processName]=list()
#        moduleConsumes[processName]=list()
    if modules.match(l):
        values = re.match(modules,l).groups()
        processName = values[1]
        moduleRelations[processName]=list()
    if consumes.match(l):
        values = re.match(consumes,l).groups()
        processName = values[1]
        moduleConsumes[processName]=list()
    if fourspace.match(l):
        fields=re.match(fourspace,l).groups()
        consumedBy[fields[1]].append(processName)
        if not fields[1] == "@EmptyLabel@":
            if fields[-1] == "HLT":
                moduleConsumes[processName].append(fields[1]+"/HLT")
            else:
                moduleConsumes[processName].append(fields[1])

import networkx as nx
C=nx.DiGraph(moduleRelations)
Q=nx.dfs_preorder_nodes(C,"RECOoutput")
consumed=set(Q)

            
with open('module-storage2get.json', 'w') as outfile:
   outfile.write(json.dumps(moduleConsumes, indent=4))

with open('module-relations.json', 'w') as outfile:
   outfile.write(json.dumps(moduleRelations, indent=4))

with open('module-timings.json', 'r') as infile:
    moduleTimings=json.load(infile)


nEvents="100"
eventTimes = moduleTimings['RECOoutput']
storageToWrite = list()
mods=set(moduleConsumes['RECOoutput'])
for mod in mods:
    storageToWrite.append({"label":mod, "product":""})


config = {
 "process" :
 {
   "label" : "TEST",
   "options" :
   {
     "nSimultaneousEvents" : 1,
     "nThreads" : 1,
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
       "@type" : "demo::EventTimesBusyWaitPassFilter",
       "threadType" : "ThreadSafeBetweenModules",
       "eventTimes": eventTimes[50:150],
       "toGet" : storageToWrite
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

timed=set(moduleTimings.keys())
HLTProds=["rawDataCollector", "TriggerResults/HLT", "hltGtStage2ObjectMap/HLT",  "hltTriggerSummaryAOD/HLT"]
for s in HLTProds:
    constructed.add(s)
    timed.add(s)


#add producers
producers = config["process"]["producers"]


for mod in consumed:
   if not mod in constructed:
       print "module with label %s not constructed" % mod
       continue
   if not mod in timed:
       print "module with label %s has no event times" % mod
   eventTimes = moduleTimings.get(mod,[0.0])
   times=[0.0]
   if len(eventTimes)==200:
       times=eventTimes[50:150]
   else:
       print 'module %s is consumed but has no event times' % mod
   toGet = list()
   for d in list(set(moduleConsumes[mod])):
       if d not in constructed:
           print "module label %s dependency label %s not constructed" % (mod,d)
       else:
           toGet.append({"label":d, "product":""})
   c = { "@label" : mod,
     "@type" : "demo::EventTimesBusyWaitProducer",
     "threadType" : "ThreadSafeBetweenInstances",
     "eventTimes": times,
     "toGet" :toGet
   }
   producers.append(c)

#add sources
eventTimes = moduleTimings.get("source/HLT",[0.])
r = { "@label": "rawDataCollector",
      "@type" : "demo::EventTimesBusyWaitProducer",
      "threadType" : "ThreadUnsafe",
       "eventTimes": eventTimes[0:100],
       "toGet" : []
    }
t = { "@label": "TriggerResults/HLT",
      "@type" : "demo::EventTimesBusyWaitProducer",
      "threadType" : "ThreadUnsafe",
       "eventTimes": [0.0]*100,
       "toGet" : []
    }
h = { "@label": "hltGtStage2ObjectMap/HLT",
      "@type" : "demo::EventTimesBusyWaitProducer",
      "threadType" : "ThreadUnsafe",
       "eventTimes": [0.0]*100,
       "toGet" : []
    }
a = { "@label": "hltTriggerSummaryAOD/HLT",
      "@type" : "demo::EventTimesBusyWaitProducer",
      "threadType" : "ThreadUnsafe",
       "eventTimes": [0.0]*100,
       "toGet" : []
    }
producers.append(r)
producers.append(t)
producers.append(h)
producers.append(a)

with open('config.json','w') as outfile:
    outfile.write(json.dumps(config,indent=3))


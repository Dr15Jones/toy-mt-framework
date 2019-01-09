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
module2space=re.compile("^([^']*)/'([^']*)'\n")
module4space=re.compile("^([^']*)/'([^']*)'\n")
consume4space=re.compile("^([^']*)\s'([^']*)'(.*)")
modules=re.compile("^([^'/]*)/'([^']*)' consumes products from these modules:$")
consumes=re.compile("^([^'/]*)/'([^']*)' consumes:$")
constructing=re.compile("\+\+\+\+ starting: constructing module with label '(.*)' id = (\S*)")
fourpluses=re.compile("^\+\+\+\+")

constructed=set()
processName=None
tag=None
for l in f:
    if constructing.match(l):
        labels = re.match(constructing,l).groups()
        constructed.add(labels[0])
        # print labels
        continue
    if fourpluses.match(l):
        continue
    if module4space.match(l):
        labels=re.match(module4space,l).groups()
        # print labels
        if len(labels) > 1:
            if not labels[1] == '':
                moduleRelations[processName].append(labels[1])
        continue
    if modules.match(l):
        values = re.match(modules,l).groups()
        processName = values[1]
        moduleRelations[processName]=list()
        # print 'process %s modules' % processName
        continue
    if consumes.match(l):
        values = re.match(consumes,l).groups()
        processName = values[1]
        # print 'process %s consumes' % processName
        moduleConsumes[processName]=list()
        continue
    if consume4space.match(l):
        fields=re.match(consume4space,l).groups()
        consumedBy[fields[1]].append(processName)
        if not fields[1] == "@EmptyLabel@":
            if fields[-1] == " '' 'HLT'":
                consumed = fields[1]+"/HLT"
                moduleConsumes[processName].append(consumed)
            else:
                moduleConsumes[processName].append(fields[1])
        continue
    # print 'unmatched line "%s" ' % l.rstrip('\n')

import networkx as nx
C=nx.DiGraph(moduleConsumes)
Q=nx.dfs_preorder_nodes(C,"RECOoutput")


            
with open('module-consumes.json', 'w') as outfile:
   outfile.write(json.dumps(moduleConsumes, indent=4))

with open('module-relations.json', 'w') as outfile:
   outfile.write(json.dumps(moduleRelations, indent=4))

with open('module-timings.json', 'r') as infile:
    moduleTimings=json.load(infile)


nEvents="200"
eventTimes = moduleTimings['RECOoutput']

storageToWrite = list()
for mod in moduleConsumes['RECOoutput']:
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
       "eventTimes": eventTimes,
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
#    timed.add(s)


#add producers
producers = config["process"]["producers"]


for mod in sorted(Q):
   if not mod in constructed:
       print "%s not constructed" % mod
       continue
   if not mod in timed:
       print "%s has no event times" % mod
       continue
   eventTimes = moduleTimings.get(mod,[0.0])
   times=[0.0]*200
   if len(eventTimes)==200:
       times=eventTimes
   # else:
   #     print '%s has no event times len' % mod
   toGet = list()
   for d in list(moduleConsumes[mod]):
       if d not in constructed:
           # print "%s not constructed and is a dependency of %s" % (d,mod)
           continue
       toGet.append({"label":d, "product":""})
   c = { "@label" : mod,
     "@type" : "demo::EventTimesBusyWaitProducer",
     "threadType" : "ThreadSafeBetweenInstances",
     "eventTimes": times,
     "toGet" :toGet
   }
   producers.append(c)

#add sources
eventTimes = moduleTimings.get("source/HLT", [0.0])
r = { "@label": "rawDataCollector",
      "@type" : "demo::EventTimesBusyWaitProducer",
      "threadType" : "ThreadUnsafe",
       "eventTimes": eventTimes,
       "toGet" : []
    }
t = { "@label": "TriggerResults/HLT",
      "@type" : "demo::EventTimesBusyWaitProducer",
      "threadType" : "ThreadUnsafe",
       "eventTimes": [0.0],
       "toGet" : []
    }
h = { "@label": "hltGtStage2ObjectMap/HLT",
      "@type" : "demo::EventTimesBusyWaitProducer",
      "threadType" : "ThreadUnsafe",
       "eventTimes": [0.0],
       "toGet" : []
    }
a = { "@label": "hltTriggerSummaryAOD/HLT",
      "@type" : "demo::EventTimesBusyWaitProducer",
      "threadType" : "ThreadUnsafe",
       "eventTimes": [0.0],
       "toGet" : []
    }
producers.append(r)
producers.append(t)
producers.append(h)
producers.append(a)

with open('config.json','w') as outfile:
    outfile.write(json.dumps(config,indent=3))


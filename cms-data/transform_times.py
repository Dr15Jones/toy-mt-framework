import sys

f = open(sys.argv[1])

moduleTiming = dict()

eventN = 0
numEvents = 0

for l in f:
    splitLine = l.split()
    if eventN != int(splitLine[1]):
        numEvents +=1
        if numEvents == 1000:
            break
        eventN = int(splitLine[1])
    moduleTiming.setdefault(splitLine[3],list()).append(splitLine[5]) 

for m,t in moduleTiming.iteritems():
    print m," ".join(t)


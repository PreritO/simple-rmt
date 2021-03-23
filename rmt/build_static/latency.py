#!/usr/bin/python

import sys
import csv
import operator
import itertools
import time

# Function for finding first and last
# occurrence of an elements
def findFirstAndLast(arr, x) :
    first = -1
    last = -1
    for i in range(0, len(arr)) :
        if (x != arr[i]) :
            continue
        if (first == -1) :
            first = i
        last = i
     
    if (first != -1) :
        print( "First Occurrence = ", first,
               " \nLast Occurrence = ", last)
        return [first,last]
    else :
        print("Not Found")

if __name__=="__main__":

    IngressTrace = list(csv.reader(open("IngressTrace.csv"), delimiter=","))
    EgressTrace = list(csv.reader(open("EgressTrace.csv"), delimiter=","))
    sortedEgressTrace = sorted(EgressTrace, key=lambda x: float(x[0]), reverse=False)
    # print "---- Egress Sort----"
    # for line in sortedEgressTrace:
    #    print line
    # print "---- Ingress assumed to be sorted----"
    # for line in IngressTrace:
    #    print line
    #print "---- Latency----"

    avgsum =0
    count =0
    latencyCDFFile = "latencyCDF.txt"
    with open(latencyCDFFile,"w") as fileh:
        for IngressVal,EgressVal in itertools.izip(IngressTrace,sortedEgressTrace):
            #print IngressVal + EgressVal
            diff = float(EgressVal[1])-float(IngressVal[1])
            avgsum = avgsum + diff
            count = count + 1
            fileh.write(str(IngressVal[0]) + ": " + str(diff) + "\n")

    avg_latency = avgsum/count


    # print EgressTrace
    sortedEgressTimeTrace = sorted(EgressTrace, key=lambda x: float(x[1]), reverse=False)
    avg_thrp =len(sortedEgressTimeTrace) / (float(sortedEgressTimeTrace[-1][1]) - float(sortedEgressTimeTrace[0][1]))



   # print "Latency "+str(avg_latency)
   # print "Throupt "+str(avg_thrp)
    runname = ""
    if sys.argv[1:]:
        runname = str(sys.argv[1])
    else:
        runname = "Simualtion Run@ "+str(time.asctime( time.localtime(time.time()) ))
    avgfile = "avg.txt"
    with open(avgfile,"a") as avgfh :
        avgfh.write(str(runname)+" ,")
        avgfh.write(str(avg_latency))
        avgfh.write(", ")
        avgfh.write(str(avg_thrp))
        avgfh.write('\n')
    print "End-to-End Latency:    ", avg_latency
    print "Throughput: ", avg_thrp

    NVMTrace = list(csv.reader(open("NVMLookup.csv"), delimiter=","))
    sortedNVMTrace = sorted(NVMTrace, key=lambda x: float(x[0]), reverse=False)
    avg_nvm_latency = 0
    
    pktId = 0
    for i, line in enumerate(sortedNVMTrace):
        startTime = sortedNVMTrace[i][1]
        while line[0] == pktId:
            i+=1
        endTime = sortedNVMTrace[i][2]
        pktId +=1
        avg_nvm_latency += float(endTime) - float(startTime)
            
    avg_nvm = avg_nvm_latency/pktId
    print "Average Lookup Latency: ", avg_nvm

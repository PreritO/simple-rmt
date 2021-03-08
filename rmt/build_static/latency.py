#!/usr/bin/python

import sys
import csv
import operator
import itertools
import time

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

    dramTrace = list(csv.reader(open("DRAMLookup.csv"), delimiter=","))
    avg_dram_latency = 0
    total_pkt = 0
    for line in dramTrace:
        avg_dram_latency += float(line[2])-float(line[1])
        total_pkt+=1

    avg_dram = avg_dram_latency/total_pkt
    print "Average Lookup Latency: ", avg_dram

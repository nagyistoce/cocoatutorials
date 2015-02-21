#!/bin/bash

miles=$(expr $(for i in $(cat $0 | grep '\d:' | cut -d: -f 2); do echo -n $i '+ ' ; done ; echo 0 ))
trips=$(cat $0 | grep '\d:' | wc -l)
average=$(( miles / trips))

echo trips: $trips miles: $miles average: $average

exit 0

echo <<HERE

Sabbatical    2003: 2500
Montana       2004: 2000
Mississippi   2005: 2000
B'm Marathon  2006: 2000
Dakotas       2006: 5000
Grand Canyon  2006: 1500
Mid-West      2007: 2500
Dermers       2008: 2600
Oregon        2008: 3000
Yellowstone   2009: 4000
Poje's GC     2010: 2000
Zion          2010: 3000
Washington    2011: 2500
50 States     2011: 2500
NY State      2012: 1500
Great Trip    2013: 7500
Montana Xmas  2013: 2500
Retirement    2014: 3500
AZ            2014: 3500
Texas         2015: 3600

CA Christmas Trips
Joshua Tree   2008: 1500
Channel Isles 2009: 1500
QM+Catalina   2010: 1500
Palm Springs  2012: 1500

CA Trips
Kings Canyon  2005: 1000
Lassen        2010: 1000
Eureka        2004: 1000

HERE

# That's all Folks!
##
m
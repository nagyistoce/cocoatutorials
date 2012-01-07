#!/bin/bash

sudo /usr/sbin/systemsetup -getsleep
ls -alt /Library/LaunchDaemons/ | grep macbe
ls -alt /usr/sbin               | grep 'setSys'

# That's all Folks
##

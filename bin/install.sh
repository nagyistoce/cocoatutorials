#!/bin/bash

sudo cp com.apple.macbe.setSystemSleep.plist /Library/LaunchDaemons/
sudo cp setSystemSetup                       /usr/sbin/setSystemSleep  
sudo chmod +x                                /usr/sbin/setSystemSleep

echo OK
./verify.sh

# That's all Folks
##

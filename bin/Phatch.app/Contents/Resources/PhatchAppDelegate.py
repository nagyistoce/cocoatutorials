#
#  PhatchAppDelegate.py
#  Phatch
#
#  Created by Robin Mills on 8/26/09.
#  Copyright __MyCompanyName__ 2009. All rights reserved.
#

from Foundation import *
from AppKit import *

class PhatchAppDelegate(NSObject):
    def applicationDidFinishLaunching_(self, sender):
        NSLog("Application did finish launching.")

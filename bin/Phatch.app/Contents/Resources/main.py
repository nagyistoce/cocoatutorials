#
#  main.py
#  Phatch
#
#  Created by Robin Mills on 8/26/09.
#  Copyright __MyCompanyName__ 2009. All rights reserved.
#

#import modules required by application
import objc
import Foundation
import AppKit

from PyObjCTools import AppHelper

# import modules containing classes required to start application and load MainMenu.nib
import PhatchAppDelegate

# pass control to AppKit
AppHelper.runEventLoop()

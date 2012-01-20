#!/usr/bin/python

# http://toys.jacobian.org/presentations/2005/appscript/

##################################
# This isn't working 
# tabs = windows fails and I don't know enough AppleScript to fix it
##################################

from appscript import *

sys_prefs = app('System Preferences.app')
sys_prefs.activate()
sys_prefs.current_pane.set(sys_prefs.panes['com.apple.preferences.users'])

ui = app('System Events.app')
process = ui.processes['System Preferences']
tabs = windows['Accounts'].tab_groups[1]
tabs.radio_buttons[1].click()

print tabs.text_fields[1].value.get()

# That's all Folks!
##

ReadMe.txt for MenuBar/WhatIsMyIP.app
-------------------------------------

This archive contains a built program and all the source code.

The application is "WhatIsMyIP.app" and it will install a preference panel
"WhatIsMyIPprefs.prefPane".  The pref panel also includes an Apple
Sample Code Utility: LoginItemsAETest.app

The Preference Panel is Universal (x86_64, i386 ppc7400)
The Applications are 32 bit Universal (ppc i386)
(the applications require some carbon 32 bit apis)

Install and Uninstall
---------------------
Double-click the program "Double Click to Install.app".
- WhatIsMyIP.app installs into ~/Applications
- You don't need to copy it to your /Applications directory.
- You don't need administrator privileges.

To uninstall, use the System Preferences Application WhatIsMyIP Preference Panel.  Push "Uninstall"

Configuration
-------------

You configure the program with the file:
~/Library/Preferences/WhatIsMyIP.json

What's to be done
-----------------
I haven't finished the Tutorial Document MenuBar.pdf
   
What I would like
-----------------

Bug reports, suggestions, help - anything you'd like to contribute.

Why am I doing this?
--------------------

I've been writing some tutorials about Cocoa and this seemed interesting and useful.
I'm a moderator on the forum WhatIsMyIP.com and we thought this might be useful
to the Forum readers and our web site users.

Ideas for the future
--------------------

We could add all sorts of "magic" to the JSON file.  For example:

Title    : "What you want to show" = a title
Hottitle : "echo $HOST"   => the menu builder runs the command and puts the output in the menu!
Menu     :    "pattern" => the menu builder builds a submenu of files that match the pattern
Show     : 0 => don't show the result
Show     : 1 => show the result (as a string)
Show     : 2 => show the result (as an image)
Show     : 3 => show the result in Growl (if it's installed)


Robin Mills
http://clanmills.com
email: robin@clanmills.com
Skype: clanmills

Version History

20100509  Added the preference panel UI (install, uninstall etc)
20100505  Fixed some stuff
20100504  First drop for comment

/////////////////////////////////////////////////////////////
////// About running AppleScript ////////////////////////////
On May 5, 2010, at 2:04 PM, Adam Bell wrote:

I've been playing with the JSON file too after it struck me as a way to display other things. For example, I've changed Current User's command to "whoami" so I can see how I logged in. I tried an "echo osascript "do shell script  \"short appleScript\"" but that fails so there must be an expectation for simple commands. 

For example this: which returns the system version

set SysVersion to dec_to_hex(get system attribute "sysv")
set sv to system version of (get system info) -- returns  "10.5.8"

on dec_to_hex(l)
	set l to l as list
	set c to "0123456789ABCDEF"
	repeat with i in l
		set v to i as integer
		set h to ""
		repeat while v > 0
			set h to c's character (v mod 16 + 1) & h
			set v to v div 16
		end repeat
		set i's contents to h
	end repeat
	tell l to if (count) is 1 then return beginning
	l
end dec_to_hex

Adam
---------------------------------------------
On May 5, 2010, at 4:47 PM, Robin Mills wrote:
Adam

I got this to work:

		{	"Title"		: "OSVersion"
		,	"Cmd"		: "cat ~/temp/adam.script | osascript -  "
		}

However this:
  ... "Cmd" : "osascript ~/temp/adam.script" 

Say "status 0".

I think osascript is some kind of magic that runs in the dungeon somewhere.  If you say osascript file, he returns instantly and delegates running of the script to somebody else.  So we're done and the guy in the dungeon is still thinking.  However when we cat bbb | osascript - the whole mess gets run synchronously.

(or something like that).  Most certainly an interesting bug to be investigated.  The job is run by NSTask and I know he can handle asynchronous pipes.  I'll note this in my file ReadMe.txt so that this doesn't get forgotten.

Robin
http://clanmills.com

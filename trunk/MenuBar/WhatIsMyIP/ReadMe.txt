/*
These are note made by Robin

They'll get edited and inserted in the documentation.
 {
   GUID = 473459372;
   "tile-data" =
   {
     "dock-extra" = 0;
     "file-data" =                 {
     "_CFURLAliasData" = <00000000 00d80003 00010000 c6cf8316 0000482b 00000000 007827d5 007827d6 0000c7ee 53740000 00000920 fffe0000 00000000 0000ffff ffff0001 001c0078 27d50078 27c80077 c5ed007b e6b20010 91e2000a 7bca0000 798e000e 0016000a 004c0069 00730074 0065006e 002e0061 00700070 000f000e 00060069 004d0061 00630048 00440012 003d5573 6572732f 726d696c 6c732f50 726f6a65 6374732f 5475746f 7269616c 732f4c69 7374656e 2f627569 6c642f44 65627567 2f4c6973 74656e2e 61707000 00130001 2f000015 0002000d ffff0000>;
     "_CFURLString" = "/Users/rmills/Projects/Tutorials/Listen/build/Debug/Listen.app/";
     "_CFURLStringType" = 0;
   };
   "file-label" = Listen;
   "file-mod-date" = 3354288299;
   "file-type" = 41;
   "parent-mod-date" = 3354284916;
   };
   "tile-type" = "file-tile";
 }
 
  <dict>
    <key>tile-data</key>
    <dict>
       <key>file-data</key>
       <dict>
         <key>_CFURLString</key>
         <string>/Users/rmills/Projects/Tutorials/Listen/build/Debug/Listen.app</string>
		 <key>_CFURLStringType</key>
         <integer>0</integer>
       </dict>
    </dict>
 </dict>
 
 
 To make our icon live in the dock:
 x=`defaults read com.apple.dock | grep WhatIsMyIP`
 if [ $?==0]; then
	defaults write com.apple.dock persistent-apps -array-add "<dict><key>tile-data</key><dict><key>file-data</key><dict><key>_CFURLString</key><string>/Users/rmills/Projects/Tutorials/Listen/build/Release/Listen.app</string><key>_CFURLStringType</key><integer>0</integer></dict></dict></dict>"
	killall Dock
 fi
 # and we need something similar for com.apple.loginitems
 
 // http://developer.apple.com/mac/library/samplecode/LoginItemsAE/Listings/Read_Me_About_LoginItemsAE_txt.html#//apple_ref/doc/uid/DTS10003788-Read_Me_About_LoginItemsAE_txt-DontLinkElementID_6
 
 // http://developer.apple.com/mac/library/documentation/Darwin/Reference/ManPages/man1/launchctl.1.html
 // man launchctl
 
 // http://stackoverflow.com/questions/2564394/add-a-start-up-item-via-command-line-mac
 defaults write com.apple.loginitems CustomListItems -array-add <dict><key>Name</key><string>Listen.app</string></dict>
 
 {
   Alias = <00000000 00d80003 00010000 c6cf8316 0000482b 00000000 007827d5 007827d6 0000c7ee 53740000 00000920 fffe0000 00000000 0000ffff ffff0001 001c0078 27d50078 27c80077 c5ed007b e6b20010 91e2000a 7bca0000 798e000e 0016000a 004c0069 00730074 0065006e 002e0061 00700070 000f000e 00060069 004d0061 00630048 00440012 003d5573 6572732f 726d696c 6c732f50 726f6a65 6374732f 5475746f 7269616c 732f4c69 7374656e 2f627569 6c642f44 65627567 2f4c6973 74656e2e 61707000 00130001 2f000015 0002000d ffff0000>;
   CustomItemProperties =  {
     "com.apple.loginitem.legacyprefs" =                     {
         AliasData = <00000000 00d80003 00010000 c6cf8316 0000482b 00000000 007827d5 007827d6 0000c7ee 53740000 00000920 fffe0000 00000000 0000ffff ffff0001 001c0078 27d50078 27c80077 c5ed007b e6b20010 91e2000a 7bca0000 798e000e 0016000a 004c0069 00730074 0065006e 002e0061 00700070 000f000e 00060069 004d0061 00630048 00440012 003d5573 6572732f 726d696c 6c732f50 726f6a65 6374732f 5475746f 7269616c 732f4c69 7374656e 2f627569 6c642f44 65627567 2f4c6973 74656e2e 61707000 00130001 2f000015 0002000d ffff0000>;
         Hide = 0;
         Path = "/Users/rmills/Projects/Tutorials/Listen/build/Debug/Listen.app";
     };
   };
   Icon = <496d6752 00000120 00000000 4642494c 00000114 00000002 00000000 00000000 01040003 00000000 c6cf8316 0000482b 00000000 007827da 007827de 0000c7ee 53750000 00000920 fffe0000 00000000 0000ffff ffff0001 00280078 27da0078 27d70078 27d60078 27d50078 27c80077 c5ed007b e6b20010 91e2000a 7bca0000 798e000e 0018000b 004c0069 00730074 0065006e 002e0069 0063006e 0073000f 000e0006 0069004d 00610063 00480044 0012005c 55736572 732f726d 696c6c73 2f50726f 6a656374 732f5475 746f7269 616c732f 4c697374 656e2f62 75696c64 2f446562 75672f4c 69737465 6e2e6170 702f436f 6e74656e 74732f52 65736f75 72636573 2f4c6973 74656e2e 69636e73 00130001 2f000015 0002000d ffff0000>;
   Name = "Listen.app";
 }
 
 
 
 */

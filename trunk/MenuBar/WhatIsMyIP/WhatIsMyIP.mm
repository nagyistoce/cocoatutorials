//
//  WhatIsMyIP.mm
//  This file is part of WhatIsMyIP
// 
//  WhatIsMyIP is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  WhatIsMyIP is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with WhatIsMyIP.  If not, see <http://www.gnu.org/licenses/>.
//
//  Written by Robin Mills, San Jose, CA, USA. 2010 http://clanmills.com
//

#import  "WhatIsMyIP.h"
#include "../LoginItemsAE/LoginItemsAE.h"
#include "../SharedCode/alerts.h"
#import  "JSON.h"

@implementation WhatIsMyIP

#pragma mark ---- properties

@synthesize lastEventId;
@synthesize jsonModDate;
@synthesize jsonArray ;
@synthesize	shrink ;
@synthesize	plistPath;
@synthesize	stream;

#pragma mark ---- life management

- (void) dealloc
{
	[appName release];
	[appPath release];
	[plistPath release];
	[statusItem release];
	[aboutBox release];
	[lastEventId release];
	[jsonModDate release];
	[jsonPath release];
	[jsonArray release];
	[iconImage release];
	[menuTitle release];
	[shrink release];
	[super dealloc];
}

- (id) init
{
	self		= [ super init];
	appName		= nil;
	appPath		= nil;
	plistPath	= nil;
	statusItem	= nil;
	aboutBox	= nil;
	shrink      = nil;
	bDebug      = false;
	lastEventId	= [[NSNumber alloc]initWithInt:0];
	jsonModDate	= [[NSDate distantPast]retain];
	jsonPath	= [[NSString alloc]initWithFormat:@"%@/Library/Preferences/WhatIsMyIP.json",NSHomeDirectory()];
	fm			= [NSFileManager defaultManager];
	
	return self ;
}

- (void) awakeFromNib
{
	[NSApp setDelegate:self];
	[self processCommandLine]; // might die!

	////
	// open our eyes and figure where we are
	NSDictionary* dict = [[NSBundle mainBundle] infoDictionary];
	dictDump(dict) ;
	
	appPath		= [[NSString alloc]initWithString:[dict objectForKey:@"NSBundleResolvedPath"]];
	appName		= [[NSString alloc]initWithString:[dict objectForKey:@"CFBundleName"]];
	plistPath	= [[NSString alloc]initWithFormat:@"%@/../Info.plist",[[NSBundle mainBundle]resourcePath]];
	dockIcon	= ![[dict valueForKey:@"LSUIElement"]boolValue];

	NSString*	  iconFile	= @"WhatIsMyIP.gif" ; // dict      ? [NSMutableString stringWithString:[dict objectForKey:@"CFBundleIconFile"]] : nil ;
	NSString*	  iconPath	= iconFile  ? [NSString stringWithFormat:@"%@/%@",[[NSBundle mainBundle]resourcePath],iconFile] : nil ;
	iconImage	= iconPath  ? [[NSImage alloc] initWithContentsOfFile:iconPath] : nil ;
	menuTitle   = [[NSString alloc]initWithString:iconImage?@"":appName];
	
	[self installProgram]; // might die

	////
	// build the menu
	[self jsonRead]; // might die
	NSString* libraryPreferences = [NSString stringWithFormat:@"%@/Library/Preferences/",NSHomeDirectory()];
	initializeEventStream(libraryPreferences,self,@selector(jsonRead));
	
	statusItem	= [[[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength] retain];
	[statusItem setMenu:statusMenu];
	[statusItem setToolTip:@"WhatIsMyIP.com's friendly toolbar for MacOSX"];
	[statusItem setHighlightMode:YES];
	[statusItem setTitle:menuTitle];
	[statusItem setImage:iconImage];
	
	//  http://stackoverflow.com/questions/623852/how-to-drag-nsstatusitems
	//	[statusItem sendActionOn: NSLeftMouseDraggedMask];// (NSLeftMouseUpMask | NSLeftMouseDownMask | NSLeftMouseDraggedMask | NSPeriodicMask) ];
	//	[statusItem setTarget:self];
	//	[statusItem setAction:@selector(actionMan:)];
}

- (void) processCommandLine
{
	NSLog(@"processCommandLine");
	for ( int i = 1 ; i < __argc ; i++ ) {
		if ( strcmp(__argv[i],"--uninstall") == 0 ) {
			[self setAsLoginItem:NO];
			NSString* cmd = [NSString stringWithFormat:
							 @"sleep 3\n"
							 @"rm -rf ~/Applications/WhatIsMyIP.app/\n"
							 @"rm -rf ~/Library/PreferencePanes/WhatIsMyIPprefs.prefPane\n"
							 @"rm -rf ~/Library/Preferences/WhatIsMyIP*\n"
							 ];
			system([cmd UTF8String]) ;
			exit(0);
		}
	}
}

#pragma mark --- json and menu management


- (ShowStatus) showStatus:(NSDictionary*)menuDict
{
	ShowStatus result = menuDict ? showItem : showNone ;
	if ( menuDict ) {
		// look for title
		@try{
			NSMutableString* title = [menuDict objectForKey:@"Title"];
			[title	replaceOccurrencesOfString:@"-"
								   withString:@"" 
									  options:NSLiteralSearch
										range:NSMakeRange( 0,[title length])
			 ];
			if ( ![title length] ) result = showSeparator ; // if there's nothing there
		}
		@catch (id e) { result = showNone ; }
		@finally {}
		
		// look for enabled=0
		@try{
			id enabled = [menuDict objectForKey:@"Enabled"];
			if ( enabled && ![enabled intValue] ) result = showNone ;
		}
		@catch (id e) {}
		@finally {}

		// look for separator=1
		@try{
			id separator = [menuDict objectForKey:@"Separator"];
			if ( separator && [separator intValue] ) result = showSeparator ;
		}
		@catch (id e) {}
		@finally {}
	}
	return result ;
}

- (BOOL) menuBoolean:(NSDictionary*) dict key:(NSString*) key
{
	BOOL bResult = NO ;
	if ( dict && [[[dict class]description]isEqualTo:@"NSCFDictionary"]) {
		bResult = [[dict objectForKey:key]intValue]?YES:NO;
	}
	return bResult;
}

- (void) addMenuItem:(NSString*) title action:(SEL)action target:(id)target tag:(NSInteger)tag
{
	NSMenuItem* newMenuItem = [[NSMenuItem alloc]init];
	[newMenuItem setTitle:title];
	[newMenuItem setTarget:target];
	[newMenuItem setAction:action];
	[newMenuItem setTag:tag];
	[statusMenu addItem:newMenuItem];
}

- (void) rebuildMenu:(NSDictionary*) dict
{
	////
	// change the jsonArray if it smells good
	if ( dict && [[[dict class]description]isEqualTo:@"NSCFDictionary"]) {
		id array = [dict objectForKey:@"Menu"];
		if ([[[array class]description]isEqualTo:@"NSCFArray"]) {
			[self setJsonArray: array] ;
		}
	}

	////
	// remove everything from the old menu
	NSInteger m ;
	NSInteger mmax = [[statusMenu itemArray]count];
	for ( m = 0 ; m < mmax ; m++ )
		[statusMenu removeItemAtIndex:0];
	
	////
	// cycle over the jsonArray and set up a new menu
	bool bWantSeparator = false ;
	for ( m = 0 ; m < [jsonArray count] ; m++ ) {
		NSDictionary* menuDict = [jsonArray objectAtIndex:m];
		switch ([self showStatus:menuDict]) {
			case showItem: {
				[self addMenuItem:[menuDict objectForKey:@"Title"]
						   action:@selector(menuCallback:) target:self tag:m
				];
				bWantSeparator = true;
			}	break;
			
			case showSeparator : 
				[statusMenu addItem:[NSMenuItem separatorItem]];
				bWantSeparator = false;
			break;
			
			default: case showNone:		break;
		}
	}
	
	////
	// ----------- (if we have a menu)
	// ToggleDock  (only when debugging)
	// About       (if we're in the dock)
	// Quit        (always)
	if ( bWantSeparator ) [statusMenu addItem:[NSMenuItem separatorItem]];
	if ( bDebug	  ) [self addMenuItem:@"ToggleDock" action:@selector(toggleDock:)					target: self  tag:0];
	if ( dockIcon ) [self addMenuItem:@"About..."	action:@selector(orderFrontStandardAboutPanel:) target: NSApp tag:0];
					[self addMenuItem:@"Quit"		action:@selector(quitApplication:)				target: self  tag:0];	

	// what about the dock?
	if ( dockIcon != [self menuBoolean:dict key:@"RunInDock"] ) [self showInDock:!dockIcon];
	[self setAsLoginItem:[self menuBoolean:dict key:@"RunAtLogin"]];
}

- (void) jsonRead
{
	NSDictionary*	fileAttributes	= [fm attributesOfItemAtPath:jsonPath error:NULL];
	NSDate*			fileModDate		= [fileAttributes objectForKey:NSFileModificationDate];
	
	////
	// reread the JSON file if it has changed (older or newer, but different)
	if ( [fileModDate compare:[self jsonModDate]] != NSOrderedSame)
	{
		[self setJsonModDate:fileModDate];
		NSStringEncoding	enc;
		NSError*			error;
		NSString* jsonString	  = [NSString 
		 stringWithContentsOfFile : jsonPath
		 			 usedEncoding : &enc
							error : &error
		] ;
		// NSLog(@"string = %@",jsonString);
		id dict = [jsonString JSONValue];
		NSLog(@"JSON Modified %@ JSON %s",jsonPath,dict?"Good": "Bad");
		if ( dict ) [self rebuildMenu:dict];
	}
}

- (void) menuCallback:(id)sender
{
	id dict = nil;
	id cmd  = nil;
	id show = nil;
	@try{
		dict = [jsonArray objectAtIndex:[sender tag]];
		cmd  = [dict objectForKey:@"Cmd"];
		show = [dict objectForKey:@"Show"];
	}
	@catch (id e) {}
	@finally	  {}
	
	bool d = show==nil;
	if ( show ) d = [show intValue] != 0 ;
	if ( cmd  ) 	[self invoke:cmd displayResult:d?YES:NO];
}

# pragma mark ---- task management

- (void) terminateTask:(NSTask*) task
{
	if( [ task isRunning] ) {
		[task terminate];
		bKilled = true ;
	}
}

- (void) invoke:(NSString*) command  displayResult: (BOOL) displayResult
{
	bKilled				= false ;
	NSTask* task		= [[NSTask alloc] init];
	NSPipe* pipe		= [[NSPipe alloc] init];
	NSFileHandle* file	= [pipe fileHandleForReading];
	
    [task setLaunchPath: @"/bin/bash"];	
	[task setArguments:[NSArray arrayWithObjects: @"-c", command, nil]];
	[task setStandardOutput: pipe];
    [task launch];
	
	NSData*			 data	= [file availableData];
	NSString*		 output = [[NSString alloc ]initWithData: data encoding: NSMacOSRomanStringEncoding];
	NSMutableString* result = [NSMutableString stringWithString: output];
	[output release];
	output = nil;
	
	[result	replaceOccurrencesOfString:@"\n"
							withString:@"" 
							   options:NSLiteralSearch
								 range:NSMakeRange( 0,[result length])
	];
	[file closeFile];
	
	[self performSelector: @selector(terminateTask:)
				withObject: task
				afterDelay: 3.0
	];
	[task waitUntilExit];
	if ( displayResult ) {
		if ( ![result length] )
			[result appendString:[NSString stringWithFormat:@"status = %d",[task terminationStatus]]];
	
		[statusItem setTitle:result] ; // bKilled?result:@"killed"];
	} else {
		[statusItem setTitle:menuTitle];
	}
	
	// tell the status bar to shrink after a little while
	int delay = 5;
	[self setShrink:[NSDate dateWithTimeIntervalSinceNow:delay-1]];
	[self performSelector:@selector(shrinkFootprint:) withObject:self afterDelay:delay];
}

#pragma mark --- installation

- (BOOL) pathExists:(NSString*)path
{
	BOOL r,w,m;
	NSString *d, *t;
	return [[NSWorkspace sharedWorkspace]
	getFileSystemInfoForPath: path isRemovable:&r	isWritable:&w	isUnmountable:&m description:&d	type:&t
	];
}

- (void) installProgram
{
	NSString* app = [appPath lastPathComponent];
	NSString* dir = [[appPath substringToIndex:[appPath length]-[app length]]lastPathComponent];
	NSLog(@"programDir = %@",dir);
	if ( [dir isEqualTo:@"Debug"] ) {
		// ensure the preference panel is installed
		[self installPreferences];
		NSLog(@"keep going when debugging");
	} else if ( ![dir isEqualTo:@"Applications"] ) {
		NSString* applicationsPath		= [NSString stringWithFormat:@"%@/Applications",NSHomeDirectory()];
		NSString* applicationsAppPath	= [NSString stringWithFormat:@"%@/WhatIsMyIP.app",applicationsPath];
		if ( ![self pathExists:applicationsAppPath] ) {
			if ( ask(@"OK to install to ~/Applications directory ?") ) {
				// ensure the preference panel is installed
				[self installPreferences];
				NSString* cmd = [NSString stringWithFormat:
				@"if [ ! -e \"%@\" ]; then\n  mkdir \"%@\"        \nfi\n" // applicationsPath,applicationPath
				@"if [ ! -e \"%@\" ]; then\n  ditto \"%@\" \"%@\" \nfi\n" // applicationsAppPath,appPath,applicationsAppPath
				, applicationsPath,applicationsPath
				, applicationsAppPath,appPath,applicationsAppPath
				];
				//[self invoke:cmd displayResult:NO];
				system([cmd UTF8String]);
			}
		}
		
	//	NSString* exePath = [NSString stringWithFormat:@"%@/Contents/MacOS/WhatIsMyIP",applicationsAppPath];
	//	NSString* c = [NSString stringWithFormat:@"\"%@\" &",exePath]; 
	//	system([c UTF8String]);

		// kick off the application, we have to die first
		NSString* openApp = [NSString stringWithFormat:@"sleep 2 ; open \"%@\" &",applicationsAppPath];
		[self invoke:openApp displayResult:NO]; // I don't know why this doesn't work
		NSLog(@"starting %@",openApp);
		exit(0) ; // [self quitApplication:self];
	}
}

- (void) installPreferences
{
	NSString* cmd = [NSString stringWithFormat: @"\necho ----\n"
	@"SELF=\"%@\"\n"
	@"PREF=\"%@\"\n"
	@"PANES=\"%@\"\n"
	@"FILES=\"%@\"\n"
	@"JSON=\"%@\"\n"
	@"if [ ! -e \"$PANES/$PREF\"  ]; then\n  ditto \"$SELF/Prefs/$PREF\"      \"$PANES/$PREF\"\nfi\n"
//	@"                                       ditto \"$SELF/Prefs/$PREF\"      \"$PANES/$PREF\"\n\n"
	@"if [ ! -e \"$FILES/$JSON\"  ]; then\n  cp    \"$SELF/Prefs/\"*.json     \"$FILES\"\nfi\n"
    @"                                       cp    \"$SELF/Prefs/\"*.defaults \"$FILES\"\n"
	@"echo ---\n"
	, appPath                                                 // SELF
	, @"WhatIsMyIPprefs.prefPane"                             // PREF
	, @"$HOME/Library/PreferencePanes"                        // PANES
	, @"$HOME/Library/Preferences"                            // FILES
	, @"WhatIsMyIP.json"                                      // JSON
	];
	printf("installPreferences cmd = %s",[cmd UTF8String]) ;
	[self invoke:cmd displayResult:NO];
	// NSLog(@"installPreferences cmd = %*",cmd) ;

	/*
				PANES                   PREF                                   SELF        PREF                      PANES                      PREF 
	 if [! -e ~/Library/PreferencePanes/WhatIsMyIPprefs.prefPane ]; then ditto [app]/Prefs/WhatIsMyIPprefs.prefPane  ~/Library/PreferencePanels/WhatIs.Pane fi
	 if [! -e ~/Library/Preferences/WhatIsMyIPprefs.json         ]; then cp    [app]/Prefs/*.json                    ~/Library/Preferences/                 fi
			    FILES               JSON                                                                             FILES
	                                                                      cp    [app]/Prefs/*.defaults               FILES
	*/
}

- (void) setAsLoginItem:(BOOL) bAdd
{
	NSLog(@"setAsLoginItem") ;
	
	NSString* sURL = [NSString stringWithFormat:@"file://localhost%@/",appPath];
	NSURL*     url = [NSURL URLWithString:sURL]; //@"file://localhost/Users/rmills/bin/aaa.app/"];
	NSArray* itemsPtr = NULL ;
	LIAECopyLoginItems((CFArrayRef*)&itemsPtr);
	
	// run over the logon list from end to start and remove all copies in use
	NSString* myAppName = [NSString stringWithFormat:@"%@.app",appName] ;
	for ( int i = [itemsPtr count] ; i ; ) {
		i-- ;
		id item = [[itemsPtr objectAtIndex:i] valueForKey:@"URL"] ;
		NSString* app = [[item description] lastPathComponent];
		if ( [app isEqualTo:myAppName] ) {
			LIAERemove(i);
			NSLog(@"removing %@ appName = %@",app);
		}
	}
	
	if ( bAdd ) {
		NSLog(@"adding %@",url);
		/*
		NSString* cmd = [NSString stringWithFormat:
						 @"defaults write com.apple.dock persistent-apps -array-add "
						 @"'<dict><key>tile-data</key><dict><key>file-data</key><dict><key>_CFURLString</key><string>"
						 @"%@"
						 @"</string><key>_CFURLStringType</key><integer>0</integer></dict></dict></dict>'"
						 ,appPath
						 ];
		[self invoke:cmd displayResult:NO];
		 */
		BOOL hide = !dockIcon ;
		LIAEAddURLAtEnd((CFURLRef)url,hide);
	}
}

- (void) showInDock:(BOOL)bDock
{
	NSString* cmd = [NSString stringWithFormat
	:@"killall WhatIsMyIP ; sleep 1 ; "
	 @"/usr/libexec/PlistBuddy -c 'Set LSUIElement %s' '%@' ;"
	 @"'%@/Contents/MacOS/WhatIsMyIP' &"
	, bDock?"false":"true"
	, plistPath
	, appPath
	];
	NSLog(@"cmd = %@",cmd);
	[self invoke:cmd displayResult:NO];
	sleep(1);
//	[self quitApplication:nil]; // no need to die, killall did it!
}

#pragma mark --- misc UI

- (void) shrinkFootprint:(id)sender
{
	NSDate* now = [NSDate date];
	if ( [now compare:shrink] == NSOrderedDescending ) {
		[statusItem setTitle:menuTitle] ;
	} else {
		[self performSelector:@selector(shrinkFootprint:) withObject:self afterDelay:2]; // all lines are busy, call again!
	}
}

- (void) toggleDock:(id)sender
{
	[self showInDock:!dockIcon];
}

#pragma mark --- close down

- (NSApplicationTerminateReply) applicationShouldTerminate : (NSApplication *)app
{
	NSLog(@"applicationShouldTerminate");
    FSEventStreamStop(stream);
    FSEventStreamInvalidate(stream);
    return NSTerminateNow;
}

- (IBAction) quitApplication:(id)sender
{
	[NSApp stop:sender];
}

@end

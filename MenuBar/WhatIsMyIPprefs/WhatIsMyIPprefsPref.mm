//
//  WhatIsMyIPprefsPref.m
//  WhatIsMyIPprefs
//
//  Created by Robin Mills on 5/1/10.
//  Copyright (c) 2010 RMSC, San Jose, CA, USA. All rights reserved.
//

#import  "WhatIsMyIPprefsPref.h"
#include "../SharedCode/alerts.h"
#include "../SharedCode/common.h"

PrefDocController*		theDocController;
WhatIsMyIPprefsPref*	thePrefPanel;

@implementation WhatIsMyIPprefsPref

@synthesize appPath;

- (void) dealloc
{
	[appPath release];
	alertSetTitle(nil);
	[super dealloc];
}

- (id) init
{
	self = [super init];
	appPath = nil ;
	bSniffing = false;
	alertSetTitle(@"WhatISMyIP Prefs");
	
	return self;
}

- (void) keepSniffing
{
	[self sniffForApplicationRunning];
	[self performSelector:@selector(keepSniffing) withObject:self afterDelay:5];
}

- (void) sniffForApplicationRunning
{
/*
    This doesn't 'see' invisible processes!
 
	static int count = 0 ;
	bool bLog = (count++ % 10) ==2 ;
	NSArray* processArray = [[NSWorkspace sharedWorkspace]launchedApplications];
	bAppIsRunning = false ;
	for ( NSInteger p = 0 ; (!bAppIsRunning) && p < [processArray count] ; p++ ) {
		NSString* name = [[processArray objectAtIndex:p]objectForKey:@"NSApplicationName"];
		if ( bLog ) NSLog(@"application %d = %@",p,name);
		bAppIsRunning = [name isEqualTo:@"WhatIsMyIP"] ;
	}
*/	
	NSString* x = [self invoke:@"ps ax | egrep -e WhatIsMyIP\\.app | egrep -v -e grep"];
	bAppIsRunning = [x length] > 10 ;
	[runstopButton setTitle:bAppIsRunning?@"STOP":@"Start"];
	//	NSLog(@"ps grep = %@",x);
}

- (void) awakeFromNib
{
	thePrefPanel = self ;
	[self keepSniffing];
}

- (void) mainViewDidLoad
{
	NSDictionary* dict = [[self bundle] infoDictionary];
	dictDump(dict) ;
	[self setAppPath:[[NSString alloc]initWithString:[dict objectForKey:@"NSBundleResolvedPath"]]];
	[self sniffForApplicationRunning];
}

- (void) terminateTask:(NSTask*) task
{
	if( [ task isRunning] ) {
		[task terminate];
		bKilled = true ;
	}
}

- (NSString*) invoke:(NSString*) command
{
	bKilled = false ;
	NSTask *task = [[NSTask alloc] init];
	NSPipe *pipe = [[NSPipe alloc] init];
	NSFileHandle *file = [pipe fileHandleForReading];
	
    [task setLaunchPath: @"/bin/bash"];	
	[task setArguments:[NSArray arrayWithObjects: @"-c", command, nil]];
	[task setStandardOutput: pipe];
    [task launch];
	
	NSData*		data = [file availableData];
	NSString* output = [[NSString alloc ]initWithData: data encoding: NSMacOSRomanStringEncoding];
	NSMutableString* result = [NSMutableString stringWithString: output];
	[output release];
	output = nil;
	/*
	[result	replaceOccurrencesOfString:@"\n"
							withString:@"" 
							   options:NSLiteralSearch
								 range:NSMakeRange( 0,[result length])
	];
	*/
	[file closeFile];
	
	[self performSelector: @selector(terminateTask:)
			   withObject: task
			   afterDelay: 3.0
	];
	[task waitUntilExit];
	
	return result;
}

- (void) invoke:(NSString*) command displayResult: (BOOL) displayResult
{
	NSString* result = [self invoke:command];
	if ( displayResult ) {
		//if ( ![result length] )
		//	[result appendString:[NSString stringWithFormat:@"status = %d",[task terminationStatus]]];
		
		alert(result) ;
		//[statusItem setTitle:result] ; // bKilled?result:@"killed"];
	} 
}	

- (IBAction) launchLoginItemsAETest:(id)sender
{
	NSLog(@"launchLoginItemsAETest") ;
	
	NSString* openLoginApp = [NSString stringWithFormat:@"open %@/Utilities/LoginItemsAETest.app",appPath];
	[self invoke:openLoginApp displayResult:NO];
}

- (IBAction) runAtLogin:(id)sender
{
	NSLog(@"runAtLogin = %d",[sender intValue]);
}

- (IBAction) runInDock:(id)sender
{
	NSLog(@"dock = %d",[sender intValue]);
}

- (IBAction) unInstall:(id)sender
{
	[self sniffForApplicationRunning];
	
	if ( ask(@"Do you really want to uninstall?") ) {
		if ( bAppIsRunning ) [self runstop:sender] ;
		NSString* applicationPath = [NSString stringWithFormat:@"%@/Applications/WhatIsMyIP.app/Contents/MacOS/WhatIsMyIP",NSHomeDirectory()];
		NSString* unInstall = [NSString stringWithFormat:@"\"%@\" --uninstall",applicationPath] ;
		[self invoke: unInstall displayResult:NO];
		sleep(1);
		exit(0);
	}
}

- (IBAction) runstop:(id)sender
{
	NSLog(@"runstop");
	NSString* applicationPath = [NSString stringWithFormat:@"%@/Applications/WhatIsMyIP.app",NSHomeDirectory()];
	NSString* killAll = @"killall WhatIsMyIP" ;
	NSString* open    = [NSString stringWithFormat:@"open \"%@\"",applicationPath] ;
	[self sniffForApplicationRunning];
	[self invoke: bAppIsRunning?killAll:open displayResult:NO];
	sleep(1);
	[self sniffForApplicationRunning];
}

@end

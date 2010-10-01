//
//  WhatIsMyIPprefsPref.h
//  WhatIsMyIPprefs
//
//  Created by Robin Mills on 5/1/10.
//  Copyright (c) 2010 RMSC, San Jose, CA, USA. All rights reserved.
//

#import <PreferencePanes/PreferencePanes.h>
#import "MenuItem.h"

@interface WhatIsMyIPprefsPref : NSPreferencePane 
{
	// outlets
	IBOutlet NSButton*		runstopButton;

	// properties
	NSString*				appPath;
	bool					bKilled;
	bool					bAppIsRunning;
	bool					bSniffing;
}

// actions
- (IBAction) launchLoginItemsAETest:(id)sender;
- (IBAction) runInDock:(id)sender;
- (IBAction) runAtLogin:(id)sender;
- (IBAction) unInstall:(id)sender;
- (IBAction) runstop:(id)sender;

// methods
- (NSString*) invoke:(NSString*)cmd;
- (void)      invoke:(NSString*)cmd displayResult:(BOOL)bDisplay;
- (void)      terminateTask:(NSTask*) task;
- (void)      sniffForApplicationRunning;

// property getters/setters
@property (readwrite,retain) NSString* appPath ;

@end

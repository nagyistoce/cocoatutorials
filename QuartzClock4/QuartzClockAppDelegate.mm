//
//  QuartzClockAppDelegate.m
//  This file is part of QuartzClock
// 
//  QuartzClock is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  QuartzClock is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with QuartzClock.  If not, see <http://www.gnu.org/licenses/>.
//
//  Written by Robin Mills, San Jose, CA, USA. 2010 http://clanmills.com
//

#import "QuartzClockAppDelegate.h"
#import "QuartzClockView.h"
#import "QuartzClockPreferences.h"

@implementation QuartzClockAppDelegate

@synthesize window;
@synthesize preferences;

- (void) applicationDidFinishLaunching : (NSNotification *) aNotification
{ 
	NSLog(@"applicationDidFinishLaunching");
    NSColor* background = [NSColor colorWithDeviceRed:0.0 green:0.0 blue:1.0 alpha:0.0];
	[window setDelegate:self];
	[window setLevel:NSFloatingWindowLevel];
    [window setOpaque:NO];
	[window setStyleMask:[QuartzClockView borderNone]];
    [window setBackgroundColor:background];
     preferences = [[QuartzClockPreferences alloc]initWithWindowNibName:@"Preferences"];
}

- (IBAction) preferencesShow : (id) sender
{
    NSLog(@"preferencesShow");
    [preferences showWindow:sender];
    [preferences setWindowView:[window contentView]];
    [preferences setIconView:[[window contentView]myIcon]];
    [[preferences colorWell] setColor:[[preferences iconView] dockedBackground]];
}    

- (BOOL) windowShouldClose : (id) sender
{
    [window orderOut:nil];
	return NO ;
}

- (BOOL) applicationShouldHandleReopen : (NSApplication*) theApplication
                     hasVisibleWindows : (BOOL) flag
{
    NSLog(@"applicationShouldHandleReopen");
    if ([window isVisible ] )
        [window orderOut:nil];
    else
        [window makeKeyAndOrderFront:nil];
    return YES;
}

static int min(int x,int y) { return x < y ? x : y ; }

- (NSSize) windowWillResize : (NSWindow*) sender
					 toSize : (NSSize) frameSize
{
	//int x = min(frameSize.width,frameSize.height);
	//if ( 500 < x && x < 550 ) x = 512 ;
	//NSSize result = { x,x };
	
	return frameSize;
}

@end

// That's all Folks!
////
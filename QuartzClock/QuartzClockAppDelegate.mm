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

@implementation QuartzClockAppDelegate

@synthesize window;

- (void) applicationDidFinishLaunching : (NSNotification *) aNotification
{ 
	NSLog(@"applicationDidFinishLaunching");
	[NSApp hide:window];
	[window makeKeyAndOrderFront:nil];
	[window setDelegate:self];
	[window setLevel:NSFloatingWindowLevel];
//	[window setStyleMask:NSBorderlessWindowMask];
}

- (BOOL) windowShouldClose : (NSNotification *) notification
{
//  [NSApp stop:nil];
	[NSApp hide:window];
	return NO ;
}

static int min(int x,int y) { return x < y ? x : y ; }

- (NSSize) windowWillResize:(NSWindow *)sender
					 toSize:(NSSize)frameSize
{
	int x = min(frameSize.width,frameSize.height);
	if ( 500 < x && x < 550 ) x = 512 ;
	NSSize result = { x,x };
	
	return result ;
}


- (IBAction) saveAs:(id)sender
{
	NSLog(@"saveAs");
}

@end

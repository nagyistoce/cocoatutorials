//
//  ClockAppDelegate.mm
//  This file is part of Clock
// 
//  Clock is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Clock is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with Clock.  If not, see <http://www.gnu.org/licenses/>.
//
//  Written by Robin Mills, San Jose, CA, USA. 2010 http://clanmills.com
//

#import "ClockAppDelegate.h"

@implementation ClockAppDelegate

@synthesize window;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	// Insert code here to initialize your application

	NSLog(@"applicationDidFinishLaunching");
	[window setDelegate:self];
	[window setLevel:NSFloatingWindowLevel];
}

- (BOOL) windowShouldClose:(NSNotification *)notification
{
	NSLog(@"windowShouldClose");
	[NSApp stop:nil];
	return YES;
}

@end

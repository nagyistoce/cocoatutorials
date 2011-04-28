//
//  QuartzClockAppDelegate.mm
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
#import "Extensions.h"

@implementation QuartzClockAppDelegate

@synthesize windowView;
@synthesize window;
@synthesize dockView;
@synthesize preferences;

//- (void) applicationWillTerminate:(NSNotification *)notification
//{
//    NSLog(@"windows borderMask = %lu otherNone = %lu",[QuartzClockView borderMask],[QuartzClockView borderNone]);
//}

- (void) applicationDidFinishLaunching : (NSNotification *) aNotification
{ 
//	NSLog(@"applicationDidFinishLaunching");

    //
    // http://developer.apple.com/library/mac/#documentation/Cocoa/Conceptual/WinPanel/Tasks/SavingWindowPosition.html%23//apple_ref/doc/uid/20000229-BCIDIHBB
    // calling setRepresentedFilename causes a horrible icon to appear in the title bar!
    // and it seems to cause the window to "shrink" (it's smaller ever time the app starts)
    // this is probably something to do with changing the styleMask.
    // [window setTitleWithRepresentedFilename:@"QuartzClock"];
    // [[window windowController] setShouldCascadeWindows:NO];      // Tell the controller to not cascade its windows.
    // [window setFrameAutosaveName:[window representedFilename]];
    //
    // Reporting bugs on Apple RADAR is a waste of time
    // Reason: Apple employ Apple Lovers to say "Not a bug"
    // Worse:  Apple Lovers always defend Apple
    //         Apple Lovers are the reason that Windows Users detest the Mac
    //

    NSColor* background = [NSColor colorWithDeviceRed:0.0 green:0.0 blue:0.0 alpha:0.0];
    [window setBackgroundColor:background];
	[window setDelegate:self];
	[window setLevel:NSFloatingWindowLevel];
    [window setOpaque:NO];
	[window setStyleMask:[QuartzClockView borderNone]];
    
    NSString* clockPos    = [[NSUserDefaults standardUserDefaults] objectForKey:@"ClockPos"];
    NSRect    windowFrame = [clockPos length] ?  NSRectFromString(clockPos) : NSMakeRect(400,400,400,400);
    [window setFrame:windowFrame display:YES];

    dockView                 = [[QuartzClockView alloc]initInDock] ;
    dockView.backgroundColor = [NSColor blueColor];
    dockView.gradientColor   = [NSColor blueColor];
    dockView.rimColor        = [NSColor whiteColor];
    dockView.handsColor      = [NSColor whiteColor];
    dockView.marksColor      = [NSColor blueColor];
	[[NSApp dockTile] setContentView: dockView];
    
    theDockView     = dockView;
    theWindowView   = windowView;
}

- (IBAction) preferencesShow : (id) sender
{
//  NSLog(@"preferencesShow");
    if ( !preferences ) {
         preferences = [[QuartzClockPreferences alloc]initWithWindowNibName:@"Preferences"];
    }
    [preferences showWindow:sender];
}    

- (BOOL) windowShouldClose : (id) sender
{
    [window orderOut:nil];
	return NO ;
}

- (BOOL) applicationShouldHandleReopen : (NSApplication*) theApplication
                     hasVisibleWindows : (BOOL) flag
{
//  NSLog(@"applicationShouldHandleReopen");
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
    BOOL bSquare = [NSEvent isCommandKeyDown] || [NSEvent isOptionKeyDown] ;
    int  x       = min(frameSize.width,frameSize.height); 
//  if ( 500 < x && x < 550 ) x = 512 ;
	NSSize square = NSMakeSize(x,x) ;
	return bSquare ? square : frameSize ;
}

@end

// That's all Folks!
////
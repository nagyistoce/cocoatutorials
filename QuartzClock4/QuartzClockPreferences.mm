//
//  QuartzClockPreferences.mm
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

#import "QuartzClockPreferences.h"
#import "QuartzClockView.h"


@implementation QuartzClockPreferences
@synthesize iconView;
@synthesize windowView;
@synthesize colorWell;

- (id) initWithWindow : (NSWindow *) aWindow
{
    self = [super initWithWindow:aWindow];
    if ( self ) {
        // Initialization code here.
        // self.window = aWindow;
        // self.window.delegate = self;
        //[colorWell setDelegate:self];
    }
    return self;
}

- (IBAction) changeColor : (id) sender
{
    NSLog(@"changeColor");
    [iconView setDockedBackground:[sender color]];
    [iconView update];
}

- (IBAction) changeColorSelected:(id) sender
{
    NSLog(@"changeColorSelected");
}

- (void) awakeFromNib
{
    NSLog(@"QuartzClockPreferences::awakeFromNib window = %@",[self window]);
}

- (BOOL) windowShouldClose:(id)sender
{
    [[self window] orderOut:nil];
    return NO;
}

- (void) dealloc
{
    [super dealloc];
}

- (void) windowDidLoad
{
    [super windowDidLoad];
   
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

@end

// That's all Folks!
////

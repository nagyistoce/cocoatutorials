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

#import  "QuartzClockPreferences.h"
#import  "QuartzClockView.h"

@implementation QuartzClockPreferences
@synthesize dockView;
@synthesize windowView;

- (id) initWithWindow : (NSWindow *) aWindow
{
    self = [super initWithWindow:aWindow];
    if ( self ) {
        // Initialization code here.
    }
    
    return self;
}

- (void) syncColor : (id) sender
{
    if ( [clock respondsToSelector:selectedElement] ) {
        NSColor* color = [clock performSelector:selectedElement];
        [colorWell setColor:color];
    }
    [clock update];
}

- (IBAction) changeColor : (id) sender
{
    // NSLog(@"changeColor");
    NSColor* color = [colorWell color];
    [clock performSelector:colorSelector withObject:color];
    [self syncColor:sender];
}

- (IBAction) windowViewSelected:(id)sender
{
    [self syncColor:sender];
}

- (IBAction) dockViewSelected:(id)sender
{
    [self syncColor:sender];
}

- (IBAction) backgroundSelected:(id)sender
{
    NSLog(@"backgroundSelected");
    colorSelector   = @selector(setBackgroundColor:);
    selectedElement = @selector(backgroundColor);
    [self syncColor:sender];
}

- (IBAction) gradientSelected:(id)sender
{
    NSLog(@"gradientSelected");
    colorSelector   = @selector(setGradientColor:);
    selectedElement = @selector(gradientColor);
    [self syncColor:sender];
}

- (IBAction) handsSelected:(id)sender
{
    NSLog(@"handsSelected");
    colorSelector   = @selector(setHandsColor:);
    selectedElement = @selector(handsColor);
    [self syncColor:sender];
}

- (IBAction) rimSelected:(id)sender
{
    NSLog(@"rimSelected");
    colorSelector   = @selector(setRimColor:);
    selectedElement = @selector(rimColor);
    [self syncColor:sender];
}

- (IBAction) marksSelected:(id)sender
{
    NSLog(@"marksSelected");
    colorSelector   = @selector(setMarksColor:);
    selectedElement = @selector(marksColor);
    [self syncColor:sender];
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
    [self backgroundSelected:self];
    [self dockViewSelected:self];
     clock.ignoreMouse = YES;
}

@end

// That's all Folks!
////

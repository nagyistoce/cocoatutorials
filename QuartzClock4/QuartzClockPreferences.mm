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

- (void) syncColor : (id) sender
{
    NSLog(@"selectedElement = %@",NSStringFromSelector(selectedElement));
    if ( [selectedView respondsToSelector:selectedElement] ) {
        NSColor* color = [selectedView performSelector:selectedElement];
        //if ( [color class]==NSClassFromString(@"NSColor") ) {
            [colorWell setColor:color];
            NSLog(@"Preferences::syncColor set the color %@",color);
        //} else {
        //    NSLog(@"Preferences::syncColor DID NOT set the color selectedView = %@ color = %@",selectedView,color);
        //}
    } else {
        NSLog(@"Preferences::syncColor object %@ doesn't respond to selector %@",selectedView,NSStringFromSelector(selectedElement));
    }
}

- (IBAction) changeColor : (id) sender
{
    // NSLog(@"changeColor");
    if ([selectedView respondsToSelector:colorSelector] ) {
        [selectedView performSelector   :colorSelector withObject:[sender color]];
        [selectedView   update];
        [self syncColor:sender];
    } else {
        NSLog(@"object %@ does not respond to %@",selectedView,NSStringFromSelector(colorSelector));
    }
}

- (IBAction) windowViewSelected:(id)sender
{
    NSLog(@"windowViewSelected = %@",windowView);
    selectedView = windowView;
    
    [self syncColor:sender];
}

- (IBAction) dockViewSelected:(id)sender
{
    NSLog(@"dockViewSelected = %@",dockView);
    selectedView = dockView;
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

- (IBAction) ticksSelected:(id)sender
{
    NSLog(@"ticksSelected");
//    colorSelector   = @selector(setTicksColor:);
//    selectedElement = @selector(ticksColor);
//    [self syncColor:sender];
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

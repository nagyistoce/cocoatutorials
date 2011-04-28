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
#import  "IconFamily.h"

@implementation QuartzClockPreferences
@synthesize     clock;
@synthesize     windowMatrix;

- (BOOL) bDirty
{
    return bDirty;
}

- (void) setBDirty : (BOOL) b
{
     bDirty = b;
    [applyButton setEnabled:b];
    [cancelButton setEnabled:b];
}

- (id) initWithWindow : (NSWindow*) aWindow
{
    self = [super initWithWindow:aWindow];
    if ( self ) {
        bDirty = NO;
    }
    
    return self;
}

- (void) syncColor : (id) sender
{
    if ( [clock respondsToSelector:selectedElement] ) {
        NSColor* color = [clock performSelector:selectedElement];
        [colorWell setColor:color];
    }
    [clock setAngle:[angle floatValue]];
    [clock setRadialGradient : [radialGradient state]==NSOnState ];
    [clock update];
}

- (IBAction) changeColor : (id) sender
{
    // NSLog(@"changeColor");
    NSColor* color = [colorWell color];
    [self setBDirty:YES];
    [clock performSelector:colorSelector withObject:color];
    [self syncColor:sender];
}

- (void) cancelChanges:(id) sender; 
{
    [windowMatrix selectCell:sender];
}

// I don't like this, however it's only way I could get to work
// I send a message to myself to fix radio buttons later!
- (void) changeTheClock : (BOOL) bSetWindows
{
    id whichCell = bSetWindows ? dockCell    : windowCell    ;
    id copyView  = bSetWindows ? theWindowView : theDockView ;

    if ( bDirty ) {
        switch (NSRunAlertPanel(@"Unsaved changes",@"what do you want to do?",@"Apply Changes",@"Cancel",@"Forget Changes")) {
            case  0  : // cancel
                [self performSelector:@selector(cancelChanges:) withObject:whichCell afterDelay:0.0];
                return ;
            break ; 
            case  1  : // apply
                [copyView copyFrom:clock];
            break ; 
            // case -1  : break ; // forget
        }
    }

    [self setBDirty:NO];
    [clock copyFrom:copyView];
    [self syncColor:self];
    bDock = !bSetWindows;
}

- (void) dockSelected : (NSNotification*) aNote
{
    [self changeTheClock:NO];
}

- (void) windowSelected : (NSNotification*) aNote
{
    [self changeTheClock:YES];
}

- (void) awakeFromNib
{
    [clock setAngle:[angle floatValue]];
    [dockCell setTarget:self];
    [dockCell setAction:@selector(dockSelected:)];
    [windowCell setTarget:self];
    [windowCell setAction:@selector(windowSelected:)];
}

- (IBAction) apply : (id) sender
{
    QuartzClockView* target = bDock ? theDockView : theWindowView ;
    [target copyFrom:clock];   
    [self setBDirty:NO];
}

- (IBAction) cancel: (id) sender
{
    QuartzClockView* target = bDock ? theDockView : theWindowView ;
    [clock copyFrom:target];   
    [self setBDirty:NO];
}

- (IBAction) backgroundSelected : (id) sender
{
    colorSelector   = @selector(setBackgroundColor:);
    selectedElement = @selector(backgroundColor);
    [self syncColor:sender];
}

- (IBAction) gradientSelected : (id) sender
{
    colorSelector   = @selector(setGradientColor:);
    selectedElement = @selector(gradientColor);
    [self syncColor:sender];
}

- (IBAction) handsSelected : (id) sender
{
    colorSelector   = @selector(setHandsColor:);
    selectedElement = @selector(handsColor);
    [self syncColor:sender];
}

- (IBAction) rimSelected : (id) sender
{
    colorSelector   = @selector(setRimColor:);
    selectedElement = @selector(rimColor);
    [self syncColor:sender];
}

- (IBAction) marksSelected : (id) sender
{
    colorSelector   = @selector(setMarksColor:);
    selectedElement = @selector(marksColor);
    [self syncColor:sender];
}

- (BOOL) windowShouldClose : (id) sender
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
     clock.ignoreMouse = YES;
}

- (IBAction) saveIcon : (id) sender
{
    // Take a snap of the clock into an NSImage (256 or 512) NSImage
    const NSUInteger size  = 512;
    NSRect           frame = NSMakeRect(0,0,size,size);
    NSImage*         snap  = [[[NSImage alloc] initWithSize:frame.size]autorelease];

    // Create a newClock to fill the snap frame (and copy its props from clock)
    QuartzClockView* newClock = [[[QuartzClockView alloc]initWithFrame:frame]autorelease];
    [newClock copyFrom:clock];
    
    // Draw the newClock into the snap
    [snap setFlipped:YES];
    [snap lockFocus];
    [newClock drawRect: [newClock frame]];
    [snap unlockFocus];
    
    // Convert to an Icon
    IconFamily* snapIcon = [[IconFamily alloc] initWithThumbnailsOfImage:snap];
    
    // Run a save panel
    NSSavePanel* save = [NSSavePanel savePanel];
    [save setTitle:@"Save Icon"];
    [save setAllowedFileTypes:[NSArray arrayWithObject:@"icns"]];
    [save setAllowsOtherFileTypes:NO];
    NSInteger returnCode = [save runModal];
    
    // Save the icon and open it with the finder
    if (returnCode == NSFileHandlingPanelOKButton) {
        NSURL * chosenURL = [save URL];
        [snapIcon writeToFile:[chosenURL path]];
        [[NSWorkspace sharedWorkspace] openURL:chosenURL];
    }
}

@end

// That's all Folks!
////

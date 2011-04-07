//
//  QuartzClockPrerences.mm
//  QuartzClock
//
//  Created by Robin Mills on 2011-04-05.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "QuartzClockPreferences.h"


@implementation QuartzClockPreferences

- (id) initWithWindow : (NSWindow *) aWindow
{
    self = [super initWithWindow:aWindow];
    if ( self ) {
        // Initialization code here.
        // self.window = aWindow;
        // self.window.delegate = self;
    }
    return self;
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

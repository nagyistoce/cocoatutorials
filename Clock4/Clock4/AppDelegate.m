//
//  AppDelegate.m
//  Clock4
//
//  Created by  on 2012-06-13.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "AppDelegate.h"
#import <Cocoa/Cocoa.h>

@implementation ClockWindow

- (void) tick:(id)sender
{
    static NSDateFormatter* dateFormatter = nil;
    if (!dateFormatter ) {
        dateFormatter = [[NSDateFormatter alloc] init];
        [dateFormatter setDateStyle:NSDateFormatterLongStyle];
        [dateFormatter setTimeStyle:NSDateFormatterLongStyle];
    }
    NSString* now = [dateFormatter stringFromDate:[NSDate date]];
    [ label setStringValue:now];
    [ self performSelector:@selector(tick:)
                withObject: nil
                afterDelay: 1.0
    ];
}

- (void)awakeFromNib
{
    [self tick:nil];
}
@end

@implementation AppDelegate

@synthesize window = _window;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
}

@end

//
//  AppDelegate.h
//  Clock4
//
//  Created by  on 2012-06-13.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface ClockWindow : NSWindow
{
    IBOutlet NSTextField* label;
}
- (void) tick:(id)sender;
@end


@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet ClockWindow* window;

@end

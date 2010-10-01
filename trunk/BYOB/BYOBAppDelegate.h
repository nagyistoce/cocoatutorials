//
//  BYOBAppDelegate.h
//  BYOB
//
//  Created by Robin Mills on 2/9/10.
//  Copyright 2010 RMSC, San Jose, CA, USA. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface BYOBAppDelegate : NSObject <NSApplicationDelegate> {
    NSWindow *window;
}

@property (assign) IBOutlet NSWindow *window;

@end

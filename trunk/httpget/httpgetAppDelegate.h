//
//  httpgetAppDelegate.h
//  httpget
//
//  Created by Robin Mills on 8/5/10.
//  Copyright 2010 RMSC, San Jose, CA, USA. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface httpgetAppDelegate : NSObject <NSApplicationDelegate> {
    NSWindow *window;
}

@property (assign) IBOutlet NSWindow *window;

@end

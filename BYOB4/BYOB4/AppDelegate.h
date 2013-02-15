//
//  AppDelegate.h
//  BYOB4
//
//  Created by Robin Mills on 13-2-9.
//  Copyright (c) 2013 Robin Mills. All rights reserved.
//
#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>

@interface AppDelegate : NSObject <NSApplicationDelegate> {
    IBOutlet NSTextField* addressField ;
    IBOutlet WebView* webView ;
}

@property (assign) IBOutlet NSWindow *window;

- (IBAction) go : (id) sender ;

@end

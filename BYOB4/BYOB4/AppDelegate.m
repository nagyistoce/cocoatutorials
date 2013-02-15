//
//  AppDelegate.m
//  BYOB4
//
//  Created by Robin Mills on 13-2-9.
//  Copyright (c) 2013 Robin Mills. All rights reserved.
//

#import "AppDelegate.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application
}

- (void) awakeFromNib {
    [addressField setStringValue:@"http://clanmills.com"] ;
}

- (void) go :(id) sender
{
    NSLog(@"go baby go");
    NSString* address = [addressField stringValue] ;
    [[webView mainFrame] loadRequest:
     [NSURLRequest requestWithURL:[NSURL URLWithString: address ]
      ]];
}

@end

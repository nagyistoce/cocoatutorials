//
//  BYOBcontroller.h
//  BYOB
//
//  Created by Robin Mills on 2/9/10.
//  Copyright 2010 RMSC, San Jose, CA, USA. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import <WebKit/WebKit.h>


@interface BYOB4controller : NSObject {
	IBOutlet NSTextField* addressField ;
	IBOutlet WebView*     webView      ;
}

- (IBAction) go : (id) sender ;

@end

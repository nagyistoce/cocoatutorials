//
//  BYOBcontroller.mm
//  BYOB
//
//  Created by Robin Mills on 2/9/10.
//  Copyright 2010 RMSC, San Jose, CA, USA. All rights reserved.
//

#import "BYOB4controller.h"


@implementation BYOB4controller

- (void) awakeFromNib {
	[addressField setStringValue:@"http://clanmills.com"] ;
}

- (IBAction) go : (id) sender 
{
	NSString* address = [ addressField stringValue] ;
	NSLog(@"%@",address) ;
	// display the address in the webview
    [[webView mainFrame] loadRequest:
	 [NSURLRequest requestWithURL:[NSURL URLWithString: address ]
	]];
}


@end

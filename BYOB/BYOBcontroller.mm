//
//  BYOBcontroller.mm
//  BYOB
//
//  Created by Robin Mills on 2/9/10.
//  Copyright 2010 RMSC, San Jose, CA, USA. All rights reserved.
//

#import "BYOBcontroller.h"
#import "../myLibrary/alerts.h"


@implementation BYOBcontroller

- (void) awakeFromNib {
	[ addressField setStringValue:@"http://clanmills.com"] ;
}

- (IBAction) go : (id) sender 
{
	NSString* address = [ addressField stringValue] ;
	NSLog(@"%@",address) ;
#if 0
	// sniff for http:// and if missing, update the UI and the address
	NSString* http   = @"http://" ;
	UInt	  http_l = [http length];
	bool bHttp = [address length] > http_l ;
	if ( bHttp ) bHttp = [[address substringToIndex:http_l] 
						  caseInsensitiveCompare: http] == NSOrderedSame;
	if (!bHttp ) {
		// update the UI
		NSString* newAddress = [[NSString alloc]initWithFormat:@"%@%@",http,address] ;
		[addressField setStringValue:newAddress];
		[newAddress release] ;
		// reread address from the control
		address = [ addressField stringValue] ;
		NSLog(@"rewritten as %@",address) ;
	}
#endif	
	// display the address in the webview
    [[webView mainFrame] loadRequest:
	 [NSURLRequest requestWithURL:[NSURL URLWithString: address ]
	]];
}


@end

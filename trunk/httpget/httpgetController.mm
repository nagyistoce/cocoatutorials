//
//  httpgetController.mm
//  httpget
//
//  Created by Robin Mills on 8/5/10.
//  Copyright 2010 RMSC, San Jose, CA, USA. All rights reserved.
//

#import "httpgetController.h"
#import <Cocoa/Cocoa.h>
#import <Foundation/Foundation.h>

@implementation httpgetController

@synthesize bFinished ;
@synthesize data	  ;
@synthesize body	  ;

- (void) awakeFromNib
{
	url.stringValue = @"http://clanmills.com" ;
	bFinished = NO ;
}

- (void) connection : (NSURLConnection*) aConnection
 didReceiveResponse : (NSURLResponse*) aResponse
{
	bFinished = NO ;
    [data setLength:0];
	body = @"" ;
	response.stringValue = body ;
	NSLog(@"didReceiveResponse");
}

- (void) connection : (NSURLConnection*) aConnection
	 didReceiveData : (NSData*) aData
{
	[data  appendData:aData];
	NSLog(@"didReceiveData length = %d,%d",[aData length],[data length]);
	progress.stringValue = [NSString stringWithFormat:@"%d",[data length]];
}

- (void) connectionDidFinishLoading : (NSURLConnection*) aConnection
{
	bFinished = YES ;
	NSLog(@"connectionDidFinishLoading");
	[body release];
	body  = [[NSString alloc ]initWithData: data encoding: NSUTF8StringEncoding];
	response.stringValue = body ;
}

- (IBAction) go: (id) sender 
{
	NSLog(@"go for %@",url.stringValue);

	// Create the request.
	NSURLRequest* theRequest=[NSURLRequest requestWithURL:[NSURL URLWithString:url.stringValue]
											  cachePolicy:NSURLRequestReloadIgnoringLocalAndRemoteCacheData
										  timeoutInterval:60.0];

	NSURLConnection* theConnection=[[NSURLConnection alloc] initWithRequest:theRequest delegate:self];
	if (theConnection) {
		data = [[NSMutableData data] retain];
		// [theConnection start];
	} else {
		NSLog(@"unable to create theConnection");
	}
	
}


@end

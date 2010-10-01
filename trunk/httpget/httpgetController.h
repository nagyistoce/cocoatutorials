//
//  httpgetController.h
//  httpget
//
//  Created by Robin Mills on 8/5/10.
//  Copyright 2010 RMSC, San Jose, CA, USA. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface httpgetController : NSWindowController {
//	outlets
	IBOutlet NSTextField* url		;
	IBOutlet NSTextField* progress	;
	IBOutlet NSTextField* response	;

// properties	
	BOOL			bFinished	;
	NSMutableData*	data		;
	NSString*		body		;
}

// actions
- (IBAction) go : (id) sender ;

// property getters/setters
@property (readwrite,retain)	NSMutableData*	data		;
@property						BOOL			bFinished	;
@property (readwrite,copy)		NSString*		body		;

@end

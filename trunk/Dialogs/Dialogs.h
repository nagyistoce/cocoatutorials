//
//  Dialogs.h
//  D
//
//  Created by Robin Mills on 4/27/10.
//  Copyright 2010 RMSC, San Jose, CA, USA. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "DialogBoxController.h"

extern "C" NSString* nIconPathChanged ;

@interface Dialogs : NSWindowController {
	// outlets
	IBOutlet NSImageView*   imageView ;
	
	// properties
	DialogBoxController*	modelessDialog	;
	DialogBoxController*	modalDialog		;
	NSNotificationCenter*	nc				;
	NSImageView*			dockImageView	;
	NSString*				imagePath		;
}
// actions
- (IBAction) showModeless:(id)sender;
- (IBAction) showModal:(id)sender;
- (IBAction) showSheet:(id)sender;

// property getters/setters

// methods

@end

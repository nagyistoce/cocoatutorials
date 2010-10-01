//
//  DialogBoxController.h
//  D
//
//  Created by Robin Mills on 4/27/10.
//  Copyright 2010 RMSC, San Jose, CA, USA. All rights reserved.
//

#import <Cocoa/Cocoa.h>


@interface DialogBoxController : NSWindowController
	<NSWindowDelegate>
{
	// outlets
	IBOutlet NSTextField*	textField ;
	IBOutlet NSImageView*	imageView ;
	IBOutlet NSTextField*	imageField;
	
	// properties
	NSNotificationCenter*	nc		  ;
	NSString*				imagePath ;
}
// actions
- (IBAction) closeButton:(id)sender;
- (IBAction) imageFile:(id)sender;
- (IBAction) alert:(id)sender;

// property getters/setters

- (void) setImagePath:(NSString*)aImagePath;

@end

//
//  DialogBoxController.mm
//  D
//
//  Created by Robin Mills on 4/27/10.
//  Copyright 2010 RMSC, San Jose, CA, USA. All rights reserved.
//

#import "DialogBoxController.h"
#import "Dialogs.h"

@implementation DialogBoxController

- (void) dealloc
{
	[nc release];
	[imagePath release];
	[super dealloc];
}

- (void) iconPathChanged:(NSNotification*)note
{
	NSLog(@"%s iconPathChanged %@",__FILE__,note);
	[self setImagePath:[note object]];
}


- (id) init
{
	self = [super initWithWindowNibName:@"DialogBox"];
	NSLog(@"DialogBoxController init self = %@",self);
	
	nc = [[NSNotificationCenter defaultCenter]retain] ;
	[nc addObserver:self
		   selector:@selector(iconPathChanged:)
			   name:nIconPathChanged
			 object:nil
	];
	imagePath = nil;
	
	return self ;
}

- (void) setImagePath: (NSString*) aImagePath
{
	@try {
		[aImagePath retain];
		[imagePath release];
		imagePath = aImagePath;
		
		[imageField setStringValue:imagePath];
		NSImage *image = [[NSImage alloc] initWithContentsOfFile:imagePath];
		[imageView setImage:image];
	}
	@catch (NSException * e) {
		NSLog(@"e = %@",e);
	}
	@finally {
		//
	}
}

- (BOOL) windowShouldClose:(id)sender
{
	NSLog(@"window should close");
	[self closeButton:sender];
	return YES;
}

- (IBAction) closeButton:(id)sender 
{
	NSLog(@"closeButton");

	int result = 12345 ;
	[NSApp stopModalWithCode:result]; // not convinced this does anything
	[NSApp endSheet:[self window] returnCode:result];
	
	[[self window] orderOut:sender];
	[[self window] close] ;
}

- (void) openPanelDidEnd:(NSOpenPanel *)op
			 returnCode:(int)returnCode
			contextInfo:(void *)ci
{
	if (returnCode == NSOKButton) {
		NSString* path = [op filename];
		[nc postNotificationName:nIconPathChanged object:path];
	}
}

- (IBAction) imageFile:(id)sender
{

	NSOpenPanel *op = [NSOpenPanel openPanel];
	
	[op beginSheetForDirectory: imagePath
						  file: [imagePath lastPathComponent]
						 types: [NSImage imageFileTypes]
				modalForWindow: [self window]
				 modalDelegate: self
				didEndSelector: @selector(openPanelDidEnd:returnCode:contextInfo:)
				   contextInfo: nil
	];
	
	NSLog(@"imageFile pressed currentString = %@",imagePath);
}

- (IBAction) alert:(id)sender
{
	NSLog(@"alert pressed");
}


@end

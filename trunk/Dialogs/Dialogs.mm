//
//  Dialogs.mm
//
//  Created by Robin Mills on 4/27/10.
//  Copyright 2010 RMSC, San Jose, CA, USA. All rights reserved.
//

#import "Dialogs.h"

NSString* nIconPathChanged = @"iconPathChanged" ;

@implementation Dialogs

- (void) dealloc
{
	[nc release];
	[modelessDialog release];
	[modalDialog    release];
	[dockImageView  release];
	[imagePath		release];
	[super dealloc];
}

- (void) setImagePath:(NSString*) aImagePath
{
	@try {
		[aImagePath retain] ;
		[imagePath release];
		imagePath = aImagePath;
	
		NSImage* image = [[NSImage alloc] initWithContentsOfFile:imagePath];
		if ( image ) {
			[NSApp setApplicationIconImage: image];
			[imageView setImage:image];
		}
	}
	@catch (NSException * e) {
		NSLog(@"e = %@",e);
	}
	@finally {
		//
	}
	
}

- (void) iconPathChanged:(NSNotification*)note
{
	NSLog(@"dialogs.mm iconPathChanged %@",note);
	[self setImagePath: [note object]] ;

}

- (id) init
{
	self = [ super init] ;
	modalDialog	   = nil ;
	modelessDialog = nil ;
	dockImageView  = nil ;
	imagePath	= nil ;
	
	nc = [[NSNotificationCenter defaultCenter]retain] ;
	[nc addObserver:self
		   selector:@selector(iconPathChanged:)
			   name:nIconPathChanged
			 object:nil
	];

	// set up our default preferences
	NSMutableDictionary* dict = [[NSMutableDictionary alloc]init] ;
	[dict setObject:@"please type something here" forKey:@"typeHereString"] ;
	
	[[NSUserDefaultsController sharedUserDefaultsController] setInitialValues:dict];
	[[NSUserDefaultsController sharedUserDefaultsController] setAppliesImmediately:YES];
	
	////
	// find our bundle's icon image
	NSDictionary* infoDict	= [[NSBundle mainBundle] infoDictionary];
	NSString*	  iconFile	= infoDict  ? [NSMutableString stringWithString:[infoDict objectForKey:@"CFBundleIconFile"]] : nil ;
	NSString*	  iconPath	= iconFile  ? [NSString stringWithFormat:@"%@/%@.icns",[[NSBundle mainBundle]resourcePath],iconFile] : nil ;
	NSLog(@"iconPath = %@",iconPath);
	[self setImagePath:iconPath];
	
	return self ;
}

- (IBAction) showModeless:(id)sender
{
	NSLog(@"showModeless");
	if ( !modelessDialog ) {
		NSLog(@"showDialog alloc init");
		modelessDialog = [[DialogBoxController alloc]init] ;
	}
	
	if ( modelessDialog ) {
		NSLog(@"showDialog showWindow");
		[modelessDialog showWindow:self];
		[modelessDialog setImagePath:imagePath];
	}
}

- (void) createModalDialog
{
	if ( !modalDialog ) {
		NSLog(@"showSheet alloc init");
		modalDialog = [[DialogBoxController alloc]init] ;
		
		if ( modalDialog ) {
			// I've we dont do this, things are happy
			// I think it has something to do the default size of the box
			[modalDialog showWindow:self];
			[[modalDialog window]setDelegate:modalDialog];
			[modalDialog close];
		}
		if ( modalDialog ) {
			[modalDialog setImagePath:imagePath];
		}
	}
}

- (IBAction) showModal:(id)sender
{
	NSLog(@"showModal");
	[self createModalDialog];
	
	if ( modalDialog ) {
		NSLog(@"showModal runModalForWindow");
		int result = [NSApp runModalForWindow:[modalDialog window]];
		NSLog(@"result = %d",result) ;
	}
}

- (void) sheetDidEnd : (DialogBoxController*) sender
		  returnCode : (int) code
		 contextInfo : (void *)context
{
	NSLog(@"sheetDidEnd returnCode = %d",code);
}

- (IBAction) showSheet:(id)sender
{
	NSLog(@"showSheel");
	[self createModalDialog ];
	
	if ( modalDialog ) {
		NSLog(@"showSheet beinSheet");
		[ NSApp beginSheet : [modalDialog window]
			modalForWindow : [self window]
			 modalDelegate : self
			didEndSelector : @selector(sheetDidEnd:returnCode:contextInfo:)
			   contextInfo : modalDialog
		 ];
	}
}

@end

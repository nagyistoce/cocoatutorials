//
//  PrefDocController.h
//
//  Created by Robin Mills on 5/3/10.
//  Copyright 2010 RMSC, San Jose, CA, USA. All rights reserved.
//

#import <Cocoa/Cocoa.h>

// to disable table sorting:
// http://lists.apple.com/archives/Cocoa-dev/2003/Nov/msg00832.html
//

@interface PrefDocController : NSDocument
{
	// outlets
	IBOutlet NSTableView* tableView ;
	IBOutlet NSArrayController* arrayController;
	IBOutlet NSButton*	runAtLoginButton ;
	IBOutlet NSButton*	runInDockButton  ;
	
	// properties
	NSMutableArray* menuItems;
	NSString*		jsonPath;
	NSString*		defsPath;
	BOOL			runInDock;
	BOOL			runAtLogin;
	
	NSDate*			jsonModDate;
	NSFileManager*	fm;
}

// actions
- (IBAction) up       : (id) sender ;
- (IBAction) down     : (id) sender ;
- (IBAction) test     : (id) sender ;
- (IBAction) save     : (id) sender ;
- (IBAction) open     : (id) sender ;
- (IBAction) defaults : (id) sender ;
- (IBAction) apply    : (id) sender ;
- (IBAction) textedit : (id) sender ;

// methods
- (void) jsonRead;
- (void) jsonReadPath:(NSString*)path;

// property setters/getters
@property (readwrite,retain) NSMutableArray*	menuItems;
@property (readwrite,retain) NSString*			jsonPath;
@property (readwrite,retain) NSString*			defsPath;
@property (readwrite)		 BOOL				runAtLogin;
@property (readwrite)		 BOOL				runInDock;
@property (readwrite,copy)	 NSDate*			jsonModDate;

@end

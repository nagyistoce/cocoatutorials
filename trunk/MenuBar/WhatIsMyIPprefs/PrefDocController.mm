//
//  PrefDocController.mm
//
//  Created by Robin Mills on 5/3/10.
//  Copyright 2010 RMSC, San Jose, CA, USA. All rights reserved.
//

#import "PrefDocController.h"
#import "MenuItem.h"
#import "JSON.h"
#import "WhatIsMyIPprefsPref.h"

#include "../SharedCode/alerts.h"
#include "../SharedCode/common.h"

@implementation PrefDocController

@synthesize menuItems;
@synthesize jsonPath;
@synthesize defsPath;
@synthesize runAtLogin;
@synthesize runInDock;
@synthesize jsonModDate;


#pragma mark ---- object life management

- (void) dealloc
{
	[menuItems release];
	[jsonPath release];
	[defsPath release];
	[jsonModDate release];
	[fm release];

	[super dealloc];
}

- (id) init
{
	self = [super init];
	[self setFileType:MENU_FILETYPE];
	menuItems = [[NSMutableArray alloc]init];
	jsonPath  = [[NSString alloc]initWithString:[NSString stringWithFormat:@"%@/Library/Preferences/WhatIsMyIP.json",NSHomeDirectory()]];
	defsPath  = [[NSString alloc]initWithString:[NSString stringWithFormat:@"%@/Library/Preferences/WhatIsMyIP.defaults",NSHomeDirectory()]];
	fm		  = [NSFileManager defaultManager];
	jsonModDate = [NSDate distantPast];
	return self ;
}

- (BOOL) windowShouldClose:(id)sender
{
	NSLog(@"windowShouldClose");
	return [self isDocumentEdited]?NO:YES;
}

- (void) awakeFromNib
{
	theDocController = self ;
	initializeEventStream(@"~/Library/Preferences/",self,@selector(jsonRead));
	[self jsonRead];
}

#pragma mark ---- menu management
- (IBAction) upDown : (id) sender up:(BOOL) up
{
	int inc = up ? -1 : +1 ;
	NSUInteger index =[tableView selectedRow];
	if ( (up&&index > 0) || (index < ([menuItems count]-1)) ) {
		NSInteger row = index+inc;
		NSIndexSet* indexes = [NSIndexSet indexSetWithIndex:row];
		[menuItems exchangeObjectAtIndex:index withObjectAtIndex:row];
		[tableView selectRowIndexes:(NSIndexSet *)indexes byExtendingSelection:NO];
	}
//	NSLog(@"up count = %d selectedRow = %d",[menuItems count],index);
}

- (IBAction) up       : (id) sender 
{
	[self upDown:sender up:YES];
}

- (IBAction) down      : (id) sender 
{
	[self upDown:sender up:NO];
}

#pragma mark ---- json 
- (NSNumber*) optionalInt: (NSDictionary*) menuDict key:(NSString*) key def:(BOOL)def
{
	int result = def;
	id  value  = [menuDict objectForKey:key];
	if ( value ) result = [value intValue]?YES:NO;
	return [NSNumber numberWithInt:result] ;
}

- (NSNumber*) optionalSep: (NSDictionary*) menuDict key:(NSString*) key def:(BOOL) def
{
	int result = def ;
	id value = [menuDict objectForKey:key];
	if ( value ) {
		result = [value intValue]?YES:NO;
	} else {
		NSMutableString* title = [NSMutableString stringWithString:[menuDict objectForKey:@"Title"]];
		[title	replaceOccurrencesOfString:@"-"
							   withString:@"" 
								  options:NSLiteralSearch
									range:NSMakeRange( 0,[title length])
		];
		if ( ![title length] ) result = YES ; // if there's nothing there
	}

	return [NSNumber numberWithInt:result] ;
}

- (BOOL) rebuildDocument:(NSDictionary*) dict
{
	if ( !dict ) return NO ;
	NSArray* jsonArray = nil ;
	////
	// change the jsonArray if it smells good
	if ( [[[dict class]description]isEqualTo:@"NSCFDictionary"]) {
		id array = [dict objectForKey:@"Menu"];
		if ([[[array class]description]isEqualTo:@"NSCFArray"]) {
			jsonArray = array ;
		}
	}
	if ( !jsonArray ) return NO ;
	
	////
	// remove everything from the old menu
	while ([menuItems count]) [menuItems removeLastObject];
	
	////
	// cycle over the jsonArray and set up the new document
	for ( NSInteger m = 0 ; m < [jsonArray count] ; m++ ) {
		NSDictionary* menuDict = [jsonArray objectAtIndex:m];
		MenuItem* menuItem		= [[MenuItem alloc]init] ;
		[menuItem setTitle		: [menuDict objectForKey:@"Title"]];
		[menuItem setCmd		: [menuDict objectForKey:@"Cmd"]];
		[menuItem setShowresult	: [self optionalInt:menuDict key:@"Show" def:YES]] ;
		[menuItem setEnabled	: [self optionalInt:menuDict key:@"Enabled" def:YES]];
		[menuItem setReadonly	: [self optionalInt:menuDict key:@"Readonly" def:NO]];
		[menuItem setSeparator	: [self optionalSep:menuDict key:@"Separator" def:NO]];
		[menuItems addObject:menuItem];
	}
	[arrayController setContent:menuItems];
	// I couldn't get this to work
	[tableView reloadData]; 
	
	runInDock  =  [[self optionalInt:dict key:@"RunInDock"  def:NO ]intValue]; 
	runAtLogin =  [[self optionalInt:dict key:@"RunAtLogin" def:YES]intValue]; 
	[runInDockButton  setIntValue:runInDock];
	[runAtLoginButton setIntValue:runAtLogin];
	
	return YES ;
}

- (void) jsonReadPath:(NSString*) path;
{
	NSStringEncoding	enc;
	NSError*			error;
	
	bool			bRead = path ? true : false;
	if ( !path ) {

		path = jsonPath;
		NSDictionary*	fileAttributes	= [fm attributesOfItemAtPath:jsonPath error:NULL];
		NSDate*			fileModDate		= [fileAttributes objectForKey:NSFileModificationDate];
	
		////
		// reread the JSON file if it has changed (older or newer, but different)
		if ( [fileModDate compare:[self jsonModDate]] != NSOrderedSame)
		{
			bRead = true ;
			[self setJsonModDate:fileModDate] ;
		}
	}
	
	if ( bRead ) {
		NSString* jsonString = [
							NSString stringWithContentsOfFile: path
							usedEncoding: &enc   error: &error
							];

		// NSLog(@"string = %@",jsonString);
		id   dict = [jsonString JSONValue];
		if ( dict ) {
			[self rebuildDocument:dict];
		}
	//	NSLog(@"jsonRead %s %@",dict?"GOOD":"BAD",path) ;
	//	else alert(@"not a good document!");
	}
}

- (void) jsonRead
{
	[self jsonReadPath:nil];
}

- (void) jsonWrite: (NSString*) path
{
	SBJsonWriter* writer = [[SBJsonWriter alloc]init];
	NSDictionary* d = [NSDictionary dictionaryWithObjectsAndKeys
	:	[NSNumber numberWithInt:[runInDockButton intValue]]		,@"RunInDock"
	,	[NSNumber numberWithInt:[runAtLoginButton intValue]]	,@"RunAtLogin"
	,	@"WhatIsMyIP"											,@"Name"
	,	@"Quit"													,@"Quit"
	,	@"About..."												,@"About"
	,	[[NSDate date]description]								,@"Written"
	,	menuItems												,@"Menu"
	,	nil
	];
	[writer setHumanReadable:YES];
	NSString* json = [writer stringWithObject: d] ;
	// alert(json);
	
	// NSURL* url = [NSURL URLWithString:[NSString stringWithFormat:@"file://%@",path]];
	NSStringEncoding enc = NSUTF8StringEncoding ; // [NSStringEncoding NSUTF8StringEncoding] ;
	NSError*  error ;
	[json writeToFile:path atomically: YES encoding: enc error: &error];
	[writer release];
}

#pragma mark ---- command buttons
- (IBAction) apply : (id) sender 
{
	NSLog(@"apply");
	[self jsonWrite:jsonPath];
}

- (IBAction) test     : (id) sender 
{
	int row = [tableView selectedRow];
	if ( row >= 0 && row < [menuItems count] ) {
		MenuItem* mi = [menuItems objectAtIndex:[tableView selectedRow]];
		NSString* cmd = [mi cmd];
		NSLog  (@"cmd = %@",cmd);
		NSString* result = [thePrefPanel invoke:cmd];
		NSLog  (@"result = %@",result);
		if ( [[mi showresult] intValue]) alert(result);
	}
}

- (IBAction) save     : (id) sender 
{
	NSLog(@"save");
	// [menuItems writeToURL:[NSURL URLWithString:@"file:///Users/rmills/shit"] atomically:YES];
	[self saveDocument:sender];
}


#if 0
	[self updateChangeCount:NSChangeDone];
	SBJsonWriter* writer = [[SBJsonWriter alloc]init];
	[writer setHumanReadable:YES];
	/*
	 NSMutableString* json = [NSMutableString stringWithCapacity:100];
	 
	 for ( NSUinteger i = 0 ; i < [menuItems count] ; i++ ) {
	 MenuItem* menuItem = [menuItems objectAtIndex:i];
	 [json appendString:[menuItem jsonString];
	 }
	 */
	/*
	 NSDictionary* dict = [ NSDictionary dictionaryWithObjectsAndKeys
	 :  [NSNumber numberWithInt:1],@"One"
	 , @"Two"   ,@"key: two"
	 , @"Buckle",@"key: my shoe"
	 , nil
	 ] ;
	 */
	NSString* json = [menuItems count]?[writer stringWithObject: menuItems]:@"no items" ;
	alert(json);
#endif

#if 0
	NSStringEncoding	enc;
	NSError*			error;
	NSString* jsonString = [
							NSString stringWithContentsOfFile: jsonPath
							usedEncoding: &enc   error: &error
							];
	// NSLog(@"string = %@",jsonString);
	id dict = [jsonString JSONValue];
	if ( ! [self rebuildDocument:dict] )
	alert(@"not a good document!");
#endif


- (void) openPanelDidEnd:(NSOpenPanel *)op
			  returnCode:(int)returnCode
			 contextInfo:(void *)ci
{
	if (returnCode == NSOKButton) {
		NSString *path = [op filename];
		[self jsonReadPath:path];
	}
}

- (IBAction)open:(id)sender;
{
	NSLog(@"open");

	NSOpenPanel *op = [NSOpenPanel openPanel];
	
	[op beginSheetForDirectory:nil
						  file:nil
						 types:nil
				modalForWindow:[sender window]
				 modalDelegate:self
				didEndSelector:@selector(openPanelDidEnd:returnCode:contextInfo:)
				   contextInfo:self];
}

- (IBAction) defaults : (id) sender 
{
	NSString* cmd = [NSString stringWithFormat:@"cp \"%@\" \"%@\"",jsonPath,defsPath];
	[thePrefPanel invoke:cmd];
	[self jsonRead];
}

- (IBAction) textedit : (id) sender 
{
	// I'm not calling this from the UI
	// For reasons I can't understand it causes the Pref panel to "freeze" until the user terminates textedit
	//	NSString* cmd = [NSString stringWithFormat:@"/Applications/TextEdit.app/Contents/MacOS/TextEdit \"%@\" &",jsonPath];
	//	alert(cmd);
	NSString* cmd = [NSString stringWithFormat:@"open -e \"%@\" &",jsonPath];
	[thePrefPanel invoke:cmd];
}

- (NSString *)windowNibName {
    // Implement this to return a nib to load 
	// OR implement -makeWindowControllers to manually create your controllers.
    return @"PrefDocController";
}

#pragma mark ---- read/write for cocoa

- (NSData *) dataOfType : (NSString *)typeName error : (NSError **)outError
{
    // [[self delegate]:updateChangeCount:NSChangeDone];
    // [[self delegate]:updateChangeCount:NSChangeCleared];
    return 	[NSKeyedArchiver archivedDataWithRootObject:menuItems];
}

- (BOOL) readFromData : (NSData *) data ofType : (NSString *) typeName error : (NSError **) outError
{
	[self updateChangeCount:NSChangeCleared];
	[self setMenuItems:[NSKeyedUnarchiver unarchiveObjectWithData:data]];
    return YES;
}

//- (BOOL) isDocumentEdited
//{
//	return YES;
//}

@end

//
//  MyDocument.mm
//  This file is part of Diary
// 
//  Diary is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Diary is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with Diary.  If not, see <http://www.gnu.org/licenses/>.
//
//  Written by Robin Mills, San Jose, CA, USA. 2010 http://clanmills.com
//
//

#import "MyDocument.h"
#import "EventsView.h"
#import "Event.h"

@implementation MyDocument
@synthesize events ;
@synthesize ovents ;

#pragma mark -------- alloc/dealloc

- (id)init
{
    self = [super init];
    if (self) {
		events = [[NSMutableArray alloc]init] ;
		ovents = [[NSArray alloc]init];
    }
    return self;
}

- (void) dealloc
{
	[events release];
	[ovents release];
	[super dealloc];
}

- (void) awakeFromNib
{
	[fontNameField setStringValue:@"Monaco"];
	[fontSizeField setIntValue:12];
}

- (NSString*) windowNibName
{
    // Override returning the nib file name of the document
    // If you need to use a subclass of NSWindowController or if your document supports multiple NSWindowControllers, you should remove this method and override -makeWindowControllers instead.
    return @"MyDocument";
}

- (void) windowControllerDidLoadNib : (NSWindowController*) aController
{
    [super windowControllerDidLoadNib:aController];
    // Add any code here that needs to be executed once the windowController has loaded the document's window.
}

#pragma mark -------- read/write

- (BOOL) isDocumentEdited
{
	bool same = [events count ] == [ovents count] ? YES : NO  ;
	for ( NSUInteger i = 0 ; same && i < [events count] ; i++ ) {
		Event* a = [events objectAtIndex:i] ;
		Event* b = [ovents objectAtIndex:i] ;
		same = [ a isEqualTo:b] ;
	}
	return same?NO:YES ;
}

- (NSData*) dataOfType : (NSString *) typeName error : (NSError **) outError
{
    // Insert code here to write your document to data of the specified type.
    if ( outError != NULL ) {
		*outError = [NSError errorWithDomain:NSOSStatusErrorDomain code:unimpErr userInfo:NULL];
	}
	[ovents release];
	ovents = [[NSArray alloc]initWithArray:events];
	
	return [NSKeyedArchiver archivedDataWithRootObject:events];;
}

- (BOOL) readFromData : (NSData *) data ofType : (NSString *) typeName error : (NSError **) outError
{
    // Insert code here to read your document from the given data of the specified type.
    if ( outError != NULL ) {
		*outError = [NSError errorWithDomain:NSOSStatusErrorDomain code:unimpErr userInfo:NULL];
	}
	[self setEvents:[NSKeyedUnarchiver unarchiveObjectWithData:data]];

	[ovents release];
	ovents = [[NSArray alloc]initWithArray:events];
	
    return YES;
}

#pragma mark -------- printing

- (NSPrintOperation*) printOperationWithSettings : (NSDictionary*) printSettings
										   error : (NSError**) outError
{
	NSString* fontName	= [fontNameField stringValue] ;
	NSInteger fontSize	= [fontSizeField integerValue];
	EventsView* view	= [[EventsView alloc]initWithEvents:events
											fontWithName:fontName
													size:fontSize
	];
	
	NSPrintInfo*		printInfo	= [self printInfo] ;
	NSPrintOperation*	printOp		= [NSPrintOperation
			printOperationWithView:view
			printInfo: printInfo
	];
	return printOp ;
}

#pragma mark -------- font

- (void) reflectFont
{
	if ([NSFontPanel sharedFontPanelExists] == NO) 	return;
	
	NSString* fontName	= [fontNameField stringValue];
	NSFont*   font		= [NSFont fontWithName:fontName size:12];
	
	[[NSFontPanel sharedFontPanel] setPanelFont:font isMultiple:NO] ;
}

- (void) changeFont : (id) sender
{
	NSLog(@"changeFont");
	
	NSFont* newFont = [NSFont fontWithName:@"Courier" size:10] ;
	newFont = [ sender convertFont: newFont];
	[fontNameField setStringValue:[newFont fontName]];
	[fontSizeField setIntValue:[newFont pointSize]];
}

// actions
- (IBAction) showFontPanel : (id) sender;
{
	NSLog(@"font");
	[[NSFontPanel sharedFontPanel] makeKeyAndOrderFront: self];
	[[NSFontManager sharedFontManager] setDelegate: self];
	[self reflectFont] ;
}

@end

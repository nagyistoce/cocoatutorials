//
//  Controller.m
//  This file is part of Favorites
// 
//  Favorites is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Favorites is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with Favorites.  If not, see <http://www.gnu.org/licenses/>.
//
//  This file is based on original work by Prince Kumar, Mumbai, India
//  And modified by Robin Mills, San Jose, CA 2010 http://clanmills.com

#import "Controller.h"

#define FAV_BOOK  @"FavBook"
#define FAV_CITY  @"FavCity"
#define FAV_COLOR @"FavColor"
#define FAV_FOOD  @"FavFood"
#define FAV_IMAGE @"FavImage"
#define FAV_RED   @"FavRed"
#define FAV_GREEN @"FavGreen"
#define FAV_BLUE  @"FavBlue"

#define lengthof(x) sizeof(x)/sizeof(x[0])

@implementation Controller
- (id) init
{
	[super init];
	
	NSMutableDictionary * defaultprefs = [NSMutableDictionary dictionary];
	
	NSString* favImage = [NSString stringWithFormat:@"%@%s",[[NSBundle mainBundle]resourcePath],"/clanmills.gif"];
	
	[defaultprefs setObject:favImage forKey:FAV_IMAGE];
	[defaultprefs setObject:@"Cocoa Programming for Mac® OS X" forKey:FAV_BOOK];
	[defaultprefs setObject:@"San Francisco" forKey:FAV_CITY];
	[defaultprefs setObject:@"Clanmills" forKey:FAV_COLOR];
	[defaultprefs setObject:@"Mexican" forKey:FAV_FOOD];

	[defaultprefs setObject:[NSNumber numberWithDouble:0.0] forKey:FAV_RED];
	[defaultprefs setObject:[NSNumber numberWithDouble:0.6] forKey:FAV_GREEN];
	[defaultprefs setObject:[NSNumber numberWithDouble:1.0] forKey:FAV_BLUE];
	
	[defaultprefs retain];
	prefs = [[NSUserDefaults standardUserDefaults] retain];
	[self setMustClose:NO];
	return self;
}

- (BOOL) hasNotChanged
{
	return	[[bookField stringValue	] isEqualToString:[prefs stringForKey:FAV_BOOK	]]
	&&		[[cityField stringValue	] isEqualToString:[prefs stringForKey:FAV_CITY	]]
	&&		[[foodField stringValue	] isEqualToString:[prefs stringForKey:FAV_FOOD	]]
	&&		[[colorField stringValue] isEqualToString:[prefs stringForKey:FAV_COLOR	]]
	&&      [imageName                isEqualToString:[prefs stringForKey:FAV_IMAGE	]]
	&&		red		== [[prefs valueForKey:FAV_RED] doubleValue]
	&&		green	== [[prefs valueForKey:FAV_GREEN] doubleValue]
	&&		blue	== [[prefs valueForKey:FAV_BLUE] doubleValue]
	;
}

- (BOOL) hasChanged { return ![self hasNotChanged]; }

- (void) windowWillClose:(NSNotification *)notification
{
	NSLog(@"windowWillClose");
}

- (void) dealloc
{
	[prefs release];
	[imageName release];
	[super dealloc];
}

- (void) awakeFromNib
{
	NSColor* color;
	
	[NSApp		setDelegate:self];
	[MyWindow	setDelegate:self];	

	[bookField setStringValue:[prefs stringForKey:FAV_BOOK]];
	[cityField setStringValue:[prefs stringForKey:FAV_CITY]];
	[colorField setStringValue:[prefs stringForKey:FAV_COLOR]];
	[foodField setStringValue:[prefs stringForKey:FAV_FOOD]];
	[self setImageName:[prefs stringForKey:FAV_IMAGE]];
	
	red   = [[prefs stringForKey:FAV_RED]doubleValue];
	green = [[prefs stringForKey:FAV_GREEN]doubleValue];
	blue  = [[prefs stringForKey:FAV_BLUE]doubleValue];
	//	NSLog(@"r,g,b = %f %f %f",red,green,blue);
	color = [NSColor colorWithDeviceRed:red
							   green:green
								blue:blue
							   alpha:1.0
	];
	[colorWell setColor:color] ;
}

- (void) saveChanges : (id) sender
{
	NSLog(@"saveChanges");

	[prefs setObject:[bookField stringValue]  forKey:FAV_BOOK];
	[prefs setObject:[cityField stringValue]  forKey:FAV_CITY];
	[prefs setObject:[colorField stringValue] forKey:FAV_COLOR];
	[prefs setObject:[foodField stringValue]  forKey:FAV_FOOD];
	
	[prefs setObject:imageName                forKey:FAV_IMAGE];
	[prefs setObject:[NSNumber numberWithDouble:red]   forKey:FAV_RED];
	[prefs setObject:[NSNumber numberWithDouble:green] forKey:FAV_GREEN];
	[prefs setObject:[NSNumber numberWithDouble:blue]  forKey:FAV_BLUE];

//	NSLog(@"r,g,b = %f %f %f",red,green,blue);
	
	[MyWindow setDocumentEdited:NO];
}

- (void) alertDidEnd : (NSAlert *)alert
		  returnCode : (int) code
		 contextInfo : (void *)context
{
	if ( code == 1000 ) [self saveChanges:nil] ; // Save button
	bool bClose = code != 1001; // NOT Cancel button
	if ( bClose ) {
		[self setMustClose:YES];
		[MyWindow close] ;
	}
	NSLog(@"alertDidEnd returnCode = %d",code);
}

- (BOOL) canCloseDocument : (id) sender
{
	BOOL result = YES ;
    if ([self hasChanged]) {
		[MyWindow makeKeyAndOrderFront:nil];
		NSAlert *alert = [[[NSAlert alloc] init] autorelease];
		[alert addButtonWithTitle:@"Save"];
		[alert addButtonWithTitle:@"Cancel"];
		[alert addButtonWithTitle:@"Don't Save"];
		[alert setMessageText:@"Unsaved Changes"];
		[alert setInformativeText:@"You'll lose unsaved changes"];
		[alert setAlertStyle:NSWarningAlertStyle];
		[alert beginSheetModalForWindow:MyWindow
						  modalDelegate:self
						 didEndSelector:@selector(alertDidEnd:returnCode:contextInfo:)
							contextInfo:sender];
		
        result = NO ;
	}
	
	NSLog(@"canCloseDocument");
    return result;
}

- (BOOL) mustClose : (id) sender
{
	return mustClose ;
}

- (void) setMustClose : (BOOL) bMustClose
{
	mustClose = bMustClose ;
}

typedef struct {
	double r ;
	double g ;
	double b ;
	NSString* n;
} color_t,*color_p ;

static bool colorCompare(color_p c,id color)
{
	float d = fabs([color redComponent] - c->r)
			+ fabs([color greenComponent] - c->g)
			+ fabs([color blueComponent] - c->b)
			;
	return d < 0.3 ;
}

/*
 (RGB and CMY are complimentary C = 1.0 - R etc)
     R G B <-> C M Y Name
     -----     -----          
	 1  0  0     0  1  1 Red
     0  1  0     1  0  0 Green
     0  0  1     1  1  0 Blue
     0  1  1     1  0  0 Cyan
	 1  0  1     0  1  0 Magenta
	 1  1  0     0  0  1 Yellow
     0  0  0     1  1  1 Black
     1  1  1     0  0  0 White
     0 .5  0     1 .5  1 DarkGreen
	 0 .6  1     1 .5  1 Clanmills
*/

static NSString* colorName(id color)
{
	color_t    colors[] = 
	{ {1.0, 0.0, 0.0,@"Red"}
	, {0.0, 1.0, 0.0,@"Green"}
	, {0.0, 0.0, 1.0,@"Blue"}
	, {0.0, 1.0, 1.0,@"Cyan"}
	, {1.0, 0.0, 1.0,@"Magenta"}
	, {1.0, 1.0, 0.0,@"Yellow"}
	, {0.0, 0.0, 0.0,@"Black"}
	, {1.0, 1.0, 1.0,@"White"}
	, {0.0, 0.5, 0.0,@"DarkGreen"}
	, {0.0, 0.6, 1.0,@"Clanmills"}
	} ;

	NSString* result = @"Custom" ;
	for ( int c = 0 ; c < lengthof(colors) ; c++ ) {
		if ( colorCompare(&colors[c],color) ) result = colors[c].n ;
	}
	return result ;
}

- (IBAction) changeBackgroundColor:(id)sender
{
	NSColor* color = [sender color];
	[colorField setStringValue:colorName(color)];
	red = [color redComponent];
	green = [color greenComponent];
	blue = [color blueComponent];
//	NSLog(@"Color changed: %@ class = %@",color,[color class]) ;
}

- (BOOL) applicationShouldTerminateAfterLastWindowClosed : (NSApplication *) app
{
	return YES;
}

- (NSApplicationTerminateReply) applicationShouldTerminate : (NSApplication *) app
{
	NSLog(@"applicationShouldTerminate");
	if ( [self mustClose:app] ) return NSTerminateNow ;
	return [self canCloseDocument:app] ? NSTerminateNow : NSTerminateCancel ;
}

- (BOOL) windowShouldClose : (id) sender
{
	NSLog(@"windowShouldClose");
	if ( [ self mustClose:sender ] ) return YES ;
	
	BOOL result     = [self hasNotChanged] ;
	if ( result==NO ) [self canCloseDocument:sender] ;
	return result ;
}

- (IBAction) textFieldChanged : (id) sender
{
	NSLog(@"textFieldChanged");
//	[MyWindow setDocumentEdited:YES];
//	[self saveChanges:sender] ; // call this if you want changes applied immediately
}

- (void) setImageName : (NSString*) fileName
{
	NSImage *image = [[NSImage alloc] initWithContentsOfFile:fileName];
	[[imageView image] release];
	[imageView setImage: image ];

	[fileName retain];
	[imageName release];
	imageName = fileName ;
}

- (IBAction) insertpics : (id) sender
{
	NSArray *fileTypes = [NSArray arrayWithObjects:@"jpg", @"gif",@"png", @"psd", @"tga", nil];

	NSOpenPanel* oPanel = [NSOpenPanel openPanel];
	[oPanel setCanChooseDirectories:YES];		
	[oPanel setCanChooseFiles:YES];				
	[oPanel setCanCreateDirectories:YES];		
	[oPanel setAllowsMultipleSelection:NO];		
	[oPanel setAlphaValue:0.95];				
	[oPanel setTitle:@"Select a file to open"];
	
	if ( [oPanel runModalForDirectory:nil file:nil types:fileTypes] == NSOKButton )
	{
		NSArray* files = [oPanel filenames];

		for( int i = 0; i < [files count]; i++ )
		{
			NSString* fileName = [files objectAtIndex:i];
			NSLog(@"%@",fileName);
			[self setImageName:fileName];
		}
	}
}

@end

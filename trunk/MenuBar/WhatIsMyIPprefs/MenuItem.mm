//
//  MenuItems.mm
//  WhatIsMyIPprefs
//
//  Created by Robin Mills on 5/3/10.
//  Copyright 2010 RMSC, San Jose, CA, USA. All rights reserved.
//

#import "MenuItem.h"

@implementation MenuItem 

@synthesize title;
@synthesize cmd;
@synthesize enabled;
@synthesize readonly;
@synthesize showresult;
@synthesize separator;

- (void) dealloc
{
	[title release];
	[cmd release];
	[enabled release];
	[readonly release];
	[showresult release];
	[super dealloc];
}

- (id) proxyForJson
{
//	NSString* t = separator ? @"--" : title;
	NSString* t = title      ? title      : @"" ;
	NSString* c = cmd        ? cmd        : @"" ;
	NSNumber* s = showresult ? showresult : [NSNumber numberWithInt:0] ;
	NSNumber* e = enabled    ? enabled    : [NSNumber numberWithInt:1];
	NSNumber* r = readonly   ? readonly   : [NSNumber numberWithInt:0];
	NSNumber* x = separator  ? separator  : [NSNumber numberWithInt:0];
	NSDictionary* d =  [	NSDictionary dictionaryWithObjectsAndKeys
						:	t	, @"Title"
						,	c	, @"Cmd"
						,	s	, @"Show"
						,	e	, @"Enabled"
						,	r	, @"Readonly"
						,	x	, @"Separator"
						, 	nil
						];
//	NSLog(@"proxyForJson d = %@",d);
	return d;
}

- (NSString*) description
{
	return [NSString stringWithFormat:@"MenuItem title=%@ cmd=%@",title,cmd];
}

- (id) initWithCoder : (NSCoder*) coder
{
	self = [super init];
	[self setTitle      : [coder decodeObjectForKey: MENU_TITLE]];
	[self setCmd        : [coder decodeObjectForKey: MENU_CMD]];
	[self setEnabled    : [coder decodeObjectForKey: MENU_ENABLED]];
	[self setReadonly   : [coder decodeObjectForKey: MENU_READONLY]];
	[self setShowresult : [coder decodeObjectForKey: MENU_SHOWRESULT]];
	[self setSeparator  : [coder decodeObjectForKey: MENU_SEPARATOR]];
	return self ;
}

- (void) encodeWithCoder : (NSCoder*) encoder
{
	[super init];
	[encoder encodeObject : title      forKey: MENU_TITLE];
	[encoder encodeObject : cmd        forKey: MENU_CMD];
	[encoder encodeObject : enabled    forKey: MENU_ENABLED];
	[encoder encodeObject : readonly   forKey: MENU_READONLY];
	[encoder encodeObject : showresult forKey: MENU_SHOWRESULT];
	[encoder encodeObject : separator  forKey: MENU_SEPARATOR];
}

static int counter = 100 ;

- (id) init
{
	counter++ ;

	self		= [super init];
	title		= [[NSString alloc]initWithFormat:@"Title:%d",counter];
	cmd			= [[NSString alloc]initWithFormat:@"Cmd:%d",counter];
	enabled		= [NSNumber numberWithInt:1];
	readonly	= [NSNumber numberWithInt:0];
	showresult	= [NSNumber numberWithInt:1];
	separator	= [NSNumber numberWithInt:0];
	
	return self;
}

@end

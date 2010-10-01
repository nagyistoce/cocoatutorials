//
//  Event.mm
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

#import "Event.h"


@implementation Event

@synthesize eventName;
@synthesize eventDate;

#define lengthof(x) sizeof(x)/sizeof(x[0])

// this is just a convenience for init to create lots of different dates
static int e_ = 0 ;
static struct {
	NSString* name ;
	NSString* date ;
} events_[] = 
{	{ @"New Year's Day"		, @"01/01/2010" }
,   { @"Easter Sunday"		, @"04/04/2010" }
,   { @"Tartan Day"			, @"04/06/2010" }
,   { @"Independance Day"	, @"07/04/2010" }
,   { @"Thanksgiving"		, @"11/25/2010" }
,   { @"St Andrew's Day"	, @"11/30/2010" }
,   { @"Christmas Day"		, @"12/25/2010" }
};

- (id) init  
{
	[super init];
	eventName = [[NSString alloc]initWithString:events_[e_].name] ;
	eventDate = [[NSCalendarDate dateWithNaturalLanguageString: events_[e_].date]retain] ;
	NSLog(@"Event init %@ %@",eventName,eventDate);
	
	e_ = ++e_ < lengthof(events_) ? e_ : 0 ;
	return self ;
}

- (void) dealloc
{
	NSLog(@"dealloc %@ %@",eventName,eventDate);
	[eventName release];
	[eventDate release];
	[super dealloc];
}

#define EVENT_NAME @"NAME"
#define EVENT_DATE @"DATE"

- (id) initWithCoder : (NSCoder*) coder
{
	[ super init ] ;
	[ self setEventName: [ coder decodeObjectForKey : EVENT_NAME]] ;
	[ self setEventDate: [ coder decodeObjectForKey : EVENT_DATE ]] ;
	NSLog(@"initWithCode %@ %@",eventName,eventDate);
	return self ;
}

- (void) encodeWithCoder : (NSCoder*) encoder
{
	[ encoder encodeObject : eventName  forKey : EVENT_NAME ] ;
	[ encoder encodeObject : eventDate	forKey : EVENT_DATE ] ;
	
	NSLog(@"encoding eventName, eventDate = %@,%@",eventName,eventDate);
}

- (BOOL) isEqualTo:(id)object;
{
	BOOL result = [self class] == [object class] ;
	if ( result ) result = [eventDate isEqualTo:[ object eventDate]];
	if ( result ) result = [eventName isEqualTo:[ object eventName]];
	return result ;
}

@end

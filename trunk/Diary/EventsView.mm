//
//  EventsView.mm
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

#import "EventsView.h"
#import "Event.h"

@implementation EventsView
@synthesize events ;

#pragma mark -------- alloc/dealloc

// little helper 'C' function
static int bigger(int a,int b) { return a > b ? a : b ; } 

- (id)initWithEvents: (NSArray *) aEvents
		fontWithName: (NSString*) fontName /* Monaco */
				size: (NSInteger) fontHeight /* 12 */ 
{
	if ( !fontName ) fontName = @"Monaco" ;
	if ( !fontHeight )fontHeight = 12 ;
	
	[super initWithFrame:NSMakeRect(0,0, 600, 600)];
	
	events = [aEvents copy];
	attributes = [[NSMutableDictionary alloc] init];
	NSFont *font = [NSFont fontWithName:fontName  size:fontHeight];
	lineHeight = [font capHeight] * 1.7;
	[attributes setObject:font  forKey:NSFontAttributeName];

	////
	// find our bundle's icon image
	NSDictionary* dict		= [[NSBundle mainBundle] infoDictionary];
	NSString*	  iconFile	= dict      ? [NSMutableString stringWithString:[dict objectForKey:@"CFBundleIconFile"]] : nil ;
	NSString*	  iconPath	= iconFile  ? [NSString stringWithFormat:@"%@/%@.icns",[[NSBundle mainBundle]resourcePath],iconFile] : nil ;
	NSImage*	  iconImage	= iconPath  ? [[NSImage alloc] initWithContentsOfFile:iconPath] : nil ;
	if ( iconImage)
	{
		NSLog(@"iconPath = %@",iconPath) ;
		NSArray* reps = [iconImage representations] ;
		////
		// find the biggest representation
		int jBig	= -1 ;
		int biggest = 0 ;
		for ( int j = 0 ; j < [reps count] ; j++ ) {
			NSImageRep* rep = [ reps objectAtIndex:j] ;
			int width	= [rep pixelsWide];
			int height	= [rep pixelsHigh];
			int big		= bigger(width,height) ;
			if ( big > biggest ) {
				jBig = j ;
				biggest = big ;
			}
		}
		
		if ( jBig >= 0 ) {
			imageRep= [reps objectAtIndex:jBig];
			[imageRep retain] ;
			imageRect.origin.x = 0 ;
			imageRect.origin.y = 0 ;
			imageRect.size.width = biggest ;
			imageRect.size.height = biggest ;
		}
		[iconImage release];
	}
	
	return self;
}

- (void)dealloc
{
	[events release];
	[imageRep release];
	[attributes release];
	[super dealloc];
}

#pragma mark -------- printing/pagination

- (BOOL)knowsPageRange:(NSRangePointer)range
{
	range->location = 1;
	
	NSPrintOperation* po = [NSPrintOperation currentOperation];
	NSPrintInfo* pi = [po printInfo];
	
	// Where can I draw?
	pageRect = [pi imageablePageBounds];
	
	// How many lines will fit on a page?
	linesPerPage = pageRect.size.height / lineHeight;
	
	// How many pages will it take?
	range->length = [events count] / linesPerPage;
	if ([events count] % linesPerPage) {
		range->length = range->length + 1;
	}

	// Where to put the logo?
	logoRect = [pi imageablePageBounds];
	logoRect.size.width  /= 2;
	logoRect.size.height /= 2 ;
	logoRect.origin.x = logoRect.origin.x + logoRect.size.height - 2*imageRect.size.width  ;
	logoRect.origin.y = logoRect.origin.y + logoRect.size.height - 2*imageRect.size.height ;
	
	return YES;
}

- (NSRect)rectForPage:(NSInteger)index
{
	// Note the current page
	currentPage = (index - 1);
	
	// Return the pageRect
	return pageRect;
}

#pragma mark printing/drawing

- (BOOL)isFlipped
{
	return YES;
}

- (void)drawRect:(NSRect)rect 
{
	NSRect nameRect;
	NSRect dateRect;

	dateRect.size.height = nameRect.size.height = lineHeight;
	nameRect.origin.x = pageRect.origin.x;
	nameRect.size.width = 200.0;
	dateRect.origin.x = NSMaxX(nameRect);
	dateRect.size.width = pageRect.size.width - nameRect.size.width ;

#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1060
	[imageRep drawInRect:logoRect fromRect:imageRect operation:NSCompositeCopy fraction:0.3 respectFlipped:YES hints:nil];
#endif	

	for (int i = 0; i < linesPerPage; i++)
	{
		int index = (currentPage * linesPerPage) + i;
		if (index >= [events count]) {
			break;
		}
		Event* p = [events objectAtIndex:index];
		
		dateRect.origin.y = nameRect.origin.y = pageRect.origin.y + i * lineHeight;
		
		NSString *nameString = [NSString stringWithFormat:@"%2d %@", index+1, [p eventName]];
		[nameString drawInRect:nameRect withAttributes:attributes];
		
		NSString* dateString = [NSString stringWithFormat:@" : %@%", [[p eventDate]description]];
		[dateString drawInRect:dateRect withAttributes:attributes];
	}
}

@end


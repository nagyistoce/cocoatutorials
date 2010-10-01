//
//  EventsView.h
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

#import <Cocoa/Cocoa.h>

@interface EventsView : NSView {
	// outlets
	
	// properties
	NSArray*				events		; // the events to be painted on the page
	NSMutableDictionary*	attributes	; // printing parameters
	float					lineHeight	; // based on the font size
	NSRect					pageRect	; // area of the page to be drawn
	int						linesPerPage; // calculated from pageRect and font height
	int						currentPage	; // index of the page we are painting
	
	NSImageRep*				imageRep	; // the logo
	NSRect					logoRect	; // where on the page to draw our logo
	NSRect					imageRect	; // rectangle of our image rep 
}
// actions

// property getters/setters
@property (readwrite,copy) NSArray* events ;

// methods
- (id) initWithEvents: (NSArray*) events
		 fontWithName: (NSString*) fontName /* Monaco */
				 size: (NSInteger) fontHeight /* 12 */ ;

@end

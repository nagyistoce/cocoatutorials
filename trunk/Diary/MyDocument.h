//
//  MyDocument.h
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

@interface MyDocument : NSDocument
{
	// outlets
	IBOutlet NSTextField* fontNameField;
	IBOutlet NSTextField* fontSizeField;
	
	// properties
	NSMutableArray* events ;
	NSArray*		ovents ;
}
// actions
- (IBAction) showFontPanel:(id)sender;

// property getters/setters
@property (readwrite,copy) NSMutableArray*  events ;
@property (readwrite,copy) NSArray*			ovents ;
@end

//
//  Controller.h
//  This file is part of FavoriteDocument
// 
//  FavoriteDocument is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  FavoriteDocument is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with FavoriteDocument.  If not, see <http://www.gnu.org/licenses/>.
//
//  This file is based on original work by Prince Kumar, Mumbai, India
//  And modified by Robin Mills, San Jose, CA 2010 http://clanmills.com

#import <Cocoa/Cocoa.h>

@interface Controller : NSDocument 
#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1060
								<NSWindowDelegate>
#endif
{
	// outlets
	IBOutlet NSTextField*	bookField;
	IBOutlet NSTextField*	cityField;
	IBOutlet NSTextField*	colorField;
	IBOutlet NSTextField*	foodField;
	IBOutlet NSWindow*		myWindow;
    IBOutlet NSImageView*	imageView;
	IBOutlet NSColorWell*	colorWell;

	// properties
	NSUserDefaults*	prefs;
	bool			mustClose;
	double			red;
	double			green;
	double			blue;
	NSString*		imageName;
}
// actions
- (IBAction)textFieldChanged:(id)sender;
- (IBAction)insertpics:(id)sender;
- (IBAction)saveChanges:(id)sender;
- (IBAction)changeBackgroundColor:(id)sender;
- (IBAction)saveAs:(id)sender;
- (IBAction)open:(id)sender;
- (IBAction)defaults:(id)sender;

// property getters/setters
- (BOOL) mustClose	 :(id)sender;
- (void) setMustClose:(BOOL)mustClose;
- (void) setImageName:(NSString*)fileName;

// methods (for menu manipulation)
- (void) menuItemDump	: (NSMenuItem*) menuItem;
- (void) menuDump		: (NSMenu*) menu;
- (void) menuItemEnable : (NSMenuItem*) menuItem tag:(int)aTag;
- (void) menuEnable		: (NSMenu*) menu tag:(int)aTag;

// methods (for data manipulation)
- (NSDictionary*) defaultPrefs;
- (void) updateUI:(NSDictionary*)dictionary;


@end

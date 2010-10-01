//
//  Controller.h
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

#import <Cocoa/Cocoa.h>

@interface Controller : NSObject 
#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1060
								<NSWindowDelegate>
#endif
{
	// outlets
	IBOutlet NSTextField*	bookField;
	IBOutlet NSTextField*	cityField;
	IBOutlet NSTextField*	colorField;
	IBOutlet NSTextField*	foodField;
	IBOutlet NSWindow*		MyWindow;
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

// property getters/setters
- (BOOL) mustClose:(id)sender;
- (void) setMustClose:(BOOL)mustClose;
- (void) setImageName:(NSString*)fileName;


@end

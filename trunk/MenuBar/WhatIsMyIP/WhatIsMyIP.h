//
//  WhatIsMyIP.h
//  This file is part of WhatIsMyIP
// 
//  WhatIsMyIP is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  WhatIsMyIP is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with WhatIsMyIP.  If not, see <http://www.gnu.org/licenses/>.
//
//  Written by Robin Mills, San Jose, CA, USA. 2010 http://clanmills.com
//

#import <Cocoa/Cocoa.h>

typedef enum 
{	showNone=0
,	showSeparator
,	showItem
} ShowStatus;

@interface WhatIsMyIP : NSWindowController
#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1060
					  <NSWindowDelegate,NSApplicationDelegate>
#endif
{
	// outlets
	IBOutlet NSMenu*	statusMenu;
	
	// properties
	bool				bKilled;
	NSStatusItem*		statusItem;
	NSString*			appPath;
	NSString*			appName;
	NSString*			plistPath;
	NSImage*			iconImage;
	NSString*			menuTitle;
	BOOL				dockIcon;
	BOOL				bDebug;
	
	NSWindowController* aboutBox;
	
	FSEventStreamRef	stream;
	NSNumber*			lastEventId;
	NSDate*				jsonModDate;
	NSDate*				shrink;
    NSFileManager*		fm;
	NSString*			jsonPath;
	NSArray*			jsonArray;
}

// property getters/setters
@property (readwrite,retain)	NSNumber*	lastEventId;
@property (readwrite)			FSEventStreamRef	stream;
@property (readwrite,retain)	NSString*	plistPath;
@property (readwrite,retain)	NSDate*		jsonModDate;
@property (readwrite,retain)	NSArray*	jsonArray;
@property (readwrite,retain)	NSDate*		shrink;

// actions
- (IBAction) quitApplication:(id)sender;

// dynamic menu callback methods
- (void) menuCallback:(id)sender;

// methods
- (void) installPreferences;
- (void) installProgram;
- (void) jsonRead;
- (void) processCommandLine;
- (void) setAsLoginItem:(BOOL) bAdd;
- (void) invoke				: (NSString *) command displayResult:(BOOL)displayResult;
- (void) rebuildMenu		: (NSDictionary*)dict;
- (ShowStatus) showStatus	: (NSDictionary*)menuDict;
- (BOOL) menuBoolean		: (NSDictionary*) dict key:(NSString*)key;
- (void) shrinkFootprint	: (id)sender;
- (void) showInDock			: (BOOL)bDock;
- (void) addMenuItem:(NSString*)title action:(SEL)action target:(id)target tag:(NSInteger)tag;
- (BOOL) pathExists:(NSString*)path;

@end

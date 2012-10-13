//
//  QuartzClockView.h
//  This file is part of QuartzClock
// 
//  QuartzClock is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  QuartzClock is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with QuartzClock.  If not, see <http://www.gnu.org/licenses/>.
//
//  Written by Robin Mills, San Jose, CA, USA. 2010 http://clanmills.com
//

#import <Cocoa/Cocoa.h>

@interface QuartzClockView : NSView
{
	// properties
	NSTimer*			timer;
    NSPoint             initialLocation;  // position of mouse at mouse down
    NSSize              initialSize;      // size of window at mouse down
    NSColor*            backgroundColor;
    NSColor*            gradientColor;
    NSColor*            handsColor;
    NSColor*            rimColor;
    NSColor*            marksColor;
    BOOL                radialGradient;
    BOOL                ignoreMouse;
    float               angle;
}

// Class methods
+ (NSUInteger) borderMask ;
+ (NSUInteger) borderNone ;
+ (NSUInteger) borderToggle : (NSUInteger) now;

// actions
- (IBAction) saveAs       : (id) sender;
- (IBAction) setTitleNow  : (id) sender;
- (IBAction) borderToggle : (id) sender;
- (IBAction) showHide     : (id) sender;

// methods
- (id)   initInDock;
- (void) stopClockUpdates;
- (void) startClockUpdates;
- (void) update;
- (void) copyFrom : (QuartzClockView*) other;

// property getter/setters
@property           NSPoint             initialLocation; // position of mouse at mouse down
@property           NSSize              initialSize;     // size of window at mouse down
@property (retain)  NSColor*            backgroundColor;
@property (retain)  NSColor*            gradientColor;
@property (retain)  NSColor*            handsColor;
@property (retain)  NSColor*            rimColor;
@property (retain)  NSColor*            marksColor;
@property           BOOL                radialGradient;
@property           BOOL                ignoreMouse;
@property           float               angle;

@end

extern QuartzClockView* theWindowView;
extern QuartzClockView* theDockView;

// That's all Folks!
////

//
//  Extensions.h
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

#import <Foundation/Foundation.h>

NSMutableArray* menuFindByTitle(NSMenu* menu,NSString* title,NSMutableArray* menus = nil );

// http://www.cocoadev.com/index.pl?TestForKeyDownOnLaunch

@interface NSEvent (ModifierKeys)
+ (BOOL) isControlKeyDown;
+ (BOOL) isOptionKeyDown;
+ (BOOL) isCommandKeyDown;
+ (BOOL) isShiftKeyDown;
@end

// http://www.cocoadev.com/index.pl?GradientFill
@interface NSBezierPath (GradientFill)
- (void)fillGradientFrom : (NSColor*) inStartColor to : (NSColor*) inEndColor angle : (float)inAngle ;
@end

// That's all Folks!
////

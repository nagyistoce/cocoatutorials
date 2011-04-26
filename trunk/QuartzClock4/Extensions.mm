//
//  Extensions.mm
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

#import "Extensions.h"
#import <Carbon/Carbon.h>
#import <Quartz/Quartz.h>

NSMutableArray* menuFindByTitle(NSMenu* menu,NSString* title,NSMutableArray* menus)
{
	if ( !menus ) menus = [NSMutableArray array];
    if ( menu && menus ) {
        for ( int i = 0 ; i < [menu numberOfItems] ; i++ )
        {
            NSMenuItem* mi = [menu itemAtIndex:i];
            if ( [mi hasSubmenu] ) {
                menus = menuFindByTitle([mi submenu],title,menus);
            } else if ( [[mi title]isEqual:title] ) {
                [menus addObject:mi];
            }
        }
    }
    return menus ;
}

// http://www.cocoadev.com/index.pl?TestForKeyDownOnLaunch
@implementation NSEvent (ModifierKeys)

+ (BOOL) isControlKeyDown
{
    return (GetCurrentKeyModifiers() & controlKey)?YES:NO;
}

+ (BOOL) isOptionKeyDown
{
    return (GetCurrentKeyModifiers() & optionKey)?YES:NO;
}

+ (BOOL) isCommandKeyDown
{
    return (GetCurrentKeyModifiers() & cmdKey)?YES:NO;
}

+ (BOOL) isShiftKeyDown
{
    return (GetCurrentKeyModifiers() & shiftKey)?YES:NO;
}

@end

// That's all Folks!
////

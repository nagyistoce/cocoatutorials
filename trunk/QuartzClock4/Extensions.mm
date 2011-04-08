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
            if ( [mi hasSubmenu] )
                menus = menuFindByTitle([mi submenu],title,menus);
            else if ( [[mi title]isEqual:title] ) {
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
    return (GetCurrentKeyModifiers() & controlKey) != 0;
}

+ (BOOL) isOptionKeyDown
{
    return (GetCurrentKeyModifiers() & optionKey) != 0;
}

+ (BOOL) isCommandKeyDown
{
    return (GetCurrentKeyModifiers() & cmdKey) != 0;
}

+ (BOOL) isShiftKeyDown
{
    return (GetCurrentKeyModifiers() & shiftKey) != 0;
}

@end


// http://www.cocoadev.com/index.pl?GradientFill
@implementation NSBezierPath (GradientFill)
- (void) fillGradientFrom : (NSColor*) inStartColor
                       to : (NSColor*) inEndColor
                    angle : (float) inAngle
{
	CIImage*	coreimage;
	
	inStartColor = [inStartColor colorUsingColorSpaceName:NSCalibratedRGBColorSpace];
	inEndColor = [inEndColor colorUsingColorSpaceName:NSCalibratedRGBColorSpace];
	
	CIColor* startColor = [CIColor colorWithRed:[inStartColor redComponent] green:[inStartColor greenComponent] blue:[inStartColor blueComponent] alpha:[inStartColor alphaComponent]];
	CIColor* endColor = [CIColor colorWithRed:[inEndColor redComponent] green:[inEndColor greenComponent] blue:[inEndColor blueComponent] alpha:[inEndColor alphaComponent]];
	
	CIFilter* filter;
	
	filter = [CIFilter filterWithName:@"CILinearGradient"];
	[filter setValue:startColor forKey:@"inputColor0"];
	[filter setValue:endColor forKey:@"inputColor1"];
	
	CIVector* startVector;
	CIVector* endVector;
	
	startVector = [CIVector vectorWithX:0.0 Y:0.0];
	endVector = [CIVector vectorWithX:0.0 Y:[self bounds].size.height];
	
	[filter setValue:startVector forKey:@"inputPoint0"];
	[filter setValue:endVector forKey:@"inputPoint1"];
	
	coreimage = [filter valueForKey:@"outputImage"];
	
	[[NSGraphicsContext currentContext] saveGraphicsState];
	
	CIContext* context;
	
	context = [[NSGraphicsContext currentContext] CIContext];
	
	[self setClip];
	
	[context drawImage:coreimage atPoint:CGPointZero fromRect:CGRectMake( 0.0, 0.0, [self bounds].size.width + 100.0, [self bounds].size.height + 100.0 )];
	
	[[NSGraphicsContext currentContext] restoreGraphicsState];
}

@end

// That's all Folks!
////

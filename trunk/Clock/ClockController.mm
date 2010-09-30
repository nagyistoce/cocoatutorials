//
//  ClockController.mm
//  This file is part of Clock
// 
//  Clock is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Clock is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with Clock.  If not, see <http://www.gnu.org/licenses/>.
//
//  Written by Robin Mills, San Jose, CA, USA. 2010 http://clanmills.com
//

#import "ClockController.h"

// Clock icon was obtained here:
// http://commons.wikimedia.org/wiki/File:Cambiosrecientes.svg

@implementation ClockController

- (void) awakeFromNib
{
	[self tick:nil];
}

- (void) tick:(id)sender
{
	[ timeString setStringValue:[[NSDate date]description]];
	[ self performSelector: @selector(tick:)
				withObject: nil
				afterDelay: 1.0
	];
}


@end

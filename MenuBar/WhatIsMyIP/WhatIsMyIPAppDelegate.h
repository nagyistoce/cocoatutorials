//
//  WhatIsMyIPAppDelegate.h
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

@interface WhatIsMyIPAppDelegate : NSObject
#if __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 1060
		<NSApplicationDelegate>
#endif
{
    NSWindow *window;
}

@property (assign) IBOutlet NSWindow *window;

@end

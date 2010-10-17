//
//  NSString_RE.h
//  This file is part of RegularExpressions
// 
//  RegularExpressions is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  RegularExpressions is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with RegularExpressions.  If not, see <http://www.gnu.org/licenses/>.
//
//  Written by Robin Mills, San Jose, CA, USA. 2010 http://clanmills.com
//
//
//  Modified from the following code:
//  Created by John R Chang on 2005-11-08.
//  This code is Creative Commons Public Domain.  You may use it for any purpose whatsoever.
//  http://creativecommons.org/licenses/publicdomain/
//

#import <Foundation/Foundation.h>
#include <regex.h>

/*
	For regular expression help, see re_format(7) man page.
*/

@interface NSString (RE)

/*
	Common <options> are REG_ICASE and REG_NEWLINE.  For other possible option flags,
	see regex(3) man page.  You don't need to specify REG_EXTENDED.
	
	<nmatch> is the number of subexpressions to match.
	Returns an array of strings.  The first string is the matching substring,
		the remaining are the matching subexpressions, up to nmatch+1 number.

	If nmatch is -1, works like grep.  Returns an array containing self if matching.
		
	Returns nil if regular expression does not match or if an error has occurred.
*/
- (NSArray *) substringsMatchingRegularExpression : (NSString *)pattern
										 maxcount : (int)nmatch
										  options : (int)options
										   ranges : (NSArray **)ranges
											error : (NSError **)error;

- (BOOL) grep:(NSString *) pattern options :(int)options ;

- (NSString *) stringWithRegularExpression  : (NSString*) pattern
									replace : (NSString*) replace
								   maxcount : (int) maxcount
									options : (int) options
									  error : (NSError**) error;

@end

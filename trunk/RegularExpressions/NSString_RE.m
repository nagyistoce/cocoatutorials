//
//  NSString_RE.m
//
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
//  Modified from the following code:
//  Created by John R Chang on 2005-11-08.
//  This code is Creative Commons Public Domain.  You may use it for any purpose whatsoever.
//  http://creativecommons.org/licenses/publicdomain/
//

#import "NSString_RE.h"

@implementation NSString (RegExpression)

- (NSArray *) substringsMatchingRegularExpression : (NSString *) pattern
										 maxcount : (int) nmatch
										  options : (int) options
										   ranges : (NSArray **) ranges
											error : (NSError **) error
{
	int	errcode = 0 ;
	options |= REG_EXTENDED;
	
	if (error) 	*error = nil;
	if ( nmatch < 1 ) nmatch = 1 ;

	NSMutableArray* result = [NSMutableArray array];
	if ( ranges )  *ranges = [NSMutableArray array];
	
	// Compile the regular expression
	regex_t			preg;
	errcode = regcomp(&preg, [pattern UTF8String], options);
	
	if ( !errcode ) {

		// Match the regular expression against substring self
		regmatch_t match ;
		errcode = regexec(&preg, [self UTF8String], 1, &match, 0);
	
		// run the string (one match at a time)
		int start = 0 ;
		while ( !errcode && [result count] < nmatch) {
			errcode = regexec(&preg, [self UTF8String]+start, 1, &match, 0);
			if ( !errcode ) {
			//	printf("from,to = %d,%d\n",(int)(start+match.rm_so),(int)(match.rm_eo-match.rm_so)); 
				if (match.rm_so == -1 || match.rm_eo == -1)
					break;

				NSRange range = NSMakeRange(start+match.rm_so, match.rm_eo - match.rm_so);
				NSString * substring = [self substringWithRange:range];
				[result addObject:substring];

				if (ranges)
				{
					NSValue * value = [NSValue valueWithRange:range];
					[(NSMutableArray *)*ranges addObject:value];
				}
				start += range.location+range.length;
			}
		}
	}

	if ( errcode != 0 && error)
	{
		// Construct error object
		NSMutableDictionary * userInfo = [NSMutableDictionary dictionary];
		char errbuf[256];
		int len = regerror(errcode, &preg, errbuf, sizeof(errbuf));
		if (len > 0)
			[userInfo setObject:[NSString stringWithUTF8String:errbuf] forKey:NSLocalizedDescriptionKey];
		*error = [NSError errorWithDomain:@"regerror" code:errcode userInfo:userInfo];
	}

	// cleanup
	regfree(&preg);
	return result;
}

- (BOOL) grep:(NSString *)pattern options:(int)options
{
	NSArray * substrings = [self substringsMatchingRegularExpression:pattern maxcount:1 options:options ranges:NULL error:NULL];
	return (substrings && [substrings count] > 0);
}

/*
 This part of the extension has been written by Robin Mills robin@clanmills.com
*/
- (NSString *) stringWithRegularExpression : (NSString*) pattern
								   replace : (NSString*) replace
								   maxcount: (int) maxcount
								   options : (int) options
									 error : (NSError**) error
{
	NSMutableString* result = [NSMutableString stringWithCapacity:[self length]];
	NSArray* ranges = nil ;
	[self substringsMatchingRegularExpression:pattern maxcount:maxcount options:options ranges:&ranges error:error];
	int from = 0 ;
	if ( ranges ) for ( int i = 0 ; i < [ranges count] ; i++ ) {
		NSValue* value	= [ranges objectAtIndex:i];
		NSRange		r	= [value rangeValue];
		printf("from, to = %d,%d",from,(int)r.location);
		NSRange copy	= { from,r.location-from };
		[result appendString:[self substringWithRange:copy]];
		[result appendString:replace];
		from = r.location+r.length;
	}
	NSRange copy = { from,[self length]-from };
	[result appendString:[self substringWithRange:copy]];
	return result ;
}

@end

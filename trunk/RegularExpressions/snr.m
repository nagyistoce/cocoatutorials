//
//  snr.m
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

#import <Foundation/Foundation.h>
#import "NSString_RE.h"

enum
{	errorOK=0
,	errorSyntax
,	errorCantRead
,	errorCantWrite
}	error = errorOK ;

int errorMessage(int e,NSString* message,id arg)
{
	NSString* m = [NSString stringWithFormat:message,arg];
	printf("%s\n",[m UTF8String]);
	error = e;
	return e ;
}

int main (int argc, const char * argv[])
{
	if (argc < 4) return errorMessage(errorSyntax,@"Syntax: snr pattern replacement file ...",nil);

	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	int		  arg = 1 ;
	NSArray*  args = [[NSProcessInfo processInfo] arguments];
	NSString* from = [args objectAtIndex:arg++];
	NSString* to   = [args objectAtIndex:arg++];

	while ( !error && arg < argc ) {
		NSError*		 idc		= nil ;
		NSString*		 filename	= [args objectAtIndex:arg++] ;
			NSString*	str			= [NSString stringWithContentsOfFile:filename encoding:NSUTF8StringEncoding error:&idc];
		if (idc) 
			error = errorMessage(errorCantRead,@"%@ was not readable", filename);
		
		if ( !error ) {
			int  max = 100 ;
			NSArray* array		= [str substringsMatchingRegularExpression:from maxcount:max options:0 ranges:nil error:&idc];
			NSString* replace	= [str stringWithRegularExpression:from replace:to maxcount:max options:REG_ICASE error:&idc];
			[replace writeToFile:filename atomically:YES encoding: NSUTF8StringEncoding error: &idc];
		}
	}
	[pool drain];
	return error;
}

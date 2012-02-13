//
//  JSON.mm
//  This file is part of ix4
// 
//  ix4 is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  ix is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with Favorites.  If not, see <http://www.gnu.org/licenses/>.
//
//  This file is original work by Robin Mills, San Jose, CA 95112, USA
//  Created 2012 http://clanmills.com robin@clanmills.com
//

#import "JSON.h"


// http://stackoverflow.com/questions/4323983/json-framework-add-in-xcode

@implementation JSON

- (id)init
{
    self = [super init];
    if (self) {
        // Initialization code here.
    }
    
    return self;
}

-(NSObject*)jsonToObj:(NSString *)json {
    int nextOffset;
    return [self jsonToObj:json offset:0 nextOffset:&nextOffset];
}

static int indexOfChar(NSString *str, unichar c, int offset) {
    NSUInteger length = [str length];
    for (int i = offset; i < length; i++) {
        if ([str characterAtIndex:i] == c) {
            return i;
        }
    }
    return -1;
}

static int indexOfNonWhitespace(NSString *str, int offset) {
    NSUInteger len = [str length];
    NSCharacterSet *cSet = [NSCharacterSet whitespaceAndNewlineCharacterSet];
    for (int i = offset; i < len; i++) {
        if (![cSet characterIsMember:[str characterAtIndex:i]]) {
            return i;
        }
    }
    return -1;
}

static int indexOfNonNumber(NSString *str, int offset) {
    NSUInteger len = [str length];
    for (int i = offset; i < len; i++) {
        unichar c = [str characterAtIndex:i];
        if (c != '-' && c != '.' && (c < '0' || c > '9')) return i; 
    }
    return -1;
}

static int parseHex(NSString *hexStr) {
    unsigned int result = 0;
    NSUInteger len = [hexStr length];
    for (int i = 0; i < len; i++) {
        result <<= 4;
        unsigned int v = 0;
        unichar c = [hexStr characterAtIndex:i];
        if (c >= '0' && c <= '9') {
            v = c-'0';
        } else if (c >= 'a' && c <= 'f') {
            v = c-'a'+10;
        } else if (c >= 'A' && c <= 'F') {
            v = c-'A'+10;
        }
        result += v;
    }
    return result;
}

-(NSObject*) jsonToObj:(NSString *)json
                offset:(int)offset
            nextOffset:(int*)nextOffset
{
    offset = indexOfNonWhitespace(json, offset);
    static NSString *jsonExceptionName = @"InvalidJSONException";
    if (offset == -1) {
        @throw [NSException exceptionWithName:jsonExceptionName reason:@"no non-whitespace 1" userInfo:nil];
    }
    unichar startChar = [json characterAtIndex:offset];
    if (startChar == '{') {
        
        NSMutableDictionary *result = [NSMutableDictionary dictionary];
        
        int curPos = offset+1;
        while (YES) {
            curPos = indexOfNonWhitespace(json, curPos);
            if (curPos == -1) {
                @throw [NSException exceptionWithName:jsonExceptionName reason:@"no non-whitespace 2" userInfo:nil];
            }
            unichar curChar = [json characterAtIndex:curPos];
            if (curChar == ',') {
                curPos++;
                continue;
            } else if (curChar == '}') {
                *nextOffset = curPos+1;
                return result;
            }
            unichar quotChar = curChar;
            if ((quotChar != '\'' && quotChar != '"') || [json characterAtIndex:curPos-1]=='\\') {
                quotChar = 0;
            }
            NSString *key = nil;
            int semiIndex = 0;
            if (quotChar != 0) {
                int quotStart = curPos+1;
                int quotEnd = quotStart;
                while (YES) {
                    quotEnd = indexOfChar(json, quotChar, quotEnd);
                    if (quotEnd == -1) {
                        @throw [NSException exceptionWithName:jsonExceptionName reason:@"quotation-end not found 1" userInfo:nil];
                    }
                    if ([json characterAtIndex:quotEnd-1] != '\\') break;
                    else quotEnd++;
                }
                key = [json substringWithRange:NSMakeRange(quotStart, quotEnd-quotStart)];
                semiIndex = indexOfChar(json, ':', quotEnd);
                if (semiIndex == -1) {
                    @throw [NSException exceptionWithName:jsonExceptionName reason:@"semicolon not found 1" userInfo:nil];
                }
            } else {
                semiIndex = indexOfChar(json, ':', curPos);
                if (semiIndex == -1) {
                    @throw [NSException exceptionWithName:jsonExceptionName reason:@"semicolon not found 2" userInfo:nil];
                }
                key = [[json substringWithRange:NSMakeRange(curPos, semiIndex-curPos)] stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]];
            }
            
            [result setObject:[self jsonToObj:json offset:semiIndex+1 nextOffset:&curPos] forKey:key];
        }
    } else if (startChar == '[') {
        
        NSMutableArray *result = [NSMutableArray array];
        
        int curPos = offset+1;
        while (YES) {
            curPos = indexOfNonWhitespace(json, curPos);
            if (curPos == -1) {
                @throw [NSException exceptionWithName:jsonExceptionName reason:@"no non-whitespace 3" userInfo:nil];
            }
            unichar curChar = [json characterAtIndex:curPos];
            if (curChar == ',') {
                curPos++;
                continue;
            } else if (curChar == ']') {
                *nextOffset = curPos+1;
                return result;
            }
            [result addObject:[self jsonToObj:json offset:curPos nextOffset:&curPos]];
        }
    } else {
        unichar quotChar = startChar;
        if ((quotChar != '\'' && quotChar != '"') || [json characterAtIndex:offset-1]=='\\') {
            quotChar = 0;
        }
        if (quotChar != 0) {
            int quotStart = offset+1;
            int quotEnd = quotStart;
            while (YES) {
                quotEnd = indexOfChar(json, quotChar, quotEnd);
                if (quotEnd == -1) {
                    @throw [NSException exceptionWithName:jsonExceptionName reason:@"quotation-end not found 2" userInfo:nil];
                }
                if ([json characterAtIndex:quotEnd-1] != '\\') break;
                else quotEnd++;
            }
            *nextOffset = quotEnd+1;
            NSString *str = [json substringWithRange:NSMakeRange(quotStart, quotEnd-quotStart)];
            str = [str stringByReplacingOccurrencesOfString:@"\\\"" withString:@"\""];
            NSMutableString *result = [NSMutableString stringWithCapacity:[str length]];
            NSUInteger len = [str length];
            for (int i = 0; i < len; i++) {
                unichar c = [str characterAtIndex:i];
                if (c != '\\') {
                    [result appendString:[NSString stringWithCharacters:&c length:1]];
                } else if (i+1 < len) {
                    unichar c2 = [str characterAtIndex:i+1];
                    if (c2 == '\\') {
                        [result appendString:[NSString stringWithCharacters:&c2 length:1]];
                        i++;
                    } else if (c2 == 'u') {
                        unichar cUni = parseHex([str substringWithRange:NSMakeRange(i+2, 4)]);
                        [result appendString:[NSString stringWithCharacters:&cUni length:1]];
                        i += 5;
                    }
                }
            }
            return result;
        } else {
            int numberEnd = indexOfNonNumber(json, offset);
            if (numberEnd == -1) {
                @throw [NSException exceptionWithName:jsonExceptionName reason:@"number-end not found" userInfo:nil];
            }
            //BOOL isFloat = indexOfChar(json, '.', offset)!=-1;
            NSString *numberStr = [json substringWithRange:NSMakeRange(offset, numberEnd-offset)];
            *nextOffset = numberEnd;
            return numberStr;
        }
    }
}

@end

// That's all Folks!
////

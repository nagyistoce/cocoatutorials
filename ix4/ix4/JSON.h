//
//  JSON.h
//  ix4
//
//  Created by Robin Mills on 2012-02-07.
//  Copyright 2012 RMSC San Jose, CA, USA. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface JSON : NSObject

-(NSObject*) jsonToObj:(NSString *)json ;
-(NSObject*) jsonToObj:(NSString *)json offset:(int)offset nextOffset:(int*)nextOffset;
@end

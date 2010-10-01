//
//  MenuItems.h
//  WhatIsMyIPprefs
//
//  Created by Robin Mills on 5/3/10.
//  Copyright 2010 RMSC, San Jose, CA, USA. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#define MENU_TITLE      @"title"
#define MENU_CMD        @"cmd"
#define MENU_ENABLED    @"enable"
#define MENU_READONLY   @"readonly"
#define MENU_SHOWRESULT @"showresult"
#define MENU_SEPARATOR  @"menuseparator"
#define MENU_FILETYPE   @"whatIsmyIP"

@interface MenuItem : NSObject
{
	NSString* title ;
	NSString* cmd   ;
	NSNumber* enabled;
	NSNumber* readonly;
	NSNumber* showresult;
	NSNumber* separator;
}

@property (readwrite,copy) NSString* title;
@property (readwrite,copy) NSString* cmd;
@property (readwrite,copy) NSNumber* enabled;
@property (readwrite,copy) NSNumber* readonly;
@property (readwrite,copy) NSNumber* showresult;
@property (readwrite,copy) NSNumber* separator;

@end



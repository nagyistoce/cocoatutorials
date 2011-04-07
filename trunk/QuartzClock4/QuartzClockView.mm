//
//  QuartzClockView.mm
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
//  Published on Google Code 2010-09-29
//  Demonstration modifying file to Ali
//

#import "QuartzClockView.h"
#import "QuartzClockAppDelegate.h"
#import <Quartz/Quartz.h>

#import <Carbon/Carbon.h>

// http://www.cocoadev.com/index.pl?TestForKeyDownOnLaunch

@interface NSEvent (ModifierKeys)
+ (BOOL) isControlKeyDown;
+ (BOOL) isOptionKeyDown;
+ (BOOL) isCommandKeyDown;
+ (BOOL) isShiftKeyDown;
@end

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

@interface NSBezierPath (Additions)
- (void)fillGradientFrom : (NSColor*) inStartColor to : (NSColor*) inEndColor angle : (float)inAngle ;
@end

@implementation NSBezierPath (Additions)

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

NSMutableArray* menuFindByTitle(NSMenu* menu,NSString* title,NSMutableArray* menus = nil );
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

@implementation QuartzClockView
@synthesize isDocked;
@synthesize initialLocation;
@synthesize initialSize;
@synthesize dockedBackground;
@synthesize myIcon;


- (void) dealloc 
{
	[self stopClockUpdates];
	[myIcon release];
	[super dealloc];
}

- (id) init
{
	self = [super init];
	isDocked = NO;
    dockedBackground = [NSColor redColor];

	return self ;
}

- (id) initWithFrame : (NSRect) frame {
	NSLog(@"QuartzClockView initWithFrame");
    
    self = [super initWithFrame:frame];
    if (self) {
		isDocked = NO;
    }
    return self;
}

- (void) awakeFromNib
{
	NSLog(@"QuartzClockView awakeFromNib");
    myIcon = [[QuartzClockView alloc]init] ;
	[myIcon setIsDocked:YES] ;
	[[NSApp dockTile] setContentView: myIcon];
    [[self window] setContentView:self];
	[self startClockUpdates];
}

+ (NSUInteger) borderNone { return NSBorderlessWindowMask ; }
+ (NSUInteger) borderMask { return (  NSTitledWindowMask  | NSClosableWindowMask | NSResizableWindowMask )
                          & ~NSMiniaturizableWindowMask
                          ; }
+ (NSUInteger) borderToggle : (NSUInteger) now 
{
    return now == [QuartzClockView borderNone] ? [QuartzClockView borderMask] : [QuartzClockView borderNone] ;
}

- (void) borderToggle : (id) sender
{
    [self.window setStyleMask:[QuartzClockView borderToggle : self.window.styleMask]];
}


- (IBAction) faceToggle:(id)sender
{
    self.isDocked = !self.isDocked;
    for ( NSMenuItem* mi in menuFindByTitle([NSApp mainMenu],@"Gradient") ) {
        mi.state = self.window.styleMask== self.isDocked ?  NSOnState : NSOffState;
    }    
}
// http://iphone-dev-tips.alterplay.com/2010/03/analog-clock-using-quartz-core.html
static double rad(double deg)
{
	return deg / 180.0 * M_PI;
}
static int      lesser(int     a,int     b) { return a < b ? a : b ; }
static int      larger(int     a,int     b) { return a > b ? a : b ; }
static CGFloat  lesseR(CGFloat a,CGFloat b) { return a < b ? a : b ; }
static CGFloat  largeR(CGFloat a,CGFloat b) { return a > b ? a : b ; }

- (NSPoint) center : (NSRect) r
{
	NSPoint result ;
	result.x = r.origin.x + r.size.width / 2 ;
	result.y = r.origin.y + r.size.height/ 2 ;
	return result ;
}

- (NSPoint) size : (NSRect) r
{
	NSPoint result ;
	result.x = r.size.width / 2 ;
	result.y = r.size.height/ 2 ;
	return result ;
}

- (IBAction) setTitleNow : (id) sender
{
    for ( NSMenuItem* mi in menuFindByTitle([NSApp mainMenu],@"Border") ) {
        mi.state = self.window.styleMask== [QuartzClockView borderNone] ?  NSOffState : NSOnState;
    }
}

- (void) mouseDown : (NSEvent*) theEvent {    
    // Get the mouse location in window coordinates.
    self.initialLocation = [theEvent locationInWindow];
    self.initialSize     = self.window.frame.size;
    
/*
    NSLog (@"QuartzClockView::mouseDown"
           @" Window pos %4.0f,%4.0f,size = %4.0f,%4.0f"
           @" mouse = %4.0f,%4.0f"
           , b.origin.x,b.origin.y
           , b.size.width,b.size.height
           , self.initialLocation.x,initialLocation.y
           );
*/
 }

- (void) mouseDragged : (NSEvent*) theEvent
{
    if ( [NSEvent isCommandKeyDown]  ) {
        NSPoint newMouse = [theEvent locationInWindow];
        NSPoint oldMouse = self.initialLocation;
        
        CGFloat dx =  (newMouse.x - oldMouse.x)/2;
        CGFloat dy =  (newMouse.y - oldMouse.y)/2;
        NSRect  newFrame = [self.window frame];
        newFrame.origin.x -= dx;
        newFrame.origin.y -= dy;
        newFrame.size.width += dx*2;
        newFrame.size.height += dy*2;

    //  if ( newFrame.size.width  > 1800 ) newFrame.size.width  = 1800;
    //  if ( newFrame.size.height > 1200 ) newFrame.size.height = 1200;
        
        [self.window setFrame:newFrame display:YES];
         self.initialSize     = newFrame.size;
         self.initialLocation = newMouse;
        
    } else {
        NSRect  screenVisibleFrame = [[NSScreen mainScreen] visibleFrame];
        NSRect  windowFrame = [self.window frame];
        NSPoint newOrigin   = windowFrame.origin;
    
        // Get the mouse location in window coordinates.
        NSPoint currentLocation = [theEvent locationInWindow];
        // Update the origin with the difference between the new mouse location and the old mouse location.
        newOrigin.x += (currentLocation.x - initialLocation.x);
        newOrigin.y += (currentLocation.y - initialLocation.y);
    
        // Don't let window get dragged up under the menu bar
        if ((newOrigin.y + windowFrame.size.height) > (screenVisibleFrame.origin.y + screenVisibleFrame.size.height)) {
            newOrigin.y = screenVisibleFrame.origin.y + (screenVisibleFrame.size.height - windowFrame.size.height);
        }
    
        // Move the window to the new location
        [self.window setFrameOrigin:newOrigin];
    }
}


- (void) rightMouseDown : (NSEvent*) theEvent {    
    NSLog(@"QuartzClockView::rightMouseDown");
}

- (BOOL) acceptsFirstResponder
{
    NSLog(@"QuartzClockView::acceptsFirstResponder");
    return YES;
}

- (BOOL) acceptsFirstMouse : (NSEvent*) theEvent
{
    NSLog(@"QuartzClockView::acceptsFirstMouse");
    return YES;
}

- (void) mouseMoved : (NSEvent*) theEvent
{
    static int count = 0 ;
    NSLog(@"QuartzClockView::mouseMoved %d",count++);
}

- (void) drawLineForContext : (const CGContextRef&) context
					  width : (float) _width
					  angle : (double) _angle
                    radiusx : (double) radiusx
                    radiusy : (double) radiusy
{
	CGPoint c = CGPointMake(self.frame.size.width/2.0, self.frame.size.height/2.0);
	CGContextSetLineWidth(context, _width);
	NSPoint center = [self center:[self bounds]];
	NSPoint size = [self size:[self bounds]];
	CGContextMoveToPoint(context, center.x,center.y); //  self.bounds.center.x, self.bounds.center.y );
	CGPoint addLines[] =
	{
		CGPointMake(size.x,size.y), // self.frame.size.width/2.0, self.frame.size.height/2.0),
		CGPointMake(radiusx*cos(_angle) +c.x, radiusy*sin(_angle) +c.y),
	};
	CGContextAddLines(context, addLines, sizeof(addLines)/sizeof(addLines[0]));
	CGContextStrokePath(context);
}

- (BOOL) isFlipped 
{
	return YES ;
}

- (void) drawRect : (NSRect) dirtyRect
{
	// NSLog(@"drawRect");
	CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	NSRect r = [self bounds] ;
	
	NSBezierPath* rectPath = [NSBezierPath bezierPath];
	[rectPath appendBezierPathWithRect:r];
	
	int margin = larger(lesser(r.size.width /(isDocked?10:20),10) ,4) ;
	
	r.origin.x    += margin   ;
	r.origin.y	  += margin   ;
	r.size.width  -= margin*2 ;
	r.size.height -= margin*2 ;
	
	NSBezierPath* circlePath = [NSBezierPath bezierPath];
	[circlePath appendBezierPathWithOvalInRect:r];
	
	if ( isDocked ) {
		// zap the background
		[[NSColor clearColor] set];
		[rectPath fill];
		
		// fill a blue circle
        [self.dockedBackground setFill];
		[circlePath fill];
		
		// stroke a white circle
		[[NSColor whiteColor]setStroke];
		[circlePath setLineWidth:margin/2];
		[circlePath stroke];
		
		// prepare to draw the hands in white
		CGContextSetRGBStrokeColor(context, 1.0, 1.0, 1.0, 1.0);
	} else {
		// gradient background and circle
		// [rectPath   fillGradientFrom:[NSColor blueColor] to:[NSColor blueColor/*greenColor*/] angle:0.0];
		[circlePath fillGradientFrom:[NSColor redColor] to:[NSColor yellowColor] angle:0.0];
		
		// stroke a white circle with a shadow
		CGContextSetShadow(context, CGSizeMake(4.0f, -4.0f), 2.0f);
		[[NSColor whiteColor]setStroke];
		[circlePath setLineWidth:margin];
		[circlePath stroke];
		CGContextSetShadow(context, CGSizeMake(6.0f, -6.0f), 0.0f);
		
		// prepare to draw the hands in black
		CGContextSetRGBStrokeColor(context, 0.0, 0.0, 0.0, 1.0);
	}
	
	CGContextSetLineCap(context,kCGLineCapRound);
	
	NSCalendar *cal = [[NSCalendar alloc] initWithCalendarIdentifier:NSGregorianCalendar];
	NSDate* now = [NSDate date];
	int h = [[cal components:NSHourCalendarUnit fromDate:now] hour];
	int m = [[cal components:NSMinuteCalendarUnit fromDate:now] minute];
	int s = [[cal components:NSSecondCalendarUnit fromDate:now] second];
	[cal release];
	
	BOOL isAM = h<12;
	
	double hAlpha = rad((isAM?h:h-12)*30 +(30*m/60) -90);
	
	double mAlpha = rad(m*6 -90);
	double sAlpha = rad(s*6 -90);
	
	[self drawLineForContext:context width:8.0 angle:hAlpha radiusx:self.frame.size.width/2.0 - 18 radiusy:self.frame.size.height/2.0 - 18];
	[self drawLineForContext:context width:5.0 angle:mAlpha radiusx:self.frame.size.width/2.0 - 14 radiusy:self.frame.size.height/2.0 - 14];
	[self drawLineForContext:context width:2.0 angle:sAlpha radiusx:self.frame.size.width/2.0 - 12 radiusy:self.frame.size.height/2.0 - 12];
}

- (void) update
{
	[self setNeedsDisplay:YES];
	[[NSApp dockTile] display];
	// NSLog(@"update");
}

-(void) startClockUpdates
{
	[self update];
	timer = [NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(update) userInfo:nil repeats:YES];
}

-(void) stopClockUpdates
{
	[timer invalidate];
	timer = nil;
}


- (IBAction) saveAs : (id) sender
{
	NSLog(@"QuartzClockView::saveAs");
}

@end



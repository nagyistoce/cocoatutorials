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
//  but WITHOUT ANY WARRANTY= [NSColor redColor]; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with QuartzClock.  If not, see <http://www.gnu.org/licenses/>.
//
//  Written by Robin Mills, San Jose, CA, USA. 2010 http://clanmills.com
//

#import <Quartz/Quartz.h>
#import "QuartzClockView.h"
#import "QuartzClockAppDelegate.h"
#import "Extensions.h"

@implementation QuartzClockView

@synthesize isDocked;
@synthesize initialLocation;
@synthesize initialSize;
@synthesize backgroundColor;
@synthesize gradientColor;
@synthesize handsColor;
@synthesize rimColor;
@synthesize marksColor;

- (void) dealloc 
{
	[self stopClockUpdates];
	[super dealloc];
}

- (void) initColors
{
	isDocked             = NO ;
    backgroundColor      = [NSColor colorWithCalibratedRed:1.0 green:0.0 blue:0.0 alpha:1.0];
    gradientColor        = [NSColor colorWithCalibratedRed:1.0 green:1.0 blue:0.0 alpha:1.0];
    handsColor           = [NSColor colorWithCalibratedRed:0.0 green:0.0 blue:0.0 alpha:1.0];
    [self setMarksColor:[NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:1.0]];
    rimColor             = [NSColor colorWithCalibratedRed:1.0 green:1.0 blue:1.0 alpha:1.0];
    NSLog(@"initColors %@",self);
}

- (id) init
{
	self = [super init];
    [self initColors];
	return self ;
}

- (id) initWithFrame : (NSRect) frame
{
	NSLog(@"QuartzClockView initWithFrame");
    self = [super initWithFrame:frame];
    [self initColors];
    return self;
}

- (id) initInDock
{
	NSLog(@"QuartzClockView initInDock");
    self = [super init];
    [self initColors];
	isDocked = YES ;
    return self;
}

- (void) awakeFromNib
{
	NSLog(@"QuartzClockView awakeFromNib");
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

- (void) mouseDown : (NSEvent*) theEvent
{    
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
        NSSize  size     = [self bounds].size;
        NSPoint oldMouse = self.initialLocation;
        
        // adjust relative to center of circle
        int X = newMouse.x > size.width/2.0  ? 1 : -1;
        int Y = newMouse.y > size.height/2.0 ? 1 : -1;

        CGFloat dx = X * (newMouse.x - oldMouse.x) /2.0 ;
        CGFloat dy = Y * (newMouse.y - oldMouse.y) /2.0 ;

        NSRect  newFrame = [self.window frame];
        newFrame.origin.x    -= dx;
        newFrame.origin.y    -= dy;
        newFrame.size.width  += dx*2;
        newFrame.size.height += dy*2;

        if ( newFrame.size.width  > 3000 ) newFrame.size.width  = 3000;
        if ( newFrame.size.height > 2000 ) newFrame.size.height = 2000;
        if ( newFrame.size.width  <   10 ) newFrame.size.width  =   10;
        if ( newFrame.size.height <   10 ) newFrame.size.height =   10;
        
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
	NSRect rect = dirtyRect ; // [self bounds] ;
	
	NSBezierPath* rectPath = [NSBezierPath bezierPath];
	[rectPath appendBezierPathWithRect:rect];
	
	int margin = larger(lesser(rect.size.width /(isDocked?10:20),10) ,4) ;
	
	rect.origin.x     += margin   ;
	rect.origin.y	  += margin   ;
	rect.size.width  -= margin*2 ;
	rect.size.height -= margin*2 ;
	
	NSBezierPath* circlePath = [NSBezierPath bezierPath];
	[circlePath appendBezierPathWithOvalInRect:rect];
	
	if ( isDocked ) {
		// zap the background
		[[NSColor clearColor] set];
		[rectPath fill];
		
		// fill a blue circle
        [self.backgroundColor setFill];
		[circlePath fillGradientFrom:backgroundColor to:gradientColor angle:0.0];
		
		CGContextSetShadow(context, CGSizeMake(4.0f, -4.0f), 2.0f);
		// stroke a white circle
		[rimColor setStroke];
		[circlePath setLineWidth:margin/2];
		[circlePath stroke];
		
	} else {
		// gradient background and circle
		// [rectPath   fillGradientFrom:[NSColor blueColor] to:[NSColor blueColor/*greenColor*/] angle:0.0];
		[circlePath fillGradientFrom:backgroundColor to:gradientColor angle:0.0];
		
		// stroke a white circle with a shadow
		CGContextSetShadow(context, CGSizeMake(4.0f, -4.0f), 2.0f);
		[rimColor setStroke];
		[circlePath setLineWidth:margin];
		[circlePath stroke];
		CGContextSetShadow(context, CGSizeMake(6.0f, -6.0f), 0.0f);
		
	}
    // prepare to draw the hands in white
    float r = [handsColor redComponent];
    float g = [handsColor greenComponent];
    float b = [handsColor blueComponent];
    float a = [handsColor alphaComponent];
    
    CGContextSetRGBStrokeColor(context, r, g, b, a);
	
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

// That's all Folks!
////

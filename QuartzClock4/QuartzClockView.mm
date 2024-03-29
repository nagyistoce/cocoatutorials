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

#import <Quartz/Quartz.h>
#import "QuartzClockView.h"
#import "QuartzClockAppDelegate.h"
#import "Extensions.h"
#import "IconFamily.h"

QuartzClockView* theWindowView;
QuartzClockView* theDockView;

@implementation QuartzClockView

@synthesize initialLocation;
@synthesize initialSize;
@synthesize backgroundColor;
@synthesize gradientColor;
@synthesize handsColor;
@synthesize rimColor;
@synthesize marksColor;
@synthesize radialGradient;
@synthesize ignoreMouse;
@synthesize angle;

- (void) dealloc 
{
	[self stopClockUpdates];
	[super dealloc];
}

- (void) initColors
{
    radialGradient  = NO;
    angle           = 0.0;
    backgroundColor = [NSColor yellowColor];
    gradientColor   = [NSColor    redColor];
    handsColor      = [NSColor  blackColor];
    marksColor      = [NSColor  blackColor];
    rimColor        = [NSColor  whiteColor];
}

- (id) init
{
	self = [super init];
    [self initColors];
	return self ;
}

- (id) initWithFrame : (NSRect) frame
{
    self = [super initWithFrame:frame];
    [self initColors];
    return self;
}

- (id) initInDock
{
    self = [super init];
    [self initColors];
    return self;
}

- (void) awakeFromNib
{
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

- (IBAction) borderToggle : (id) sender
{
    [self.window setStyleMask:[QuartzClockView borderToggle : self.window.styleMask]];
}

- (IBAction) borderMustHaveFrame : (id) sender
{
    if ( self.window.styleMask != [QuartzClockView borderMask] )
        [self borderToggle:sender];
}
          
- (IBAction) showHide : (id) sender;
{
    if ( self.window.isVisible  ) {
        [self.window orderOut:sender] ;
    } else {
        [self.window orderFront:sender] ;
    }
    for ( NSMenuItem* mi in menuFindByTitle([NSApp mainMenu],@"Clock") ) {
        mi.state = self.window.styleMask == self.window.isVisible ?  NSOffState : NSOnState;
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
}

- (void) mouseDragged : (NSEvent*) theEvent
{
    if ( self.ignoreMouse ) return ;

    NSRect  newFrame = [self.window frame];
    if ( [NSEvent isCommandKeyDown]  ) {
        NSPoint newMouse = [theEvent locationInWindow];
        NSSize  size     = [self bounds].size;
        NSPoint oldMouse = self.initialLocation;
        BOOL    bSquare  = [NSEvent isOptionKeyDown];
        
        // adjust relative to center of circle
        int X = newMouse.x > size.width/2.0  ? 1 : -1;
        int Y = newMouse.y > size.height/2.0 ? 1 : -1;

        CGFloat dx = X * (newMouse.x - oldMouse.x) /2.0 ;
        CGFloat dy = Y * (newMouse.y - oldMouse.y) /2.0 ;

        newFrame.origin.x    -= dx;
        newFrame.origin.y    -= dy;
        newFrame.size.width  += dx*2;
        newFrame.size.height += dy*2;

        if ( bSquare ) {
            CGFloat s = lesseR(newFrame.size.width,newFrame.size.height);
            dx = (s-newFrame.size.width ) /2.0;
            dy = (s-newFrame.size.height) /2.0;
            newFrame.origin.x    -= dx;
            newFrame.origin.y    -= dy;
            newFrame.size.width  += dx*2;
            newFrame.size.height += dy*2;
        }

        if ( newFrame.size.width  > 3000 ) newFrame.size.width  = 3000;
        if ( newFrame.size.height > 2000 ) newFrame.size.height = 2000;
        if ( newFrame.size.width  <   10 ) newFrame.size.width  =   10;
        if ( newFrame.size.height <   10 ) newFrame.size.height =   10;
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
        newFrame.origin = newOrigin;
    }

    [self.window setFrame:newFrame display:YES];
    [[NSUserDefaults standardUserDefaults] setObject:NSStringFromRect(newFrame) forKey:@"ClockPos"];
}

- (void) rightMouseDown : (NSEvent*) theEvent {    
    NSLog(@"QuartzClockView::rightMouseDown");
}

- (BOOL) acceptsFirstResponder
{
//  NSLog(@"QuartzClockView::acceptsFirstResponder");
    return YES;
}

- (BOOL) acceptsFirstMouse : (NSEvent*) theEvent
{
//  NSLog(@"QuartzClockView::acceptsFirstMouse");
    return YES;
}

- (void) mouseMoved : (NSEvent*) theEvent
{
//    static int count = 0 ;
//  NSLog(@"QuartzClockView::mouseMoved %d",count++);
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
	CGContextRef  context  = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
	NSRect        rect     = dirtyRect ; // [self bounds] ;
	NSBezierPath* rectPath = [NSBezierPath bezierPath];

	[rectPath appendBezierPathWithRect:rect];
	
    CGFloat radius = lesseR(rect.size.width,rect.size.height)/2.0;
	BOOL    small  = radius < 70.0;
	int     margin = larger(lesser(rect.size.width /(small?10:20),10) ,4) ;
	
	rect.origin.x    += margin   ;
	rect.origin.y	 += margin   ;
	rect.size.width  -= margin*2 ;
	rect.size.height -= margin*2 ;
    
    // zap the background
    [[NSColor clearColor] set];
    [rectPath fill];
    
    CGFloat W  = rect.size.width/2.0;
    CGFloat H  = rect.size.height/2.0;
    NSPoint C  = NSMakePoint(rect.origin.x+W,rect.origin.y+H) ;
    CGFloat A  = ((int)angle)%360;
    CGFloat R  = A*lesseR(W,H)/360.0 + lesseR(W,H);

    // http://developer.apple.com/library/mac/#documentation/Cocoa/Conceptual/CocoaDrawingGuide/AdvancedDrawing/AdvancedDrawing.html
    NSBezierPath* circlePath = [NSBezierPath bezierPath];
    [circlePath appendBezierPathWithOvalInRect:rect];
    
    NSGradient* aGradient = [[NSGradient alloc] 
                             initWithStartingColor : backgroundColor
                             endingColor : gradientColor
                             ];
    // fill circle
    CGContextSaveGState(context);
    [circlePath setClip];
    if ( radialGradient ) { 
        [aGradient drawFromCenter:C radius:0 toCenter:C radius:R options:0xff]; 
    } else {
        [aGradient drawInRect:rect angle:angle+90.0];
    }
    CGContextRestoreGState(context);

    // stroke the rim
    CGContextSetShadow(context, CGSizeMake(4.0f, -4.0f), 2.0f);
	[rimColor setStroke];
    [circlePath setLineWidth:small?margin/2:margin];
    [circlePath stroke];
    
	// get the time
    NSCalendar *cal = [[NSCalendar alloc] initWithCalendarIdentifier:NSGregorianCalendar];
	NSDate* now = [NSDate date];
	int h = [[cal components:NSHourCalendarUnit fromDate:now] hour] %12 ;
	int m = [[cal components:NSMinuteCalendarUnit fromDate:now] minute];
	int s = [[cal components:NSSecondCalendarUnit fromDate:now] second];
	[cal release];
	
	// calculate angle of the hands
	double hAlpha = rad(h*30 +(30*m/60) -90);
	double mAlpha = rad(m*6 -90);
	double sAlpha = rad(s*6 -90);
	
	// color of hands
    float r = [[handsColor colorUsingColorSpaceName:NSCalibratedRGBColorSpace]redComponent];
    float g = [[handsColor colorUsingColorSpaceName:NSCalibratedRGBColorSpace]greenComponent];
    float b = [[handsColor colorUsingColorSpaceName:NSCalibratedRGBColorSpace]blueComponent];
    float a = [[handsColor colorUsingColorSpaceName:NSCalibratedRGBColorSpace]alphaComponent];
    
    CGContextSetShadow(context, CGSizeMake(6.0f, -6.0f), 0.0f);
    CGContextSetRGBStrokeColor(context, r, g, b, a);
	CGContextSetLineCap(context,kCGLineCapRound);
    
    // width of hands
    float hour   = 10; // small ? 8.0 : 10.0;
    float minute =  6; // small ? 5.0 :  6.0;
    float second =  3;

    // draw hands
    [self drawLineForContext:context width:hour   angle:hAlpha radiusx:self.frame.size.width/2.0 - 18 radiusy:self.frame.size.height/2.0 - 18];
	[self drawLineForContext:context width:minute angle:mAlpha radiusx:self.frame.size.width/2.0 - 14 radiusy:self.frame.size.height/2.0 - 14];
	[self drawLineForContext:context width:second angle:sAlpha radiusx:self.frame.size.width/2.0 - 12 radiusy:self.frame.size.height/2.0 - 12];
}

- (void) update
{
	[self setNeedsDisplay:YES];
	[[NSApp dockTile] display];
    [self setTitleNow:nil];
	// NSLog(@"update");
}

- (void) startClockUpdates
{
	[self update];
	timer = [NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(update) userInfo:nil repeats:YES];
}

- (void) stopClockUpdates
{
	[timer invalidate];
	timer = nil;
}

- (IBAction) saveAs : (id) sender
{
	NSLog(@"QuartzClockView::saveAs");
#if 0   
    // NSView* view = theWindowView ;
/*
    [view imageRepresentationType:
    NSImage * i = [[NSImage alloc] initWithSize:[view frame].size];
     
    NSImage* i = [NSImage alloc]initWithData:[NSData * d = [i TIFFRepresentation];
    [i lockFocus];
    if ([view lockFocusIfCanDrawInContext:[NSGraphicsContext currentContext]]) {
        [view displayRectIgnoringOpacity:[view frame] inContext:[NSGraphicsContext currentContext]];
        [view unlockFocus];
    }
    [i unlockFocus];
*/                                              
    NSImage* screenshot = [[[NSImage alloc] initWithSize:NSMakeSize(512,512)]autorelease];
    [screenshot setFlipped:YES];
    [screenshot lockFocus];
    NSGraphicsContext*  graphicsContext = [NSGraphicsContext currentContext];
    CGContextRef        contextRef      = (CGContextRef) [graphicsContext graphicsPort];
    NSSize      scale = NSMakeSize(512.0/self.frame.size.width,512.0/self.frame.size.height);
    CGContextScaleCTM(contextRef, scale.width, scale.height);
    [theWindowView drawRect: [self frame]];
    [screenshot unlockFocus];
    IconFamily* imageAsIcon = [[IconFamily alloc] initWithThumbnailsOfImage:screenshot];

//  NSData * d = [screenshot TIFFRepresentation];
//  [d writeToFile:@"/path/to/my/test.tiff" atomically:YES];
    
    NSSavePanel * sp = [NSSavePanel savePanel];
    [sp setTitle:@"Save File"];
    [sp setAllowedFileTypes:[NSArray arrayWithObject:@"icns"]];
    [sp setAllowsOtherFileTypes:NO];
    NSInteger returnCode = [sp runModal];
    if (returnCode == NSFileHandlingPanelOKButton) {
        NSURL * chosenURL = [sp URL];
        [imageAsIcon writeToFile:[chosenURL path]];
        [[NSWorkspace sharedWorkspace] openURL:chosenURL];
    }
#endif
    
}

- (void) copyFrom : (QuartzClockView*) other
{
    self.backgroundColor    = other.backgroundColor;
    self.gradientColor      = other.gradientColor;
    self.handsColor         = other.handsColor;
    self.rimColor           = other.rimColor;
    self.marksColor         = other.marksColor;
    self.radialGradient     = other.radialGradient;
//  self.ignoreMouse        = other.ignoreMouse;
    self.angle              = other.angle;
    [self update];
}

@end

// That's all Folks!
////

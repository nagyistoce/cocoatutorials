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

#import "QuartzClockView.h"
#import <Quartz/Quartz.h>

// http://www.cocoadev.com/index.pl?GradientFill

@interface NSBezierPath (Additions)
- (void)fillGradientFrom:(NSColor*)inStartColor to:(NSColor*)inEndColor angle:(float)inAngle;
@end

@implementation NSBezierPath (Additions)

- (void)fillGradientFrom:(NSColor*)inStartColor to:(NSColor*)inEndColor angle:(float)inAngle
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

@implementation QuartzClockView

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
	[self startClockUpdates];
}

// http://iphone-dev-tips.alterplay.com/2010/03/analog-clock-using-quartz-core.html
inline double rad(double deg)
{
	return deg / 180.0 * M_PI;
}

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


- (void) drawLineForContext: (const CGContextRef&) context
					  width: (float) _width
					  angle: (double) _angle
					 length: (double) radius
{
	CGPoint c = CGPointMake(self.frame.size.width/2.0, self.frame.size.height/2.0);
	CGContextSetLineWidth(context, _width);
	NSPoint center = [self center:[self bounds]];
	NSPoint size = [self size:[self bounds]];
	CGContextMoveToPoint(context, center.x,center.y); //  self.bounds.center.x, self.bounds.center.y );
	CGPoint addLines[] =
	{
		CGPointMake(size.x,size.y), // self.frame.size.width/2.0, self.frame.size.height/2.0),
		CGPointMake(radius*cos(_angle) +c.x, radius*sin(_angle) +c.y),
	};
	CGContextAddLines(context, addLines, sizeof(addLines)/sizeof(addLines[0]));
	CGContextStrokePath(context);
}

- (BOOL) isFlipped 
{
	return YES ;
}

static int lesser(int a,int b) { return a < b ? a : b ; }
static int larger(int a,int b) { return a > b ? a : b ; }

#define NsString NSString* ;

- (void) drawRect : (NSRect) dirtyRect
{
	// NSLog(@"drawRect");
	NSString* f = [[NSString alloc]initWithFormat:@"hello world"];
	NSLog(@"%s",f);
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
		[[NSColor blueColor]setFill];
		[circlePath fill];
		
		// stroke a white circle
		[[NSColor whiteColor]setStroke];
		[circlePath setLineWidth:margin/2];
		[circlePath stroke];
		
		// prepare to draw the hands in white
		CGContextSetRGBStrokeColor(context, 1.0, 1.0, 1.0, 1.0);
	} else {
		// gradient background and circle
		[rectPath   fillGradientFrom:[NSColor blueColor] to:[NSColor blueColor/*greenColor*/] angle:0.0];
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
	
	[self drawLineForContext:context width:8.0 angle:hAlpha length:self.frame.size.width/2.0 - 18];
	[self drawLineForContext:context width:5.0 angle:mAlpha length:self.frame.size.width/2.0 - 14];
	[self drawLineForContext:context width:2.0 angle:sAlpha length:self.frame.size.width/2.0 - 12];
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

- (BOOL) isDocked { return isDocked;}
- (void) setIsDocked:(BOOL)t { isDocked = t ; }

@end

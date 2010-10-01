//
//  ix.mm
//  This file is part of ix
// 
//  ix is free software: you can redistribute it and/or modify
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
//  Created 2010 http://clanmills.com robin@clanmills.com
//

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>
#import <Quartz/Quartz.h>
#import <ApplicationServices/ApplicationServices.h>

#define FILENAME @"FileName"
#define IMAGE    @"Image"

static enum
{	OK = 0 
,	SYNTAX 
,   NOT_IMAGE
,	NO_IMAGES
,	NO_PDF_FILENAME
}	error = OK ;

static void notImage(NSString* filename)
{
	NSLog(@"%@ is not an image file",filename) ;
//	error = NOT_IMAGE ; // only warn, don't terminate
}

static void noImages()
{
	NSLog(@"Unable to create images array") ;
	error = NO_IMAGES ;
}

static void noPDFFileName(const char* filename)
{
	NSLog(@"Unable to create pdfFileName %s",filename) ;
	error = NO_PDF_FILENAME ;
}

static void syntax(void)
{
	NSLog(@"syntax: ix <file.pdf> [icns]+") ;
	error = SYNTAX ;
}

static NSComparisonResult comp(NSString* a,NSString* b) { return [a localizedCaseInsensitiveCompare : b]; }

static NSComparisonResult sortFunction (id a, id b, void* )
{
	a = [a valueForKey:FILENAME];
	b = [b valueForKey:FILENAME];
	NSComparisonResult result = comp([a lastPathComponent],[b lastPathComponent]);
	if ( !result )     result = comp(a,b) ;
	return result ;
}

static void doIt(NSString* pdfFileName,NSMutableArray* images)
{
	// sort the images using the filename (case insensitive)
    [images sortUsingFunction:sortFunction context:nil];
	
	PDFDocument* document = [[PDFDocument alloc]init] ;
	if ( document ) {

		////
		// store an outline (with the name of the file on the label for every page)
		PDFOutline* outlines = [[PDFOutline alloc]init] ;
		[document setOutlineRoot:outlines] ;
		
		////
		// cycle over the images
		for ( int i = 0 ; i < [images count] ; i++ ) {
			NSDictionary* dict		= [images objectAtIndex:i];
			NSImageRep* imageRep	= [dict  valueForKey:IMAGE];
			NSString*	fileName	= [dict  valueForKey:FILENAME];
			int			width		= [imageRep pixelsWide];
			int			height		= [imageRep pixelsHigh];
			NSLog(@"%@ width,height = %dx%d",fileName,width,height) ;
		
			////
			// create the PDF page with the image and insert it at the end of the document
			NSImage* image = [[NSImage alloc]init] ;
			[image addRepresentation:imageRep] ;
			PDFPage* page = [[ PDFPage alloc]initWithImage:(CIImage*)image];

			if ( page ) {
				[document insertPage:page atIndex:i] ;
				
				////
				// store the filename in the Outline
#if 0
				// if fileName begins with '/', throw in another one
				// PDFKit get's confused if a string begins with '/' and creates a name
				// then Preview doesn't know how to render Outline /Title when it's a name
				// Maybe we should report a bug to Apple (this is a solid bug)
				// Oh, and for good measure, the method in the sample code for labelling pages doesn't work
				// And, the documentation about subclass PDFDocument and PDFPage to create labels is also wrong
				// Good Quality Apple Software and Documentation
				bool		bSlash		= [[fileName substringToIndex:1] compare: @"/"] == 0 ;
				const char*	slash		= bSlash ? "/" : "" ; 
				NSString*   label       = [NSString stringWithFormat:@"%s%@",slash,fileName];
#endif			
				PDFOutline* outline = [[PDFOutline alloc]init] ;
				
				[outline setLabel:[fileName lastPathComponent]] ;
				NSPoint point = { width/2, height/2 } ;
			
				PDFDestination* dest = [[PDFDestination alloc]initWithPage:page atPoint:point];
				[outline setAction:[[PDFActionGoTo alloc]initWithDestination:dest]];
				[outlines insertChild:outline atIndex:i];

				[page release];
			}
		}

		////
		// write the PDF file and display it
		if ( [document writeToFile:pdfFileName] ) {
			[[NSWorkspace sharedWorkspace] openFile:pdfFileName];
		} else {
			NSLog(@"unable to write pdffile %@",pdfFileName);
		}
	}
}

static int bigger(int a,int b) { return a > b ? a : b ; }

int main (int argc, const char * argv[])
{
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
	
	if ( argc < 3 ) syntax() ;
	
	NSMutableArray* images = error==OK ? [[NSMutableArray alloc]init]			: nil ;
	NSString* pdfFileName  = error==OK ? [NSString stringWithUTF8String:argv[1]]: nil ;
	
	if ( !error && !images ) noImages() ;
	if ( !error && !pdfFileName ) noPDFFileName(argv[1]);
	
	////
	// run over the command line arguments and make sure everything is an image
	for ( int i = 2 ; !error && i < argc ; i++ ) {
		NSString* fileName =  [NSString stringWithUTF8String:argv[i]];
		NSImage*  image    =  [[NSImage alloc] initWithContentsOfFile:fileName];
		if ( !image ) notImage(fileName) ;
		
		if ( image)
		{
			NSArray* reps = [image representations] ;
			////
			// find the biggest representation
			int jBig	= -1 ;
			int biggest = 0 ;
			for ( int j = 0 ; j < [reps count] ; j++ ) {
				NSImageRep* imageRep = [ reps objectAtIndex:j] ;
				int width	= [imageRep pixelsWide];
				int height	= [imageRep pixelsHigh];
				int big		= bigger(width,height) ;
				if ( big > biggest ) {
					jBig = j ;
					biggest = big ;
				}
			}

			////
			// save the imageRep and filename in a dictionary in the images array
			if ( jBig >= 0 ) {
				NSImageRep*		imageRep= [reps objectAtIndex:jBig];
				NSDictionary*	dict	= [NSDictionary dictionaryWithObjectsAndKeys:imageRep,IMAGE,fileName,FILENAME,nil] ;
				[images addObject:dict] ;
			}
		}
	}
	
	if ( !error ) doIt(pdfFileName,images) ;
	
    [pool drain];
    return 0;
}

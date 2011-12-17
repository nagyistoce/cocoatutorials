//	Copyright (c) 2007 Adam Knight
//
//	Permission is hereby granted, free of charge, to any person obtaining a
//	copy of this software and associated documentation files (the
//	"Software"), to deal in the Software without restriction, including
//	without limitation the rights to use, copy, modify, merge, publish,
//	distribute, sublicense, and/or sell copies of the Software, and to
//	permit persons to whom the Software is furnished to do so, subject to
//	the following conditions:
//
//	The above copyright notice and this permission notice shall be included
//	in all copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
//	OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//	IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
//	CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
//	TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//	SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#import <AppKit/AppKit.h>
#include <getopt.h>

// make -B SetIcon "LDFLAGS=-framework AppKit" MACOSX_DEPLOYMENT_TARGET=10.5


void usage() {
	printf("usage: SetFile -i image target\n");
	exit(EXIT_FAILURE);
}

int main (int argc, char * argv[]) {
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	char option;
	NSString *sourceFile = nil;
	NSString *targetFile = nil;
	const char* filename = NULL;
	BOOL result;
	
	while ((option = getopt(argc, argv, "i:")) != -1) {
		switch (option) {
			case 'i':
				sourceFile = [NSString stringWithUTF8String:optarg];
				filename = optarg ;
				break;
			default:
				usage();
				break;
		}
	}
	
	if (optind < argc)
		targetFile = [NSString stringWithUTF8String:(char*)argv[optind]];
	else
		usage();
	
	// Begin
	result = [[NSFileManager defaultManager] fileExistsAtPath:sourceFile];

	if (!result) {
		printf("file does not exist: %s\n", filename);
		exit(EXIT_FAILURE);
	}

	NSImage *icon = [[[NSImage alloc] initWithContentsOfFile:sourceFile] autorelease];
	
	if (!icon) {
		printf("file is not a valid image file: %s\n", filename);
		exit(EXIT_FAILURE);
	}
	
	result = [[NSFileManager defaultManager] fileExistsAtPath:targetFile];

	if (!result) {
		printf("file does not exist: %s\n", filename);
		exit(EXIT_FAILURE);
	}
	
	result = [[NSWorkspace sharedWorkspace] setIcon:icon forFile:targetFile options:0];

	if (!result) {
		printf("failed to set icon for file: %s\n", filename);
		exit(EXIT_FAILURE);
	}
	
	[pool release];
    return 0;
}

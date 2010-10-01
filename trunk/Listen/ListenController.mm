//
//  ListenController.m
//  This file is part of Listen
// 
//  Listen is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Listen is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with Listen.  If not, see <http://www.gnu.org/licenses/>.
//
//  Written by Robin Mills, San Jose, CA, USA. 2010 http://clanmills.com
//

#import "ListenController.h"

@implementation ListenController

static NSDictionary* dictDump(NSDictionary* dictionary)
{
	NSLog(@"dictDump");
	for (id key in dictionary) {
		NSLog(@"key: %@, value: %@", key, [dictionary objectForKey:key]);
	}
	return dictionary ;
}

- (void) awakeFromNib
{
	NSLog(@"awakeFromNib");
	
	[[self window] setDelegate:self];
	[[self window] setReleasedWhenClosed:YES];
	
	[listenButton setEnabled:YES] ;
	[stopButton setEnabled:NO] ;
	
}

- (id) init
{
	NSDictionary* dict = [[NSBundle mainBundle] infoDictionary];
	dictDump(dict) ;

	[super init];
	synth = [[NSSpeechSynthesizer alloc] init]; 
	[synth setDelegate:self];
	
	recogn = nil;
	return self ;
}

- (void) dealloc
{
	[synth release];
	[recogn release];
	[super dealloc];
} 

- (void) speechRecognizer:(NSSpeechRecognizer *)sender didRecognizeCommand:(id)aCmd
{
	NSLog(@" didRecognizeCommand %@",aCmd) ;
	
	if ([aCmd isEqualToString:@"jump"]) {
        [synth startSpeakingString:@"How High?"];
    }
	if ([aCmd isEqualToString:@"roll"]) {
        [synth startSpeakingString:@"Over beethoven?"];
    }
	if ([aCmd isEqualToString:@"sing, sing a song"]) {
        [synth startSpeakingString:@"sing out loud"];
    }
	if ([aCmd isEqualToString:@"one"]) {
        [synth startSpeakingString:@"in the morning?"];
    }
	if ([aCmd isEqualToString:@"goodbye"] || [aCmd isEqualToString:@"stop"] ) {
		[self stop:aCmd];
    }
}

- (IBAction) listen:(id)sender
{
	[listenButton setEnabled:NO] ;
	if ( !recogn ) {
		recogn = [[NSSpeechRecognizer alloc] init];
		NSLog(@"count = %d",[recogn retainCount]);
		[recogn setDelegate:self];
		[recogn setListensInForegroundOnly:NO];
		[recogn setBlocksOtherRecognizers:YES];
		[recogn startListening];
		NSLog(@"count = %d",[recogn retainCount]);
		
		[listenButton setEnabled:NO] ;
		[stopButton setEnabled:YES] ;
	}
	
	NSString* title = @"What do you want me to do?" ;
	
	[recogn setDisplayedCommandsTitle:title];
	NSArray *cmds = [NSArray arrayWithObjects:@"sing, sing a song", @"jump", @"roll", @"one", @"goodbye", @"stop", nil];
	[recogn setCommands:cmds];
	
	[synth startSpeakingString:title];
}

- (IBAction) stop:(id)sender
{
	if ( recogn ) {
		[listenButton setEnabled:YES] ;
		[stopButton setEnabled:NO] ;
		
		[synth startSpeakingString:@"OK.  Press listen when you want me back."];
		NSLog(@"count = %d",[recogn retainCount]);
		[recogn stopListening];
		NSLog(@"count = %d",[recogn retainCount]);
		[recogn setDelegate:nil];
		NSLog(@"count = %d",[recogn retainCount]);
		[recogn release];
		recogn = nil ;
		[listenButton setEnabled:YES] ;
	}
}

- (BOOL) windowShouldClose:(id)sender
{
	NSLog(@"windowShouldClose");
	return YES ;
}

@end

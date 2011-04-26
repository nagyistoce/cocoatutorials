//
//  QuartzClockPreferences.h
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

#import <Cocoa/Cocoa.h>
#import "QuartzClockView.h"


@interface QuartzClockPreferences : NSWindowController
    <NSWindowDelegate>
{
//  Outlets
    IBOutlet NSColorWell*       colorWell;
    IBOutlet QuartzClockView*   clock;
    IBOutlet NSButton*          applyButton;
    IBOutlet NSButton*          cancelButton;
    IBOutlet NSButton*          marks;
    IBOutlet NSButton*          radialGradient;
    IBOutlet NSButton*          onTop;
    IBOutlet NSSlider*          angle;
    
//  Properties
    SEL                         colorSelector;
    SEL                         selectedElement;
    BOOL                        bDirty;
    BOOL                        bDock;
@private
    
}

// actions
- (IBAction) changeColor        : (id) sender; // from the color well

- (IBAction) backgroundSelected : (id) sender; // from the radio group
- (IBAction) gradientSelected   : (id) sender;
- (IBAction) handsSelected      : (id) sender;
- (IBAction) rimSelected        : (id) sender;
- (IBAction) marksSelected      : (id) sender;

- (IBAction) dockViewSelected   : (id) sender; // from the radio group
- (IBAction) windowViewSelected : (id) sender;

- (IBAction) apply              : (id) sender; // standalone buttons
- (IBAction) cancel             : (id) sender;

// methods
- (void)     syncColor          : (id) sender;

@property (assign) QuartzClockView* clock;

// property getters/setters
- (BOOL)    bDirty ;
- (void) setBDirty : (BOOL) b ;

@end

// That's all Folks!
////

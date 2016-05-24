//
//  AC_Controller.h
//  Acid.Cam.2
//
//  Created by Jared Bruni on 6/3/13.
//  Copyright (c) 2013 Jared Bruni. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#include<sstream>

typedef void (*pixel)(int x, int y, unsigned char *pixels);
typedef void (*drawn)();
extern pixel pix;
extern drawn d;

@interface AC_Controller : NSObject<NSTableViewDataSource, NSTableViewDelegate>{

    IBOutlet NSPopUpButton *resolution;
    IBOutlet NSPopUpButton *vid_format;
    IBOutlet NSPopUpButton *device_index, *startaction;
    IBOutlet NSTextField   *prefix_input, *framecount, *goto_ti, *goto_fr;
    IBOutlet NSButton      *record_op, *record_audio;
    IBOutlet NSButton      *prefix_grab;
    IBOutlet NSWindow      *window1, *window2;
    IBOutlet NSButton      *videoFileInput, *selectVideoFile;
    
    IBOutlet NSTextField *prefix_input_2, *video_file;
    IBOutlet NSPopUpButton *vid_format_2;
    IBOutlet NSButton *record_op2,*startaction_2;
    IBOutlet NSTextView *t_view;
    IBOutlet NSMenuItem *menuNegative, *menuNegFlash, *menuBlurFirst, *menuBlurSecond, *menuSecondPass, *menuReverseColors, *menuSlideShow, *menuSlideRand, *menuPaused, *stop_prog;
    IBOutlet NSButton *startProg;
    IBOutlet NSPopUpButton *current_filter, *corder, *output_Type;
    IBOutlet NSComboBox *filter_combo;
    IBOutlet NSWindow   *custom_window, *goto_frame;
    IBOutlet NSTableView *table_view;
    
    IBOutlet NSSlider *goto_t, *goto_f;
    IBOutlet NSSlider *slider_red, *slider_green, *slider_blue;
    IBOutlet NSTextField *slider_pos, *slider_red_pos, *slider_green_pos, *slider_blue_pos;
    NSImage *current_image;
    IBOutlet NSPopUpButton *filter_selector;
    IBOutlet NSButton *check_box, *reverse_box, *opposite_dir;
    IBOutlet NSPopUpButton *rgb_box;
    IBOutlet NSWindow *alpha_window;
	IBOutlet NSWindow *plugin_window;
	IBOutlet NSTextField *plugin_name;
   

}
- (IBAction) selectPlugin: (id) sender;
- (IBAction) startProgram: (id) sender;
- (IBAction) openWebcamDialog: (id) sender;
- (IBAction) startVideoProgram: (id) sender;
- (IBAction) selectFile: (id) sender;
- (IBAction) takeSnopshot: (id) sender;
- (IBAction) takeSnapshotPNG: (id) sender;
- (IBAction) checkChanged: (id) sender;
- (IBAction) selectFileForPrefix: (id) sender;
- (IBAction) quitProgram: (id) sender;
- (IBAction) setOption_Negative: (id) sender;
- (IBAction) setOption_NegFlash: (id) sender;
- (IBAction) setOption_BlurFirst: (id) sender;
- (IBAction) setOption_BlurSecond: (id) sender;
- (IBAction) setOption_SecondPass: (id) sender;
- (IBAction) setOption_ReverseColors: (id) sender;
- (IBAction) setOption_SlideShow: (id) sender;
- (IBAction) setOption_SlideRand: (id) sender;
- (IBAction) stopProgram: (id) sender;
- (IBAction) changeFilter: (id) sender;
- (IBAction) addCustomItem: (id) sender;
- (IBAction) removeCustomItem: (id) sender;
- (IBAction) changeOrder: (id) sender;
- (IBAction) pauseProgram: (id) sender;
- (IBAction) goto_Frame: (id) sender;
- (IBAction) setGoto: (id) sender;
- (IBAction) openGoto: (id) sender;
- (IBAction) changeFilterIndex: (id) sender;
- (IBAction) changeRGB: (id) sender;
- (IBAction) changeReverse: (id) sender;
- (IBAction) changeNegate: (id) sender;
- (void) cvProc: (id) sender;
- (IBAction) pauseVideo: (id) sender;
- (void) stopCV_prog;
- (pixel) loadPlugin: (NSString *)str;

@end

extern void flushToLog(std::ostringstream &sout);
extern void setFrameLabel(std::ostringstream &text);
extern void setSliders(int frame_count);
extern void jumptoFrame(int frame);
extern NSTimer *renderTimer, *validProc;
extern AC_Controller *controller;
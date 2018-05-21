//
//  AC_Controller.h
//  Acid.Cam.2
//
//  Created by Jared Bruni on 6/3/13.
//  https://github.com/lostjared
//  Copyright (c) 2018 Jared Bruni. All rights reserved.
/*
 
 This software is dedicated to all the people that struggle with mental illness. 
 
 Website: http://lostsidedead.com
 YouTube: http://youtube.com/LostSideDead
 Instagram: http://instagram.com/jaredbruni
 Twitter: http://twitter.com/jaredbruni
 Facebook: http://facebook.com/LostSideDead0x
 
 You can use this program free of charge and redistrubute as long
 as you do not charge anything for this program. This program is 100%
 Free.
 
 
 BSD 2-Clause License
 
 Copyright (c) 2018, Jared Bruni
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this
 list of conditions and the following disclaimer.
 
 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#undef check
#include"videocapture.h"
#include<sstream>

// type def
typedef void (*pixel)(int x, int y, unsigned char *pixels);
typedef void (*drawn)();

// global variables
extern pixel pix;
extern drawn d;
extern int camera_mode;
extern bool disableFilter;
extern NSThread *background;
extern  bool camera_active;
extern unsigned long frame_proc;
extern bool resize_value;
extern void SearchForString(NSString *s);

@interface SearchController : NSObject<NSTableViewDataSource, NSTableViewDelegate> {
    
}
@end

// Main controller class for the program
@interface AC_Controller : NSObject<NSTableViewDataSource, NSTableViewDelegate>{
    // interface builder cocoa
    IBOutlet NSPopUpButton *resolution;
    IBOutlet NSPopUpButton *vid_format;
    IBOutlet NSPopUpButton *device_index, *startaction;
    IBOutlet NSTextField   *prefix_input, *framecount, *goto_ti, *goto_fr, *t_red, *t_green, *t_blue;
    IBOutlet NSButton      *record_op;
    IBOutlet NSButton      *prefix_grab;
    IBOutlet NSWindow      *window1, *window2;
    IBOutlet NSButton      *videoFileInput, *selectVideoFile;
    IBOutlet NSButton *chk_repeat;
    IBOutlet NSTextField *prefix_input_2, *video_file;
    IBOutlet NSPopUpButton *vid_format_2;
    IBOutlet NSButton *record_op2,*startaction_2;
    IBOutlet NSTextView *t_view;
    IBOutlet NSMenuItem *menuPaused, *stop_prog, *disable_filters, *pause_step, *menu_freeze;
    IBOutlet NSMenuItem *rotate_v, *rotate_h;
    IBOutlet NSButton *startProg;
    IBOutlet NSPopUpButton *current_filter, *corder, *output_Type, *categories, *current_filter_custom, *categories_custom;
    IBOutlet NSWindow   *custom_window, *goto_frame;
    IBOutlet NSTableView *table_view;
    IBOutlet NSSlider *goto_t, *goto_f;
    IBOutlet NSSlider *slider_red, *slider_green, *slider_blue;
    IBOutlet NSTextField *slider_pos, *slider_red_pos, *slider_green_pos, *slider_blue_pos;
    IBOutlet NSPopUpButton *filter_selector;
    IBOutlet NSButton *check_box, *reverse_box, *opposite_dir;
    IBOutlet NSPopUpButton *rgb_box;
    IBOutlet NSWindow *alpha_window;
    IBOutlet NSWindow *plugin_window;
    IBOutlet NSTextField *plugin_name;
    IBOutlet NSComboBox *plugin_dir;
    IBOutlet NSButton *negate_checked;
    IBOutlet NSWindow *image_select;
    IBOutlet NSComboBox *image_combo;
    IBOutlet NSWindow *finish_queue;
    IBOutlet NSProgressIndicator *finish_queue_progress;
    IBOutlet NSSlider *red_slider, *green_slider, *blue_slider;
    IBOutlet NSMenuItem *stretch_scr;
    IBOutlet NSPopUpButton *color_map;
    IBOutlet NSButton *apply_after;
    IBOutlet NSSlider *brightness, *gamma, *saturation;
    IBOutlet NSButton *fade_filter;
    IBOutlet NSTableView *find_table;
    IBOutlet NSTextField *find_text;
    IBOutlet NSWindow *filter_search_window;
    IBOutlet NSColorWell *color_well;
    IBOutlet NSButton *color_chk;
    IBOutlet NSPopUpButton *image_to_set;
    IBOutlet NSPopUpButton *procMode;
    IBOutlet NSWindow *procMode_Window;
    IBOutlet NSButton *up4k;
    IBOutlet NSTextField *cam_frame_rate;
    IBOutlet NSButton *cam_frame_rate_chk;
    SearchController *search_controller;
    
    // variables
    bool pauseStepTrue;
    long frames_captured;
    NSMenu *menu_cat;
    NSMenu *menu_all;
    NSMenu *menu_items[13];
    NSMenu *menu_items_custom[13];
    NSMenu *menu_all_custom;
    NSMenu *menu_cat_custom;
    NSImage *current_image;
    unsigned long freeze_count;
    NSInteger current_fade;
    double current_fade_alpha;
    bool upscale_video;
    bool set_frame_rate;
    double set_frame_rate_val;
}
// classes messages
- (void) createMenu: (NSMenu **)cat menuAll: (NSMenu **)all items: (NSMenu **)it_arr custom:(BOOL)cust;
- (void) fillMenuWithString: (NSMenu *)menu stringValues:(const char **) items;
- (IBAction) menuSelected: (id) sender;
- (IBAction) customMenuSelected:(id) sender;
- (IBAction) selectPlugin: (id) sender;
- (IBAction) startProgram: (id) sender;
- (IBAction) openWebcamDialog: (id) sender;
- (IBAction) startVideoProgram: (id) sender;
- (IBAction) selectFile: (id) sender;
- (IBAction) takeSnopshot: (id) sender;
- (IBAction) takeSnapshotPNG: (id) sender;
- (IBAction) checkChanged: (id) sender;
- (IBAction) disableFilters: (id) sender;
- (IBAction) selectFileForPrefix: (id) sender;
- (IBAction) quitProgram: (id) sender;
- (IBAction) stopProgram: (id) sender;
- (IBAction) changeFilter: (id) sender;
- (IBAction) addCustomItem: (id) sender;
- (IBAction) removeCustomItem: (id) sender;
- (IBAction) moveCustomUp: (id) sender;
- (IBAction) moveCustomDown: (id) sender;
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
- (void) loadDir: (std::string) str;
- (IBAction) setPlugin: (id) sender;
- (IBAction) setNegative: (id) sender;
- (IBAction) selectImage: (id) sender;
- (IBAction) setAsImage: (id) sender;
- (IBAction) downloadNewestVersion: (id) sender;
- (IBAction) stepPause: (id) sender;
- (IBAction) showCustom: (id) sender;
- (IBAction) showActivityLog: (id) sender;
- (IBAction) showSelectImage: (id) sender;
- (IBAction) showAlpha: (id) sender;
- (IBAction) showPlugins: (id) sender;
- (IBAction) setRotate_V:(id) sender;
- (IBAction) setRotate_H:(id) sender;
- (IBAction) rewindToStart:(id) sender;
- (void) camProc: (id) sender;
- (void) camThread: (id) sender;
- (void) stopCamera;
- (IBAction) setRGB_Values: (id) sender;
- (IBAction) menuFreeze: (id) sender;
- (IBAction) changeVideoPos: (id) sender;
- (NSRect) getScreenSize;
- (IBAction) setStretch: (id) sender;
- (IBAction) setColorMap: (id) sender;
- (IBAction) selectedCustomFilter: (id) sender;
- (IBAction) addSearchItem: (id) sender;
- (IBAction) searchForItem: (id) sender;
- (IBAction) viewSearchWindow: (id) sender;
- (void) reloadTable;
- (IBAction) clearBlend: (id) sender;
- (IBAction) reloadCameraInfo: (id) sender;
- (IBAction) setPref: (id) sender;
- (IBAction) showPrefs:(id) sender;
@end

// global variables / functions
extern NSInteger _NSRunAlertPanel(NSString *msg1, NSString *msg2, NSString *button1, NSString *button2, NSString *button3);
extern void flushToLog(std::ostringstream &sout);
extern void setFrameLabel(std::ostringstream &text);
extern void setSliders(long frame_count);
extern void jumptoFrame(long frame);
extern NSTimer *renderTimer, *validProc;
extern AC_Controller *controller;
extern cv::Mat old_frame;
extern bool resize_frame;
extern NSRect rc;

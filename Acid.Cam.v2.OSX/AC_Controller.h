//
//  AC_Controller.h
//  Acid.Cam.2
//
//  Created by Jared Bruni on 6/3/13.
//  https://github.com/lostjared
//  Copyright (c) 2017 Jared Bruni. All rights reserved.
//


#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#undef check
#include"videocapture.h"
#include<sstream>

typedef void (*pixel)(int x, int y, unsigned char *pixels);
typedef void (*drawn)();
extern pixel pix;
extern drawn d;
extern int camera_mode;
extern bool disableFilter;
extern NSThread *background;
extern  bool camera_active;


@interface AC_Controller : NSObject<NSTableViewDataSource, NSTableViewDelegate>{
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
    NSImage *current_image;
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
    bool pauseStepTrue;
    int frames_captured;
    NSMenu *menu_cat;
    NSMenu *menu_all;
    NSMenu *menu_items[12];
    NSMenu *menu_items_custom[12];
    NSMenu *menu_all_custom;
    NSMenu *menu_cat_custom;
    unsigned int freeze_count;
   
}
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
- (void) camProc: (id) sender;
- (void) camThread: (id) sender;
- (void) stopCamera;
- (IBAction) setRGB_Values: (id) sender;
- (IBAction) menuFreeze: (id) sender;

@end

extern void flushToLog(std::ostringstream &sout);
extern void setFrameLabel(std::ostringstream &text);
extern void setSliders(int frame_count);
extern void jumptoFrame(int frame);
extern NSTimer *renderTimer, *validProc;
extern AC_Controller *controller;
extern cv::Mat old_frame;


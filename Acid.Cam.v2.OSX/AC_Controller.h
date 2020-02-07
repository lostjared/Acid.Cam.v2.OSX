/*
 * Software written by Jared Bruni https://github.com/lostjared

This software is dedicated to all the people that experience mental illness.

Website: http://lostsidedead.com
YouTube: http://youtube.com/LostSideDead
Instagram: http://instagram.com/lostsidedead
Twitter: http://twitter.com/jaredbruni
Facebook: http://facebook.com/LostSideDead0x

You can use this program free of charge and redistrubute it online as long
as you do not charge anything for this program. This program is meant to be
100% free.

BSD 2-Clause License

Copyright (c) 2019, Jared Bruni
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
#import<Syphon/Syphon.h>
#import<Foundation/Foundation.h>
#import<Cocoa/Cocoa.h>
#undef check
#include"videocapture.h"
#include<sstream>
#include<string>
#include<unordered_map>
#include<cstdlib>
#include<cstdio>
#include<iomanip>
#include "AC_Renderer.h"
#include "AC_SearchController.h"
#include <GameController/GameController.h>


#ifndef DEBUG_MODE
#define ENABLE_VERSION_UPDATE
#endif

// Filter Info type

// global variables
extern int camera_mode;
extern bool disableFilter;
extern NSThread *background;
extern  bool camera_active;
extern unsigned long frame_proc;
extern bool resize_value;
typedef void (*pixel)(cv::Mat &frame);

extern NSTimer *renderTimer, *validProc;
extern cv::Mat old_frame;
extern bool resize_frame;
extern NSRect rc;
// global function prototypes
extern cv::Mat resizeKeepAspectRatio(const cv::Mat &input, const cv::Size &dstSize, const cv::Scalar &bgcolor);
extern void CustomFilter(cv::Mat &frame, NSMutableArray *list, NSMutableArray *sublist, NSMutableArray *filter_states);
extern void SearchForString(NSString *s);
extern NSInteger _NSRunAlertPanel(NSString *msg1, NSString *msg2, NSString *button1, NSString *button2, NSString *button3);
extern void flushToLog(std::ostringstream &sout);
extern void setFrameLabel(std::ostringstream &text);
extern void setSliders(long frame_count);
extern void jumptoFrame(BOOL showJump, long frame);
extern cv::ocl::Context context;
extern int p_s;
extern int elapsed_counter;
extern NSButton *cycle_chk_val;
extern int delay_value;
@interface Download : NSObject <NSApplicationDelegate,NSURLDownloadDelegate>
@end


class UserArgType {
public:
    int index;
    std::string name;
    std::string other;
    UserArgType(int i, std::string n) : index{i}, name{n} {}
    UserArgType(int i, std::string n1, std::string n2) : index{i}, name{n1}, other{n2} {}
    UserArgType() = default;
};


bool operator<(const UserArgType &o1, const UserArgType &o2);

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
    IBOutlet NSMenuItem *chk_repeat_menu;
    IBOutlet NSTextField *prefix_input_2, *video_file;
    IBOutlet NSPopUpButton *vid_format_2;
    IBOutlet NSButton *record_op2,*startaction_2;
    IBOutlet NSTextView *t_view;
    IBOutlet NSMenuItem *menuPaused, *stop_prog, *disable_filters, *pause_step, *menu_freeze;
    IBOutlet NSMenuItem *rotate_v, *rotate_h;
    IBOutlet NSMenuItem *syphon_enable;
    IBOutlet NSWindow *syphon_window;
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
    IBOutlet NSPopUpButton *proc_change;
    IBOutlet NSPopUpButton *blend_source_amt;
    IBOutlet NSPopUpButton *sy_size;
    IBOutlet NSButton *chk_replace;
    IBOutlet NSTextField *selectedFilename, *colorkey_range, *colorkey_r_low, *colorkey_r_high, *colorkey_g_low, *colorkey_g_high, *colorkey_b_low, *colorkey_b_high;
    IBOutlet NSComboBox *blocked_colors;
    IBOutlet NSColorWell *blocked_color_well, *blocked_color_well_high;
    IBOutlet NSWindow *block_colors_window;
    IBOutlet NSTextField *val_colorkey_r_low, *val_colorkey_r_high, *val_colorkey_g_low, *val_colorkey_g_high, *val_colorkey_b_low, *val_colorkey_b_high;
    IBOutlet NSButton *key_tolerance, *key_range, *chk_blocked_replace, *chk_blocked_key, *chk_spill;
    IBOutlet NSComboBox *user_filter_name;
    IBOutlet NSButton *user_filter_add, *user_filter_remove,*user_filter_set, *user_filter_save;
    IBOutlet NSButton *load_settings;
    IBOutlet NSTextField *video_width, *video_height;
    IBOutlet NSButton *chk_stretch;
    IBOutlet NSImageView *view_image;
    IBOutlet NSImageView *cur_selected_image;
    IBOutlet NSTextField *num_threads;
    IBOutlet NSTextField *label_path;
    IBOutlet NSTextField *col_intense;
    IBOutlet NSButton *chk_preview;
    IBOutlet NSButton *chk_rand_repeat;
    IBOutlet NSTextField *chk_rand_frames;
    IBOutlet NSPopUpButton *chk_rand_mode;
    IBOutlet NSTextField *filename_textfield;
    IBOutlet NSButton *copy_audio;
    IBOutlet NSTextField *ffmpeg_path;
    IBOutlet NSMenuItem *speed_normal, *speed_fast, *speed_vfast;
    IBOutlet NSButton *ffmpeg_support;
    IBOutlet NSTextField *max_frames;
    IBOutlet NSMenuItem *output_video, *check_update;
    IBOutlet NSTextField *video_value;
    IBOutlet NSTextField *video_position;
    IBOutlet NSPopUpButton *set_color_map;
    IBOutlet NSButton *cycle_chk;
    IBOutlet NSTextField *custom_cycle_delay;
    IBOutlet NSTextField *memory_text;
    IBOutlet NSTextField *group_text;
    IBOutlet NSWindow *group_window;
    IBOutlet NSTextField *plugin_name2;
    IBOutlet NSColorWell *color_start, *color_stop;
    IBOutlet NSWindow *color_range_;
    IBOutlet NSTextField *wait_text;
    SearchController *search_controller;
    NSMenu *user_menu;
    NSThread *proc_cv;
    // variables
    bool reset_memory;
    bool pauseStepTrue;
    long frames_captured;
    NSMenu *menu_cat;
    NSMenu *menu_all;
    NSMenu *menu_items[14];
    NSMenu *menu_items_custom[14];
    NSMenu *menu_all_custom;
    NSMenu *menu_cat_custom;
    NSImage *current_image;
    NSInteger current_fade;
    double current_fade_alpha;
    bool upscale_video;
    bool set_frame_rate;
    double set_frame_rate_val;
    BOOL syphon_enabled;
    NSString *custom_path_prefix;
    BOOL restartFilter;
    int program_speed;
    BOOL outputVideo;
    GCExtendedGamepad *profile;
    GCMicroGamepad *micro;
    std::string directory_path;
    BOOL use_color_range;
}
// classes messages
- (void) createMenu: (NSMenu **)cat menuAll: (NSMenu **)all items: (NSMenu **)it_arr custom:(BOOL)cust adduser: (BOOL) addu;
- (void) fillMenuWithString: (NSMenu *)menu stringValues:(const char **) items;
- (IBAction) menuSelected: (id) sender;
- (IBAction) customMenuSelected:(id) sender;
- (IBAction) selectPlugin: (id) sender;
- (IBAction) startProgram: (id) sender;
- (IBAction) openWebcamDialog: (id) sender;
- (IBAction) startVideoProgram: (id) sender;
- (IBAction) selectFile: (id) sender;
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
- (IBAction) setPref: (id) sender ;
- (void) updatePref: (BOOL)display_msg;
- (IBAction) showPrefs:(id) sender;
- (IBAction) resetAllFrames:(id) sender;
- (IBAction) resetAllFilters: (id) sender;
- (IBAction) releaseFrames:(id)sender;
- (IBAction) setSubFilter:(id) sender;
- (IBAction) clearSubFilter: (id) sender;
- (IBAction) enableSpyhon: (id) sender;
- (IBAction) setCustomSubFromSearch: (id) sender;
- (IBAction) updateLabelText: (id) sender;
- (IBAction) addToBlocked: (id) sender;
- (IBAction) removedFromBlocked: (id) sender;
- (IBAction) openBlockedColors: (id) sender;
- (IBAction) setColorsEnabled: (id) sender;
- (IBAction) setColorValuesRange: (id) sender;
- (IBAction) setRangeTolerance:(id) sender;
- (IBAction) addToRange: (id) sender;
- (IBAction) addToTolerance: (id) sender;
- (IBAction) setCheckBoxBlocked: (id) sender;
- (IBAction) setCheckBoxImage: (id) sender;
- (IBAction) user_Set: (id) sender;
- (IBAction) user_Save: (id) sender;
- (BOOL) userSave: (std::string *)name;
- (IBAction) user_Load: (id) sender;
- (IBAction) user_Clear: (id) sender;
- (void) loadFileData: (const char *)path off: (int) index_offset;
- (void) loadMenuList;
- (IBAction) jumpToCustom: (id) sender;
- (IBAction) custom_Save: (id) sender;
- (IBAction) custom_Load: (id) sender;
- (void) setCustomValue: (NSString *)sid value: (NSString *)value;
- (IBAction) copyCustomToString: (id) sender;
- (IBAction) insertFilter: (id) sender;
- (IBAction) insertFilter_Custom: (id) sender;
- (IBAction) scaleToggle: (id) sender;
- (IBAction) saveLogToFile: (id) sender;
- (IBAction) changeImageSelected: (id) sender;
- (IBAction) toggleFilterState: (id) sender;
- (IBAction) changePrefix: (id) sender;
- (NSInteger) checkForNewVersion: (BOOL) showMessage useVal:(int)value;
- (IBAction) checkboxClose: (id) sender;
- (IBAction) clearImageList: (id) sender;
- (IBAction) checkForFFMPEG: (id) sender;
- (IBAction) menuMoveNormal: (id) sender;
- (IBAction) menuMoveFast: (id) sender;
- (IBAction) menuMoveVeryFast: (id) sender;
- (IBAction) writeSettings: (id) sender;
- (IBAction) readSettings: (id) sender;
- (IBAction) toggleOutput: (id) sender;
- (IBAction) checkForUpdate: (id) sender;
- (IBAction) setVideoKey: (id) sender;
- (IBAction) clrVideoKey: (id) sender;
- (IBAction) videoRestart:(id) sender;
- (IBAction) setRandomFilter:(id)sender;
- (IBAction) selectRandomFilter:(id) sender;
- (void)getGraphicsInfo:(std::string *)s;
- (IBAction) saveKeys: (id) sender;
- (IBAction) loadKeys: (id) sender;
- (IBAction) randomUserDefined: (id) sender;
- (IBAction) nextFilter: (id) sender;
- (IBAction) prevFilter: (id) sender;
- (IBAction) setAsPlayList:(id) sender;
- (IBAction) initControllers: (id) sender;
- (IBAction) releaseControllers: (id) sender;
- (IBAction) pollJoystick: (id) sender;
- (IBAction) toggleFullScreenGL: (id) sender;
- (IBAction) changedCameraDevice: (id) sender;
- (IBAction) saveFilterGroup:(id) sender;
- (IBAction) setColorRange:(id)sender;
- (IBAction) showColorRange: (id)sender;
- (IBAction) disableColorRange: (id) sender;

- (void) getNameFilter: (std::string *)str index:(int) value;
- (NSTextField *)getMemoryText: (id) sender;
- (void) updateDirPath: (std::string *)s;
- (NSString *) saveCustomFilter: (NSString *) name withPlugin: (NSString *)plug;
- (IBAction) filterCollectionComboChanged:(id) sender;
@property(retain) GCController *gameController;
@property(retain) GCGamepad *theController;

@end

extern AC_Controller *controller;


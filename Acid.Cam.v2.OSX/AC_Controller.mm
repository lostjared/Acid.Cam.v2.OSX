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
 
 Copyright (c) 2020, Jared Bruni
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
#import "AC_Controller.h"
#import "AC_AppDelegate.h"
#import<IOKit/IOKitLib.h>
#include<iomanip>
#include<string>
#include<dlfcn.h>
#include<unistd.h>
#include<dirent.h>
#include<time.h>
#include<vector>
#include<algorithm>
#include<sys/stat.h>
#include<AVKit/AVKit.h>
#include<fstream>
#include"tokenize.hpp"
// Global varaibles
NSTextView *logView;
NSTextField *frame_count;
NSMutableArray *custom_array;
NSMutableArray *custom_subfilters;
NSMutableArray *filter_on;
bool isPaused = false;
NSSlider *frame_slider;
NSMenuItem *stop_prog_i;
AC_Controller *controller = nil;
bool plugin_loaded = false;
std::ostringstream ftext;
std::ostringstream stream;
//cv::Mat blend_image;
int camera_mode = 0;
bool disableFilter;
cv::VideoCapture *capture;
NSThread *background;
bool camera_active = false;
cv::Mat old_frame;
unsigned long frame_proc = 0;
bool resize_frame = false;
NSRect rc;
bool resize_value = false;
void custom_filter(cv::Mat &frame);
void plugin_callback(cv::Mat &frame);
NSMutableArray *search_results;
std::string set_filenames[4] = {"None", "None", "None", "None"};
std::vector<ac::Keys> green_blocked;
cv::ocl::Context context;
cv::Mat test_image;
std::vector<int> image_shuffle;
int image_shuffle_index = 0;
std::default_random_engine img_rng(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
int p_s = 1;
int elapsed_counter = 0;
NSButton *cycle_chk_val;
int delay_value = 60;
using ac::user_filter;
using ac::UserFilter;
int index_offset = 0;
std::string crf = "24";

bool operator<(const UserArgType &o1, const UserArgType &o2) {
    if(o1.index < o2.index) return true;
    return false;
}

void log_callback(std::string text) {
    std::ostringstream s;
    s << text << "\n";
    flushToLog(s);
}

FILE *fptr = 0;

//  Function below from Stack Overflow
// https://stackoverflow.com/questions/28562401/resize-an-image-to-a-square-but-keep-aspect-ratio-c-opencv
cv::Mat resizeKeepAspectRatio(const cv::Mat &input, const cv::Size &dstSize, const cv::Scalar &bgcolor)
{
    cv::Mat output;
    double h1 = dstSize.width * (input.rows/(double)input.cols);
    double w2 = dstSize.height * (input.cols/(double)input.rows);
    if( h1 <= dstSize.height) {
        ac::ac_resize( input, output, cv::Size(dstSize.width, h1));
    } else {
        ac::ac_resize( input, output, cv::Size(w2, dstSize.height));
    }
    int top = (dstSize.height-output.rows) / 2;
    int down = (dstSize.height-output.rows+1) / 2;
    int left = (dstSize.width - output.cols) / 2;
    int right = (dstSize.width - output.cols+1) / 2;
    cv::copyMakeBorder(output, output, top, down, left, right, cv::BORDER_CONSTANT, bgcolor );
    return output;
}
// convert Vector to array of char pointers
const char **convertToStringArray(std::vector<std::string> &v) {
    char **arr = new char*[v.size()+2];
    for(int i = 0; i < v.size(); ++i) {
        std::string::size_type len = v[i].length();
        arr[i] = new char [len+2];
        snprintf(arr[i],len+1,"%s", v[i].c_str());
    }
    arr[v.size()] = 0;
    return (const char**)arr;
}

// erase Array of char pointers
void eraseArray(const char **szArray, unsigned long size) {
    for(unsigned long i = 0; i < size; ++i)
        delete [] szArray[i];
    delete [] szArray;
}

// Apple removed this function so I rewrote it
NSInteger _NSRunAlertPanel(NSString *msg1, NSString *msg2, NSString *button1, NSString *button2, NSString *button3) {
    NSAlert *alert = [[NSAlert alloc] init];
    if(button1 != nil) [alert addButtonWithTitle:button1];
    if(button2 != nil) [alert addButtonWithTitle:button2];
    if(msg1 != nil) [alert setMessageText:msg1];
    if(msg2 != nil) [alert setInformativeText:msg2];
    NSInteger rt_val = [alert runModal];
    [alert release];
    return rt_val;
}

// flush string stream to output
void flushToLog(std::ostringstream &sout) {
    NSTextView *sv = logView;
    NSString *value = [[sv textStorage] string];
    NSString *newValue = [[NSString alloc] initWithFormat: @"%@%s", value, sout.str().c_str()];
    [sv setString: newValue];
    [sv scrollRangeToVisible:NSMakeRange([[sv string] length], 0)];
    [newValue release];
    sout.str("");
}

// set the Frame's label
void setFrameLabel(std::ostringstream &text) {
    NSString *str = [NSString stringWithUTF8String: text.str().c_str()];
    [frame_count setStringValue: str];
    text.str("");
}

// set Enabled
void setEnabledProg() {
    [stop_prog_i setEnabled: NO];
}

// Objective-C implemtation of class AC_Controller
@implementation AC_Controller

//@synthesize gameController;
//@synthesize theController;

- (void) stopCV_prog {
    [startProg setEnabled: YES];
    [record_op setEnabled: YES];
    [videoFileInput setEnabled:YES];
    [chk_rand_frames setEnabled:YES];
    if([videoFileInput state] == NSControlStateValueOn) {
        [up4k setEnabled:YES];
        if([up4k state] == NSControlStateValueOn) {
            [video_width setEnabled:YES];
            [video_height setEnabled:YES];
            [chk_stretch setEnabled:YES];
        }
    }
    programRunning = false;
}

- (IBAction) quitProgram: (id) sender {
    if(programRunning == true) {
        breakProgram = true;
        //camera_active = false;
    }
    else {
        [self writeSettings:self];
        [NSApp terminate:nil];
    }
}

- (void) reloadTable {
    [find_table reloadData];
}

- (void) dealloc {
    [custom_subfilters release];
    [custom_array release];
    [filter_on release];
    [self closePlugin];
    [menu_cat release];
    [menu_all release];
    [menu_cat_custom release];
    [menu_all_custom release];
    for(int i = 1; i < 10; ++i) {
        [menu_items[i] release];
        [menu_items_custom[i] release];
    }
    [search_results dealloc];
    [search_controller release];
    [super dealloc];
}

- (void) awakeFromNib {
    ac::setLogCallback(log_callback);
    custom_path_prefix = nil;
    current_fade = 0;
    current_fade_alpha = 1.0;
    controller = self;
    search_results = [[NSMutableArray alloc] init];
    search_controller = [[SearchController alloc] init];
    [video_file setEnabled: NO];
    [resolution setEnabled: NO];
    [device_index setEnabled: NO];
    logView = t_view;
    frame_count = framecount;
    [window1 setLevel: NSStatusWindowLevel];
    [window2 setLevel: NSStatusWindowLevel];
    [custom_window setLevel: NSStatusWindowLevel];
    [alpha_window setLevel: NSStatusWindowLevel];
    [plugin_window setLevel: NSStatusWindowLevel];
    [goto_frame setLevel: NSStatusWindowLevel];
    [filter_search_window setLevel:NSStatusWindowLevel];
    [image_select setLevel: NSStatusWindowLevel];
    ac::fill_filter_map();
    [self createMenu: &menu_cat menuAll:&menu_all items:menu_items custom:NO adduser:YES];
    [self createMenu: &menu_cat_custom menuAll: &menu_all_custom items:menu_items_custom custom:YES adduser:YES];
    [categories setMenu: menu_cat];
    [categories_custom setMenu:menu_cat_custom];
    [current_filter setMenu: menu_items[0]];
    [current_filter_custom setMenu: menu_items_custom[0]];
    custom_array = [[NSMutableArray alloc] init];
    custom_subfilters = [[NSMutableArray alloc] init];
    filter_on = [[NSMutableArray alloc] init];
    [table_view setDelegate:self];
    [table_view setDataSource:self];
    [find_table setDelegate:search_controller];
    [find_table setDataSource:search_controller];
    [menuPaused setEnabled: NO];
    stop_prog_i = stop_prog;
    frame_slider = goto_f;
    ftext.setf(std::ios::fixed, std::ios::floatfield);
    ftext.precision(2);
    p_s = 1;
    srand((unsigned int)time(0));
    pauseStepTrue = false;
    camera_mode = 0;
    colorkey_set = false;
    colorkey_bg = false;
    colorkey_filter = false;
    colorkey_replace = false;
    frame_proc = 0;
    ac::setCustom(custom_filter);
    ac::setPlugin(plugin_callback);
    [self reloadCameraInfo: self];
    upscale_video = false;
    [up4k setEnabled: YES];
    [record_op setEnabled: YES];
    set_frame_rate = false;
    set_frame_rate_val = 24;
    reset_memory = false;
    syphon_enabled = NO;
    [self setColorValuesRange:self];
    user_menu = [[NSMenu alloc] init];
    [self loadMenuList];
    [video_width setEnabled:NO];
    [video_height setEnabled:NO];
    [chk_stretch setEnabled:NO];
    outputVideo = YES;
    [output_video setState: NSControlStateValueOn];
    elapsed_counter = 0;
    time_t t = time(0);
    NSUserDefaults *d = [NSUserDefaults standardUserDefaults];
    NSString *def = [d objectForKey:@"directory_path"];
   if(def != nil) {
        directory_path = [def UTF8String];
   }
    std::cout << "Directory path: " << directory_path << "\n";
    struct tm *m;
    m = localtime(&t);
    std::ostringstream time_stream;
    time_stream << "Acid Cam Opened On: " << (m->tm_year + 1900) << "." << std::setw(2) << std::setfill('0') << (m->tm_mon + 1) << "." << std::setw(2) << std::setfill('0') << m->tm_mday << " At: " << std::setw(2) << std::setfill('0') << m->tm_hour << ":" << std::setw(2) << std::setfill('0') << m->tm_min << ":" << std::setw(2) << std::setfill('0') << m->tm_sec <<  "\n";
    flushToLog(time_stream);
    std::ostringstream sout;
    // load from settings
    NSBundle *bundlex = [NSBundle mainBundle];
    NSString *val = [bundlex pathForResource:@"ffmpeg" ofType:@""];
    ffmpeg_string_path =  [val UTF8String];
    [ffmpeg_path setStringValue:val];
    std::string gfx_name;
    [self getGraphicsInfo:&gfx_name];
    sout << "GPU: " << gfx_name << "\n";
    if(!context.create(cv::ocl::Device::TYPE_ALL)) {
        sout << "Could not create OpenCL Context\n";
    } else {
        for(unsigned int i = 0; i < context.ndevices(); ++i) {
            cv::ocl::Device device = context.device(i);
            sout << "Name: " << device.name() << "\n"  << "OpenCL version: " << device.OpenCL_C_Version() << "\n";
        }
        cv::ocl::Device(context.device(0));
    }
    low_res_mode = false;
    flushToLog(sout);
    ac::setThreadCount(4);
    restartFilter = NO;
    NSBundle *bundle = [NSBundle mainBundle];
    NSString *str = [bundle pathForResource:@"clouds" ofType:@"tiff"];
    NSString *s_ffmpeg_path = [bundle pathForResource:@"ffmpeg" ofType:@""];
    [ffmpeg_path setStringValue:s_ffmpeg_path];
    ffmpeg_path_str = [s_ffmpeg_path UTF8String];
    test_image = cv::imread([str UTF8String]);
    [bundle release];
    [self readSettings:self];
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSInteger update_check_on = [defaults integerForKey:@"update_on"];
    if(update_check_on == 0) {
        [check_update setState: NSControlStateValueOff];
    } else {
        [check_update setState: NSControlStateValueOn];
    }
    unsigned long mem = ac::calculateMemory();
    std::ostringstream stream;
    stream << "Frame Memory Allocated: " << ((mem > 0) ? (mem/1024/1024) : 0) << " MB - " << "Filters Initalized: " << ac::all_objects.size() << " - Frames Allocated: " << ac::getCurrentAllocatedFrames() << "\n";
    std::string name = stream.str();
    [memory_text setStringValue: [NSString stringWithUTF8String:name.c_str()]];
    [self checkForNewVersion:NO useVal:0];
    cycle_chk_val = cycle_chk;
    //std::cout << cv::getBuildInformation() << "\n";
   /* for(int i = 0; i < ac::draw_strings.size(); ++i) {
        std::cout << "v.push_back(\"" << ac::draw_strings[i] << "\");\n";
    }*/
}

- (IBAction) reloadCameraInfo: (id) sender {
    [device_index removeAllItems];
    NSArray *devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
    if([devices count] > 0) {
        for (AVCaptureDevice *device in devices) {
            NSLog(@"Device: %@", [device localizedName]);
            [device_index addItemWithTitle: [device localizedName]];
        }
    } else {
        [device_index addItemWithTitle: @"0"];
        [device_index addItemWithTitle: @"1"];
        [device_index addItemWithTitle: @"2"];
        [device_index addItemWithTitle: @"3"];
        [device_index addItemWithTitle: @"4"];
        [device_index addItemWithTitle: @"5"];
        [device_index addItemWithTitle: @"6"];
        [device_index addItemWithTitle: @"7"];
    }
}

- (IBAction) changedCameraDevice: (id) sender {
    /*
     NSInteger index = [device_index indexOfSelectedItem];
     cv::VideoCapture cap(static_cast<int>(index));
     std::vector<cv::Size> res;
     if(ac::getSupportedResolutions(cap, res)) {
     std::cout << "Resolution...\n";
     for(int i = 0; i < res.size(); ++i) {
     std::cout << "Supports resolution: " << res[i].width << "x" << res[i].height << "\n";
     }
     }*/
}

- (void) createMenu: (NSMenu **)cat menuAll: (NSMenu **)all items: (NSMenu **)it_arr custom:(BOOL)cust adduser: (BOOL)add_u{
    *cat = [[NSMenu alloc] init];
    [*cat addItemWithTitle:@"All" action:nil keyEquivalent:@""];
    [*cat addItemWithTitle:@"All Sorted" action:nil keyEquivalent:@""];
    [*cat addItemWithTitle:@"Blend" action:nil keyEquivalent:@""];
    [*cat addItemWithTitle:@"Distort" action:nil keyEquivalent:@""];
    [*cat addItemWithTitle:@"Pattern" action:nil keyEquivalent:@""];
    [*cat addItemWithTitle:@"Gradient" action:nil keyEquivalent:@""];
    [*cat addItemWithTitle:@"Mirror" action:nil keyEquivalent:@""];
    [*cat addItemWithTitle:@"Strobe" action:nil keyEquivalent:@""];
    [*cat addItemWithTitle:@"Blur" action:nil keyEquivalent:@""];
    [*cat addItemWithTitle:@"Image/Video" action:nil keyEquivalent:@""];
    [*cat addItemWithTitle:@"Square" action:nil keyEquivalent:@""];
    [*cat addItemWithTitle:@"Other" action:nil keyEquivalent:@""];
    [*cat addItemWithTitle:@"SubFilter" action: nil keyEquivalent:@""];
    [*cat addItemWithTitle:@"Special" action:nil keyEquivalent:@""];
    
    if(add_u == YES)
        [*cat addItemWithTitle:@"User" action:nil keyEquivalent:@""];
    
    for(int i = 1; i < 14; ++i) {
        it_arr[i] = [[NSMenu alloc] init];
    }
    
    std::vector<std::string> all_sorted;
    for(int x = 0; x < ac::draw_max-4; ++x)
        all_sorted.push_back(ac::draw_strings[x]);
    std::sort(all_sorted.begin(), all_sorted.end());
    const char **szAllSorted = convertToStringArray(all_sorted);
    [self fillMenuWithString: it_arr[1] stringValues:szAllSorted];
    eraseArray(szAllSorted, all_sorted.size());
    std::sort(ac::vzBlend.begin(), ac::vzBlend.end());
    const char **szBlend = convertToStringArray(ac::vzBlend);
    [self fillMenuWithString: it_arr[2] stringValues:szBlend];
    eraseArray(szBlend, ac::vzBlend.size());
    std::sort(ac::svDistort.begin(), ac::svDistort.end());
    const char **szDistort = convertToStringArray(ac::svDistort);
    [self fillMenuWithString: it_arr[3] stringValues:szDistort];
    eraseArray(szDistort, ac::svDistort.size());
    std::vector<std::string> svPattern { "Blend Fractal","Blend Fractal Mood","Diamond Pattern" };
    std::sort(ac::svPattern.begin(), ac::svPattern.end());
    const char **szPattern = convertToStringArray(ac::svPattern);
    [self fillMenuWithString: it_arr[4] stringValues:szPattern];
    eraseArray(szPattern, svPattern.size());
    std::sort(ac::svGradient.begin(), ac::svGradient.end());
    const char **szGradient = convertToStringArray(ac::svGradient);
    [self fillMenuWithString: it_arr[5] stringValues:szGradient];
    eraseArray(szGradient, ac::svGradient.size());
    std::sort(ac::svMirror.begin(), ac::svMirror.end());
    const char **szMirror = convertToStringArray(ac::svMirror);
    [self fillMenuWithString: it_arr[6] stringValues:szMirror];
    eraseArray(szMirror, ac::svMirror.size());
    std::sort(ac::svStrobe.begin(), ac::svStrobe.end());
    const char **szStrobe = convertToStringArray(ac::svStrobe);
    [self fillMenuWithString: it_arr[7] stringValues:szStrobe];
    eraseArray(szStrobe, ac::svStrobe.size());
    std::sort(ac::svBlur.begin(), ac::svBlur.end());
    const char **szBlur = convertToStringArray(ac::svBlur);
    [self fillMenuWithString: it_arr[8] stringValues:szBlur];
    eraseArray(szBlur, ac::svBlur.size());
    std::sort(ac::svImage.begin(),ac::svImage.end());
    const char **szImage = convertToStringArray(ac::svImage);
    [self fillMenuWithString: it_arr[9] stringValues:szImage];
    eraseArray(szImage, ac::svImage.size());
    std::sort(ac::svOther.begin(), ac::svOther.end());
    const char **szOther = convertToStringArray(ac::svOther);
    std::sort(ac::svOther_Custom.begin(), ac::svOther_Custom.end());
    const char **szOther_Custom = convertToStringArray(ac::svOther_Custom);
    std::sort(ac::svSquare.begin(), ac::svSquare.end());
    const char **szSquare = convertToStringArray(ac::svSquare);
    [self fillMenuWithString: it_arr[10] stringValues:szSquare];
    eraseArray(szSquare, ac::svSquare.size());
    std::sort(ac::vSub.begin(), ac::vSub.end());
    const char **zSub = convertToStringArray(ac::vSub);
    
    if(cust == NO) {
        [self fillMenuWithString: it_arr[12] stringValues:zSub];
        [self fillMenuWithString: it_arr[11] stringValues:szOther];
        [self fillMenuWithString: it_arr[13] stringValues:ac::szCustom];
    }
    else {
        [self fillMenuWithString: it_arr[12] stringValues:zSub];
        [self fillMenuWithString: it_arr[11] stringValues:szOther_Custom];
        [self fillMenuWithString: it_arr[13] stringValues:ac::szCustom_Spec];
    }
    
    eraseArray(szOther, ac::svOther.size());
    eraseArray(szOther_Custom, ac::svOther_Custom.size());
    eraseArray(zSub, ac::vSub.size());
    
    *all = [[NSMenu alloc] init];
    
    for(int i = 0; i < ac::draw_max-3; ++i){
        NSString *s = [NSString stringWithUTF8String: ac::draw_strings[i].c_str()];
        if(cust == YES) {
            if(ac::draw_strings[i] != "Custom") {
                NSMenuItem *item_custom = [[NSMenuItem alloc] initWithTitle:s action:NULL keyEquivalent:@""];
                [*all addItem:item_custom];
                [item_custom release];
            }
        } else {
            NSMenuItem *item_custom = [[NSMenuItem alloc] initWithTitle:s action:NULL keyEquivalent:@""];
            [*all addItem:item_custom];
            [item_custom release];
        }
    }
    it_arr[0] = *all;
}

- (void) fillMenuWithString: (NSMenu *)menu stringValues:(const char **) items {
    for(int q = 0; items[q] != 0; ++q) {
        [menu addItemWithTitle: [NSString stringWithUTF8String:items[q]] action:nil keyEquivalent:@""];
    }
}

- (IBAction) menuSelected: (id) sender {
    NSInteger index = [categories indexOfSelectedItem];
    if(index == 14) {
        if([user_menu numberOfItems] > 0)
            [current_filter setMenu: user_menu];
    }
    else
        [current_filter setMenu: menu_items[index]];
    
}

- (IBAction) customMenuSelected:(id) sender {
    NSInteger index = [categories_custom indexOfSelectedItem];
    if(index == 14) {
        if([user_menu numberOfItems] > 0)
            [current_filter_custom setMenu: user_menu];
        
    } else {
        [current_filter_custom setMenu: menu_items_custom[index]];
    }
}

- (IBAction) changeFilter: (id) sender {
    static NSInteger prev_index = [current_filter indexOfSelectedItem];
    NSInteger current = [current_filter indexOfSelectedItem];
    //NSInteger index = [categories indexOfSelectedItem];
    if(prev_index != current) {
        prev_index = current;
        ac::reset_filter = true;
        //ac::release_all_objects();
    }
    
    //NSMenuItem *item = [menu_items[index] itemAtIndex:current];
    if(current >= 0 && current < [current_filter numberOfItems]) {
        NSMenuItem *item = [current_filter itemAtIndex:current];
        NSString *title = [item title];
        if([fade_filter state] == NSControlStateValueOff) {
            ac::draw_offset = ac::filter_map[[title UTF8String]];
        } else {
            current_fade = ac::draw_offset;
            current_fade_alpha = 1.0;
            ac::draw_offset = ac::filter_map[[title UTF8String]];
        }
        std::ostringstream strout;
        strout << "Filter set to: " << ac::draw_strings[ac::draw_offset] << "\n";
        flushToLog(strout);
        if(ac::draw_strings[ac::draw_offset] == "Custom") {
            [negate_checked setIntegerValue: NSControlStateValueOff];
            [custom_window orderFront:self];
            [filter_search_window orderFront: self];
        }
        if(ac::draw_strings[ac::draw_offset] == "ParticleRelease" || ac::draw_strings[ac::draw_offset] == "ParticleBlend" || ac::draw_strings[ac::draw_offset] == "ParticleFlash" || ac::draw_strings[ac::draw_offset] == "ParticleAlpha") {
            emiter.reset();
            strout.str("");
            strout << "ParticleRelease Variables Reset\n";
        }
        
        if(ac::draw_strings[ac::draw_offset] == "ColorRange") {
            ac::colors[0] = rand()%255;
            ac::colors[1] = rand()%255;
            ac::colors[2] = rand()%255;
            strout.str("");
            strout << "ColorRange random pixels set to: " << ac::colors[0] << "," << ac::colors[1] << "," << ac::colors[2] << "\n";
            flushToLog(strout);
        }
        
        if(ac::draw_strings[ac::draw_offset] == "Alpha Flame Filters") {
            [alpha_window orderFront:self];
        }
        if(ac::draw_strings[ac::draw_offset] == "Plugin") {
            [plugin_window orderFront:self];
        }
        
        std::string filter_value = ac::draw_strings[ac::draw_offset];
        if(filter_value.find("MultiVideo") != std::string::npos)
            [video_indow orderFront:self];
        else
        if(filter_value.find("Video") != std::string::npos) {
            [image_select orderFront:self];
            strout.str("");
            strout << "Select Secondary Video File...\n";
            flushToLog(strout);
        } else if(filter_value.find("Image") != std::string::npos) {
            [image_select orderFront: self];
            strout.str("");
            strout << "Image filter select use Select image window to set image...\n";
            flushToLog(strout);
        } else if(filter_value.find("SubFilter") != std::string::npos) {
            strout.str("");
            strout << "Filters that require SubFilter should be used in the Custom Filter Mode.\n";
            [custom_window orderFront:self];
            flushToLog(strout);
        } else if((filter_value.find("SlitScan") != std::string::npos) || filter_value == "SlitScanGUI" || filter_value == "SlitScanGUI_RGB" || filter_value == "SlitScanXGUI_RGB" || filter_value == "SlitScanXGUI" || filter_value == "SlitScanRandom") {
            [slit_win orderFront:self];
        } else if(filter_value == "BlendWithSourcePercent") {
             [blend_percent_window orderFront:self];
        }
    }
    ac::reset_filter = true;
}

- (IBAction) downloadNewestVersion: (id) sender {
    NSInteger value = [self checkForNewVersion:YES useVal:1];
    if(value == NO) {
        NSLog(@"Value is equal to %d\n", (int)value);
    }
}

- (IBAction) stopProgram: (id) sender {
    stopProgram = true;
    [menuPaused setEnabled: NO];
    [menu_freeze setEnabled: NO];
    [record_op setEnabled: YES];
    [videoFileInput setEnabled:YES];
    stopCV();
    fptr = 0;
    [startProg setTitle:@"Start Session"];
    ac::setRunning(false);
}

- (IBAction) selectPlugin: (id) sender {
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setCanChooseFiles:NO];
    [panel setCanChooseDirectories:YES];
    if([panel runModal]) {
        NSString *file_type = [[panel URL] path];
        [plugin_dir removeAllItems];
        [plugin_name setStringValue: file_type];
        [self loadDir:[file_type UTF8String]];
    }
}

- (IBAction) setPlugin: (id) sender {
    NSString *file_type = [NSString stringWithFormat: @"%@/%@", [plugin_name stringValue], [plugin_dir objectValueOfSelectedItem]];
    pixel pix = [self loadPlugin: file_type];
    if(pix == NULL)
        plugin_loaded = false;
    else
        plugin_loaded = true;
    
    std::ostringstream plug;
    plug << "Loaded Plugin: " << [file_type UTF8String] << "\n";
    flushToLog(plug);
}

- (void) loadDir: (std::string) str {
    DIR *dir = opendir(str.c_str());
    if (dir == NULL)
    {
        std::cerr << "Error could not open directory.\n";
        return;
    }
    dirent *e;
    while ((e = readdir(dir)))
    {
        if (e->d_type == DT_REG)
        {
            std::string file = e->d_name;
            if (file.find(".acidcam") != -1)
            {
                NSString *s = [NSString stringWithUTF8String: e->d_name];
                [plugin_dir addItemWithObjectValue: s];
                
            }
        }
    }
    closedir(dir);
}

- (pixel) loadPlugin: (NSString *)str {
    
    if(directory_path.length()==0) {
        _NSRunAlertPanel(@"Error you must set the directory path in custom filter window press the dir button", @"Set path", @"Ok",nil,nil);
        return 0;
    }
    
    
    if([[plugin_name2 stringValue] length] == 0) {
        _NSRunAlertPanel(@"You must give the plugin a name", @"Requires a name.", @"Ok", nil, nil);
        return 0;
    }
    void *library = dlopen([str UTF8String], RTLD_LAZY);
    if(library == NULL) {
        std::cerr << "Error could not open: " << [str UTF8String] << "\n";
        _NSRunAlertPanel(@"Error Occoured Loading Plugin", @"Exiting...", @"Ok", nil, nil);
        return 0;
    }
    void *addr;
    // load the plugin function to process pixels
    addr = dlsym(library, "filter");
    pixel pixf;
    pixf = reinterpret_cast<pixel>(addr);
    const char *error;
    error = dlerror();
    if(error) {
        std::cerr << "Could not load pixel: " << error << "\n";
        _NSRunAlertPanel(@"Could not load Plugin", @"Error loading plugin", @"Ok", nil,nil);
        return NULL;
    }
    std::string fname = "User_";
    fname += [[plugin_name2 stringValue] UTF8String];
    int index = 0;
    int val1 = -1;
    for(index = 0; index < ac::draw_strings.size(); ++index) {
        if(ac::draw_strings[index] == fname) {
            val1 = index;
            break;
        }
    }
    int index_offset = 0;
    if(val1 == -1) {
        ac::draw_strings.push_back(fname);
        val1 = static_cast<int>(ac::draw_strings.size()-1);
    }
    ++index_offset;
    ac::filter_map[fname] = val1;
    user_filter[fname].name = fname;
    user_filter[fname].other_name = fname;
    user_filter[fname].index = ac::filter_map[fname];
    user_filter[fname].sort_num = index_offset;
    user_filter[fname].func = pixf;
    user_filter[fname].library = library;
    user_filter[fname].plug_file = [str UTF8String];
    if(user_filter[fname].func == 0) {
        _NSRunAlertPanel(@"Could not open file", @"Plugin would not load", @"Ok", nil, nil);
        return 0;
    }
    
    NSString *sval = [NSString stringWithUTF8String: fname.c_str()];
    if(fname.find("User_") == std::string::npos) {
        [user_filter_name addItemWithObjectValue:sval];
        [user_filter_name setStringValue:@""];
    }
    [self loadMenuList];
    [table_view reloadData];
    
    std::fstream inf, outf;
    inf.open([str UTF8String], std::ios::in | std::ios::binary);
    if(!inf.is_open()) {
        _NSRunAlertPanel(@"Could not open file", @"Plugin would not load", @"Ok", nil, nil);
        return 0;
    }
    std::ostringstream stream_;
    stream_ << directory_path << "/" << fname << ".acf";
    outf.open(stream_.str(), std::ios::out | std::ios::binary);
    if(!outf.is_open()) {
        _NSRunAlertPanel(@"Could not open file", @"Plugin would not load", @"Ok", nil, nil);
        return 0;
    }
    while(!inf.eof()) {
        char buf[1024];
        inf.read(buf, sizeof(buf));
        size_t bytes = inf.gcount();
        outf.write(buf, bytes);
    }
    inf.close();
    outf.close();
    std::ostringstream stream;
    stream << "User set: " << fname << " to: " << fname << "\n";
    NSString *val = [self saveCustomFilter: [NSString stringWithUTF8String: fname.c_str()] withPlugin:[NSString stringWithUTF8String:stream_.str().c_str()]];
    std::string sname;
    sname = fname + ".acl";
    user_filter[fname].filename = [val UTF8String];
    flushToLog(stream);
    return pixf;
}

- (void) closePlugin {
    for(auto i = user_filter.begin(); i != user_filter.end(); ++i) {
        if(i->second.library != 0)
            dlclose(i->second.library);
    }
}

-(IBAction) startProgram: (id) sender {
    
    crf = [[crf_field stringValue] UTF8String];
    
    if(!ac::frames_.empty()) {
        ac::frames_.erase(ac::frames_.begin(), ac::frames_.end());
    }
    
    if([[startProg title] isEqualToString: @"Start Session"]) {
        if([videoFileInput integerValue] == 0 && [[[output_Type itemAtIndex:[output_Type indexOfSelectedItem]] title] isEqualToString:@"MOV - HEVC/H.265"]) {
            _NSRunAlertPanel(@"You can only use HEVC in Video mode because it takes a long time to process", @"HEVC Requires Video File", @"Ok", nil, nil);
            return ;
        }
        
        if(!image_shuffle.empty()) {
            std::shuffle(image_shuffle.begin(), image_shuffle.end(), img_rng);
            image_shuffle_index = 0;
        }
        [self menuMoveNormal:self];
        elapsed_counter = 0;
        std::string input_file;
        NSInteger cap_width = [video_width integerValue];
        NSInteger cap_height = [video_height integerValue];
        copy_sound = ([copy_audio integerValue] == 0) ? false : true;
        if(copy_sound == true) {
            std::fstream file;
            file.open(ffmpeg_string_path, std::ios::in);
            if(!file.is_open() || !file.good()) {
                _NSRunAlertPanel(@"FFMPEG must be installed, check README", @"FFMPEG should be installed with Homebrew package manager. It is free you can find it here: https://brew.sh/", @"Ok", nil, nil);
                return;
            }
        }
        [video_width setEnabled: NO];
        [video_height setEnabled: NO];
        [chk_stretch setEnabled:NO];
        [videoFileInput setEnabled:NO];
        [self videoRestart:self];
        if([up4k state] == NSControlStateValueOn) {
            if((cap_width < 320 || cap_height < 240) || (cap_width > 3840 || cap_height > 2160)) {
                _NSRunAlertPanel(@"Invalid Screen Resolution...", @"Invalid", @"Ok", nil, nil);
                return;
            }
        }
        if([up4k state] == NSControlStateValueOff && [chk_stretch state] == NSControlStateValueOn) {
            _NSRunAlertPanel(@"Cannot Stretch", @"Please turn off Stretching as requires resize to be enabled", @"Ok", nil, nil);
            return;
        }
        
        if([up4k state] == NSControlStateValueOn && [videoFileInput state] == NSControlStateValueOff) {
            _NSRunAlertPanel(@"Error",@" Scaling only available in video mode", @"Ok", nil,nil);
            return;
        }
        cv::Size res_arr[] = { {0, 0}, {320, 240}, {640, 480}, {800, 600}, {1024, 576}, {1280, 720}, {1920, 1080}, {2560, 1440}, {3840, 2160}, {0, 0}};
        bool use_resized_res = false;
        if([videoFileInput state] == NSControlStateValueOn) {
            input_file = [[video_file stringValue] UTF8String];
            if(input_file.length() == 0) {
                _NSRunAlertPanel(@"No Input file selected\n", @"No Input Selected", @"Ok", nil, nil);
                [videoFileInput setEnabled:YES];
                return;
            }
            use_resized_res = true;
            camera_mode = 1;
        } else camera_mode = 0;
        NSInteger res = [resolution indexOfSelectedItem];
        bool r;
        if([record_op integerValue] == 1)
            r = false;
        else
            r = true;
        frame_proc = 0;
        NSInteger checkedState = [menuPaused state];
        isPaused = (checkedState == NSControlStateValueOn) ? true : false;
        std::ostringstream fname_stream;
        std::string filename;
        NSInteger popupType = [output_Type indexOfSelectedItem];
        time_t t = time(0);
        struct tm *m;
        m = localtime(&t);
        std::ostringstream time_stream;
        time_stream << "-" << (m->tm_year + 1900) << "." << std::setw(2) << std::setfill('0') << (m->tm_mon + 1) << "." << std::setw(2) << std::setfill('0') << m->tm_mday << "_" << std::setw(2) << std::setfill('0') << m->tm_hour << "." << std::setw(2) << std::setfill('0') << m->tm_min << "." << std::setw(2) << std::setfill('0') << m->tm_sec <<  "_";
        if(popupType == 0)
            fname_stream << time_stream.str();
        else
            fname_stream << time_stream.str();
        
        filename = fname_stream.str();
        std::string add_path;
        NSArray* paths;
        
        if(custom_path_prefix == nil) {
            paths = NSSearchPathForDirectoriesInDomains( NSMoviesDirectory, NSUserDomainMask, YES );
            add_path = std::string([[paths objectAtIndex: 0] UTF8String])+std::string("/")+[[prefix_input stringValue] UTF8String];
        } else {
            add_path = std::string([custom_path_prefix UTF8String])+std::string("/") + [[prefix_input stringValue] UTF8String];
        }
        [menuPaused setEnabled: YES];
        [up4k setEnabled: NO];
        [record_op setEnabled: NO];
        ac::reset_filter = true;
        
        if(camera_mode == 1)
            capture = capture_video.get();
        else
            capture = capture_camera.get();
        
        bool u4k = ([up4k state] == NSControlStateValueOn) ? true : false;
        int value_w = 0, value_h = 0;
        if([up4k state] == NSControlStateValueOn) {
            value_w = (int)cap_width;
            value_h = (int)cap_height;
        }
        
        double fps_val = [fps_field doubleValue];
        
        int ret_val = 0;
        if(use_resized_res == false)
            ret_val = program_main(fps_val,outputVideo, 0, 0,syphon_enabled, set_frame_rate, set_frame_rate_val, u4k, (int)popupType, input_file, r, filename, res_arr[res].width, res_arr[res].height,(int)[device_index indexOfSelectedItem], 0, 0.75f, add_path);
        else
            ret_val = program_main(fps_val,outputVideo, value_w, value_h, syphon_enabled, set_frame_rate, set_frame_rate_val, u4k, (int)popupType, input_file, r, filename, (int)cap_width, (int)cap_height,(int)[device_index indexOfSelectedItem], 0, 0.75f, add_path);
        if(ret_val == 0) {
            if(camera_mode == 1) {
                renderTimer = [NSTimer timerWithTimeInterval:1.0/ac::fps target:self selector:@selector(cvProc:) userInfo:nil repeats:YES];
            }
            else {
                //renderTimer = [NSTimer timerWithTimeInterval:1.0/1000 target:self selector:@selector(camProc:) userInfo:nil repeats:YES];
                renderTimer = [NSTimer timerWithTimeInterval:1.0/ac::fps/2 target:self selector:@selector(camProc:) userInfo:nil repeats:YES];
//                [[NSRunLoop currentRunLoop] addTimer:renderTimer forMode:NSEventTrackingRunLoopMode];
                [[NSRunLoop currentRunLoop] addTimer:renderTimer forMode:NSDefaultRunLoopMode];
            }
            [[NSRunLoop currentRunLoop] addTimer:renderTimer forMode:NSRunLoopCommonModes];
        }
        restartFilter = YES;
        
        if(ret_val != 0) {
            _NSRunAlertPanel(@"Failed to initalize capture device\n", @"Init Failed\n", @"Ok", nil, nil);
            std::cout << "DeviceIndex: " << (int)[device_index indexOfSelectedItem] << " input file: " << input_file << " filename: " << filename << " res: " << res_arr[res].width << "x" << res_arr[res].height << "\n";
            programRunning = false;
            [startProg setTitle:@"Start Session"];
            [window1 orderOut:self];
            [record_op setEnabled:YES];
            [videoFileInput setEnabled:YES];
        } else {
            if([menu_freeze state] == NSControlStateValueOn) {
                capture->read(old_frame);
                ++frame_cnt;
                ++frame_proc;
            }
            if(resize_value == true) {
                [stretch_scr setState: NSControlStateValueOn];
            } else {
                [stretch_scr setState: NSControlStateValueOff];
            }
            if(camera_mode == 0) {
                isPaused = false;
                [menuPaused setState:NSControlStateValueOff];
                frames_captured = 0;
                background = [[NSThread alloc] initWithTarget:self selector:@selector(camThread:) object:nil];
                [background start];
                camera_active = true;
            }
            [window1 orderFront:self];
            NSSize sz;
            if([up4k state] == NSControlStateValueOn) {
                value_w = (int)cap_width;
                value_h = (int)cap_height;
                sz.width = value_w;
                sz.height = value_h;
            } else {
               int wx = capture->get(CV_CAP_PROP_FRAME_WIDTH);
               int wy = capture->get(CV_CAP_PROP_FRAME_HEIGHT);
                sz.width = wx;
                sz.height = wy;
            }
            [syphon_window setContentSize: sz];
            [startProg setTitle:@"Stop"];
            ac::pix.setInit(false);
            ac::setRunning(true);
        }
    } else {
        [self stopProgram:self];
        [startProg setTitle:@"Start Session"];
    }
}

- (void) stopCamera {
    camera_active = false;
    if(renderTimer != nil && renderTimer.valid) {
        [renderTimer invalidate];
        renderTimer = nil;
    }
}

- (void) camProc: (id) sender {
    if(breakProgram == true || stopProgram == true) {
        [self stopCamera];
        return;
    }
    if(isPaused && pauseStepTrue == true) {
        pauseStepTrue = false;
    }
    else if(isPaused) return;
    if(capture_camera->isOpened() && camera_active == true) {
        if([menu_freeze state] == NSControlStateValueOff) {
            capture_camera->grab();
            frames_captured++;
        }
    }
}

- (void) camThread: (id) sender {
    __block cv::Mat frame;
    __block bool got_frame = true;
    while(camera_active && got_frame) {
        if(isPaused) continue;
        
        __block NSInteger after = 0;
        __block NSInteger slide_value1 = 0;
        __block NSInteger slide_value2 = 0;
        __block NSInteger slide_value3 = 0;
        __block NSInteger fade_state = 0;
        __block NSInteger color_key_set = 0;
        __block cv::Vec3b well_color;
        __block NSInteger up4ki = 0;
        
        dispatch_sync(dispatch_get_main_queue(), ^{
            cv::Mat temp_frame;
            if([menu_freeze state] == NSControlStateValueOff) {
                got_frame = capture->retrieve(frame);
                ac::orig_frame = frame.clone();
                old_frame = frame.clone();
            } else {
                frame = old_frame.clone();
            }
            if([rotate_v state] == NSControlStateValueOn) {
                cv::flip(frame, temp_frame, 1);
                frame = temp_frame.clone();
            }
            if([rotate_h state] == NSControlStateValueOn) {
                cv::flip(frame, temp_frame, 0);
                frame = temp_frame.clone();
            }
            ac::orig_frame = frame.clone();
            ++frame_cnt;
            ++frame_proc;
            
            NSInteger stat_test = [chk_rand_repeat integerValue];
            if(stat_test == NSControlStateValueOn) {
                [chk_rand_frames setEnabled: NO];
                NSInteger val;
                static NSInteger value_index = 0, frame_index = 0;
                val = [image_combo numberOfItems];
                if(val >= 2) {
                    NSInteger value_max = [chk_rand_frames integerValue];
                    if(value_max >= 2) {
                        NSInteger next_index = 0;
                        NSInteger mode = [chk_rand_mode indexOfSelectedItem];
                        ++value_index;
                        if(value_index > value_max) {
                            if(mode == 0)
                                next_index = rand()%val;
                            else if(mode == 1)
                                next_index = frame_index;
                            else if(mode == 2) {
                                next_index = image_shuffle[image_shuffle_index];
                                ++image_shuffle_index;
                                if(image_shuffle_index > image_shuffle.size()-1) {
                                    std::shuffle(image_shuffle.begin(), image_shuffle.end(), img_rng);
                                    image_shuffle_index = 0;
                                }
                            }
                            ++frame_index;
                            if(frame_index > val-1)
                                frame_index = 0;
                            NSString *str_value = [image_combo itemObjectValueAtIndex:next_index];
                            value_index = 0;
                            
                            blend_image = cv::imread([str_value UTF8String]);
                            if(!blend_image.empty()) {
                                blend_set = true;
                                ac::image_cycle_reset = true;
                            } else {
                                blend_set = false;
                            }
                            std::string value = [str_value UTF8String];
                            auto pos = value.rfind("/");
                            if(pos != std::string::npos) {
                                value = value.substr(value.rfind("/")+1, value.length());
                                NSString *string_value = [NSString stringWithUTF8String: value.c_str()];
                                [filename_textfield setStringValue:string_value];
                            }
                            NSImage *img = [[NSImage alloc] initWithContentsOfFile:str_value];
                            [cur_selected_image setImage:img];
                            [img release];
                        }
                    }
                }
            } else {
                [chk_rand_frames setEnabled:YES];
            }
            
            if(ac::v_cap.isOpened()) {
                double value = ac::v_cap.get(cv::CAP_PROP_POS_FRAMES);
                double length = ac::v_cap.get(cv::CAP_PROP_FRAME_COUNT);
                std::ostringstream s;
                NSString *text = [NSString stringWithFormat:@"Video Frame Index: [%d/%d]", static_cast<int>(value), static_cast<int>(length)];
                [video_position setStringValue: text];
            }
            
            if(reset_memory == true) {
                ac::reset_filter = true;
                reset_memory = false;
            }
            if(ac::draw_strings[ac::draw_offset] != "Custom") {
                if([negate_checked integerValue] == NSControlStateValueOff) ac::isNegative = false;
                else ac::isNegative = true;
                ac::color_order = (int) [corder indexOfSelectedItem];
            }
            after = [apply_after integerValue];
            slide_value1 = [brightness integerValue];
            slide_value2 = [gamma integerValue];
            slide_value3 = [saturation integerValue];
            fade_state = [fade_filter state];
            color_key_set = [color_chk state];
            NSColor *color_value = [color_well color];
            double rf = 0, gf = 0, bf = 0;
            [color_value getRed:&rf green:&gf blue:&bf alpha:nil];
            unsigned int values[3];
            values[2] = rf*255.99999f;
            values[1] = gf*255.99999f;
            values[0] = bf*255.99999f;
            well_color[0] = values[0];
            well_color[1] = values[1];
            well_color[2] = values[2];
            up4ki = [up4k state];
        });
        
        if(after == NSControlStateValueOff)
            ac::ApplyColorMap(frame);
        
        if(fade_state == NSControlStateValueOff) {
            if(disableFilter == false && ac::testSize(frame)) ac::CallFilter(ac::draw_offset, frame);
        } else {
            if(current_fade_alpha >= 0) {
                ac::filterFade(frame, (int)current_fade, ac::draw_offset, current_fade_alpha);
                current_fade_alpha -= 0.08;
            } else {
                if(disableFilter == false && ac::testSize(frame)) ac::CallFilter(ac::draw_offset, frame);
            }
        }
        ac::frames_released = false;
        ac::reset_alpha = false;
        ac::image_matrix_reset = false;
        ac::image_cycle_reset = false;
        if(restartFilter == YES) {
            ac::image_matrix_reset = true;
            restartFilter = NO;
        }
        if(after == NSControlStateValueOn)
            ac::ApplyColorMap(frame);
        
        if(slide_value1 > 0)
            ac::setBrightness(frame, 1.0, (int)slide_value1);
        
        if(slide_value2 > 0) {
            cv::Mat img = frame.clone();
            ac::setGamma(img,frame,(int)slide_value2);
        }
        if(slide_value3 > 0) {
            ac::setSaturation(frame, (int)slide_value3);
        }
        
        if(color_key_set == NSControlStateValueOn && (colorkey_set == true && !color_image.empty())) {
            cv::Mat cframe = frame.clone();
            ac::filterColorKeyed(well_color, ac::orig_frame, cframe, frame);
        } else if(color_key_set == NSControlStateValueOn && colorkey_bg == true) {
            cv::Mat cframe = frame.clone();
            ac::filterColorKeyed(well_color, ac::orig_frame, cframe, frame);
        } else if(color_key_set == NSControlStateValueOn && colorkey_replace == true) {
            cv::Mat cframe = frame.clone();
            ac::filterColorKeyed(well_color, ac::orig_frame, cframe, frame);
        } else if(color_key_set == NSControlStateValueOn && colorkey_filter == true) {
            cv::Mat cframe = frame.clone();
            ac::filterColorKeyed(well_color, ac::orig_frame, cframe, frame);
        }
        dispatch_sync(dispatch_get_main_queue(), ^{
            if([corder indexOfSelectedItem] == 5) {
                cv::Mat change;
                cv::cvtColor(frame, change, cv::COLOR_BGR2GRAY);
                cv::cvtColor(change, frame, cv::COLOR_GRAY2BGR);
            }
            NSInteger mask = [[NSApp mainWindow] styleMask];
            NSString *main_window = [[NSApp mainWindow] title];
            NSWindow *main_w = [NSApp mainWindow];
            static bool fulls = false;
            if([main_window isEqualToString:@"Acid Cam v2"]) {
                if(mask & NSWindowStyleMaskFullScreen) {
                    fulls = true;
                    rc = [self getScreenSize];
                }
                else {
                    fulls = false;
                    rc = [main_w frame];
                }
            }
            if([stretch_scr state] == NSControlStateValueOn) {
                cv::Mat dst;
                dst = resizeKeepAspectRatio(frame, cv::Size(rc.size.width, rc.size.height), cv::Scalar(0,0,0));
                if(outputVideo == YES && syphon_enabled == NO && dst.size().width > 0 && dst.size().height > 0) cv::imshow("Acid Cam v2", dst);
            } else {
                if(syphon_enabled == NO) {
                    cv::resizeWindow("Acid Cam v2", frame.cols, frame.rows);
                    if(outputVideo == YES && frame.size().width > 0 && frame.size().height > 0) cv::imshow("Acid Cam v2", frame);
                }
            }
            ftext << "(Current Frame/Total Frames/Seconds/MB): " << frame_cnt << "/" << "0" << "/" << (frame_cnt/ac::fps) << "/" << ((file_size/1000)/1000) << " MB";
            if(camera_mode == 1) {
                float val = frame_cnt;
                float size = total_frames;
                if(size != 0)
                    ftext << " - " << (val/size)*100 << "% ";
            }
            setFrameLabel(ftext);
            unsigned long mem = ac::calculateMemory();
            std::ostringstream stream;
            stream << "Frame Memory Allocated: " << ((mem > 0) ? (mem/1024/1024) : 0) << " MB - " << "Filters Initalized: " << ac::all_objects.size() << " - Frames Allocated: " << ac::getCurrentAllocatedFrames() << "\n";
            std::string name = stream.str();
            [[controller getMemoryText:nil] setStringValue: [NSString stringWithUTF8String:name.c_str()]];
        });
        
        if(ac::noRecord == false) {
            cv::Mat up;
            up = frame;
            if(writer->isOpened()) {
                if((ac::syphon_in_enabled && ac::syphon_in_changed) || ac::syphon_in_enabled == false) {
                    writer->write(up);
                    if(ac::syphon_in_enabled == true) ac::syphon_in_changed = false;
                }
            }
            struct stat buf;
            stat(ac::fileName.c_str(), &buf);
            file_size = buf.st_size;
        }
        
        dispatch_sync(dispatch_get_main_queue(), ^{
            if(syphon_enabled == YES) {
                cv::Mat frame_copy = frame.clone();
                [render_ updateTexture: &frame_copy];
            }
        });
        
        if(ac::snapShot == true) {
            static unsigned int index = 0;
            stream.str("");
            time_t t = time(0);
            struct tm *m;
            m = localtime(&t);
            stream << add_path << "-" << std::left << (m->tm_year + 1900) << "." << std::setw(2) << std::setfill('0') << (m->tm_mon + 1) << "." << std::setw(2) << std::setfill('0') << m->tm_mday << "_" << std::setw(2) << std::setfill('0') << m->tm_hour << "." << std::setw(2) << std::setfill('0') << m->tm_min << "." << std::setw(2) << std::setfill('0') << m->tm_sec  << "-" << frame.cols << "x" << frame.rows << ".Acid.Cam.Image." << ac::draw_strings[ac::draw_offset] << "." << (++index) << ".png";
            imwrite(stream.str(), frame);
            sout << "Took snapshot: " << stream.str() << "\n";
            ac::snapShot = false;
            // flush to log
            dispatch_sync(dispatch_get_main_queue(), ^{
                flushToLog(sout);
            });
        }
    }
    dispatch_sync(dispatch_get_main_queue(), ^{
        [finish_queue orderOut:self];
        cv::destroyWindow("Acid Cam v2");
        cv::destroyWindow("Controls");
        if(!ac::noRecord && writer->isOpened()) {
            sout << "Wrote to Video File: " << ac::fileName << "\n";
            writer->release();
        }
        sout << (video_total_frames+frame_cnt) << " Total frames\n";
        sout << ((video_total_frames+frame_cnt)/ac::fps) << " Seconds\n";
        flushToLog(sout);
        setEnabledProg();
        if(breakProgram == true) {
            [self writeSettings:self];
            [NSApp terminate:nil];
        }
        programRunning = false;
        [startProg setEnabled: YES];
        [background release];
        camera_active = false;
        [videoFileInput setEnabled:YES];
        [startProg setTitle:@"Start Session"];
    });
}

- (void) cvProc: (id) sender {
    if(breakProgram == true || stopProgram == true) { stopCV(); return; }
    if(isPaused && pauseStepTrue == true) {
        pauseStepTrue = false;
    }
    else if(isPaused) return;
    
    [self pollJoystick:self];
    
    NSInteger stat_test = [chk_rand_repeat integerValue];
    if(stat_test == NSControlStateValueOn) {
        [chk_rand_frames setEnabled: NO];
        NSInteger val;
        static NSInteger value_index = 0, frame_index = 0;
        val = [image_combo numberOfItems];
        if(val >= 2) {
            NSInteger value_max = [chk_rand_frames integerValue];
            if(value_max >= 1) {
                NSInteger next_index = 0;
                NSInteger mode = [chk_rand_mode indexOfSelectedItem];
                ++value_index;
                if(value_index > value_max) {
                    if(mode == 0)
                        next_index = rand()%val;
                    else if(mode == 1)
                        next_index = frame_index;
                    else if(mode == 2) {
                        next_index = image_shuffle[image_shuffle_index];
                        ++image_shuffle_index;
                        if(image_shuffle_index > image_shuffle.size()-1) {
                            std::shuffle(image_shuffle.begin(), image_shuffle.end(), img_rng);
                            image_shuffle_index = 0;
                        }
                    }
                    ++frame_index;
                    if(frame_index > val-1)
                        frame_index = 0;
                    NSString *str_value = [image_combo itemObjectValueAtIndex:next_index];
                    value_index = 0;
                    blend_image = cv::imread([str_value UTF8String]);
                    if(!blend_image.empty()) {
                        blend_set = true;
                        ac::image_cycle_reset = true;
                    } else {
                        blend_set = false;
                    }
                    std::string value = [str_value UTF8String];
                    auto pos = value.rfind("/");
                    if(pos != std::string::npos) {
                        value = value.substr(value.rfind("/")+1, value.length());
                        NSString *string_value = [NSString stringWithUTF8String: value.c_str()];
                        [filename_textfield setStringValue:string_value];
                    }
                    NSImage *img = [[NSImage alloc] initWithContentsOfFile:str_value];
                    [cur_selected_image setImage:img];
                    [img release];
                }
            }
        }
    } else {
        [chk_rand_frames setEnabled:YES];
    }
    
    if(ac::v_cap.isOpened()) {
        double value = ac::v_cap.get(cv::CAP_PROP_POS_FRAMES);
        double length = ac::v_cap.get(cv::CAP_PROP_FRAME_COUNT);
        std::ostringstream s;
        NSString *text = [NSString stringWithFormat:@"Video Frame Index: [%d/%d]", static_cast<int>(value), static_cast<int>(length)];
        [video_position setStringValue: text];
    }
    
    NSInteger cap_width = [video_width integerValue];
    NSInteger cap_height = [video_height integerValue];
    cv::Mat frame;
    bool frame_read = true;
    if([menu_freeze state] == NSControlStateValueOff) {
        frame_read = capture->read(frame);
        old_frame = frame.clone();
    } else {
        frame = old_frame.clone();
    }
    NSColor *color_value = [color_well color];
    double rf = 0, gf = 0, bf = 0;
    [color_value getRed:&rf green:&gf blue:&bf alpha:nil];
    cv::Vec3b well_color;
    unsigned int values[3];
    values[2] = rf*255.99999f;
    values[1] = gf*255.99999f;
    values[0] = bf*255.99999f;
    well_color[0] = values[0];
    well_color[1] = values[1];
    well_color[2] = values[2];
    if(reset_memory == true) {
        ac::reset_filter = true;
        reset_memory = false;
    }
    if([up4k state] == NSControlStateValueOn || frame.size() == cv::Size((int)cap_width, (int)cap_height)) {
        [stretch_scr setState: NSControlStateValueOn];
        //cv::resizeWindow("Acid Cam v2", rc.size.width, rc.size.height);
    }
    static int frame_counter_speed = 0;
    if(program_speed == 0)
        frame_counter_speed = 0;
    else if(program_speed == 1) {
        frame_counter_speed = (frame_counter_speed == 0) ? 1 : 0;
    } else if(program_speed == 2) {
        ++frame_counter_speed;
        if(frame_counter_speed > 3)
            frame_counter_speed = 0;
    }
    unsigned long mem = ac::calculateMemory();
    std::ostringstream stream;
    stream << "Frame Memory Allocated: " << ((mem > 0) ? (mem/1024/1024) : 0) << " MB - " << "Filters Initalized: " << ac::all_objects.size() << " - Frames Allocated: " << ac::getCurrentAllocatedFrames() << "\n";
    std::string name = stream.str();
    [memory_text setStringValue: [NSString stringWithUTF8String:name.c_str()]];
    if(capture->isOpened() && frame_read == false) {
        ++frame_proc;
        if(frame_counter_speed == 0) ++elapsed_counter;
        double seconds = (total_frames/ac::fps);
        double cfps = (frame_cnt/ac::fps);
        double elapsed = (elapsed_counter/ac::fps);
        char elapsed_s[1024];
        snprintf(elapsed_s, 1023, "%.2f", elapsed);
        char cfps_s[1024];
        snprintf(cfps_s, 1023, "%.2f", (seconds-cfps));
        ftext  << "(Frames/Total Frames/Remaining Sec/Length Sec/MB): " << frame_cnt << "/" << total_frames << "/" << cfps_s << "/" << elapsed_s << "/" << ((file_size/1000)/1000) << " MB";
        if(ac::noRecord == false) {
            struct stat buf;
            stat(ac::fileName.c_str(), &buf);
            file_size = buf.st_size;
        }
        if(camera_mode == 1) {
            // float val = frame_cnt;
            float size = total_frames;
            if(size != 0)
                ftext << " - 100% ";
        }
        setFrameLabel(ftext);
        if([chk_repeat integerValue] != 0) {
            video_total_frames += frame_cnt;
            jumptoFrame(syphon_enabled, 0);
            return;
        }
        stopCV();
        [startProg setTitle:@"Start Session"];
        NSUserNotification *notification = [[NSUserNotification alloc] init];
        notification.title = @"Your Video has Finished Processing!";
        notification.informativeText = @"Acid Cam Video Complete";
        notification.soundName = NSUserNotificationDefaultSoundName;
        [[NSUserNotificationCenter defaultUserNotificationCenter] deliverNotification:notification];
        [notification release];
        frame_counter_speed = 0;
        return;
    }
        
    cv::Mat temp_frame;
    if([rotate_v state] == NSControlStateValueOn) {
        cv::flip(frame, temp_frame, 1);
        frame = temp_frame.clone();
    }
    if([rotate_h state] == NSControlStateValueOn) {
        cv::flip(frame, temp_frame, 0);
        frame = temp_frame.clone();
    }
    
    cv::Mat up;
    if([up4k state] == NSControlStateValueOn && frame.size() != cv::Size((int)cap_width, (int)cap_height)) {
        if([chk_stretch state] == NSControlStateValueOff) {
            frame = resizeKeepAspectRatio(frame, cv::Size((int)cap_width, (int)cap_height), cv::Scalar(0, 0, 0));
        } else {
            cv::Mat copy1 = frame.clone();
            ac::ac_resize(copy1, frame, cv::Size((int)cap_width, (int)cap_height));
        }
    }
    ac::orig_frame = frame.clone();
    if(ac::draw_strings[ac::draw_offset] != "Custom") {
        if([negate_checked integerValue] == NSControlStateValueOff) ac::isNegative = false;
        else ac::isNegative = true;
        ac::color_order = (int) [corder indexOfSelectedItem];
    }
    NSInteger after = [apply_after integerValue];
    if(after == NSControlStateValueOff)
        ac::ApplyColorMap(frame);
    
    if([fade_filter state] == NSControlStateValueOff) {
        if(disableFilter == false && ac::testSize(frame)) {
            if(low_res_mode == true) {
                cv::Size s = frame.size();
                cv::Mat small;
                ac::ac_resize(frame, small, cv::Size(640, 360));
                ac::CallFilter(ac::draw_offset, small);
                ac::ac_resize(small, frame, s);
            }
            else
                ac::CallFilter(ac::draw_offset, frame);

        }
    } else {
        if(current_fade_alpha >= 0) {
            ac::filterFade(frame, (int)current_fade, ac::draw_offset, current_fade_alpha);
            current_fade_alpha -= 0.08;
        } else {
            if(disableFilter == false && ac::testSize(frame)) {
                if(low_res_mode == true) {
                    cv::Size s = frame.size();
                    cv::Mat small;
                    ac::ac_resize(frame, small, cv::Size(640, 360));
                    ac::CallFilter(ac::draw_offset, small);
                    ac::ac_resize(small, frame, s);
                }
                else
                    ac::CallFilter(ac::draw_offset, frame);
            }
        }
    }
    ac::frames_released = false;
    ac::image_matrix_reset = false;
    ac::image_cycle_reset = false;
    if(restartFilter == YES) {
        ac::image_matrix_reset = true;
        restartFilter = NO;
    }
    ac::reset_alpha = false;
    NSInteger slide_value = [brightness integerValue];
    if(slide_value > 0)
        ac::setBrightness(frame, 1.0, (int)slide_value);
    
    slide_value = [gamma integerValue];
    if(slide_value > 0) {
        cv::Mat img = frame.clone();
        ac::setGamma(img,frame,(int)slide_value);
    }
    
    slide_value = [saturation integerValue];
    if(slide_value > 0) {
        ac::setSaturation(frame, (int)slide_value);
    }
    
    if(after == NSControlStateValueOn)
        ac::ApplyColorMap(frame);

    
    if([color_chk state] == NSControlStateValueOn && colorkey_set == true && !color_image.empty()) {
        cv::Mat cframe = frame.clone();
        ac::filterColorKeyed(well_color, ac::orig_frame, cframe, frame);
    } else if([color_chk state] == NSControlStateValueOn && colorkey_bg == true) {
        cv::Mat cframe = frame.clone();
        ac::filterColorKeyed(well_color, ac::orig_frame, cframe, frame);
    } else if([color_chk state] == NSControlStateValueOn && colorkey_replace == true) {
        cv::Mat cframe = frame.clone();
        ac::filterColorKeyed(well_color, ac::orig_frame, cframe, frame);
    } else if([color_chk state] == NSControlStateValueOn && colorkey_filter == true) {
        cv::Mat cframe = frame.clone();
        ac::filterColorKeyed(well_color, ac::orig_frame, cframe, frame);
    }
    if([menu_freeze state] == NSControlStateValueOff) {
        ++frame_cnt;
        ++frame_proc;
        if(frame_counter_speed == 0) ++elapsed_counter;
    }
    else {
        ++frame_proc;
        if(frame_counter_speed == 0) ++elapsed_counter;
    }
    if([corder indexOfSelectedItem] == 5) {
        cv::Mat change;
        cv::cvtColor(frame, change, cv::COLOR_BGR2GRAY);
        cv::cvtColor(change, frame, cv::COLOR_GRAY2BGR);
    }
    NSInteger mask = [[NSApp mainWindow] styleMask];
    NSString *main_window = [[NSApp mainWindow] title];
    NSWindow *main_w = [NSApp mainWindow];
    static bool fulls = false;
    NSRect size_i = [[NSScreen mainScreen] frame];
    if([main_window isEqualToString:@"Acid Cam v2"]) {
        
        if((mask & NSWindowStyleMaskFullScreen) || (frame.cols > size_i.size.width && frame.rows > size_i.size.height)) {
            fulls = true;
            rc = [self getScreenSize];
        }
        else {
            fulls = false;
            rc = [main_w frame];
        }
    }
    if(frame_counter_speed == 0) {
        if([stretch_scr state] == NSControlStateValueOn) {
            cv::Mat dst;
            dst = resizeKeepAspectRatio(frame, cv::Size(rc.size.width, rc.size.height), cv::Scalar(0,0,0));
            if(outputVideo == YES && syphon_enabled == NO && frame.size().width > 0 && frame.size().height > 0) cv::imshow("Acid Cam v2", dst);
        } else {
            if(!frame.empty() && frame.rows > 25 && frame.cols > 25) {
                if(frame.ptr() != NULL) {
                    if(outputVideo == YES && syphon_enabled == NO && frame.size().width > 0 && frame.size().height > 0) cv::imshow("Acid Cam v2", frame);
                }
            }
        }
    }
    
    double seconds = ((total_frames)/ac::fps);
    //double cfps = (freeze_count+video_total_frames+frame_cnt)/ac::fps);
    
    double cfps = frame_cnt/ac::fps;
    double elapsed = elapsed_counter/ac::fps;
    char elapsed_s[1024];
    snprintf(elapsed_s, 1023, "%.2f", elapsed);
    char cfps_s[1024];
    snprintf(cfps_s, 1023, "%.2f", (seconds-cfps));
    ftext << "(Frames/Total Frames/Remaining Sec/Length Sec/MB): " << frame_cnt << "/" << (total_frames) << "/" << cfps_s << "/" << elapsed_s << "/" << ((file_size/1000)/1000) << " MB";
    if(camera_mode == 1) {
        float val = frame_cnt;
        float size = total_frames;
        if(size != 0)
            ftext << " - " << (val/size)*100 << "% ";
    }
    setFrameLabel(ftext);
    if(ac::noRecord == false) {
        
        if(writer->isOpened() && frame_counter_speed == 0) {
            if((ac::syphon_in_enabled && ac::syphon_in_changed) || ac::syphon_in_enabled == false) {
                writer->write(frame);
                if(ac::syphon_in_enabled) ac::syphon_in_changed = false;
            }
        } else {
            write_ffmpeg(fptr, frame);
        }
        struct stat buf;
        stat(ac::fileName.c_str(), &buf);
        file_size = buf.st_size;
    }
    
    if(syphon_enabled == YES) [render_ updateTexture:&frame];
    
    if(ac::snapShot == true) {
        static unsigned int index = 0;
        stream.str("");
        time_t t = time(0);
        struct tm *m;
        m = localtime(&t);
        stream << add_path << "-" << (m->tm_year + 1900) << "." << std::setw(2) << std::setfill('0') << (m->tm_mon + 1) << "." << std::setw(2) << std::setfill('0') << m->tm_mday << "_" << std::setw(2) << std::setfill('0') << m->tm_hour << "." << std::setw(2) << std::setfill('0') << m->tm_min << "." << std::setw(2) << std::setfill('0') << m->tm_sec <<  "-" << frame.cols << "x" << frame.rows << ".Acid.Cam.Image." << ac::draw_strings[ac::draw_offset] << "." << (++index) << ".png";
        imwrite(stream.str(), frame);
        sout << "Took snapshot: " << stream.str() << "\n";
        ac::snapShot = false;
        // flush to log
        flushToLog(sout);
    }
}

- (IBAction) openWebcamDialog: (id) sender {
    if([startaction indexOfSelectedItem] == 0)
        [window1 orderFront: self];
    else { // load video
        [window2 orderFront: self];
    }
}

- (IBAction) startVideoProgram: (id) sender {}

- (IBAction) selectFile: (id) sender {
       
    NSOpenPanel *pan = [NSOpenPanel openPanel];
    [pan setAllowsMultipleSelection: NO];
    NSArray *ar = [NSArray arrayWithObjects: @"mov", @"avi", @"mp4", @"mkv",@"m4v",@"webm",@"mts",@"m2ts", nil];
    [pan setAllowedFileTypes:ar];
    if([pan runModal]) {
        NSString *file_name = [[pan URL] path];
        if(file_name != 0) {
            [video_file setStringValue: file_name];
        }
    }
}

- (IBAction) setRotate_V:(id) sender {
    NSInteger state = [rotate_v state];
    if(state == NSControlStateValueOff) {
        [rotate_v setState:NSControlStateValueOn];
    } else {
        [rotate_v setState:NSControlStateValueOff];
    }
    
}
- (IBAction) setRotate_H:(id) sender {
    NSInteger state = [rotate_h state];
    if(state == NSControlStateValueOff) {
        [rotate_h setState:NSControlStateValueOn];
    } else {
        [rotate_h setState:NSControlStateValueOff];
    }
}


- (IBAction) takeSnapshotPNG: (id) sender {
    ac::snapShot = true;
    ac::snapshot_Type = 1;
}

- (IBAction) checkChanged: (id) sender {
    if([videoFileInput integerValue] == 0 ) {
        [video_file setEnabled: NO];
        [resolution setEnabled: YES];
        [device_index setEnabled: YES];
        [selectVideoFile setEnabled: NO];
        [up4k setEnabled: NO];
        [video_width setEnabled: NO];
        [video_height setEnabled: NO];
        [chk_stretch setEnabled:NO];
        
    }
    else {
        [video_file setEnabled: NO];
        [resolution setEnabled: NO];
        [device_index setEnabled: NO];
        [selectVideoFile setEnabled: YES];
        [up4k setEnabled: YES];
        if([up4k state] == NSControlStateValueOn) {
            [video_width setEnabled: YES];
            [video_height setEnabled: YES];
            [chk_stretch setEnabled: YES];
        } else {
            [video_width setEnabled: NO];
            [video_height setEnabled:NO];
            [chk_stretch setEnabled:NO];
        }
    }
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex {
    NSString *str =  [[aTableColumn headerCell] stringValue];
    NSNumber *number = [custom_array objectAtIndex:rowIndex];
    NSNumber *filter_num = [custom_subfilters objectAtIndex: rowIndex];
    NSNumber *on_state = [filter_on objectAtIndex: rowIndex];
    int value = (int)[number integerValue];
    int filter_val = (int)[filter_num integerValue];
    int state_val = (int)[on_state integerValue];
    if( [str isEqualTo:@"Filter"] ) {
        std::string name = ac::draw_strings[value];
        NSString *s = [NSString stringWithFormat:@"%s", ac::draw_strings[value].c_str()];
        return s;
    }
    else if([str isEqualTo:@"State"]) {
        if(state_val == 0) {
            return @"Off";
        } else {
            return @"On";
        }
    }
    else if([str isEqualTo:@"SubIndex"]) {
        if(filter_val == -1)
            return @"Not Set";
        return [NSString stringWithFormat:@"%d", filter_val];
    }
    else if([str isEqualTo:@"Sub Filter"]) {
        if(ac::draw_strings[value].find("SubFilter") == std::string::npos)
            return @"Not Supported";
        int rt_val = filter_val; //ac::subfilter_map[value];
        if(rt_val == -1) return @"No SubFilter Set";
        
        std::string sval;
        sval = ac::draw_strings[rt_val];
        return [NSString stringWithUTF8String: sval.c_str()];
    }
    NSString *s = [NSString stringWithFormat: @"%d", (int)[number integerValue]];
    return s;
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView {
    return [custom_array count];
}

- (IBAction) addCustomItem: (id) sender {
    NSInteger index = [current_filter_custom indexOfSelectedItem];
    NSInteger cate = [categories_custom indexOfSelectedItem];
    if(cate == 14 && index >= 0) {
        std::string user;
        NSMenuItem *item = [user_menu itemAtIndex:index];
        NSString *item_text = [item title];
        std::string s = [item_text UTF8String];
        int index_value = ac::filter_map[s];
        int subf = -1;
        [custom_array addObject: [NSNumber numberWithInt:index_value]];
        [custom_subfilters addObject: [NSNumber numberWithInt: subf]];
        [filter_on addObject: [NSNumber numberWithInt: 1]];
        [table_view reloadData];
        std::ostringstream stream;
        stream << "Added Filter: " << s << " to Custom.\n";
        flushToLog(stream);
        return;
    }
    NSMenuItem *item = [menu_items_custom[cate] itemAtIndex: index];
    NSString *title = [item title];
    if(index >= 0 && cate >= 0) {
        int filter_value = ac::filter_map[[title UTF8String]];
        [custom_array addObject: [NSNumber numberWithInt: filter_value]];
        [custom_subfilters addObject: [NSNumber numberWithInt: -1]];
        [filter_on addObject: [NSNumber numberWithInt: 1]];
        [table_view reloadData];
        std::ostringstream stream;
        stream << "Added Filter: " << [title UTF8String] << " to Custom.\n";
        flushToLog(stream);
    }
}

- (IBAction) removeCustomItem: (id) sender {
    NSInteger index = [table_view selectedRow];
    if(index >= 0) {
        NSNumber *num = [custom_array objectAtIndex: index];
        std::ostringstream stream;
        stream << "Removed Filter in Custom: " << ac::draw_strings[[num integerValue]] << "\n";
        [custom_array removeObjectAtIndex:index];
        [custom_subfilters removeObjectAtIndex: index];
        [filter_on removeObjectAtIndex:index];
        [table_view reloadData];
        flushToLog(stream);
        if([custom_array count] == 0)
            [self releaseFrames:self];
    }
}

- (IBAction) moveCustomUp: (id) sender {
    NSInteger index = [table_view selectedRow];
    if(index > 0) {
        NSInteger pos = index-1;
        id obj = [custom_array objectAtIndex:pos];
        id mv = [custom_array objectAtIndex:index];
        id mv_c = [custom_subfilters objectAtIndex:index];
        id obj_c = [custom_subfilters objectAtIndex:pos];
        id var1_mv = [filter_on objectAtIndex:index];
        id var2_obj = [filter_on objectAtIndex:pos];
        [custom_array setObject:obj atIndexedSubscript:index];
        [custom_array setObject:mv atIndexedSubscript: pos];
        [custom_subfilters setObject:obj_c atIndexedSubscript:index];
        [custom_subfilters setObject:mv_c atIndexedSubscript: pos];
        [filter_on setObject:var2_obj atIndexedSubscript:index];
        [filter_on setObject:var1_mv atIndexedSubscript:pos];
        [table_view deselectAll:self];
        [table_view reloadData];
    }
}
- (IBAction) moveCustomDown: (id) sender {
    NSInteger index = [table_view selectedRow];
    if(index < [custom_array count]-1) {
        NSInteger pos = index+1;
        id obj = [custom_array objectAtIndex:pos];
        id mv = [custom_array objectAtIndex:index];
        id mv_c = [custom_subfilters objectAtIndex:index];
        id obj_c = [custom_subfilters objectAtIndex:pos];
        id mv_f = [filter_on objectAtIndex:index];
        id obj_cf = [filter_on objectAtIndex:pos];
        [custom_array setObject:obj atIndexedSubscript:index];
        [custom_array setObject:mv atIndexedSubscript: pos];
        [custom_subfilters setObject:obj_c atIndexedSubscript:index];
        [custom_subfilters setObject:mv_c atIndexedSubscript: pos];
        [filter_on setObject:obj_cf atIndexedSubscript: index];
        [filter_on setObject:mv_f atIndexedSubscript:pos];
        [table_view deselectAll:self];
        [table_view reloadData];
    }
}

- (IBAction) stepPause: (id) sender {
    pauseStepTrue = true;
    std::ostringstream stream;
    stream << "Stepped to next frame.\n";
    flushToLog(stream);
}

- (IBAction) selectFileForPrefix: (id) sender {
    NSSavePanel *panel = [NSSavePanel savePanel];
    if([panel runModal]) {
        [prefix_input setStringValue:[[panel URL] path]];
    }
}

- (IBAction) changeOrder: (id) sender {
    ac::color_order = (int) [corder indexOfSelectedItem];
}

- (IBAction) pauseProgram: (id) sender {
    if(programRunning == true && camera_mode == 0) {
        [menuPaused setState:NSControlStateValueOff];
        [pause_step setEnabled:NO];
        isPaused = false;
        ac::setRunning(true);
        _NSRunAlertPanel(@"Cannot pause in camera mode", @"Cannot pause", @"Ok", nil, nil);
        return;
    }
    NSInteger checkedState = [menuPaused state];
    std::ostringstream stream;
    if(checkedState == NSControlStateValueOn) {
        [menuPaused setState: NSControlStateValueOff];
        [pause_step setEnabled: NO];
        isPaused = false;
        ac::setRunning(true);
        stream << "Program unpaused.\n";
        flushToLog(stream);
        
    } else {
        [menuPaused setState: NSControlStateValueOn];
        isPaused = true;
        ac::setRunning(false);
        [pause_step setEnabled: YES];
        stream << "Program paused.\n";
        flushToLog(stream);
    }
}

- (IBAction) disableFilters: (id) sender {
    NSInteger checkedState = [disable_filters state];
    std::ostringstream stream;
    if(checkedState == NSControlStateValueOn) {
        [disable_filters setState: NSControlStateValueOff];
        // enable
        disableFilter = false;
        stream << "Filters enabled.\n";
        flushToLog(stream);
    } else {
        [disable_filters setState: NSControlStateValueOn];
        // disable
        disableFilter = true;
        stream << "Filters disabled.\n";
        flushToLog(stream);
    }
}

- (IBAction) goto_Frame: (id) sender {
    if(programRunning == false) return;
    int val = (int)[frame_slider integerValue];
    if(val < [frame_slider maxValue]-1) {
        jumptoFrame(syphon_enabled, val);
        std::ostringstream stream;
        stream << "Jumped to frame: " << val << "\n";
        flushToLog(stream);
    }
}

- (IBAction) setGoto: (id) sender {
    if(programRunning == false) return;
    NSInteger time_val = [frame_slider integerValue];
    double seconds = time_val/ac::fps;
    int min = 0, re = 0;;
    unsigned int sec = static_cast<unsigned int>(seconds);
    NSString *str_val;
    if(sec >= 60) {
        min = (sec/60);
        re = (sec%60);
        str_val = [NSString stringWithFormat:@"Jump to Time: %d Minutes %d Seconds @ Frame #%d",min,re, (int)time_val];
    } else {
        str_val = [NSString stringWithFormat:@"Jump to Time: %d Seconds @ Frame #%d",sec,(int)time_val];
    }
    [goto_fr setStringValue: str_val];
}

- (IBAction) openGoto: (id) sender {
    if(total_frames != 0) {
        [goto_frame orderFront:self];
    } else {
        _NSRunAlertPanel(@"Cannot jump to frame must be in video mode", @"Recording Error", @"Ok", nil, nil);
    }
    std::ostringstream stream;
    stream << "Opened Goto Window.\n";
    flushToLog(stream);
}

- (IBAction) pauseVideo: (id) sender {}

- (IBAction) changeFilterIndex: (id) sender {
    current_filterx = (int) [filter_selector indexOfSelectedItem];
}

- (IBAction) changeRGB: (id) sender {
    red = (int) [slider_red integerValue];
    green = (int) [slider_green integerValue];
    blue = (int) [slider_blue integerValue];
    [slider_red_pos setIntegerValue: red];
    [slider_green_pos setIntegerValue: green];
    [slider_blue_pos setIntegerValue: blue];
}

- (IBAction) changeReverse:(id)sender {
    reverse = (int)[rgb_box indexOfSelectedItem];
}

- (IBAction) changeNegate: (id) sender {
    negate = [check_box state] == NSControlStateValueOff ? false : true;
}

- (IBAction) setNegative: (id) sender {
    NSInteger chkvalue = [negate_checked integerValue];
    if(chkvalue == NSControlStateValueOn) ac::isNegative = true;
    else ac::isNegative = false;
}

- (IBAction) selectImage: (id) sender {
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setCanChooseDirectories:NO];
    [panel setCanChooseFiles: YES];
    [panel setAllowsMultipleSelection: YES];
    [panel setAllowedFileTypes: [NSArray arrayWithObjects: @"png", nil]];
    if([panel runModal]) {
        NSArray *file_names = [panel URLs];
        if([file_names count] > 0) {
            
            if(!image_shuffle.empty())
                image_shuffle.erase(image_shuffle.begin(), image_shuffle.end());
            
            for(int i = 0; i < [file_names count]; ++i) {
                NSURL *file_n = [file_names objectAtIndex:i];
                [image_combo addItemWithObjectValue: [file_n path]];
            }
            for(int i = 0; i < [image_combo numberOfItems]; ++i) {
                image_shuffle.push_back(i);
            }
            std::shuffle(image_shuffle.begin(), image_shuffle.end(), img_rng);
            image_shuffle_index = 0;
        }
    }
}

- (IBAction) updateLabelText: (id) sender {
    NSInteger index = [image_to_set indexOfSelectedItem];
    NSString *s = [NSString stringWithUTF8String: set_filenames[index].c_str()];
    NSImage *img = [[NSImage alloc] initWithContentsOfFile:s];
    [view_image setImage:img];
    [img release];
    [selectedFilename setStringValue: s];
}

- (IBAction) changeImageSelected: (id) sender {
    NSInteger combo_i = [image_combo indexOfSelectedItem];
    if(combo_i == -1)
        return;
    NSString *s = [image_combo itemObjectValueAtIndex:combo_i];
    NSImage *img = [[NSImage alloc] initWithContentsOfFile:s];
    [cur_selected_image setImage:img];
    [img release];
}

- (IBAction) setAsImage: (id) sender {
    if([image_combo indexOfSelectedItem] >= 0) {
        NSString *current = [image_combo itemObjectValueAtIndex: [image_combo indexOfSelectedItem]];
        NSInteger index = [image_to_set indexOfSelectedItem];
        if(index == 0) {
            
            isPaused = true;
            blend_image = cv::imread([current UTF8String]);
            isPaused = false;
            if(blend_image.empty()) {
                _NSRunAlertPanel(@"Image Not set", @"Could Not Set Image...\n", @"Ok", nil, nil);
                return;
            }
            ac::reset_filter = true;
            blend_set = true;
            std::ostringstream stream;
            stream << "Blend Image set to: " << [current UTF8String] << "\n";
            [selectedFilename setStringValue:current];;
            NSString *s = [NSString stringWithFormat:@"%s", stream.str().c_str(), nil];
            set_filenames[0] = [current UTF8String];
            _NSRunAlertPanel(@"Image set", s, @"Ok", nil, nil);
            flushToLog(stream);
        } else if(index == 1) {
            if(colorkey_bg == true || colorkey_replace == true) {
                _NSRunAlertPanel(@"You can only set one Color Key setting at a time clear the other ones", @"Only one at a time", @"Ok", nil, nil);
                return;
            }
            color_image = cv::imread([current UTF8String]);
            if(color_image.empty()) {
                _NSRunAlertPanel(@"Image Not set", @"Could Not Set Image...\n", @"Ok", nil, nil);
                return;
            }
            colorkey_set = true;
            std::ostringstream stream;
            stream << "ColorKey Image set to: " << [current UTF8String] << "\n";
            set_filenames[1] = [current UTF8String];
            NSString *s = [NSString stringWithFormat:@"%s", stream.str().c_str(), nil];
            _NSRunAlertPanel(@"Image Set", s, @"Ok", nil, nil);
            flushToLog(stream);
        } else if(index == 2) {
            if(colorkey_set == true || colorkey_replace == true) {
                _NSRunAlertPanel(@"You can only set one Color Key setting at a time clear the other ones", @"Only one at a time", @"Ok", nil, nil);
                return;
            }
            color_bg_image = cv::imread([current UTF8String]);
            if(color_bg_image.empty()) {
                colorkey_bg = false;
                _NSRunAlertPanel(@"Error could not open file...", @"Could not open file", @"Ok",nil,nil);
                return;
            }
            set_filenames[2] = [current UTF8String];
            colorkey_bg = true;
            std::ostringstream stream;
            stream << "ColorKey Background image set to: " << [current UTF8String] << "\n";
            flushToLog(stream);
            _NSRunAlertPanel(@"Set ColorKey Background", @"Color Key Image Set", @"Ok", nil, nil);
        } else if(index == 3) {
            
            if(colorkey_set == true || colorkey_bg == true) {
                _NSRunAlertPanel(@"You can only set one Color Key setting at a time clear the other ones", @"Only one at a time", @"Ok", nil, nil);
                return;
            }
            
            color_replace_image = cv::imread([current UTF8String]);
            if(color_replace_image.empty()) {
                colorkey_replace = false;
                _NSRunAlertPanel(@"Error could not open file...", @"Could not open file", @"Ok",nil,nil);
                return;
            }
            set_filenames[3] = [current UTF8String];
            colorkey_replace = true;
            std::ostringstream stream;
            stream << "ColorKey Replace Background image set to: " << [current UTF8String] << "\n";
            flushToLog(stream);
            _NSRunAlertPanel(@"Set ColorKey Replace Background", @"Color Key Image Set", @"Ok", nil, nil);
        }
    }
    [self updateLabelText:self];
    ac::pix.setInit(false);
}

- (IBAction) showCustom: (id) sender {
    [custom_window orderFront: self];
    [filter_search_window orderFront: self];
    std::ostringstream stream;
    stream << "Custom Window Now Visible.\n";
    flushToLog(stream);
}

- (IBAction) showActivityLog: (id) sender {
    [window1 orderFront: self];
    
}

- (IBAction) showSelectImage: (id) sender {
    [image_select orderFront: self];
    std::ostringstream stream;
    stream << "Image Settings Window Shown.\n";
    flushToLog(stream);
}

- (IBAction) showAlpha: (id) sender {
    [alpha_window orderFront: self];
}

- (IBAction) showPlugins: (id) sender {
    [plugin_window orderFront:self];
}

- (IBAction) setRGB_Values: (id) sender {
    NSInteger red_val = [red_slider integerValue];
    NSInteger green_val = [green_slider integerValue];
    NSInteger blue_val = [blue_slider integerValue];
    ac::swapColor_r = (unsigned int)red_val;
    ac::swapColor_g = (unsigned int)green_val;
    ac::swapColor_b = (unsigned int)blue_val;
    [t_red setIntegerValue: red_val];
    [t_green setIntegerValue: green_val];
    [t_blue setIntegerValue: blue_val];
}

- (IBAction) menuFreeze: (id) sender {
    std::ostringstream stream;
    if([menu_freeze state] == NSControlStateValueOn) {
        [menu_freeze setState: NSControlStateValueOff];
        stream << "Set Freeze Off.\n";
        
    } else {
        [menu_freeze setState: NSControlStateValueOn];
        stream << "Set Freeze On.\n";
    }
    flushToLog(stream);
}

- (IBAction) rewindToStart:(id) sender {
    [frame_slider setIntegerValue:0];
    jumptoFrame(syphon_enabled, 0);
    std::ostringstream stream;
    stream << "Video position set to start.\n";
    flushToLog(stream);
}

- (IBAction) changeVideoPos: (id) sender {
    [self goto_Frame: self];
    [self setGoto: self];
}

- (NSRect) getScreenSize {
    return [[NSScreen mainScreen] frame];
}

- (IBAction) setStretch: (id) sender {
    if([stretch_scr state] == NSControlStateValueOn) {
        [stretch_scr setState: NSControlStateValueOff];
    } else {
        [stretch_scr setState: NSControlStateValueOn];
        cv::resizeWindow("Acid Cam v2", rc.size.width, rc.size.height);
    }
}

- (IBAction) setColorMap: (id) sender {
    NSInteger index = [color_map indexOfSelectedItem];
    ac::set_color_map = (int) index;
    std::ostringstream stream;
    stream << "Color Map set to: " << ac::set_color_map << "\n";
    flushToLog(stream);
    /*
    for(int i=0;i<256;i++)
    {
        M1.at<uchar>(i)=Luta[i][0];
        M2.at<uchar>(i)=Luta[i][1];
        M3.at<uchar>(i)=Luta[i][2];
    }
    
    Mat r1,r2,r3;
    cv::LUT(temp,M1,r1);
    cv::LUT(temp,M2,r2);
    cv::LUT(temp,M3,r3);
    std::vector<cv::Mat> planes;
    planes.push_back(r1);
    planes.push_back(r2);
    planes.push_back(r3);
    cv::merge(planes,dst); */
}

- (IBAction) selectedCustomFilter: (id) sender {
    std::ostringstream strout;
    NSInteger index = [current_filter_custom indexOfSelectedItem];
    NSInteger cate = [categories_custom indexOfSelectedItem];
    if(index >= 0 && index < [menu_items_custom[cate] numberOfItems]) {
        NSMenuItem *item = [menu_items_custom[cate] itemAtIndex: index];
        NSString *title = [item title];
        std::string file_str = [title UTF8String];
        if(file_str == "ParticleRelease" || file_str == "ParticleBlend" || file_str == "ParticleFlash" || file_str == "ParticleAlpha") {
            emiter.reset();
            strout.str("");
            strout << "ParticleRelease Variables Reset\n";
        }
        if(file_str == "Alpha Flame Filters") {
            [alpha_window orderFront:self];
        }
        if(file_str == "Plugin") {
            [plugin_window orderFront:self];
        }
        if(file_str == "ColorRange") {
            ac::colors[0] = rand()%255;
            ac::colors[1] = rand()%255;
            ac::colors[2] = rand()%255;
            strout.str("");
            strout << "ColorRange random pixels set to: " << ac::colors[0] << "," << ac::colors[1] << "," << ac::colors[2] << "\n";
            flushToLog(strout);
        }
        std::string filter_value = file_str;
        if(filter_value.find("Video") != std::string::npos) {
            [image_select orderFront:self];
            strout.str("");
            strout << "Select Secondary Video File...\n";
            flushToLog(strout);
        }
        if(filter_value.find("Image") != std::string::npos) {
            [image_select orderFront: self];
            strout.str("");
            strout << "Image filter select use Select image window to set image...\n";
            flushToLog(strout);
        }
        if(filter_value.find("SubFilter") != std::string::npos) {
            strout.str("");
            strout << "To use this filter, set a subfilter in Custom Window...\n";
            flushToLog(strout);
        }
        if(filter_value.find("SlitScan") != std::string::npos || filter_value == "SlitScanGUI" || filter_value == "SlitScanGUI_RGB" || filter_value == "SlitScanXGUI_RGB" || filter_value == "SlitScanXGUI" || filter_value == "SlitScanRandom") {
            [slit_win orderFront:self];
        }
        if(filter_value == "BlendWithSourcePercent") {
             [blend_percent_window orderFront:self];
        }
        if([chk_preview integerValue] == 1 && !test_image.empty()) {
            cv::Mat copy1 = test_image.clone();
            if(ac::filter_map_str.find(file_str) != ac::filter_map_str.end()) {
                ac::CallFilter(file_str, copy1);
                cv::namedWindow("Preview_Window", cv::WINDOW_NORMAL);
                if(copy1.size().width > 0 && copy1.size().height > 0) cv::imshow("Preview_Window", copy1);
            }
        }
        if(filter_value.find("MultiVideo") != std::string::npos) {
            [video_indow orderFront:self];
        }
    }
}

- (IBAction) checkboxClose: (id) sender {
    if([chk_preview integerValue] == 0)
        cv::destroyWindow("Preview_Window");
}

- (IBAction) addSearchItem: (id) sender {
    NSInteger index = [find_table selectedRow];
    if(index >= 0 && index < [search_results count]) {
        NSNumber *num = [search_results objectAtIndex:index];
        NSNumber *subf = [NSNumber numberWithInt: -1];
        [custom_array addObject:num];
        [custom_subfilters addObject: subf];
        [filter_on addObject: [NSNumber numberWithInt: 1]];
        [table_view reloadData];
        std::ostringstream stream;
        NSInteger val = [num integerValue];
        stream << "Added Search Item: " << ac::draw_strings[val] << "\n";
        flushToLog(stream);
    }
}
- (IBAction) searchForItem: (id) sender {
    SearchForString([find_text stringValue]);
}

- (IBAction) viewSearchWindow: (id) sender {
    [filter_search_window orderFront: self];
    [custom_window orderFront: self];
}

- (IBAction) clearBlend: (id) sender {
    std::ostringstream stream;
    switch([image_to_set indexOfSelectedItem]) {
        case 0:
            blend_set = false;
            blend_image.release();
            [selectedFilename setStringValue:@""];
            _NSRunAlertPanel(@"Blend image released", @"Released Image", @"Ok", nil, nil);
            stream << "Blend image released.\n";
            flushToLog(stream);
            break;
        case 1:
            colorkey_set = false;
            color_image.release();
            _NSRunAlertPanel(@"Color Key image released", @"Released Image", @"Ok", nil, nil);
            stream << "Color Key image released.\n";
            flushToLog(stream);
            break;
        case 2:
            colorkey_bg = false;
            color_bg_image.release();
            _NSRunAlertPanel(@"Released ColorKey background", @"Released Image", @"Ok", nil, nil);
            stream << "Released ColorKey background.\n";
            flushToLog(stream);
            break;
        case 3:
            colorkey_replace = false;
            color_replace_image.release();
            _NSRunAlertPanel(@"Color Key Replace Image Cleared", @"Color Key Image Replace Released", @"Ok", nil, nil);
            stream << "Color Key Replace Image Cleared.\n";
            flushToLog(stream);
            break;
    }
    NSInteger index = [image_to_set indexOfSelectedItem];
    if(index >= 0) {
        set_filenames[index] = "None";
        [self updateLabelText:self];
    }
    [cur_selected_image setImage:nil];
    [view_image setImage:nil];
}

- (IBAction) clearImageList: (id) sender {
    blend_set = false;
    [chk_rand_repeat setIntegerValue: 0];
    [chk_rand_frames setEnabled:YES];
    [image_combo removeAllItems];
    [image_combo setStringValue:@""];
}

- (void) updatePref: (BOOL)display_msg {
    NSInteger pos = [procMode indexOfSelectedItem];
    ac::PROC_MODE_TYPE move_type;
    switch(pos) {
        case 0:
            move_type = ac::MOVEINOUT_INC;
            break;
        case 1:
            move_type = ac::MOVEINOUT;
            break;
        case 2:
            move_type = ac::MOVERESET;
            break;
    }
    ac::setProcMode(move_type);
    std::ostringstream log;
    std::string str_val[] = {"Move Out Increase, Move in", "Move Out, Move in", "Move Out, Reset", ""};
    log << "Proccess Mode Set to: " << str_val[pos] << "\n";
    if([cam_frame_rate_chk state] == NSControlStateValueOn) {
        set_frame_rate = true;
        set_frame_rate_val = [cam_frame_rate floatValue];
        if(set_frame_rate_val <= 10 || set_frame_rate_val > 60) {
            _NSRunAlertPanel(@"Frame Rate invalid Try again", @"Change Rate", @"Ok", nil, nil);
            return;
        } else {
            log << "Frame Rate set to: " << set_frame_rate_val << "\n";
        }
        
    } else {
        set_frame_rate = false;
        set_frame_rate_val = 24;
    }
    
    NSInteger rgb_low[3] = {[colorkey_b_low integerValue],[colorkey_g_low integerValue],[colorkey_r_low integerValue]};
    NSInteger rgb_high[3] = {[colorkey_b_high integerValue], [colorkey_g_high integerValue],[colorkey_r_high integerValue]};
    
    for(int i = 0; i < 3; ++i) {
        if(rgb_low[i] < 0 || rgb_low[i] > 255) {
            _NSRunAlertPanel(@"Incorrect Range Value", @"Must be between 0-255", @"Ok", nil, nil);
            return;
        }
        if(rgb_high[i] < 0 || rgb_high[i] > 255) {
            _NSRunAlertPanel(@"Incorrect Range Value", @"Must be between 0-255", @"Ok", nil, nil);
            return;
        }
    }
    cv::Vec3b color_low(rgb_low[0], rgb_low[1], rgb_low[2]);
    cv::Vec3b color_high(rgb_high[0], rgb_high[1], rgb_high[2]);
    ac::setColorKeyRange(color_low, color_high);
    NSInteger num_index = [blend_source_amt indexOfSelectedItem];
    double amount = 0.1 * (num_index+1);
    ac::setBlendPercentage(amount);
    NSInteger num = [proc_change indexOfSelectedItem];
    static double values[] = { 0, 0.001,0.009, 0.01, 0.03, 0.05, 0.07, 0.08, 0.1, 0.3, 0.7, 1.0, 3};
    static std::string str_values[] = { "Default", "0.001","0.009","0.01","0.03", "0.05", "0.07", "0.08","0.1", "0.3"," 0.7", "1.0", "3", ""};
    ac::alpha_increase = values[num];
    log << "Proccess Speed set to: " << str_values[num] << "\n";
    log << "Blend with Source Image set to: " << ((num_index+1)*10) << "%\n";
    int values_low[] = { color_low[0], color_low[1], color_low[2] };
    int values_high[] = { color_high[0], color_high[1], color_high[2] };
    log << "ColorKey Low BGR Values Set: " << values_low[0] << "," << values_low[1] << "," << values_low[2] << "\n";
    log << "ColorKey High BGR Values Set: " << values_high[0] << "," << values_high[1] << "," << values_high[2] << "\n";
    NSInteger szPtr = [sy_size indexOfSelectedItem];
    NSSize sz;
    switch(szPtr) {
        case 0:
            sz.width = 640;
            sz.height = 480;
            break;
        case 1:
            sz.width = 1280;
            sz.height = 720;
            break;
        case 2:
            sz.width = 1920;
            sz.height = 1080;
            break;
    }
    NSInteger intense = [col_intense integerValue];
    ac::setPixelCollection(static_cast<int>(intense));
    log << "Syphon Output Set To: " << sz.width << "x" << sz.height << "\n";
    NSInteger thread_num = [num_threads integerValue];
    if(thread_num <= 0) {
        _NSRunAlertPanel(@"Invalid Thread Number", @"Must use at least one thread for filters that support threads", @"Ok", nil, nil);
        return;
    }
    ac::setThreadCount(static_cast<int>(thread_num));
    [syphon_window setContentSize: sz];
    std::string output_path_ffmpeg;
    if([ffmpeg_support integerValue] == 1) {
        std::fstream file;
        std::string temp_path = [[ffmpeg_path stringValue] UTF8String];
        file.open(temp_path, std::ios::in);
        if(!file.is_open() || !file.good()) {
            _NSRunAlertPanel(@"FFMPEG must be installed, check README", @"FFMPEG should be installed with Homebrew package manager. It is free you can find it here: https://brew.sh/", @"Ok", nil, nil);
            return;
        }
        file.close();
        ffmpeg_string_path = [[ffmpeg_path stringValue] UTF8String];
        output_path_ffmpeg = ffmpeg_string_path;
    } else {
        output_path_ffmpeg = "Not Enabled.";
    }
    log << "Thread Support Filters Count Set to: " << static_cast<int>(thread_num) << "\n";
    log << "Pixel Intensity Set At: " << static_cast<int>(intense) << "\n";
    log << "FFMPEG Path Set to: " << output_path_ffmpeg << "\n";
    NSInteger max = [max_frames integerValue];
    if(max <= 50) {
        _NSRunAlertPanel(@"Invalid Maximum Stored Frames (based on your systems memory.)", @"Invalid Maximum Stored Frames. Requires a value of 300 or greater.", @"Ok", nil, nil);
        return;
    }
    NSInteger value = [set_color_map indexOfSelectedItem];
    ac::setFilterColorMap(static_cast<int>(value));
    log << "Maximum Stored Frames: " << max << "\n";
    int temp_value = atoi([[custom_cycle_delay stringValue] UTF8String]);
    if(temp_value > 0) {
        delay_value = temp_value;
        log << "Custom Cycle Delay set to: " << delay_value << "\n";
    }
    ac::setMaxAllocated(static_cast<int>(max));
    NSInteger wait_v = [wait_text integerValue];
    if(wait_v < 0) {
        _NSRunAlertPanel(@"The Wait variable cannot be a negative number", @"You entered a Negative value", @"Ok", nil, nil);
        return;
    }
    ac::setVariableWait(static_cast<int>(wait_v));
    log << "Wait set to: " << static_cast<int>(wait_v) << "\n";
    NSInteger color_v = [color_level_text integerValue];
    if(color_v < 0 || color_v > 255) {
        _NSRunAlertPanel(@"The Color variable cannot be a negative number and must be below 255", @"You entered a Incorrect value", @"Ok", nil, nil);
        return;
    }
    ac::setColorLevel(static_cast<int>(color_v));
    log << "Color Level set to: " << static_cast<int>(color_v) << "\n";
    NSInteger max_x = [max_stretch_x integerValue];
    if(max_x <= 5) {
        _NSRunAlertPanel(@"Stretch Value must be greater than 5", @"Invalid Stretch Value", @"Ok", nil, nil);
        return;
    }
    ac::setMaxStretch(static_cast<int>(max_x));
    log << "Max Stretch Set to: " << static_cast<int>(max_x) << "\n";
    int x = (int)[desk_x integerValue];
    int y = (int)[desk_y integerValue];
    int w = (int)[desk_w integerValue];
    int h = (int)[desk_h integerValue];
    ac::SetDesktopRect(x,y,w,h);
    log << "Desktop Rect set to: " << x << "," << y << " " << w << "," << h <<"\n";
    int tv = static_cast<int>([threshold_edit integerValue]);
    ac::setThreshold(tv);
    log << "Threshold: " << tv << "\n";
    NSString *val = [NSString stringWithUTF8String:log.str().c_str()];
    if(display_msg == YES) _NSRunAlertPanel(@"Settings changed", val, @"Ok", nil, nil);
    flushToLog(log);
    [self writeSettings: self];
}

- (IBAction) setPref: (id) sender {
    [self updatePref: YES];
}

- (IBAction) showPrefs:(id) sender {
    [procMode_Window orderFront:self];
}
- (IBAction) resetAllFrames:(id) sender {
    emiter.reset();
    ac::frames_released = true;
    ac::image_matrix_reset = true;
    std::ostringstream stream;
    stream << "Reset All Frames...\n";
    flushToLog(stream);
}
- (IBAction) resetAllFilters: (id) sender {
    if(ac::reset_alpha == false) {
        std::ostringstream stream;
        stream << "Animation alpha and direction reset to 1.\n";
        flushToLog(stream);
        ac::reset_alpha = true;
        ac::image_matrix_reset = true;
        emiter.reset();
        if(ac::v_cap.isOpened())
            ac::v_cap.set(cv::CAP_PROP_POS_FRAMES,0);
        [video_position setStringValue: @"Index: 0"];
    }
}

- (IBAction) releaseFrames:(id)sender {
    reset_memory = true;
    ac::image_matrix_reset = true;
    ac::release_frames = true;
    std::ostringstream stream;
    stream << "Stored Frames Memory Released...\n";
    flushToLog(stream);
}

- (IBAction) setSubFilter:(id) sender {
    NSInteger index = [current_filter_custom indexOfSelectedItem];
    if(index >= 0 && index < [current_filter_custom numberOfItems]) {
        NSMenuItem *m = [current_filter_custom itemAtIndex:index];
        NSString *s = [m title];
        std::string sub_chk = [s UTF8String];
        NSInteger rowIndex = [table_view selectedRow];
        if(rowIndex != -1) {
            NSNumber *num = [custom_array objectAtIndex:rowIndex];
            int val = static_cast<int>([num integerValue]);
            std::ostringstream stream;
            std::string sub_chk1= ac::draw_strings[val];
            if(sub_chk.find("SubFilter") != std::string::npos) {
                std::ostringstream stream;
                stream << "Filter: " << sub_chk << " cannot be set to another function that requires SubFilter\n";
                flushToLog(stream);
                return;
            } else if(sub_chk1.find("SubFilter") == std::string::npos) {
                stream << "Filter: " << sub_chk1 << " must be a SubFilter to set one.\n";
                flushToLog(stream);
                return;
            }
            stream << "Filter " << ac::draw_strings[ac::filter_map[[s UTF8String]]] << " set as SubFilter for " << ac::draw_strings[val] << "\n";
            if(val != -1) {
                int filter_pos = ac::filter_map[[s UTF8String]];
                NSNumber *fval = [NSNumber numberWithInt:filter_pos];
                [custom_subfilters setObject:fval atIndexedSubscript: rowIndex];
                flushToLog(stream);
            }
            [table_view reloadData];
        } else {
            std::ostringstream stream;
            stream << "Please select row in Table that supports SubFilter to set the SubFilter\n";
            flushToLog(stream);
            _NSRunAlertPanel(@"Select Item", @"Please select a filter in the list to assign SubFilter.", @"Ok", nil, nil);
        }
    }
}

- (IBAction) clearSubFilter: (id) sender {
    NSInteger row = [table_view selectedRow];
    if(row != -1) {
        NSNumber *num = [custom_array objectAtIndex:row];
        NSInteger filter_val = [num integerValue];
        if(filter_val != -1) {
            [custom_subfilters setObject:[NSNumber numberWithInt:-1] atIndexedSubscript:row];
            std::ostringstream stream;
            stream << "Sub Filter " << ac::draw_strings[filter_val] << " cleared\n";
            flushToLog(stream);
            [table_view reloadData];
        }
    } else {
        std::ostringstream stream;
        stream << "You must select a filter to clear...\n";
        flushToLog(stream);
    }
}

- (IBAction) setCustomSubFromSearch: (id) sender {
    NSInteger index = [find_table selectedRow];
    NSInteger pos = [table_view selectedRow];
    if(pos < 0 || index < 0) return;
    NSNumber *filter_name = [custom_array objectAtIndex:pos];
    int filter_name_value = static_cast<int>([filter_name integerValue]);
    
    if(index >= 0 && index < [search_results count]) {
        NSNumber *num = [search_results objectAtIndex:index];
        int val = static_cast<int>([num integerValue]);
        std::ostringstream stream;
        std::string sub_chk1= ac::draw_strings[val];
        std::string subval = ac::draw_strings[filter_name_value];
        if(subval.find("SubFilter") == std::string::npos) {
            stream << "Error you must select a fitler that accepts SubFilter\n";
            flushToLog(stream);
            return;
        }
        
        if(val != -1) {
            int filter_pos = ac::filter_map[sub_chk1];
            NSNumber *fval = [NSNumber numberWithInt:filter_pos];
            [custom_subfilters setObject:fval atIndexedSubscript: pos];
            stream << "Filter " << sub_chk1 << " set as SubFilter for " << ac::draw_strings[filter_name_value] << "\n";
            flushToLog(stream);
        }
        [table_view reloadData];
    } else {
        _NSRunAlertPanel(@"You need to select a filter", @"Select in Search and Filter that supports it in Custom", @"Ok", nil, nil);
    }
}

- (IBAction) enableSpyhon: (id) sender {
    std::ostringstream stream;
    if([syphon_enable state] == NSControlStateValueOn) {
        [syphon_enable setState: NSControlStateValueOff];
        [syphon_window orderOut:self];
        syphon_enabled = NO;
        stream << "Disabled Syphon Output...\n";
        if(programRunning == true) {
            cv::namedWindow("Acid Cam v2",cv::WINDOW_NORMAL);
            if(!ac::orig_frame.empty()) {
                cv::resizeWindow("Acid Cam v2", ac::orig_frame.size().width, ac::orig_frame.size().height);
            } else {
                cv::resizeWindow("Acid Cam v2", 1920, 1080);
            }
        }
    } else {
        [syphon_enable setState: NSControlStateValueOn];
        [syphon_window orderFront:self];
        syphon_enabled = YES;
        stream << "Enabled Syphon Output...\n";
        cv::destroyWindow("Acid Cam v2");
    }
    flushToLog(stream);
}

- (IBAction) addToBlocked: (id) sender {
    if([key_range state] == NSControlStateValueOn) {
        [self addToRange:self];
        return;
    } else {
        [self addToTolerance:self];
    }
}
- (IBAction) removedFromBlocked: (id) sender {
    NSInteger row = [blocked_colors indexOfSelectedItem];
    if(row >= 0) {
        [blocked_colors removeItemAtIndex:row];
        [blocked_colors reloadData];
        auto it = green_blocked.begin()+row;
        green_blocked.erase(it);
        if([blocked_colors numberOfItems] == 0)
            [blocked_colors setStringValue:@""];
        else if(row-1 >= 0 && row-1 <= [blocked_colors numberOfItems]){
            [blocked_colors selectItemAtIndex:row-1];
        }
    } else {
        _NSRunAlertPanel(@"Please Select Item Color to Remove\n", @"Select Index to Remove", @"Ok", nil, nil);
    }
}

- (IBAction) addToRange: (id) sender {
    NSInteger color_low[] = {[val_colorkey_b_low integerValue], [val_colorkey_g_low integerValue], [val_colorkey_r_low integerValue]};
    NSInteger color_high[] =  {[val_colorkey_b_high integerValue], [val_colorkey_g_high integerValue], [val_colorkey_r_high integerValue]};
    // make sure valid range 0-255
    for(int i = 0; i < 3; ++i) {
        if(!(color_low[i] >= 0 && color_low[i] <= 255)) {
            _NSRunAlertPanel(@"Low color must be valid range", @"Range between 0-255", @"Ok", nil, nil);
            return;
        }
        if(!(color_high[i] >= 0 && color_high[i] <= 255)){
            _NSRunAlertPanel(@"High color must be valid range", @"Range between 0-255", @"Ok", nil, nil);
            return;
        }
    }
    
    cv::Vec3b well_color_low(color_low[0], color_low[1], color_low[2]);
    cv::Vec3b well_color_high(color_high[0], color_high[1], color_high[2]);
    ac::Keys keys;
    if([chk_spill state] == NSControlStateValueOn)
        keys.spill = true;
    else
        keys.spill = false;
    keys.low = well_color_low;
    keys.high = well_color_high;
    keys.key_type = ac::KeyValueType::KEY_RANGE;// work on tolerance tomorrow
    
    if(!(well_color_low[0] <= well_color_high[0] && well_color_low[1] <= well_color_high[1] && well_color_low[2] <= well_color_high[2])) {
        _NSRunAlertPanel(@"Values must be a valid range", @"Color values must be a valid range between high >= low and low <= high", @"Ok", nil, nil);
        return;
    }
    green_blocked.push_back(keys);
    NSString *s_color = [NSString stringWithFormat:@"Color BGR Range: (%d, %d, %d) - (%d, %d, %d)", well_color_low[0], well_color_low[1], well_color_low[2], well_color_high[0], well_color_high[1], well_color_high[2]];
    NSInteger count = [blocked_colors numberOfItems];
    [blocked_colors addItemWithObjectValue:s_color];
    [blocked_colors selectItemAtIndex:count];
}
- (IBAction) addToTolerance: (id) sender {
    NSColor *color_value = [blocked_color_well color];
    double rf = 0, gf = 0, bf = 0;
    [color_value getRed:&rf green:&gf blue:&bf alpha:nil];
    unsigned int values[3];
    values[2] = rf*255.99999f;
    values[1] = gf*255.99999f;
    values[0] = bf*255.99999f;
    cv::Vec3b well_color_low;
    well_color_low[0] = values[0];
    well_color_low[1] = values[1];
    well_color_low[2] = values[2];
    NSInteger color_low[] = {[val_colorkey_b_low integerValue], [val_colorkey_g_low integerValue], [val_colorkey_r_low integerValue]};
    NSInteger color_high[] =  {[val_colorkey_b_high integerValue], [val_colorkey_g_high integerValue], [val_colorkey_r_high integerValue]};
    for(int i = 0; i < 3; ++i) {
        if(!(color_low[i] >= 0 && color_low[i] <= 255)) {
            _NSRunAlertPanel(@"Low color must be valid range", @"Range between 0-255", @"Ok", nil, nil);
            return;
        }
        if(!(color_high[i] >= 0 && color_high[i] <= 255)){
            _NSRunAlertPanel(@"High color must be valid range", @"Range between 0-255", @"Ok", nil, nil);
            return;
        }
    }
    cv::Vec3b low_val, high_val;
    low_val[0] = ac::size_type_cast<unsigned char>(well_color_low[0]-color_low[0]);
    low_val[1] = ac::size_type_cast<unsigned char>(well_color_low[1]-color_low[1]);
    low_val[2] = ac::size_type_cast<unsigned char>(well_color_low[2]-color_low[2]);
    high_val[0] = ac::size_type_cast<unsigned char>(well_color_low[0]+color_high[0]);
    high_val[1] = ac::size_type_cast<unsigned char>(well_color_low[1]+color_high[1]);
    high_val[2] = ac::size_type_cast<unsigned char>(well_color_low[2]+color_high[2]);
    ac::Keys keys;
    if([chk_spill state] == NSControlStateValueOn)
        keys.spill = true;
    else
        keys.spill = false;
    keys.low = low_val;
    keys.high = high_val;
    keys.key_type = ac::KeyValueType::KEY_TOLERANCE;
    if(!(low_val[0] <= high_val[0] && low_val[1] <= high_val[1] && low_val[2] <= high_val[2])) {
        _NSRunAlertPanel(@"Values must be a valid range", @"Color values must be a valid range between high >= low and low <= high", @"Ok", nil, nil);
        return;
    }
    green_blocked.push_back(keys);
    NSString *s_color = [NSString stringWithFormat:@"Color BGR Tolerance: (%d, %d, %d) - (%d, %d, %d)", low_val[0],low_val[1],low_val[2], high_val[0], high_val[1], high_val[2]];
    NSInteger count = [blocked_colors numberOfItems];
    [blocked_colors addItemWithObjectValue:s_color];
    [blocked_colors selectItemAtIndex:count];
}

- (IBAction) openBlockedColors: (id) sender {
    [block_colors_window orderFront:self];
}

- (IBAction) setColorsEnabled: (id) sender {
    ac::setBlockedColorKeys(green_blocked);
    _NSRunAlertPanel(@"Set Blocked Colors", @"Blocked Color List Set", @"Ok", nil,nil);
}

- (IBAction) setColorValuesRange: (id) sender {
    if([key_range state] == NSControlStateValueOn) {
        NSColor *color_value = [blocked_color_well color];
        double rf = 0, gf = 0, bf = 0;
        [color_value getRed:&rf green:&gf blue:&bf alpha:nil];
        unsigned int values[3];
        values[2] = rf*255.99999f;
        values[1] = gf*255.99999f;
        values[0] = bf*255.99999f;
        cv::Vec3b well_color_low;
        well_color_low[0] = values[0];
        well_color_low[1] = values[1];
        well_color_low[2] = values[2];
        [val_colorkey_b_low setIntegerValue: well_color_low[0]];
        [val_colorkey_g_low setIntegerValue: well_color_low[1]];
        [val_colorkey_r_low setIntegerValue: well_color_low[2]];
        NSColor *color_value2 = [blocked_color_well_high color];
        [color_value2 getRed:&rf green:&gf blue:&bf alpha:nil];
        values[2] = rf*255.99999f;
        values[1] = gf*255.99999f;
        values[0] = bf*255.99999f;
        cv::Vec3b well_color_high;
        well_color_high[0] = values[0];
        well_color_high[1] = values[1];
        well_color_high[2] = values[2];
        [val_colorkey_b_high setIntegerValue: well_color_high[0]];
        [val_colorkey_g_high setIntegerValue: well_color_high[1]];
        [val_colorkey_r_high setIntegerValue: well_color_high[2]];
    }
}

- (IBAction) setRangeTolerance:(id) sender {
    NSInteger key_state = [key_range state];
    if(key_state == NSControlStateValueOn) {
        [blocked_color_well_high setHidden:NO];
        [val_colorkey_b_high setSelectable:NO];
        [val_colorkey_g_high setSelectable:NO];
        [val_colorkey_r_high setSelectable:NO];
        [val_colorkey_r_low setSelectable:NO];
        [val_colorkey_g_low setSelectable:NO];
        [val_colorkey_b_low setSelectable:NO];
        [val_colorkey_r_high setEditable:NO];
        [val_colorkey_g_high setEditable:NO];
        [val_colorkey_b_high setEditable:NO];
        [val_colorkey_r_low setEditable:NO];
        [val_colorkey_g_low setEditable:NO];
        [val_colorkey_b_low setEditable:NO];
        [self setColorValuesRange:self];
    } else {
        [blocked_color_well_high setHidden:YES];
        [val_colorkey_b_high setSelectable:YES];
        [val_colorkey_g_high setSelectable:YES];
        [val_colorkey_r_high setSelectable:YES];
        [val_colorkey_r_low setSelectable:YES];
        [val_colorkey_g_low setSelectable:YES];
        [val_colorkey_b_low setSelectable:YES];
        [val_colorkey_r_high setEditable:YES];
        [val_colorkey_g_high setEditable:YES];
        [val_colorkey_b_high setEditable:YES];
        [val_colorkey_r_low setEditable:YES];
        [val_colorkey_g_low setEditable:YES];
        [val_colorkey_b_low setEditable:YES];
        [val_colorkey_r_high setStringValue:@"0"];
        [val_colorkey_g_high setStringValue:@"0"];
        [val_colorkey_b_high setStringValue:@"0"];
        [val_colorkey_r_low setStringValue:@"0"];
        [val_colorkey_g_low setStringValue:@"0"];
        [val_colorkey_b_low setStringValue:@"0"];
    }
}

- (IBAction) setCheckBoxBlocked: (id) sender {
    NSInteger state1,state2;
    state1 = [chk_blocked_replace state];
    state2 = [chk_blocked_key state];
    [color_chk setState: state2];
    [chk_replace setState: state1];
    
    if([chk_replace state] == NSControlStateValueOn)
        colorkey_filter = true;
    else
        colorkey_filter = false;
}

- (IBAction) setCheckBoxImage: (id) sender {
    NSInteger state1, state2;
    state1 = [color_chk state];
    state2 = [chk_replace state];
    [chk_blocked_replace setState: state2];
    [chk_blocked_key setState: state1];
    
    if([chk_replace state] == NSControlStateValueOn)
        colorkey_filter = true;
    else
        colorkey_filter = false;
}

- (IBAction) user_Set: (id) sender {
    
    if(directory_path.length()==0) {
        _NSRunAlertPanel(@"Press Select Output Directory", @"Select Output Directory", @"Ok", nil, nil);
        [self user_Clear:self];
        return;
    }

    NSString *s = [user_filter_name stringValue];
    if([s length] == 0) {
        _NSRunAlertPanel(@"User defined requires a valid name", @"Error forgot to set name", @"Ok", nil, nil);
        return;
    }
    NSInteger index = [table_view numberOfRows];
    if(index <= 0) {
        _NSRunAlertPanel(@"Requires at lesat one filter!", @"None Selected", @"Ok", nil, nil);
        return;
    }

    std::string fname;
    std::string uname = [s UTF8String];
    fname = "User_";
    fname += [s UTF8String];
    if(uname.find("User_") != std::string::npos) {
        fname = uname;
    }

    auto pos = std::find(ac::draw_strings.begin(), ac::draw_strings.end(), fname);
    if(pos == ac::draw_strings.end())
        ac::draw_strings.push_back(fname);
    
    int f = -1;
    for(int p = 0; p < ac::draw_strings.size(); ++p) {
        if(ac::draw_strings[p] == fname) {
            f = p;
            break;
        }
    }
    if(f == -1) {
        _NSRunAlertPanel(@"Error", @"Could not find filter error", @"Ok", nil, nil);
        return;
    }
    // check make sure value does not contain itself
    for(int i = 0; i < [custom_array count]; ++i) {
        NSNumber *nval = [custom_array objectAtIndex:i];
        NSNumber *sval = [custom_subfilters objectAtIndex:i];
        NSInteger value1 = [nval integerValue];
        NSInteger value2 = [sval integerValue];
        std::string name1, name2;
        if(value1 >= 0 && value1 < ac::draw_strings.size())
            name1 = ac::draw_strings[value1];
        
        if(value2 >= 0 && value2 < ac::draw_strings.size())
            name2 = ac::draw_strings[value1];
        if(name1 == fname || name2  == fname) {
            _NSRunAlertPanel(@"Error a filter cannot contain itself...", @"Error: Cannot contain itself.", @"Ok", nil, nil);
            return;
        } else {
            if(ac::checkFilter(fname) == false) {
                _NSRunAlertPanel(@"Error a filter cannot contain itself...", @"Error: Cannot contain itself.", @"Ok", nil, nil);
            }
        }
    }
    ac::filter_map[fname] = f;
    user_filter[fname].name = fname;
    user_filter[fname].other_name = fname;
    user_filter[fname].index = ac::filter_map[fname];
    ++index_offset;
    user_filter[fname].sort_num = index_offset;
    NSString *sval = [NSString stringWithUTF8String: fname.c_str()];
    if(uname.find("User_") == std::string::npos) {
        [user_filter_name addItemWithObjectValue:sval];
        [user_filter_name setStringValue:@""];
    }
    [self loadMenuList];
    [table_view reloadData];
    std::ostringstream stream;
    stream << "User set: " << fname << " to: " << fname << "\n";
    NSString *val = [self saveCustomFilter: [NSString stringWithUTF8String: fname.c_str()] withPlugin: nil];
    std::string sname;
    sname = fname + ".acl";
    user_filter[fname].filename = [val UTF8String];
    flushToLog(stream);
}

- (IBAction) saveFilterGroup:(id) sender {
    NSString *text = [group_text stringValue];
    if([text length] > 0) {
        std::string sname = [text UTF8String];
        sname += ".acl";
        if([self userSave:&sname] == YES) {
            _NSRunAlertPanel(@"Saved ACL file", @"File has been saved", @"Ok", nil, nil);
        } else {
            _NSRunAlertPanel(@"Could not save ACL file", @"File has not been saved", @"Ok", nil, nil);
        }
    }
}


- (NSString *) saveCustomFilter: (NSString *)fname_ withPlugin: (NSString *)plug {
    if([custom_array count] == 0 && plug == nil) {
        _NSRunAlertPanel(@"No filters to save!", @"There are no filters in the list...", @"Ok", nil, nil);
        return nil;
    }
    if(directory_path.length() == 0) {
        return nil;
    }
    std::fstream file_n;
    std::ostringstream streamx;
    std::string file_name = [fname_ UTF8String];
    streamx << directory_path << "/" << file_name;
    file_name = streamx.str();
    file_n.open(file_name,std::ios::out);
    if(!file_n.is_open()) {
        _NSRunAlertPanel(@"Could not open file", @"Could not open file do i have rights to this folder?", @"Ok", nil, nil);
        return nil;
    }
    
    if(plug != nil) {
        file_n << "*" << [fname_ UTF8String] << "\n";
    }
    
    if([load_settings integerValue] == 1) {
        NSInteger rgb[] = {[red_slider integerValue], [green_slider integerValue], [blue_slider integerValue]};
        file_n << "=red:" << (int)rgb[0] << "\n";
        file_n << "=green:" << (int)rgb[1] << "\n";
        file_n << "=blue:" << (int)rgb[2] << "\n";
        NSInteger color_m = [color_map indexOfSelectedItem];
        file_n << "=color_map:" << (int)color_m << "\n";
        NSInteger bright = [brightness integerValue];
        file_n << "=brightness:" << bright << "\n";
        NSInteger gam = [gamma integerValue];
        file_n << "=gamma:" << gam << "\n";
        NSInteger sat = [saturation integerValue];
        file_n << "=sat:" << sat << "\n";
        NSInteger chkNegate = [negate_checked integerValue];
        file_n << "=negate:" << chkNegate << "\n";
        NSInteger cord = [corder indexOfSelectedItem];
        file_n << "=color_order:" << cord << "\n";
        NSInteger procM = [procMode indexOfSelectedItem];
        file_n << "=proc:" << (int)procM << "\n";
        NSInteger mvmnt = [proc_change indexOfSelectedItem];
        file_n << "=mvmnt:"<< (int)mvmnt << "\n";
        NSInteger src_amt = [blend_source_amt indexOfSelectedItem];
        file_n << "=bsrc:" << (int)src_amt << "\n";
    }
  
    for(int i = 0; i < [custom_array count]; ++i) {
        NSNumber *nval = [custom_array objectAtIndex:i];
        NSNumber *sval = [custom_subfilters objectAtIndex:i];
        NSInteger value1 = [nval integerValue];
        NSInteger value2 = [sval integerValue];
        std::string value1s, value2s;
        value1s = ac::draw_strings[value1];
        if(value2 != -1)
            value2s = ac::draw_strings[value2];
        else
            value2s = "None";
        file_n << value1s << ":" << value2s << "\n";
    }
    std::ostringstream stream;
    stream << "Saved Custom Filter: " << file_name << "\n";
    flushToLog(stream);
    file_n.close();
    int plugin = 0;
    if(ac::LoadFilterFile([fname_ UTF8String], file_name, plugin)) {
        std::cout << "Filter loaded into Memory...\n";
    }
    return [NSString stringWithUTF8String:file_name.c_str()];
}

- (void) updateDirPath: (std::string *)s {
    auto pos = s->rfind("/");
    if(pos != std::string::npos) {
        directory_path = *s;
        //NSString *def = [defaults objectForKey:@"directory_path"];
        NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
        NSString *st = [NSString stringWithUTF8String: s->c_str()];
        [defaults setObject:st forKey:@"directory_path"];
    }
}

- (void) getNameFilter: (std::string *)str index:(int)value {
    NSString *s = [custom_array objectAtIndex:value];
    NSLog(@"%@\n",s);
}

- (BOOL) userSave: (std::string *)name {
    if(directory_path.length()==0) {
        _NSRunAlertPanel(@"Set Directory path", @"Set the Directory path", @"Ok", nil, nil);
        return NO;
    }
    std::ostringstream stream;
    stream << directory_path << "/" << name->c_str();
    std::string path = stream.str();
    std::string textname = path;
    std::fstream file;
    file.open(textname, std::ios::out);
    if(!file.is_open()) {
        std::cout << "Could not open path: " << textname << "\n";
        _NSRunAlertPanel(@"Error could not save file. Do you have access rights?", @"Cannot save file", @"Ok", nil, nil);
        return NO;
    }
    std::vector<UserArgType> items;
    for(auto i = user_filter.begin(); i != user_filter.end(); ++i) {
        if(i->second.index != -1) {
            std::cout << "loaded menu item: " << i->first << ":" << i->second.sort_num << "\n";
            items.push_back(UserArgType(i->second.sort_num, i->first.c_str(), i->second.name));
        }
    }
    std::sort(items.begin(), items.end());
    for(auto i = items.begin(); i != items.end(); ++i) {
        std::string other = "UserDefined";
        if(i->other.length() > 0)
            other = i->other;
        file << i->name << ":" << i->index << ":" << other << "\n";
    }
    file.close();
    std::ostringstream xstream;
    xstream << "User saved IDs to: " << path << "\n";
    flushToLog(xstream);
    return YES;
}

- (IBAction) user_Save: (id) sender {
    [group_window orderFront:self];
}

- (IBAction) user_Clear: (id) sender {
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setCanChooseFiles:NO];
    [panel setCanChooseDirectories:YES];
    [panel setCanCreateDirectories:YES];
    if([panel runModal]) {
        std::string s = [[[panel URL] path] UTF8String];
        [self updateDirPath:&s];
    }
}

- (void) loadFileData: (const char *)path off: (int) index_offset {
    
    if(directory_path.length() == 0)
        return;
    
    std::vector<std::string> comp;
    token::tokenize(std::string(path), std::string(":"), comp);
    if(comp.size()==0) return;
    std::string filter_name = comp[0];
    int index_value = atoi(comp[1].c_str());
    std::ostringstream fpath;
    std::string fname_path = path;
    fname_path = fname_path.substr(0, fname_path.find(":"));
    fpath << directory_path << "/" << fname_path;
    
    int plugin = 0;
    
    if(ac::LoadFilterFile(fname_path, fpath.str(), plugin)) {
        if(plugin == 1) {
            std::string name_val = fname_path;
            std::ostringstream stream;
            stream << directory_path << "/" << name_val << ".acf";
            std::string name_path = stream.str();
            void *library = dlopen(name_path.c_str(), RTLD_LAZY);
            
            if(library == NULL) {
                std::cerr << "Error could not open: " << name_path << "\n";
                _NSRunAlertPanel(@"Error Occoured Loading Plugin", @"Exiting...", @"Ok", nil, nil);
                return;
            }
            void *addr;
            // load the plugin function to process pixels
            addr = dlsym(library, "filter");
            pixel pixf;
            pixf = reinterpret_cast<pixel>(addr);
            const char *error;
            error = dlerror();
            if(error) {
                std::cerr << "Could not load pixel: " << error << "\n";
                _NSRunAlertPanel(@"Could not load Plugin", @"Error loading plugin", @"Ok", nil,nil);
                return;
            }
            
            user_filter[name_val].func = pixf;
            user_filter[name_val].library = library;
        }
        NSString *sval = [NSString stringWithUTF8String: filter_name.c_str()];
        [user_filter_name addItemWithObjectValue:sval];
        [table_view reloadData];
    }
    if(index_value == -1) {
        ac::filter_map[comp[2]] = ac::filter_map[comp[0]];
    }
}

- (IBAction) user_Load: (id) sender {
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setCanChooseDirectories:NO];
    [panel setAllowsMultipleSelection:NO];
    [panel setAllowedFileTypes:[NSArray arrayWithObjects:@"acl", nil]];
    if([panel runModal]) {
        std::string fname = [[[panel URL] path] UTF8String];
        std::fstream file;
        file.open(fname, std::ios::in);
        if(!file.is_open()) {
            _NSRunAlertPanel(@"Could not open file..", [NSString stringWithFormat:@"Could not locate file: %s", fname.c_str()], @"Ok", nil, nil);
            return;
        }
       ac::clearFilterFiles();
        
        int index_offset = 0;
        while(!file.eof()) {
            std::string file_data;
            std::getline(file, file_data);
            if(file) {
                [self loadFileData: file_data.c_str() off:index_offset];
                ++index_offset;
            }
        }
        [self loadMenuList];
        [table_view reloadData];
        std::ostringstream stream;
        stream << "Loaded Custom Filter From File: " << fname << "\n";
        flushToLog(stream);
    }
}

- (void) loadMenuList {
    if(user_menu != nil)
        [user_menu release];

    user_menu = [[NSMenu alloc] init];
    std::vector<UserArgType> items;
    for(auto i = user_filter.begin(); i != user_filter.end(); ++i) {
        if(i->second.index != -1) {
            std::cout << "loaded menu item: " << i->first << ":" << i->second.sort_num << "\n";
            items.push_back(UserArgType(i->second.sort_num, i->first.c_str()));
        }
    }
    std::sort(items.begin(), items.end());
    for(auto i = items.begin(); i != items.end(); ++i) {
        [user_menu addItemWithTitle: [NSString stringWithUTF8String:i->name.c_str()] action:nil keyEquivalent:@""];
    }
    [user_menu addItemWithTitle: [NSString stringWithUTF8String:"No Filter"] action:nil keyEquivalent:@""];
    NSInteger index_value = [categories_custom indexOfSelectedItem];
    NSInteger index_value_ex = [categories indexOfSelectedItem];
    if((index_value == 14 || index_value_ex == 14) && [user_menu numberOfItems] > 0) {
        [current_filter_custom setMenu: user_menu];
        [current_filter setMenu: user_menu];
    }
}

- (IBAction) jumpToCustom: (id) sender {
    [window1 orderFront:self];
    [categories selectItemAtIndex:0];
    [self menuSelected:self];
    [current_filter selectItemAtIndex:ac::draw_max-4];
    [self changeFilter:self];
    std::ostringstream stream;
    stream << "Jumped to Custom Filter Mode\n";
    flushToLog(stream);
}



- (IBAction) custom_Save: (id) sender {
    if([custom_array count] == 0) {
        _NSRunAlertPanel(@"No filters to save!", @"There are no filters in the list...", @"Ok", nil, nil);
        return;
    }
    std::string default_name = "User_default";
    NSSavePanel *panel = [NSSavePanel savePanel];
    [panel setCanCreateDirectories:YES];
    [panel setAllowedFileTypes: [NSArray arrayWithObject:@"filter"]];
    if([panel runModal]) {
        NSString *fileName = [[panel URL] path];
        std::fstream file_n;
        file_n.open([fileName UTF8String],std::ios::out);
        if(!file_n.is_open()) {
            _NSRunAlertPanel(@"Could not open file", @"Could not open file do i have rights to this folder?", @"Ok", nil, nil);
            return;
        }
        if([load_settings integerValue] == 1) {
            NSInteger rgb[] = {[red_slider integerValue], [green_slider integerValue], [blue_slider integerValue]};
            file_n << "=red:" << (int)rgb[0] << "\n";
            file_n << "=green:" << (int)rgb[1] << "\n";
            file_n << "=blue:" << (int)rgb[2] << "\n";
            NSInteger color_m = [color_map indexOfSelectedItem];
            file_n << "=color_map:" << (int)color_m << "\n";
            NSInteger bright = [brightness integerValue];
            file_n << "=brightness:" << bright << "\n";
            NSInteger gam = [gamma integerValue];
            file_n << "=gamma:" << gam << "\n";
            NSInteger sat = [saturation integerValue];
            file_n << "=sat:" << sat << "\n";
            NSInteger chkNegate = [negate_checked integerValue];
            file_n << "=negate:" << chkNegate << "\n";
            NSInteger cord = [corder indexOfSelectedItem];
            file_n << "=color_order:" << cord << "\n";
            NSInteger procM = [procMode indexOfSelectedItem];
            file_n << "=proc:" << (int)procM << "\n";
            NSInteger mvmnt = [proc_change indexOfSelectedItem];
            file_n << "=mvmnt:"<< (int)mvmnt << "\n";
            NSInteger src_amt = [blend_source_amt indexOfSelectedItem];
            file_n << "=bsrc:" << (int)src_amt << "\n";
        }
        for(int i = 0; i < [custom_array count]; ++i) {
            NSNumber *nval = [custom_array objectAtIndex:i];
            NSNumber *sval = [custom_subfilters objectAtIndex:i];
            NSInteger value1 = [nval integerValue];
            NSInteger value2 = [sval integerValue];
            std::string value1s, value2s;
            value1s = ac::draw_strings[value1];
            if(value2 != -1)
                value2s = ac::draw_strings[value2];
            else
                value2s = "None";
            file_n << value1s << ":" << value2s << "\n";
        }
        std::ostringstream stream;
        stream << "Saved Custom Filter: " << [fileName UTF8String] << "\n";
        flushToLog(stream);
        file_n.close();
        int plugin = 0;
        if(ac::LoadFilterFile(default_name, [fileName UTF8String], plugin)) {
            std::cout << "Filter loaded into Memory...\n";
        }
    }
}

- (void) setCustomValue: (NSString *)sid value: (NSString *)value {
    NSInteger num = [value integerValue];
    if([sid isEqualToString:@"=red"]) {
        [red_slider setIntegerValue:num];
        return;
    }
    if([sid isEqualToString:@"=green"]) {
        [green_slider setIntegerValue:num];
        return;
    }
    if([sid isEqualToString:@"=blue"]) {
        [blue_slider setIntegerValue:num];
        return;
    }
    if([sid isEqualToString:@"=color_map"]) {
        [color_map selectItemAtIndex:num];
        return;
    }
    if([sid isEqualToString:@"=brightness"]) {
        [brightness setIntegerValue:num];
        return;
    }
    if([sid isEqualToString:@"=gamma"]) {
        [gamma setIntegerValue:num];
        return;
    }
    if([sid isEqualToString:@"=sat"]) {
        [saturation setIntegerValue:num];
        return;
    }
    if([sid isEqualToString:@"=negate"]) {
        [negate_checked setIntegerValue:num];
        return;
    }
    if([sid isEqualToString:@"=color_order"]) {
        [corder selectItemAtIndex:num];
        return;
    }
    if([sid isEqualToString:@"=proc"]) {
        [procMode selectItemAtIndex:num];
        return;
    }
    if([sid isEqualToString:@"=mvmnt"]) {
        [proc_change selectItemAtIndex:num];
        return;
    }
    if([sid isEqualToString:@"=bsrc"]) {
        [blend_source_amt selectItemAtIndex:num];
        return;
    }
}

- (IBAction) custom_Load: (id) sender {
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setCanChooseFiles:YES];
    [panel setCanChooseDirectories: NO];
    [panel setAllowedFileTypes: [NSArray arrayWithObject:@"filter"]];
    if([panel runModal]) {
        int plugin = 0;
        if(ac::LoadFilterFile("User_default", [[[panel URL] path] UTF8String], plugin)) {
              auto type = user_filter["User_default"].custom_filter.options;
                for(auto v = type.begin(); v != type.end(); ++v) {
                    [self setCustomValue:[NSString stringWithUTF8String:v->first.c_str()] value:[NSString stringWithUTF8String:v->second.c_str()]];
                }
                [self setRGB_Values:self];
                [self changeOrder:self];
                [self setNegative:self];
                [self setColorMap:self];
                [self updatePref:NO];
                [table_view reloadData];
                std::ostringstream stream_;
                stream_ << "Loaded Custom Filter: " << [[[panel URL] path] UTF8String] << "\n";
                flushToLog(stream_);
        }
    }
}

- (IBAction) copyCustomToString: (id) sender {
    std::ostringstream stream;
    NSInteger count = [custom_array count];
    if(count == 0) return;
    for(int i = 0; i < count; ++i) {
        NSNumber *num1 = [custom_array objectAtIndex: i];
        NSNumber *num2 = [custom_subfilters objectAtIndex: i];
        NSInteger val1 = [num1 integerValue];
        NSInteger val2 = [num2 integerValue];
        if(val2 == -1) {
            stream << (int)val1;
        } else {
            stream << (int)val1 << ":" << (int)val2;
        }
        
        if(i == count-1)
            continue;
        else
            stream << ",";
        
    }
    if(stream.str().length() == 0)
        return;
    NSString *cmd_str = [NSString stringWithUTF8String:stream.str().c_str()];
    [[NSPasteboard generalPasteboard] clearContents];
    [[NSPasteboard generalPasteboard] setString:cmd_str forType:NSPasteboardTypeString];
    _NSRunAlertPanel(@"Copied String for Acid Cam Cli Application to Clipboard", cmd_str, @"Ok", nil, nil);
    std::ostringstream stream1;
    stream1 << "acidcam-cli filter generated code is: " << stream.str().c_str() << "\n";
    flushToLog(stream1);
}

- (IBAction) copyFilterUsed:(id) sender {
    std::ostringstream stream;
    NSInteger count = [custom_array count];
    if(count == 0) return;
    stream << "ACF: ";
    for(int i = 0; i < count; ++i) {
        NSNumber *num1 = [custom_array objectAtIndex: i];
        NSNumber *num2 = [custom_subfilters objectAtIndex: i];
        NSInteger val1 = [num1 integerValue];
        NSInteger val2 = [num2 integerValue];
        if(val2 == -1) {
            stream << ac::draw_strings[val1];
        } else {
            stream << ac::draw_strings[val1] << ":" << ac::draw_strings[val2];
        }
        
        if(i == count-1)
            continue;
        else
            stream << ", ";
        
    }
    if(stream.str().length() == 0)
        return;
    NSString *cmd_str = [NSString stringWithUTF8String:stream.str().c_str()];
    [[NSPasteboard generalPasteboard] clearContents];
    [[NSPasteboard generalPasteboard] setString:cmd_str forType:NSPasteboardTypeString];
    _NSRunAlertPanel(@"Copied String Text for ACF (Acid Cam Custom Filter) to Clipboard: ", cmd_str, @"Ok", nil, nil);
    std::ostringstream stream1;
    stream1 << "ACF is: " << stream.str().c_str() << "\n";
    flushToLog(stream1);
}


- (IBAction) insertFilter: (id) sender {
    NSInteger index = [current_filter_custom indexOfSelectedItem];
    if(index >= 0) {
        NSMenuItem *item = [current_filter_custom itemAtIndex: index];
        NSString *filter_str = [item title];
        NSInteger set_index = [table_view selectedRow];
        if(set_index >= 0) {
            int object1_value = ac::filter_map[[filter_str UTF8String ]];
            NSNumber *obj1 = [NSNumber numberWithInteger:object1_value];
            NSNumber *obj2 = [NSNumber numberWithInteger:-1];
            NSNumber *obj3 = [NSNumber numberWithInteger: 1];
            [custom_array setObject:obj1 atIndexedSubscript:set_index];
            [custom_subfilters setObject:obj2 atIndexedSubscript:set_index];
            [filter_on setObject:obj3 atIndexedSubscript:set_index];
            std::ostringstream stream;
            stream << "Filter " << [filter_str UTF8String] << " inserted\n";
            flushToLog(stream);
            [table_view reloadData];
        } else {
            _NSRunAlertPanel(@"You must select filter to replace", @"Select a Filter first", @"Ok", nil, nil);
        }
    }
}

- (IBAction) insertFilter_Custom: (id) sender {
    NSInteger index = [find_table selectedRow];
    NSInteger pos = [table_view selectedRow];
    if(pos < 0 || index < 0) {
        _NSRunAlertPanel(@"You need to select a filter", @"Select a filter in Search and select a filter in Custom Window", @"Ok", nil, nil);
        return;
    }
    if(index >= 0 && index < [search_results count]) {
        NSNumber *num = [search_results objectAtIndex:index];
        int val = static_cast<int>([num integerValue]);
        std::string sub_chk1= ac::draw_strings[val];
        if(val != -1) {
            int filter_pos = ac::filter_map[sub_chk1];
            NSNumber *fval = [NSNumber numberWithInt:filter_pos];
            [custom_array setObject:fval atIndexedSubscript: pos];
            NSNumber *obj2 = [NSNumber numberWithInteger:-1];
            NSNumber *obj3 = [NSNumber numberWithInteger: 1];
            [custom_subfilters setObject:obj2 atIndexedSubscript:pos];
            [filter_on setObject:obj3 atIndexedSubscript:pos];
            std::ostringstream stream;
            stream << "Filter " << sub_chk1 << " inserted\n";
            flushToLog(stream);
            [table_view reloadData];
        }
        [table_view reloadData];
    }
}

- (IBAction) scaleToggle: (id) sender {
    if([up4k state] == NSControlStateValueOff) {
        [video_width setEnabled:NO];
        [video_height setEnabled:NO];
        [chk_stretch setEnabled:NO];
        [chk_stretch setState: NSControlStateValueOff];
    } else {
        [video_width setEnabled:YES];
        [video_height setEnabled:YES];
        [chk_stretch setEnabled:YES];
    }
}

- (IBAction) saveLogToFile: (id) sender {
    NSTextView *sv = t_view;
    NSString *value = [[sv textStorage] string];
    NSSavePanel *panel = [NSSavePanel savePanel];
    [panel setCanCreateDirectories:NO];
    [panel setCanSelectHiddenExtension:NO];
    [panel setAllowedFileTypes:[NSArray arrayWithObject:@"log"]];
    if([panel runModal]) {
        std::string filename;
        NSString *fname = [[panel URL] path];
        filename = [fname UTF8String];
        std::fstream file;
        file.open(filename, std::ios::out);
        time_t t = time(0);
        struct tm *m;
        m = localtime(&t);
        std::ostringstream time_stream;
        time_stream << (m->tm_year + 1900) << "." << std::setw(2) << std::setfill('0') << (m->tm_mon + 1) << "." << std::setw(2) << std::setfill('0') << m->tm_mday << " At " << std::setw(2) << std::setfill('0') << m->tm_hour << ":" << std::setw(2) << std::setfill('0') << m->tm_min << ":" << std::setw(2) << std::setfill('0') << m->tm_sec <<  "\n";
        file << "Log Output Generated On: " << time_stream.str() << "\n";
        file << [value UTF8String];
        file.close();
        std::ostringstream stream;
        stream << "Log File Outputted to: " << filename << "\n";
        flushToLog(stream);
    }
}

- (IBAction) toggleFilterState: (id) sender {
    NSInteger set_index = [table_view selectedRow];
    if(set_index >= 0) {
        NSNumber *num = [filter_on objectAtIndex:set_index];
        NSNumber *new_val;
        if([num integerValue] == 1)
            new_val = [NSNumber numberWithInt: 0];
        else
            new_val = [NSNumber numberWithInt: 1];
        [filter_on setObject:new_val atIndexedSubscript:set_index];
        [table_view reloadData];
    }
}

- (IBAction) changePrefix: (id) sender {
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setCanChooseFiles:NO];
    [panel setCanChooseDirectories:YES];
    if([panel runModal]) {
        NSString *value = [[panel URL] path];
        NSString *new_value = [NSString stringWithFormat:@"Save Prefix: %@", value];
        if(custom_path_prefix == nil) {
            custom_path_prefix = [[NSString alloc] initWithFormat:@"%@", value];
        } else {
            [custom_path_prefix release];
            custom_path_prefix = [[NSString alloc] initWithFormat:@"%@", value];
        }
        [label_path setStringValue:new_value];
    }
}

- (NSInteger) checkForNewVersion: (BOOL) showMessage useVal:(int)value {
    NSControlStateValue check = [check_update state];
    if(check == NSControlStateValueOff && value == 0)
        return NO;
    __block NSInteger return_val;
#ifdef ENABLE_VERSION_UPDATE
    NSString *download_url = @"https://github.com/lostjared/Acid.Cam.v2.OSX/blob/master/README.md";
    NSURL *URL = [NSURL URLWithString:download_url];
    NSURLRequest *request = [NSURLRequest requestWithURL:URL];
    NSURLSession *session = [NSURLSession sharedSession];
    NSURLSessionDataTask *task = [session dataTaskWithRequest:request completionHandler: ^(NSData *data, NSURLResponse *response, NSError *error) {
        NSString *value = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
        NSString *ver = [NSString stringWithFormat:@"[VERSION: %s]", ac::getVersion().c_str(), nil];
        if([value length] == 0) {
            dispatch_sync(dispatch_get_main_queue(), ^{
                if(showMessage == YES) {
                    _NSRunAlertPanel(@"No Internet Connection", @"Could Not Connect to the Internet", @"Ok", nil, nil);
                }
            });
            return_val = 1;
        }
        else if([value containsString:ver] == NO) {
            std::cout << "Version not up to date...\n";
            dispatch_sync(dispatch_get_main_queue(), ^{
                NSInteger index = _NSRunAlertPanel(@"New Version...", @"New version available, Would you like to open the download page?", @"No", @"Yes", nil);
                if(index != 1000) {
                    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"https://github.com/lostjared/Acid.Cam.v2.OSX/releases"]];
                }
            });
            return_val = 2;
        } else {
            dispatch_sync(dispatch_get_main_queue(), ^{
                if(showMessage == YES) {
                    _NSRunAlertPanel(@"Acid Cam is Up to Date", @"No update available", @"Ok", nil, nil);
                }
            });
            return_val = 3;
        }
        [value release];
    }];
    [task resume];
    return return_val;
#endif
}

- (IBAction) checkForFFMPEG: (id) sender {
    
    if([ffmpeg_support integerValue] == 0) {
        _NSRunAlertPanel(@"First Enable FFMPEG Support", @"FFMPEG Support is not Enabled.", @"Ok", nil, nil);
        [copy_audio setState:NSControlStateValueOff];
        return;
    }
    std::fstream file;
    file.open(ffmpeg_string_path, std::ios::in);
    if(!file.is_open() || !file.good()) {
        _NSRunAlertPanel(@"FFMPEG must be installed, check README", @"FFMPEG should be installed with Homebrew package manager. It is free you can find it here: https://brew.sh/", @"Ok", nil, nil);
        [copy_audio setState:NSControlStateValueOff];
        return;
    }
}

- (IBAction) menuMoveNormal: (id) sender {
    program_speed = 0;
    [speed_normal setState:NSControlStateValueOn];
    [speed_fast setState:NSControlStateValueOff];
    [speed_vfast setState:NSControlStateValueOff];
    p_s = 1;
}

- (IBAction) menuMoveFast: (id) sender {
    program_speed = 1;
    [speed_normal setState:NSControlStateValueOff];
    [speed_fast setState:NSControlStateValueOn];
    [speed_vfast setState:NSControlStateValueOff];
    p_s = 2;
}

- (IBAction) menuMoveVeryFast: (id) sender {
    program_speed = 2;
    [speed_normal setState:NSControlStateValueOff];
    [speed_fast setState:NSControlStateValueOff];
    [speed_vfast setState:NSControlStateValueOn];
    p_s = 4;
}

- (IBAction) writeSettings: (id) sender {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSString *m_path = [NSString stringWithUTF8String: ffmpeg_string_path.c_str()];
    NSInteger chk_on = [ffmpeg_support integerValue];
    [defaults setObject:m_path forKey:@"ffmpeg"];
    [defaults setInteger:chk_on forKey:@"ffmpeg_on"];
    [defaults synchronize];
}

- (IBAction) readSettings: (id) sender {
}

- (IBAction) toggleOutput: (id) sender {
    if([output_video state] == NSControlStateValueOn) {
        [output_video setState: NSControlStateValueOff];
        outputVideo = NO;
        cv::destroyWindow("Acid Cam v2");
    } else {
        [output_video setState: NSControlStateValueOn];
        outputVideo = YES;
        cv::namedWindow("Acid Cam v2", cv::WINDOW_NORMAL | cv::WINDOW_GUI_EXPANDED);
        if(ac::orig_frame.rows <= 1080)
            cv::resizeWindow("Acid Cam v2", ac::orig_frame.cols, ac::orig_frame.rows);
    }
}

- (IBAction) checkForUpdate: (id) sender {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSControlStateValue value = [check_update state];
    if(value == NSControlStateValueOn) {
        [check_update setState: NSControlStateValueOff];
        [defaults setInteger:0 forKey:@"update_on"];
    } else {
        [check_update setState: NSControlStateValueOn];
        [defaults setInteger:1 forKey:@"update_on"];
    }
}

- (IBAction) setVideoKey: (id) sender {
    NSOpenPanel *panel;
    panel = [NSOpenPanel openPanel];
    [panel setCanChooseFiles:YES];
    [panel setCanChooseDirectories:NO];
    [panel setAllowedFileTypes: [NSArray arrayWithObjects :@"mov", @"avi", @"mp4", @"mkv",@"m4v",@"webm",@"mts",@"m2ts", nil]];
    if([panel runModal]) {
        NSString *val = [[panel URL] path];
        ac::v_cap = cv::VideoCapture([val UTF8String]);
        if(!ac::v_cap.isOpened()) {
            std::cerr << "Error opening file...\n";
            _NSRunAlertPanel(@"Error Could Open File", @"Error could not open file.", @"Ok", nil, nil);
            return;
        }
        std::string text_val = [val UTF8String];
        text_val = text_val.substr(text_val.rfind("/")+1, text_val.length());
        NSString *t = [NSString stringWithUTF8String:text_val.c_str()];
        [video_value setStringValue:t];
        colorkey_replace = true;
    }
}

- (IBAction) clrVideoKey: (id) sender {
    if(ac::v_cap.isOpened())
        ac::v_cap.release();
    [video_value setStringValue:@""];
    colorkey_replace = false;
}

- (IBAction) videoRestart:(id) sender {
    if(ac::v_cap.isOpened())
        ac::v_cap.set(cv::CAP_PROP_POS_FRAMES,0);
}

- (IBAction) setRandomFilter:(id)sender {
    [categories selectItemAtIndex:0];
    [self menuSelected:self];
    int rand_solo = rand()%ac::solo_filter.size();
    std::string filter_value = ac::solo_filter[rand_solo];
    [current_filter selectItemAtIndex:ac::filter_map[filter_value]];
    [self changeFilter:self];
}

- (IBAction) selectRandomFilter:(id) sender {
    [categories_custom selectItemAtIndex:0];
    [self customMenuSelected:self];
    int value = rand()%ac::solo_filter.size();
    std::string filter_value = ac::solo_filter[value];
    [current_filter_custom selectItemAtIndex:ac::filter_map[filter_value]];
}

// from stackoverflow
- (void)getGraphicsInfo:(std::string *)s 
{
    CFMutableDictionaryRef matchDict = IOServiceMatching("IOPCIDevice");
    io_iterator_t iterator;
    
    if (IOServiceGetMatchingServices(kIOMasterPortDefault,matchDict,
                                     &iterator) == kIOReturnSuccess)
    {
        io_registry_entry_t regEntry;
        
        while ((regEntry = IOIteratorNext(iterator))) {
            CFMutableDictionaryRef serviceDictionary;
            if (IORegistryEntryCreateCFProperties(regEntry,
                                                  &serviceDictionary,
                                                  kCFAllocatorDefault,
                                                  kNilOptions) != kIOReturnSuccess)
            {
                IOObjectRelease(regEntry);
                continue;
            }
            const void *GPUModel = CFDictionaryGetValue(serviceDictionary, @"model");
            
            if (GPUModel != nil) {
                if (CFGetTypeID(GPUModel) == CFDataGetTypeID()) {
                    NSString *modelName = [[NSString alloc] initWithData:
                                           (NSData *)GPUModel encoding:NSASCIIStringEncoding];
                    *s = [modelName UTF8String];
                    [modelName release];
                }
            }
            CFRelease(serviceDictionary);
            IOObjectRelease(regEntry);
        }
        IOObjectRelease(iterator);
    }
}

- (IBAction) saveKeys: (id) sender {
    NSSavePanel *panel = [NSSavePanel savePanel];
    [panel setShowsHiddenFiles:YES];
    [panel setAllowedFileTypes: [NSArray arrayWithObjects: @"ckey", nil]];
    if([panel runModal]) {
        NSString *s = [[panel URL] path];
        std::fstream file;
        file.open([s UTF8String], std::ios::out);
        if(!file.is_open()) {
            return;
        }
        std::vector<ac::Keys> blocked_color_keys = green_blocked;
        for(NSInteger i = 0; i < blocked_color_keys.size(); ++i) {
            file << ((blocked_color_keys[i].key_type == ac::KeyValueType::KEY_RANGE) ? 0 : 1) << " = " << int(blocked_color_keys[i].low[0]) << ":" << int(blocked_color_keys[i].low[1]) << ":" << int(blocked_color_keys[i].low[2]) << " - " << int(blocked_color_keys[i].high[0]) << ":" << int(blocked_color_keys[i].high[1]) << ":" << int(blocked_color_keys[i].high[1]) << "\n";
        }
        file.close();
    }
}
- (IBAction) loadKeys: (id) sender {
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setAllowedFileTypes: [NSArray arrayWithObjects: @"ckey", nil]];
    if([panel runModal]) {
        NSString *s = [[panel URL] path];
        std::fstream file;
        file.open([s UTF8String], std::ios::in);
        std::string value;
        while(!file.eof()) {
            std::getline(file, value);
            if(value.length() > 0) {
                std::istringstream in_stream(value);
                int low[3], high[3];
                int key = 0;
                char ch = 0;
                in_stream >> key;
                in_stream >> ch >> low[0] >> ch >> low[1] >> ch >> low[2] >> ch >> high[0] >> ch >> high[1] >> ch >> high[2];
                cv::Vec3b well_color_low(static_cast<unsigned char>(low[0]), static_cast<unsigned char>(low[1]), static_cast<unsigned char>(low[2]));
                cv::Vec3b well_color_high(static_cast<unsigned char>(high[0]), static_cast<unsigned char>(high[1]), static_cast<unsigned char>(high[2]));
                ac::Keys key_value;
                key_value.key_type = ((key == 0) ? ac::KeyValueType::KEY_RANGE : ac::KeyValueType::KEY_TOLERANCE);
                key_value.low = well_color_low;
                key_value.high = well_color_high;
                if(key_value.key_type == ac::KeyValueType::KEY_RANGE) {
                    green_blocked.push_back(key_value);
                    NSString *s_color = [NSString stringWithFormat:@"Color BGR Range: (%d, %d, %d) - (%d, %d, %d)", well_color_low[0], well_color_low[1], well_color_low[2], well_color_high[0], well_color_high[1], well_color_high[2]];
                    NSInteger count = [blocked_colors numberOfItems];
                    [blocked_colors addItemWithObjectValue:s_color];
                    [blocked_colors selectItemAtIndex:count];
                } else {
                    cv::Vec3b low_val = well_color_low;
                    cv::Vec3b high_val = well_color_high;
                    green_blocked.push_back(key_value);
                    NSString *s_color = [NSString stringWithFormat:@"Color BGR Tolerance: (%d, %d, %d) - (%d, %d, %d)", low_val[0],low_val[1],low_val[2], high_val[0], high_val[1], high_val[2]];
                    NSInteger count = [blocked_colors numberOfItems];
                    [blocked_colors addItemWithObjectValue:s_color];
                    [blocked_colors selectItemAtIndex:count];
                }
            }
        }
        file.close();
    }
}

- (IBAction) randomUserDefined: (id) sender {
    [categories_custom selectItemAtIndex: 14];
    [self customMenuSelected:self];
    NSInteger count = [current_filter_custom numberOfItems];
    if(count > 1) {
        NSInteger rand_index = rand()%(count-1);
        [current_filter_custom selectItemAtIndex:rand_index];
        [self selectedCustomFilter:self];
    }
}

- (IBAction) nextFilter: (id) sender {
    NSInteger index = [current_filter indexOfSelectedItem];
    NSInteger count = [current_filter numberOfItems];
    if(index+1 < count) {
        index ++;
        [current_filter selectItemAtIndex:index];
        [self changeFilter:self];
    }
    
     std::cout << "private List<FilterItem> items, sorted_items;\n";
     std::cout << "for(int i = 0; i < " << ac::solo_filter.size() << "; ++i) {\n";
     for(int i = 0; i < ac::solo_filter.size(); ++i) {
     //std::cout << "\titems[" << i << "] = new FilterItem(\"" << ac::solo_filter[i] << "\", " << ac::filter_map[ac::solo_filter[i]] << ");\n";
     std::cout << "item.add(new FilterItem(\"" << ac::solo_filter[i] << "\", " << ac::filter_map[ac::solo_filter[i]] << "));\n";
     }
     std::sort(ac::solo_filter.begin(), ac::solo_filter.end());
     for(int i = 0; i < ac::solo_filter.size(); ++i) {
     std::cout << "sorted_items.add(new FilterItem(\"" << ac::solo_filter[i] << "\", " << ac::filter_map[ac::solo_filter[i]] << "));\n";
     //std::cout << "sorted_items[" << i << "] = new FilterItem(\"" << items[0].name << "\", " << items[i].value << ");\n";
     }
     std::cout << "\n}\n";
     
}
- (IBAction) prevFilter: (id) sender {
    NSInteger index = [current_filter indexOfSelectedItem];
    if(index > 0) {
        --index;
        [current_filter selectItemAtIndex:index];
        [self changeFilter:self];
    }
}

- (IBAction) setAsPlayList:(id) sender {
}

- (IBAction) pollJoystick: (id) sender {
}

- (IBAction) initControllers:(id)sender {
    
}

- (IBAction) releaseControllers: (id) sender {

}

- (IBAction) toggleFullScreenGL: (id) sender {
    syphon_window.styleMask |= NSWindowStyleMaskResizable;
    [syphon_window toggleFullScreen:self];
}

- (NSTextField *)getMemoryText: (id) sender {
    return memory_text;
}

- (IBAction) filterCollectionComboChanged:(id) sender {
    NSString *text = [user_filter_name stringValue];
    if ([text length] > 0) {
        auto pos = user_filter.find([text UTF8String]);
        if(pos == user_filter.end())
            return;
        if([custom_array count] > 0) {
            [custom_array removeAllObjects];
            [custom_subfilters removeAllObjects];
            [filter_on removeAllObjects];
        }
        for(int i = 0; i < pos->second.custom_filter.name.size(); ++i) {
            std::string &n = pos->second.custom_filter.name[i];
            std::string &s = pos->second.custom_filter.subname[i];
            int pos_n = ac::filter_map[n];
            int pos_s = ac::filter_map[s];
            [custom_array addObject: [NSNumber numberWithInt:pos_n]];
            [custom_subfilters addObject: [NSNumber numberWithInt:pos_s]];
            [filter_on addObject: [NSNumber numberWithInt:1]];
        }
        [table_view reloadData];
    }
}

- (IBAction) setColorRange:(id)sender {
    cv::Vec3b low, high;
    NSColor *color_value = [color_start color];
    double rf = 0, gf = 0, bf = 0;
    [color_value getRed:&rf green:&gf blue:&bf alpha:nil];
    unsigned int values[3];
    values[2] = rf*255.99999f;
    values[1] = gf*255.99999f;
    values[0] = bf*255.99999f;
    low[0] = values[0];
    low[1] = values[1];
    low[2] = values[2];
    color_value = [color_stop color];
    rf = 0;
    gf = 0;
    bf = 0;
    [color_value getRed:&rf green:&gf blue:&bf alpha:nil];
    values[2] = rf*255.99999f;
    values[1] = gf*255.99999f;
    values[0] = bf*255.99999f;
    high[0] = values[0];
    high[1] = values[1];
    high[2] = values[2];
    ac::setColorRangeLowToHigh(low, high);
    _NSRunAlertPanel(@"Color Range Set", @"Range Set, please add filter ApplyColorRange to a Custom Filter where you want to use it!", @"Ok", nil, nil);
    ac::setColorRangeEnabled(true);
}

- (IBAction) disableColorRange: (id) sender {
    ac::setColorRangeEnabled(false);
}

- (IBAction) showColorRange: (id)sender {
    [color_range_ orderFront:self];
}

- (IBAction) setSlitScan:(id) sender {
    NSInteger w = [slit_width integerValue];
    NSInteger h = [slit_height integerValue];
    NSInteger f = [slit_frames integerValue];
    NSInteger r = [slit_repeat integerValue];
    NSInteger d = [slit_delay integerValue];
    NSInteger o = [slit_on integerValue];
    
    if(w >= 320 && h >= 240 && f >= 8 && r >= 1 && d >= 0 && o >= 0) {
        ac::slitScanSet(static_cast<int>(f), static_cast<int>(w), static_cast<int>(h), static_cast<int>(r), static_cast<int>(d), static_cast<int>(o));
        _NSRunAlertPanel(@"Value set", @"Value Set", @"Ok", nil, nil);
    } else {
        _NSRunAlertPanel(@"Invalid data value not set", @"Value not Set", @"Ok", nil, nil);
    }
}

- (IBAction) showSlitScan:(id) sender {
    [slit_win orderFront:self];
}

- (IBAction) setBlendPercent: (id) sender {
    int per = static_cast<int>([blend_percent_slider integerValue]);
    ac::setBlendPercent(per);
}

- (IBAction) updateBlendPercent: (id) sender {
    NSInteger pos = [blend_percent_slider integerValue];
    NSString *s = [NSString stringWithFormat:@"%d%%", (int)pos];
    [blend_percent setStringValue:s];
}

- (IBAction) viewBlendPercentage: (id) sender {
    [blend_percent_window orderFront:self];
}

- (IBAction) addVideoFiles:(id)sender {
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setAllowsMultipleSelection:YES];
    [panel setAllowedFileTypes:[NSArray arrayWithObjects:@"mov", @"mp4", @"avi", @"mkv", nil]];
    if([panel runModal]) {
        for(int i = 0; i < [[panel URLs] count]; ++i) {
            NSString *s = [[[panel URLs] objectAtIndex:i] path];
            [video_box addItemWithObjectValue:s];
        }
    }
}
- (IBAction) removeVideoFile: (id) sender {
    
    NSInteger index = [video_box indexOfSelectedItem];
    if(index >= 0) {
        [video_box removeItemAtIndex:index];
    }
    
}
- (IBAction) resetVideoFile:(id)sender {
    
}

- (IBAction) setVideoFileObject: (id) sender {
    std::vector<std::string> values;
    for(NSInteger i = 0; i < [video_box numberOfItems]; ++i) {
        std::string value = [[video_box itemObjectValueAtIndex:i] UTF8String];
        std::cout << value << "\n";
        values.push_back(value);
    }
    if(ac::setVideoFiles(values)) {
        _NSRunAlertPanel(@"Successfuly set Objects", @"Success", @"Ok", nil, nil);
    } else {
        _NSRunAlertPanel(@"Failed to set files!", @"Failed", @"Ok", nil, nil);
    }
}

- (IBAction) openVideoConcat:(id)sender {
    [video_indow orderFront:self];
}

- (IBAction) lowResMode:(id) sender {
    
    NSInteger low_r = [low_res state];
    if(low_r == NSControlStateValueOn) {
        [low_res setState: NSControlStateValueOff];
        low_res_mode = false;
    } else {
        [low_res setState: NSControlStateValueOn];
        low_res_mode = true;
    }
}

@end

void CustomFilter(cv::Mat &frame, NSMutableArray *listval, NSMutableArray *sublist, NSMutableArray *filter_states) {
    ac::in_custom = true;
    ac::swapColorState(false);
    
    for(NSInteger i = 0; i < [listval count]; ++i) {
        if(i == [listval count]-1 && ac::getColorState() == false)
            ac::in_custom = false;
        
        NSNumber *num, *fval_, *f_on;
        @try {
            num = [listval objectAtIndex:i];
            fval_ = [sublist objectAtIndex: i];
            f_on = [filter_on objectAtIndex: i];
            NSInteger index = [num integerValue];
            if([num integerValue] == [fval_ integerValue] || [f_on integerValue] == 0)
                continue;
            if(ac::testSize(frame)) {
                ac::setSubFilter(static_cast<int>([fval_ integerValue]));
                ac::CallFilter(static_cast<int>(index), frame);
            }
            [num release];
            [fval_ release];
            [f_on release];
        } @catch(NSException *e) {
            NSLog(@"%@\n", [e reason]);
        }
    }
    ac::in_custom = false;
    ac::swapColorState(false);
    ac::clearSubFilter();
    unsigned long mem = ac::calculateMemory();
    std::ostringstream stream;
    stream << "Frame Memory Allocated: " << ((mem > 0) ? (mem/1024/1024) : 0) << " MB - " << "Filters Initalized: " << ac::all_objects.size() << " - Frames Allocated: " << ac::getCurrentAllocatedFrames() << "\n";
    std::string namex = stream.str();
    [[controller getMemoryText:nil] setStringValue: [NSString stringWithUTF8String:namex.c_str()]];
    if(ac::release_frames) {
        ac::release_all_objects();
        ac::release_frames = false;
    }
}


void CustomCycle(cv::Mat &frame, NSMutableArray *listval, NSMutableArray *sublist, NSMutableArray *filter_states) {
    NSNumber *num, *fval_, *f_on;
    
    if([listval count] == 0)
        return;
    
    static int i = 0;
    @try {
        num = [listval objectAtIndex:i];
        fval_ = [sublist objectAtIndex: i];
        f_on = [filter_on objectAtIndex: i];
        NSInteger index = [num integerValue];
        if([num integerValue] == [fval_ integerValue] || [f_on integerValue] == 0)
            return;
        
        if(ac::testSize(frame) && i >= 0 && i < [listval count]) {
            ac::setSubFilter(static_cast<int>([fval_ integerValue]));
            ac::CallFilter(static_cast<int>(index), frame);
        }
        [num release];
        [fval_ release];
        [f_on release];
    } @catch(NSException *e) {
        NSLog(@"%@\n", [e reason]);
    }
    static int delay_counter = 0;
    ++delay_counter;
    if(delay_counter > (ac::fps * delay_value)) {
        delay_counter = 0;
        ++i;
        if(i > [listval count]-1) {
            i = 0;
        }
    }
}

void custom_filter(cv::Mat &frame) {
    if(camera_mode == 0) {
        dispatch_sync(dispatch_get_main_queue(), ^{
            if([cycle_chk_val integerValue] == 0)
                CustomFilter(frame, custom_array, custom_subfilters, filter_on);
            else
                CustomCycle(frame, custom_array, custom_subfilters, filter_on);
        });
    } else {
        if([cycle_chk_val integerValue] == 0)
            CustomFilter(frame, custom_array, custom_subfilters, filter_on);
        else
            CustomCycle(frame, custom_array, custom_subfilters, filter_on);
    }
}

void setSliders(long frame_count) {
    [frame_slider setMinValue: 0];
    [frame_slider setMaxValue: frame_count-1];
}

void plugin_callback(cv::Mat &frame) {
    
}

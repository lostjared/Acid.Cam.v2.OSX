//
//  AC_Controller.m
//  Acid.Cam.2
//  https://github.com/lostjared
//  Created by Jared Bruni on 6/3/13.
/*
 * Software written by Jared Bruni https://github.com/lostjared
 
 This software is dedicated to all the people that experience mental illness.
 
 Website: http://lostsidedead.com
 YouTube: http://youtube.com/LostSideDead
 Instagram: http://instagram.com/jaredbruni
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
#import "AC_Controller.h"
#include<iomanip>
#include<string>
#include<dlfcn.h>
#include<unistd.h>
#include<dirent.h>
#include<time.h>
#include<vector>
#include<algorithm>
#include"tokenize.hpp"
#include<sys/stat.h>
#include<AVKit/AVKit.h>
#include<fstream>

// Global varaibles
NSTextView *logView;
NSTextField *frame_count;
NSMutableArray *custom_array;
NSMutableArray *custom_subfilters;
bool isPaused = false;
NSSlider *frame_slider;
NSMenuItem *stop_prog_i;
AC_Controller *controller;
pixel pix;
drawn d;
bool plugin_loaded = false;
void *library = NULL;
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

//  Function below from Stack Overflow
// https://stackoverflow.com/questions/28562401/resize-an-image-to-a-square-but-keep-aspect-ratio-c-opencv
cv::Mat resizeKeepAspectRatio(const cv::Mat &input, const cv::Size &dstSize, const cv::Scalar &bgcolor)
{
    cv::Mat output;
    double h1 = dstSize.width * (input.rows/(double)input.cols);
    double w2 = dstSize.height * (input.cols/(double)input.rows);
    if( h1 <= dstSize.height) {
        cv::resize( input, output, cv::Size(dstSize.width, h1));
    } else {
        cv::resize( input, output, cv::Size(w2, dstSize.height));
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

// extern program init in another file
extern int program_main(BOOL show, std::string input_file, bool noRecord, std::string outputFileName, int capture_width, int capture_height, int capture_device, int frame_count, float pass2_alpha, std::string file_path);

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

std::string Lower(const std::string &s) {
    std::string tmp;
    for(int i = 0; i < s.length(); ++i) {
        tmp += tolower(s[i]);
    }
    return tmp;
}

void SearchForString(NSString *s) {
    [search_results removeAllObjects];
    std::string search = Lower([s UTF8String]);
    std::vector<std::string> tokens;
    token::tokenize(search, std::string(" "), tokens);
    std::vector<int> used;
    for(int i = 0; i < ac::draw_max-4; ++i) {
        std::string search_items = Lower(ac::draw_strings[i]);
        for(unsigned q = 0; q < tokens.size(); ++q) {
            if(search_items.find(tokens[q]) != std::string::npos) {
                if(std::find(used.begin(), used.end(), i) == std::end(used)) {
                    [search_results addObject: [NSNumber numberWithInt:i]];
                    used.push_back(i);
                }
            }
        }
    }
    [controller reloadTable];
}

// search Delegate
@implementation SearchController

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex {
    NSString *str =  [[aTableColumn headerCell] stringValue];
    NSNumber *number = [search_results objectAtIndex:rowIndex];
    if( [str isEqualTo:@"Filter"] ) {
        int value = (int)[number integerValue];
        NSString *s = [NSString stringWithFormat:@"%s", ac::draw_strings[value].c_str()];
        return s;
    }
    else if([str isEqualTo:@"Index"]) {
        NSString *s = [NSString stringWithFormat: @"%d", (int)[number integerValue]];
        return s;
    }
    return @"";
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView {
    return [search_results count];
}

@end

// Objective-C implemtation of class AC_Controller
@implementation AC_Controller

- (void) stopCV_prog {
    [startProg setEnabled: YES];
    programRunning = false;
}

- (IBAction) quitProgram: (id) sender {
    if(programRunning == true) {
        breakProgram = true;
        //camera_active = false;
    }
    else {
        [NSApp terminate:nil];
    }
}

- (void) reloadTable {
    [find_table reloadData];
}

- (void) dealloc {
    [custom_subfilters release];
    [custom_array release];
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
    [self createMenu: &menu_cat menuAll:&menu_all items:menu_items custom:NO adduser:NO];
    [self createMenu: &menu_cat_custom menuAll: &menu_all_custom items:menu_items_custom custom:YES adduser:YES];
    [categories setMenu: menu_cat];
    [categories_custom setMenu:menu_cat_custom];
    [current_filter setMenu: menu_items[0]];
    [current_filter_custom setMenu: menu_items_custom[0]];
    custom_array = [[NSMutableArray alloc] init];
    custom_subfilters = [[NSMutableArray alloc] init];
    [table_view setDelegate:self];
    [table_view setDataSource:self];
    [find_table setDelegate:search_controller];
    [find_table setDataSource:search_controller];
    [menuPaused setEnabled: NO];
    stop_prog_i = stop_prog;
    frame_slider = goto_f;
    ftext.setf(std::ios::fixed, std::ios::floatfield);
    ftext.precision(2);
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
    /*
     
     std::vector<std::string> valz;
     token::tokenize<std::string>(values, ",", valz);
     
     std::cout << "std::pair<std::string,DrawFunction> filters[] = {\n";
     for(int i = 0; i < ac::draw_max; ++i) {
     std::cout << "{\"" << ac::draw_strings[i] << "\"," << valz[i] << "}, ";
     }
     std::cout << "\n};\n";
     
     for(unsigned int i = 0; i < ac::draw_max; ++i) {
     std::string s = ac::draw_strings[i];
     if(s.find("Blend") != std::string::npos) {
     std::cout << "\"" << s << "\", ";
     }
     }
     } */
    
    /*
     for(int i = 0; i < ac::draw_max; ++i) {
     std::string s;
     s = ac::draw_strings[i];
	if(s.find("Strobe") != std::string::npos && s.find("SubFilter") == std::string::npos && s.find("Image") == std::string::npos)
     	std::cout << "\"" << s << "\", ";
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
    [*cat addItemWithTitle:@"Image" action:nil keyEquivalent:@""];
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
    NSInteger index = [categories indexOfSelectedItem];
    
    if(prev_index != current) {
        prev_index = current;
        ac::reset_filter = true;
    }
    
    NSMenuItem *item = [menu_items[index] itemAtIndex:current];
    NSString *title = [item title];
    
    if([fade_filter state] == NSOffState) {
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
        [negate_checked setIntegerValue: NSOffState];
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
    if(filter_value.find("Image") != std::string::npos) {
        [image_select orderFront: self];
        strout.str("");
        strout << "Image filter select use Select image window to set image...\n";
        flushToLog(strout);
    } else if(filter_value.find("SubFilter") != std::string::npos) {
        strout.str("");
        strout << "Filters that require SubFilter should be used in the Custom Filter Mode.\n";
        [custom_window orderFront:self];
        flushToLog(strout);
    }
    ac::reset_filter = true;
}

- (IBAction) downloadNewestVersion: (id) sender {
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"https://github.com/lostjared/Acid.Cam.v2.OSX/releases"]];
}

- (IBAction) stopProgram: (id) sender {
    stopProgram = true;
    [menuPaused setEnabled: NO];
    [menu_freeze setEnabled: NO];
    
    [record_op setEnabled: YES];
    
    if([videoFileInput integerValue] == 0) {
        [up4k setEnabled: NO];
    } else {
        [up4k setEnabled: YES];
    }
    stopCV();
    [startProg setTitle:@"Start Session"];
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
    [self closePlugin];
    NSString *file_type = [NSString stringWithFormat: @"%@/%@", [plugin_name stringValue], [plugin_dir objectValueOfSelectedItem]];
    pix = [self loadPlugin: file_type];
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
            if (file.find(".dylib") != -1)
            {
                NSString *s = [NSString stringWithUTF8String: e->d_name];
                [plugin_dir addItemWithObjectValue: s];
                
            }
        }
    }
    closedir(dir);
}

- (pixel) loadPlugin: (NSString *)str {
    library = dlopen([str UTF8String], RTLD_LAZY);
    if(library == NULL) {
        std::cerr << "Error could not open: " << [str UTF8String] << "\n";
        _NSRunAlertPanel(@"Error Occoured Loading Plugin", @"Exiting...", @"Ok", nil, nil);
        exit(1);
    }
    void *addr;
    // load the plugin function to process pixels
    addr = dlsym(library, "pixel");
    pixel pix;
    pix = reinterpret_cast<pixel>(addr);
    const char *error;
    error = dlerror();
    if(error) {
        std::cerr << "Could not load pixel: " << error << "\n";
        _NSRunAlertPanel(@"Could not load Plugin", @"Error loading plugin", @"Ok", nil,nil);
        return NULL;
    }
    addr = dlsym(library,"drawn");
    d = reinterpret_cast<drawn>(addr);
    error = dlerror();
    if(error) {
        std::cerr << "Could not load pixel: " << error << "\n";
        _NSRunAlertPanel(@"Could not load Plugin", @"Error loading plugin", @"Ok", nil,nil);
        return NULL;
    }
    return pix;
}

- (void) closePlugin {
    if(library != NULL)
        dlclose(library);
}

-(IBAction) startProgram: (id) sender {
    
    if([[startProg title] isEqualToString: @"Start Session"]) {
        std::string input_file;
        if([videoFileInput state] == NSOnState) {
            input_file = [[video_file stringValue] UTF8String];
            if(input_file.length() == 0) {
                _NSRunAlertPanel(@"No Input file selected\n", @"No Input Selected", @"Ok", nil, nil);
                return;
            }
            camera_mode = 1;
        } else camera_mode = 0;
        NSInteger res = [resolution indexOfSelectedItem];
        int res_x[3] = { 640, 1280, 1920 };
        int res_y[3] = { 480, 720, 1080 };
        bool r;
        if([record_op integerValue] == 1)
            r = false;
        else
            r = true;
        freeze_count = 0;
        frame_proc = 0;
        NSInteger checkedState = [menuPaused state];
        isPaused = (checkedState == NSOnState) ? true : false;
        std::ostringstream fname_stream;
        std::string filename;
        NSInteger popupType = [output_Type indexOfSelectedItem];
        time_t t = time(0);
        struct tm *m;
        m = localtime(&t);
        std::ostringstream time_stream;
        time_stream << "-" << (m->tm_year + 1900) << "." << (m->tm_mon + 1) << "." << m->tm_mday << "_" << m->tm_hour << "." << m->tm_min << "." << m->tm_sec <<  "_";
        if(popupType == 0)
            fname_stream << time_stream.str();
        else
            fname_stream << time_stream.str();
        
        filename = fname_stream.str();
        NSArray* paths = NSSearchPathForDirectoriesInDomains( NSMoviesDirectory, NSUserDomainMask, YES );
        std::string add_path = std::string([[paths objectAtIndex: 0] UTF8String])+std::string("/")+[[prefix_input stringValue] UTF8String];
        [menuPaused setEnabled: YES];
        [up4k setEnabled: NO];
        [record_op setEnabled: NO];
        ac::reset_filter = true;
        
        if(camera_mode == 1)
            capture = capture_video.get();
        else
            capture = capture_camera.get();
        
        bool u4k = ([up4k state] == NSOnState) ? true : false;
        
        int ret_val = program_main(syphon_enabled, set_frame_rate, set_frame_rate_val, u4k, (int)popupType, input_file, r, filename, res_x[res], res_y[res],(int)[device_index indexOfSelectedItem], 0, 0.75f, add_path);
        
        if(ret_val == 0) {
            if(camera_mode == 1)
                renderTimer = [NSTimer timerWithTimeInterval:1.0/ac::fps target:self selector:@selector(cvProc:) userInfo:nil repeats:YES];
            else
                renderTimer = [NSTimer timerWithTimeInterval:1.0/ac::fps target:self selector:@selector(camProc:) userInfo:nil repeats:YES];
            
            [[NSRunLoop currentRunLoop] addTimer:renderTimer forMode:NSEventTrackingRunLoopMode];
            [[NSRunLoop currentRunLoop] addTimer:renderTimer forMode:NSDefaultRunLoopMode];
        }
        
        if(ret_val != 0) {
            _NSRunAlertPanel(@"Failed to initalize capture device\n", @"Init Failed\n", @"Ok", nil, nil);
            std::cout << "DeviceIndex: " << (int)[device_index indexOfSelectedItem] << " input file: " << input_file << " filename: " << filename << " res: " << res_x[res] << "x" << res_y[res] << "\n";
            programRunning = false;
            [startProg setTitle:@"Start Session"];
            [window1 orderOut:self];
        } else {
            if([menu_freeze state] == NSOnState) {
                capture->read(old_frame);
                ++frame_cnt;
                ++frame_proc;
            }
            if(resize_value == true) {
                [stretch_scr setState: NSOnState];
            } else {
                [stretch_scr setState: NSOffState];
            }
            if(camera_mode == 0) {
                
                isPaused = false;
                [menuPaused setState:NSOffState];
                frames_captured = 0;
                background = [[NSThread alloc] initWithTarget:self selector:@selector(camThread:) object:nil];
                [background start];
                camera_active = true;
            }
            [window1 orderFront:self];
            [startProg setTitle:@"Stop"];
        }
    } else {
        [self stopProgram:self];
        [startProg setTitle:@"Start Session"];
    }
}

- (void) stopCamera {
    camera_active = false;
    [finish_queue orderFront:self];
    [finish_queue_progress startAnimation:self];
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
        if([menu_freeze state] == NSOffState) {
            capture_camera->grab();
            frames_captured++;
        }
    }
}

- (void) camThread: (id) sender {
    __block cv::Mat frame;
    bool got_frame = true;
    while(camera_active && got_frame) {
        if(isPaused) continue;
        cv::Mat temp_frame;
        if([menu_freeze state] == NSOffState) {
            got_frame = capture->retrieve(frame);
            ac::orig_frame = frame.clone();
            old_frame = frame.clone();
        } else {
            frame = old_frame.clone();
        }
        if([rotate_v state] == NSOnState) {
            cv::flip(frame, temp_frame, 1);
            frame = temp_frame.clone();
        }
        if([rotate_h state] == NSOnState) {
            cv::flip(frame, temp_frame, 0);
            frame = temp_frame.clone();
        }
        ac::orig_frame = frame.clone();
        ++frame_cnt;
        ++frame_proc;
        __block NSInteger after = 0;
        __block NSInteger slide_value1 = 0;
        __block NSInteger slide_value2 = 0;
        __block NSInteger slide_value3 = 0;
        __block NSInteger fade_state = 0;
        __block NSInteger color_key_set = 0;
        __block cv::Vec3b well_color;
        __block NSInteger up4ki = 0;
        
        dispatch_sync(dispatch_get_main_queue(), ^{
            if(reset_memory == true) {
                ac::reset_filter = true;
                reset_memory = false;
            }
            if(ac::draw_strings[ac::draw_offset] != "Custom") {
                if([negate_checked integerValue] == NSOffState) ac::isNegative = false;
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
        
        if(after == NSOffState)
            ac::ApplyColorMap(frame);
        
        if(fade_state == NSOffState) {
            if(disableFilter == false && ac::testSize(frame)) ac::draw_func[ac::draw_offset](frame);
        } else {
            if(current_fade_alpha >= 0) {
                ac::filterFade(frame, (int)current_fade, ac::draw_offset, current_fade_alpha);
                current_fade_alpha -= 0.08;
            } else {
                if(disableFilter == false && ac::testSize(frame)) ac::draw_func[ac::draw_offset](frame);
            }
        }
        ac::frames_released = false;
        ac::reset_alpha = false;
        if(after == NSOnState)
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
        
        if(color_key_set == NSOnState && (colorkey_set == true && !color_image.empty())) {
            cv::Mat cframe = frame.clone();
            ac::filterColorKeyed(well_color, ac::orig_frame, cframe, frame);
        } else if(color_key_set == NSOnState && colorkey_bg == true) {
            cv::Mat cframe = frame.clone();
            ac::filterColorKeyed(well_color, ac::orig_frame, cframe, frame);
        } else if(color_key_set == NSOnState && colorkey_replace == true) {
            cv::Mat cframe = frame.clone();
            ac::filterColorKeyed(well_color, ac::orig_frame, cframe, frame);
        } else if(color_key_set == NSOnState && colorkey_filter == true) {
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
            if([stretch_scr state] == NSOnState) {
                cv::Mat dst;
                dst = resizeKeepAspectRatio(frame, cv::Size(rc.size.width, rc.size.height), cv::Scalar(0,0,0));
                if(syphon_enabled == NO) cv::imshow("Acid Cam v2", dst);
            } else {
                if(syphon_enabled == NO) {
                    cv::resizeWindow("Acid Cam v2", frame.cols, frame.rows);
                    cv::imshow("Acid Cam v2", frame);
                }
            }
            ftext << "(Current Frame/Total Frames/Seconds/MB): " << frame_cnt << "/" << "0" << "/" << (frame_cnt/ac::fps) << "/" << ((file_size/1024)/1024) << " MB";
            if(camera_mode == 1) {
                float val = frame_cnt;
                float size = total_frames;
                if(size != 0)
                    ftext << " - " << (val/size)*100 << "% ";
            }
            setFrameLabel(ftext);
        });
        
        if(ac::noRecord == false) {
            cv::Mat up;
            if(up4ki == NSOnState && frame.size() != cv::Size(3840, 2160)) {
                up = resizeKeepAspectRatio(frame, cv::Size(3840, 2160), cv::Scalar(0, 0, 0));
            } else {
                up = frame;
            }
            if(writer->isOpened()) writer->write(up);
            struct stat buf;
            stat(ac::fileName.c_str(), &buf);
            file_size = buf.st_size;
        }
        
        dispatch_sync(dispatch_get_main_queue(), ^{
            if(syphon_enabled == YES) [render_ updateTexture: &frame];
        });
        
        if(ac::snapShot == true) {
            static unsigned int index = 0;
            stream.str("");
            time_t t = time(0);
            struct tm *m;
            m = localtime(&t);
            stream << add_path << "-" << (m->tm_year + 1900) << "." << (m->tm_mon + 1) << "." << m->tm_mday << "_" << m->tm_hour << "." << m->tm_min << "." << m->tm_sec <<  "_" << (++index) << "-" << frame.cols << "x" << frame.rows << ".Acid.Cam.Image." << ac::draw_strings[ac::draw_offset] << ((ac::snapshot_Type == 0) ? ".jpg" : ".png");
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
            [NSApp terminate:nil];
        }
        programRunning = false;
        [startProg setEnabled: YES];
        [background release];
        camera_active = false;
        [startProg setTitle:@"Start Session"];
    });
}

- (void) cvProc: (id) sender {
    
    if(breakProgram == true || stopProgram == true) { stopCV(); return; }
    if(isPaused && pauseStepTrue == true) {
        pauseStepTrue = false;
    }
    else if(isPaused) return;
    cv::Mat frame;
    bool frame_read = true;
    if([menu_freeze state] == NSOffState) {
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
    
    if([up4k state] == NSOnState || frame.size() == cv::Size(3840, 2160)) {
        [stretch_scr setState: NSOnState];
        cv::resizeWindow("Acid Cam v2", rc.size.width, rc.size.height);
    }
    
    if(capture->isOpened() && frame_read == false) {
        ++frame_cnt;
        ++frame_proc;
        double seconds = ((total_frames)/ac::fps);
        double cfps = ((freeze_count+video_total_frames+frame_cnt)/ac::fps);
        double elapsed = (frame_proc/ac::fps);
        char elapsed_s[1024];
        snprintf(elapsed_s, 1023, "%.2f", elapsed);
        char cfps_s[1024];
        snprintf(cfps_s, 1023, "%.2f", (seconds-cfps));
        
        ftext  << "(Frames/Total Frames/Remaining Sec/Length Sec/MB): " << frame_cnt << "/" << total_frames << "/" << cfps_s << "/" << elapsed_s << "/" << ((file_size/1024)/1024) << " MB";
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
        return;
    }
    cv::Mat temp_frame;
    if([rotate_v state] == NSOnState) {
        cv::flip(frame, temp_frame, 1);
        frame = temp_frame.clone();
    }
    if([rotate_h state] == NSOnState) {
        cv::flip(frame, temp_frame, 0);
        frame = temp_frame.clone();
    }
    
    cv::Mat up;
    if([up4k state] == NSOnState && frame.size() != cv::Size(3840, 2160)) {
        frame = resizeKeepAspectRatio(frame, cv::Size(3840, 2160), cv::Scalar(0, 0, 0));
    }
    
    if(([color_chk state] == NSOnState) || (ac::draw_strings[ac::draw_offset] == "Blend with Source") || (ac::draw_strings[ac::draw_offset] == "Custom") || (ac::draw_strings[ac::draw_offset] == "AlphaBlendWithSource") || (ac::draw_strings[ac::draw_offset] == "XorWithSource") || (ac::draw_strings[ac::draw_offset] == "HorizontalStripes") || (ac::draw_strings[ac::draw_offset] == "CollectionXorSourceSubFilter")) {
        ac::orig_frame = frame.clone();
    }
    if(ac::draw_strings[ac::draw_offset] != "Custom") {
        if([negate_checked integerValue] == NSOffState) ac::isNegative = false;
        else ac::isNegative = true;
        ac::color_order = (int) [corder indexOfSelectedItem];
    }
    NSInteger after = [apply_after integerValue];
    if(after == NSOffState)
        ac::ApplyColorMap(frame);
    
    if([fade_filter state] == NSOffState) {
        if(disableFilter == false && ac::testSize(frame))
            ac::draw_func[ac::draw_offset](frame);
    } else {
        if(current_fade_alpha >= 0) {
            ac::filterFade(frame, (int)current_fade, ac::draw_offset, current_fade_alpha);
            current_fade_alpha -= 0.08;
        } else {
            if(disableFilter == false && ac::testSize(frame)) ac::draw_func[ac::draw_offset](frame);
        }
    }
    ac::frames_released = false;
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
    
    if(after == NSOnState)
        ac::ApplyColorMap(frame);
    
    if([color_chk state] == NSOnState && colorkey_set == true && !color_image.empty()) {
        cv::Mat cframe = frame.clone();
        ac::filterColorKeyed(well_color, ac::orig_frame, cframe, frame);
    } else if([color_chk state] == NSOnState && colorkey_bg == true) {
        cv::Mat cframe = frame.clone();
        ac::filterColorKeyed(well_color, ac::orig_frame, cframe, frame);
    } else if([color_chk state] == NSOnState && colorkey_replace == true) {
        cv::Mat cframe = frame.clone();
        ac::filterColorKeyed(well_color, ac::orig_frame, cframe, frame);
    } else if([color_chk state] == NSOnState && colorkey_filter == true) {
        cv::Mat cframe = frame.clone();
        ac::filterColorKeyed(well_color, ac::orig_frame, cframe, frame);
    }
    if([menu_freeze state] == NSOffState) {
        ++frame_cnt;
        ++frame_proc;
    }
    else
        ++freeze_count;
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
    if([stretch_scr state] == NSOnState) {
        cv::Mat dst;
        dst = resizeKeepAspectRatio(frame, cv::Size(rc.size.width, rc.size.height), cv::Scalar(0,0,0));
        if(syphon_enabled == NO) cv::imshow("Acid Cam v2", dst);
    } else {
        if(!frame.empty() && frame.rows > 25 && frame.cols > 25) {
            if(frame.ptr() != NULL) {
                if(syphon_enabled == NO) {
                    cv::imshow("Acid Cam v2", frame);
                }
            }
        }
    }
    
    double seconds = ((total_frames)/ac::fps);
    double cfps = ((freeze_count+video_total_frames+frame_cnt)/ac::fps);
    double elapsed = (frame_proc/ac::fps);
    
    char elapsed_s[1024];
    snprintf(elapsed_s, 1023, "%.2f", elapsed);
    char cfps_s[1024];
    snprintf(cfps_s, 1023, "%.2f", (seconds-cfps));
    
    ftext << "(Frames/Total Frames/Remaining Sec/Length Sec/MB): " << frame_cnt << "/" << total_frames << "/" << cfps_s << "/" << elapsed_s << "/" << ((file_size/1024)/1024) << " MB";
    
    if(camera_mode == 1) {
        float val = frame_cnt;
        float size = total_frames;
        if(size != 0)
            ftext << " - " << (val/size)*100 << "% ";
    }
    setFrameLabel(ftext);
    if(ac::noRecord == false) {
        /*
         cv::Mat up;
         if([up4k state] == NSOnState && frame.size() != cv::Size(3840, 2160)) {
         up = resizeKeepAspectRatio(frame, cv::Size(3840, 2160), cv::Scalar(0, 0, 0));
         } else {
         up = frame;
         }*/
        
        if(writer->isOpened() )writer->write(frame);
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
        stream << add_path << "-" << (m->tm_year + 1900) << "." << (m->tm_mon + 1) << "." << m->tm_mday << "_" << m->tm_hour << "." << m->tm_min << "." << m->tm_sec <<  "_" << (++index) << "-" << frame.cols << "x" << frame.rows << ".Acid.Cam.Image." << ac::draw_strings[ac::draw_offset] << ((ac::snapshot_Type == 0) ? ".jpg" : ".png");
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
    NSArray *ar = [NSArray arrayWithObjects: @"mov", @"avi", @"mp4", @"mkv",@"m4v",@"webm", nil];
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
    if(state == NSOffState) {
        [rotate_v setState:NSOnState];
    } else {
        [rotate_v setState:NSOffState];
    }
    
}
- (IBAction) setRotate_H:(id) sender {
    NSInteger state = [rotate_h state];
    if(state == NSOffState) {
        [rotate_h setState:NSOnState];
    } else {
        [rotate_h setState:NSOffState];
    }
}

- (IBAction) takeSnopshot: (id) sender {
    ac::snapShot = true;
    ac::snapshot_Type = 0;
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
        [chk_repeat setEnabled:NO];
        [up4k setEnabled: NO];
        [up4k setState: NSOffState];
    }
    else {
        [video_file setEnabled: NO];
        [resolution setEnabled: NO];
        [device_index setEnabled: NO];
        [selectVideoFile setEnabled: YES];
        [chk_repeat setEnabled:YES];
        [up4k setEnabled: YES];
    }
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex {
    NSString *str =  [[aTableColumn headerCell] stringValue];
    NSNumber *number = [custom_array objectAtIndex:rowIndex];
    NSNumber *filter_num = [custom_subfilters objectAtIndex: rowIndex];
    int value = (int)[number integerValue];
    int filter_val = (int)[filter_num integerValue];
    if( [str isEqualTo:@"Filter"] ) {
        std::string name = ac::draw_strings[value];
        if(user_filter.find(name) != user_filter.end())
            return [NSString stringWithUTF8String: user_filter[name].other_name.c_str()];
        NSString *s = [NSString stringWithFormat:@"%s", ac::draw_strings[value].c_str()];
        return s;
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
        if(user_filter.find(sval) != user_filter.end())
            return [NSString stringWithUTF8String: user_filter[sval].other_name.c_str()];
        else
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
        [table_view reloadData];
        return;
    }
    NSMenuItem *item = [menu_items_custom[cate] itemAtIndex: index];
    NSString *title = [item title];
    if(index >= 0 && cate >= 0) {
        int filter_value = ac::filter_map[[title UTF8String]];
        [custom_array addObject: [NSNumber numberWithInt: filter_value]];
        [custom_subfilters addObject: [NSNumber numberWithInt: -1]];
        [table_view reloadData];
    }
}

- (IBAction) removeCustomItem: (id) sender {
    NSInteger index = [table_view selectedRow];
    if(index >= 0) {
        [custom_array removeObjectAtIndex:index];
        [custom_subfilters removeObjectAtIndex: index];
        [table_view reloadData];
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
        [custom_array setObject:obj atIndexedSubscript:index];
        [custom_array setObject:mv atIndexedSubscript: pos];
        [custom_subfilters setObject:obj_c atIndexedSubscript:index];
        [custom_subfilters setObject:mv_c atIndexedSubscript: pos];
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
        [custom_array setObject:obj atIndexedSubscript:index];
        [custom_array setObject:mv atIndexedSubscript: pos];
        [custom_subfilters setObject:obj_c atIndexedSubscript:index];
        [custom_subfilters setObject:mv_c atIndexedSubscript: pos];
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
        [menuPaused setState:NSOffState];
        [pause_step setEnabled:NO];
        isPaused = false;
        _NSRunAlertPanel(@"Cannot pause in camera mode", @"Cannot pause", @"Ok", nil, nil);
        return;
    }
    NSInteger checkedState = [menuPaused state];
    std::ostringstream stream;
    if(checkedState == NSOnState) {
        [menuPaused setState: NSOffState];
        [pause_step setEnabled: NO];
        isPaused = false;
        stream << "Program unpaused.\n";
        flushToLog(stream);
        
    } else {
        [menuPaused setState: NSOnState];
        isPaused = true;
        [pause_step setEnabled: YES];
        stream << "Program paused.\n";
        flushToLog(stream);
    }
}

- (IBAction) disableFilters: (id) sender {
    NSInteger checkedState = [disable_filters state];
    std::ostringstream stream;
    if(checkedState == NSOnState) {
        [disable_filters setState: NSOffState];
        // enable
        disableFilter = false;
        stream << "Filters enabled.\n";
        flushToLog(stream);
    } else {
        [disable_filters setState: NSOnState];
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
    negate = [check_box state] == NSOffState ? false : true;
}

- (IBAction) setNegative: (id) sender {
    NSInteger chkvalue = [negate_checked integerValue];
    if(chkvalue == NSOnState) ac::isNegative = true;
    else ac::isNegative = false;
}

- (IBAction) selectImage: (id) sender {
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setCanChooseDirectories:NO];
    [panel setCanChooseFiles: YES];
    [panel setAllowsMultipleSelection: YES];
    [panel setAllowedFileTypes: [NSArray arrayWithObjects: @"bmp",@"jpg", @"png", nil]];
    if([panel runModal]) {
        NSArray *file_names = [panel URLs];
        if([file_names count] > 0) {
            for(int i = 0; i < [file_names count]; ++i) {
                NSURL *file_n = [file_names objectAtIndex:i];
                [image_combo addItemWithObjectValue: [file_n path]];
            }
        }
    }
}

- (IBAction) updateLabelText: (id) sender {
    NSInteger index = [image_to_set indexOfSelectedItem];
    NSString *s = [NSString stringWithUTF8String: set_filenames[index].c_str()];
    [selectedFilename setStringValue: s];
}

- (IBAction) setAsImage: (id) sender {
    if([image_combo indexOfSelectedItem] >= 0) {
        NSString *current = [image_combo itemObjectValueAtIndex: [image_combo indexOfSelectedItem]];
        NSInteger index = [image_to_set indexOfSelectedItem];
        
        if(index == 0) {
            blend_image = cv::imread([current UTF8String]);
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
}

- (IBAction) showCustom: (id) sender {
    [custom_window orderFront: self];
    [filter_search_window orderFront: self];
}

- (IBAction) showActivityLog: (id) sender {
    [window1 orderFront: self];
}

- (IBAction) showSelectImage: (id) sender {
    [image_select orderFront: self];
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
    if([menu_freeze state] == NSOnState) {
        [menu_freeze setState: NSOffState];
    } else {
        [menu_freeze setState: NSOnState];
    }
}

- (IBAction) rewindToStart:(id) sender {
    jumptoFrame(syphon_enabled, 0);
    frame_count = 0;
    [frame_slider setIntegerValue:(NSInteger)frame_count];
}

- (IBAction) changeVideoPos: (id) sender {
    [self goto_Frame: self];
    [self setGoto: self];
}

- (NSRect) getScreenSize {
    return [[NSScreen mainScreen] frame];
}

- (IBAction) setStretch: (id) sender {
    if([stretch_scr state] == NSOnState) {
        [stretch_scr setState: NSOffState];
    } else {
        [stretch_scr setState: NSOnState];
        cv::resizeWindow("Acid Cam v2", rc.size.width, rc.size.height);
    }
}

- (IBAction) setColorMap: (id) sender {
    NSInteger index = [color_map indexOfSelectedItem];
    ac::set_color_map = (int) index;
}

- (IBAction) selectedCustomFilter: (id) sender {
    std::ostringstream strout;
    NSInteger index = [current_filter_custom indexOfSelectedItem];
    NSInteger cate = [categories_custom indexOfSelectedItem];
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
}

- (IBAction) addSearchItem: (id) sender {
    NSInteger index = [find_table selectedRow];
    if(index >= 0 && index < [search_results count]) {
        NSNumber *num = [search_results objectAtIndex:index];
        NSNumber *subf = [NSNumber numberWithInt: -1];
        [custom_array addObject:num];
        [custom_subfilters addObject: subf];
        [table_view reloadData];
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
    switch([image_to_set indexOfSelectedItem]) {
        case 0:
            blend_set = false;
            blend_image.release();
            [selectedFilename setStringValue:@""];
            _NSRunAlertPanel(@"Blend image released", @"Released Image", @"Ok", nil, nil);
            break;
        case 1:
            colorkey_set = false;
            color_image.release();
            _NSRunAlertPanel(@"Color Key image released", @"Released Image", @"Ok", nil, nil);
            break;
        case 2:
            colorkey_bg = false;
            color_bg_image.release();
            _NSRunAlertPanel(@"Released ColorKey background", @"Released Image", @"Ok", nil, nil);
            break;
        case 3:
            colorkey_replace = false;
            color_replace_image.release();
            _NSRunAlertPanel(@"Color Key Replace Image Cleared", @"Color Key Image Replace Released", @"Ok", nil, nil);
            break;
    }

    NSInteger index = [image_to_set indexOfSelectedItem];
    if(index >= 0) {
        set_filenames[index] = "None";
        [self updateLabelText:self];
    }
}

- (IBAction) setPref: (id) sender {
    NSInteger pos = [procMode indexOfSelectedItem];
    ac::setProcMode(static_cast<unsigned int >(pos));
    std::ostringstream log;
    std::string str_val[] = {"Move Out Increase, Move in", "Move Out, Move in", "Move Out, Reset", ""};
    log << "Proccess Mode Set to: " << str_val[pos] << "\n";
    if([cam_frame_rate_chk state] == NSOnState) {
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
    log << "Syphon Output Set To: " << sz.width << "x" << sz.height << "\n";
    [syphon_window setContentSize: sz];
    NSString *val = [NSString stringWithUTF8String:log.str().c_str()];
    _NSRunAlertPanel(@"Settings changed", val, @"Ok", nil, nil);
    flushToLog(log);
}

- (IBAction) showPrefs:(id) sender {
    [procMode_Window orderFront:self];
}
- (IBAction) resetAllFrames:(id) sender {
    emiter.reset();
    ac::frames_released = true;
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
        emiter.reset();
    }
}

- (IBAction) releaseFrames:(id)sender {
    reset_memory = true;
    std::ostringstream stream;
    stream << "Stored Frames Memory Released...\n";
    flushToLog(stream);
}

- (IBAction) setSubFilter:(id) sender {
    NSInteger index = [current_filter_custom indexOfSelectedItem];
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
    if([syphon_enable state] == NSOnState) {
        [syphon_enable setState: NSOffState];
        [syphon_window orderOut:self];
        syphon_enabled = NO;
        stream << "Disabled Syphon Output...\n";
        if(programRunning == true) cv::namedWindow("Acid Cam v2",cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
    } else {
        [syphon_enable setState: NSOnState];
        [syphon_window orderFront:self];
        syphon_enabled = YES;
        stream << "Enabled Syphon Output...\n";
        cv::destroyWindow("Acid Cam v2");
    }
    flushToLog(stream);
}

- (IBAction) addToBlocked: (id) sender {
    if([key_range state] == NSOnState) {
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
    if([chk_spill state] == NSOnState)
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
    if([chk_spill state] == NSOnState)
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
    if([key_range state] == NSOnState) {
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
    if(key_state == NSOnState) {
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
    
    if([chk_replace state] == NSOnState)
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
    
    if([chk_replace state] == NSOnState)
        colorkey_filter = true;
    else
        colorkey_filter = false;
    
}

- (IBAction) user_Set: (id) sender {
    NSString *s = [user_filter_name stringValue];
    if([s length] == 0) {
        _NSRunAlertPanel(@"User defined requires a valid name", @"Error forgot to set name", @"Ok", nil, nil);
        return;
    }
    NSInteger index = [current_filter_custom indexOfSelectedItem];
    if(index < 0) return;
    
    std::string ftext = [s UTF8String];
    auto fp = user_filter.find(ftext);
    if(ftext.find("User_") != std::string::npos && fp != user_filter.end()) {
        _NSRunAlertPanel(@"You already set this value", @"You use this value already", @"Ok", nil, nil);
        return;
    }
    
    NSMenuItem *item = [current_filter_custom itemAtIndex: index];
    std::string fval_name = [[item title] UTF8String];
    if(fval_name.find("User_") != std::string::npos) {
        _NSRunAlertPanel(@"You cannot set a custom name already user defined.",@"Cannot define value",@"Ok", nil, nil);
        return;
    }
    std::string fname;
    fname = "User_";
    fname += [s UTF8String];
    
    if(fval_name.find("SubFilter") != std::string::npos) {
        fname += "_SubFilter";
    }
    if(fval_name.find("Image") != std::string::npos) {
        fname += "_Image";
    }
    user_filter[fval_name].index = -1;
    user_filter[fval_name].name = fval_name;
    user_filter[fval_name].other_name = fname;
    user_filter[fname].index = ac::filter_map[fval_name];
    ac::filter_map[fname] = ac::filter_map[fval_name];
    NSString *sval = [NSString stringWithUTF8String: fname.c_str()];
    [user_filter_name addItemWithObjectValue:sval];
    [user_filter_name setStringValue:@""];
    [self loadMenuList];
    [table_view reloadData];
}
- (IBAction) user_Save: (id) sender {
    NSSavePanel *panel = [NSSavePanel savePanel];
    [panel setCanCreateDirectories:YES];
    [panel setAllowedFileTypes: [NSArray arrayWithObjects: @"acl", nil]];
    if([panel runModal]) {
        NSString *fname = [[panel URL] path];
        std::string textname = [fname UTF8String];
        std::fstream file;
        file.open(textname, std::ios::out);
        if(!file.is_open()) {
            _NSRunAlertPanel(@"Error could not save file. Do you have access rights?", @"Cannot save file", @"Ok", nil, nil);
            return;
        }
        for(auto i = user_filter.begin(); i != user_filter.end(); ++i) {
            std::string other = "UserDefined";
            if(i->second.other_name.length() > 0)
                other = i->second.other_name;
            file << i->first << ":" << i->second.index << ":" << other << "\n";
        }
        file.close();
    }
}

- (IBAction) user_Clear: (id) sender {
    if(!user_filter.empty()) {
        user_filter.erase(user_filter.begin(), user_filter.end());
        [user_filter_name removeAllItems];
        [self loadMenuList];
        [table_view reloadData];
    }
}

- (void) loadFileData: (const char *)path {
    std::vector<std::string> comp;
    token::tokenize(std::string(path), std::string(":"), comp);
    if(comp.size()==0) return;
    std::string filter_name = comp[0];
    int index_value = atoi(comp[1].c_str());
    user_filter[filter_name].name = filter_name;
    user_filter[filter_name].index = index_value;
    user_filter[filter_name].other_name = comp[2];
    
    if(index_value == -1) {
        ac::filter_map[comp[2]] = ac::filter_map[comp[0]];
    }
    
    if(user_filter[filter_name].index != -1) {
    	NSString *sval = [NSString stringWithUTF8String: filter_name.c_str()];
        [user_filter_name addItemWithObjectValue:sval];
    }
    [table_view reloadData];
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
        [self user_Clear:nil];
        while(!file.eof()) {
            std::string file_data;
            std::getline(file, file_data);
            if(file) {
                [self loadFileData: file_data.c_str()];
            }
        }
        [self loadMenuList];
        [table_view reloadData];
    }
}

- (void) loadMenuList {
    if(user_menu != nil)
        [user_menu release];
    
    NSInteger index_value = [categories_custom indexOfSelectedItem];
    user_menu = [[NSMenu alloc] init];
    
    for(auto i = user_filter.begin(); i != user_filter.end(); ++i) {
    	if(i->second.index != -1)
            [user_menu addItemWithTitle: [NSString stringWithUTF8String:i->first.c_str()] action:nil keyEquivalent:@""];
    }
    [user_menu addItemWithTitle: [NSString stringWithUTF8String:"No Filter"] action:nil keyEquivalent:@""];
    if(index_value == 14 && [user_menu numberOfItems] > 0) {
        [current_filter_custom setMenu: user_menu];
    }
}

- (IBAction) jumpToCustom: (id) sender {
    [window1 orderFront:self];
    [categories selectItemAtIndex:0];
    [self menuSelected:self];
    [current_filter selectItemAtIndex:ac::draw_max-4];
    [self changeFilter:self];
}

- (IBAction) custom_Save: (id) sender {
    if([custom_array count] == 0) {
        _NSRunAlertPanel(@"No filters to save!", @"There are no filters in the list...", @"Ok", nil, nil);
        return;
    }
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
        stream << "Wrote custom to: " << [fileName UTF8String] << "\n";
        flushToLog(stream);
        file_n.close();
    }
}

- (IBAction) custom_Load: (id) sender {
    NSOpenPanel *panel = [NSOpenPanel openPanel];
    [panel setCanChooseFiles:YES];
    [panel setCanChooseDirectories: NO];
    [panel setAllowedFileTypes: [NSArray arrayWithObject:@"filter"]];
    if([panel runModal]) {
        NSString *fileName = [[panel URL] path];
        std::fstream file;
        file.open([fileName UTF8String], std::ios::in);
        if(!file.is_open()) {
            _NSRunAlertPanel(@"Could not open file", @"Could not open file do i have rights to this folder?", @"Ok", nil, nil);
            return;
        }
        std::vector<std::string> values;
        while(!file.eof()) {
            std::string item;
            std::getline(file, item);
            if(file)
                values.push_back(item);
        }
        // check if data valid
        for(int i = 0; i < values.size(); ++i ){
            std::string item = values[i];
            std::string s_left, s_right;
            auto pos = item.find(":");
            if(pos == std::string::npos) {
                _NSRunAlertPanel(@"Incorrect File..\n", @"Values in file incorrect", @"Ok", nil, nil);
                return;
            }
            s_left = item.substr(0,pos);
            s_right = item.substr(pos+1, item.length());
            int val1 = ac::filter_map[s_left];
            int val2 = 0;
            if(s_right == "None")
                val2 = -1;
            else
                val2 = ac::filter_map[s_right];
            
            if(!(val1 >= 0 && val1 < ac::draw_max-4)) {
                _NSRunAlertPanel(@"Unsupported Value", @"Filter value out of range... wrong program revision?", @"Ok", nil, nil);
                return;
            }
            if(!(val2 == -1 || (val2 >= 0 && val2 < ac::draw_max-4))) {
                _NSRunAlertPanel(@"Unsupported SubFilter value",@"Sub Filter value of range...", @"Ok", nil, nil);
                return;
            }
        }
        [custom_array removeAllObjects];
        [custom_subfilters removeAllObjects];
        for(int i = 0; i < values.size(); ++i) {
            std::string item = values[i];
            std::string s_left, s_right;
            s_left = item.substr(0, item.find(":"));
            s_right = item.substr(item.find(":")+1, item.length());
            int val1 = ac::filter_map[s_left];
            int val2 = 0;
            if(s_right == "None")
                val2 = -1;
            else
                val2 = ac::filter_map[s_right];
            NSNumber *num1 = [NSNumber numberWithInteger: val1];
            NSNumber *num2 = [NSNumber numberWithInteger: val2];
            [custom_array addObject:num1];
            [custom_subfilters addObject:num2];
        }
        [table_view reloadData];
        file.close();
    }
}

@end

std::unordered_map<std::string, UserFilter> user_filter;

void CustomFilter(cv::Mat &frame, NSMutableArray *listval, NSMutableArray *sublist) {
    ac::in_custom = true;
    for(NSInteger i = 0; i < [listval count]; ++i) {
        if(i == [listval count]-1)
            ac::in_custom = false;
        NSNumber *num, *fval_;
        @try {
            num = [listval objectAtIndex:i];
            fval_ = [sublist objectAtIndex: i];
            NSInteger index = [num integerValue];
            if(ac::testSize(frame)) {
                ac::setSubFilter(static_cast<int>([fval_ integerValue]));
                ac::draw_func[static_cast<int>(index)](frame);
            }
        } @catch(NSException *e) {
            NSLog(@"%@\n", [e reason]);
        }
    }
    ac::in_custom = false;
    ac::clearSubFilter();
}



void custom_filter(cv::Mat &frame) {
    CustomFilter(frame, custom_array, custom_subfilters);
}

void setSliders(long frame_count) {
    [frame_slider setMinValue: 0];
    [frame_slider setMaxValue: frame_count-1];
}

void plugin_callback(cv::Mat &frame) {
    if(plugin_loaded == false) return;
    int i = 0, z = 0;
    for(z = 0; z < frame.cols; ++z) {
        for(i = 0; i < frame.rows; ++i) {
            cv::Vec3b &buffer = frame.at<cv::Vec3b>(i, z);
            unsigned char pixels[] = { buffer[0], buffer[1], buffer[2] };
            (*pix)(z, i, pixels);
            buffer[0] = pixels[0];
            buffer[1] = pixels[1];
            buffer[2] = pixels[2];
        }
    }
    (*d)();
}

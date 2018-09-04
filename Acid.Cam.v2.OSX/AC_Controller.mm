//
//  AC_Controller.m
//  Acid.Cam.2
//  https://github.com/lostjared
//  Created by Jared Bruni on 6/3/13.
/*
 * Software written by Jared Bruni https://github.com/lostjared
 
 This software is dedicated to all the people that struggle with mental illness.
 
 Website: http://lostsidedead.com
 YouTube: http://youtube.com/LostSideDead
 Instagram: http://instagram.com/jaredbruni
 Twitter: http://twitter.com/jaredbruni
 Facebook: http://facebook.com/LostSideDead0x
 
 You can use this program free of charge and redistrubute it online as long
 as you do not charge anything for this program. This program is meant to be
 100% free.
 
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

// Global varaibles
NSTextView *logView;
NSTextField *frame_count;
NSMutableArray *custom_array;
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
    else {
        NSString *s = [NSString stringWithFormat: @"%d", (int)[number integerValue]];
        return s;
    }
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
    [self createMenu: &menu_cat menuAll:&menu_all items:menu_items custom:NO];
    [self createMenu: &menu_cat_custom menuAll: &menu_all_custom items:menu_items_custom custom:YES];
    [categories setMenu: menu_cat];
    [categories_custom setMenu:menu_cat_custom];
    [current_filter setMenu: menu_items[0]];
    [current_filter_custom setMenu: menu_items_custom[0]];
    custom_array = [[NSMutableArray alloc] init];
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
    frame_proc = 0;
    ac::setCustom(custom_filter);
    ac::setPlugin(plugin_callback);
    [self reloadCameraInfo: self];
    upscale_video = false;
    [up4k setEnabled: YES];
    set_frame_rate = false;
    set_frame_rate_val = 24;
    reset_memory = false;
    syphon_enabled = NO;
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
    */
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

- (void) createMenu: (NSMenu **)cat menuAll: (NSMenu **)all items: (NSMenu **)it_arr custom:(BOOL)cust {
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
    [*cat addItemWithTitle:@"Special" action:nil keyEquivalent:@""];
    
    for(int i = 1; i < 13; ++i) {
        it_arr[i] = [[NSMenu alloc] init];
    }
    
    std::vector<std::string> all_sorted;
    for(int x = 0; x < ac::draw_max-4; ++x)
        all_sorted.push_back(ac::draw_strings[x]);
    
    std::sort(all_sorted.begin(), all_sorted.end());
    
    const char **szAllSorted = convertToStringArray(all_sorted);
    [self fillMenuWithString: it_arr[1] stringValues:szAllSorted];
    eraseArray(szAllSorted, all_sorted.size());
    std::vector<std::string> vzBlend { "Self AlphaBlend", "Self Scale", "Blend #3", "Negative Paradox",  "ThoughtMode", "RandTriBlend", "Filter3","Rainbow Blend","Rand Blend","Pixel Scale","Pulse", "Combine Pixels", "Blend_Angle", "XorMultiBlend", "UpDown","LeftRight", "BlendedScanLines","XorSine", "FrameBlend", "FrameBlendRGB", "PrevFrameBlend", "HorizontalBlend", "VerticalBlend", "OppositeBlend", "DiagonalLines", "HorizontalLines", "BlendSwitch", "IncreaseBlendHorizontal", "BlendIncrease", "ColorRange", "VectorIncrease", "BlendThree", "HorizontalStripes", "Dual_SelfAlphaRainbow", "Dual_SelfAlphaBlur", "SurroundPixelXor", "WeakBlend", "AverageVertical", "RandomAlphaBlend", "RandomTwoFilterAlphaBlend", "AlphaBlendPosition", "BlendRowAlpha", "BlendRow", "BlendRowByVar", "BlendRowByDirection", "BlendAlphaXor", "SelfXorScale", "SelfAlphaRGB", "XorSelfAlphaImage", "AlphaBlendRandom", "ChannelSortAlphaBlend", "RandomXor", "RandomXorFlash", "SoftXor", "SelfXorBlend", "SelfXorDoubleFlash", "SelfOrDoubleFlash", "BlendRowCurvedSqrt", "CycleShiftRandomAlphaBlend", "TanAlphaGrid", "BlendInAndOut", "BlendScaleInAndOut", "AcidGlitch", "LiquidFilter", "MatrixXorAnd", "XorAlpha", "SelfXorAverage", "RandomXorBlend", "RGBVerticalXor", "RGBVerticalXorScale", "RGBHorizontalXor", "RGBHorizontalXorScale", "FadeInAndOut", "InitBlend", "LagBlend", "SubFilter", "AddFilter", "AlphaBlendSubFilter", "SmoothSubFilterAlphaBlend", "IntertwineSubFilter", "RandBlend", "EveryOther", "EveryOtherSubFilter", "SmoothSubFilter", "EnergizeSubFilter", "SmoothSubFilter16", "EnergizeSubFilter16", "EnergizeSubFilter32", "SmoothSubFilter32", "HalfAddSubFilter", "HalfXorSubFilter", "StaticXorBlend", "XorScale", "ChannelMedianSubFilter", "Bitwise_XOR_Blend", "Bitwise_OR_Blend", "Bitwise_AND_Blend", "PixelReverseXor", "SilverBlend", "PixelXorBlend", "SelfAlphaScale", "SelfScaleAlpha", "RainbowXorBlend", "FadeBlend", "SelfAlphaScaleBlend", "FadeBars", "ShadeRGB", "InterRGB_SubFilter", "InterSmoothSubFilter", "InterRGB_Bars_XY", "InterRGB_Bars_X", "InterRGB_Bars_Y", "StoredFramesAlphaBlend_SubFilter", "BlendSubFilter", "BlendAlphaSubFilter", "ReverseFrameBlend", "ReverseFrameBlendSwitch", "Blend_AlphaSubFilter","RandomBlendFilter","DoubleRandomBlendFilter", "FlipBlendW", "FlipBlendH", "FlipBlendWH", "FlipBlendAll", "FrameMedianBlendSubFilter", "SelfScaleXorIncrease", "Blend_RedGreenBlue"};
    std::sort(vzBlend.begin(), vzBlend.end());
    const char **szBlend = convertToStringArray(vzBlend);
    
    [self fillMenuWithString: it_arr[2] stringValues:szBlend];
    eraseArray(szBlend, vzBlend.size());
    
    std::vector<std::string> svDistort { "Tri","Distort","CDraw","Sort Fuzz","Fuzz","Boxes","Boxes Fade", "ShiftPixels", "ShiftPixelsDown","WhitePixel", "Block", "BlockXor","BlockStrobe", "BlockScale", "InvertedScanlines", "ColorMorphing", "NegativeStrobe", "ParticleRelease","ParticleBlend","ParticleFlash","ParticleAlpha", "All Red", "All Green", "All Blue", "LineRGB", "PixelRGB", "BoxedRGB", "KruegerSweater", "RGBStatic1", "RGBStatic2", "FrameBars", "Lines", "WhiteLines", "ThickWhiteLines", "UseLineObject", "LeftLines", "ParticleFast"};
    std::sort(svDistort.begin(), svDistort.end());
    const char **szDistort = convertToStringArray(svDistort);
    [self fillMenuWithString: it_arr[3] stringValues:szDistort];
    eraseArray(szDistort, svDistort.size());
    std::vector<std::string> svPattern { "Blend Fractal","Blend Fractal Mood","Diamond Pattern" };
    std::sort(svPattern.begin(), svPattern.end());
    const char **szPattern = convertToStringArray(svPattern);
    [self fillMenuWithString: it_arr[4] stringValues:szPattern];
    eraseArray(szPattern, svPattern.size());
    std::vector<std::string> svGradient { "RandomGradient", "CosSinMultiply","New Blend","Color Accumlate1", "Color Accumulate2", "Color Accumulate3", "Filter8", "Graident Rainbow","Gradient Rainbow Flash","Outward", "Outward Square","GradientLines","GradientSelf","GradientSelfVertical","GradientDown","GraidentHorizontal","GradientRGB","GradientStripes", "GradientReverse", "GradientReverseBox", "GradientReverseVertical", "GradientNewFilter", "AverageLines", "QuadCosSinMultiply", "GradientColors", "GradientColorsVertical", "GradientXorSelfScale", "GradientLeftRight", "GraidentUpDown", "GradientLeftRightInOut", "GradientUpDownInOut", "GradientSubFilterXor"};
    std::sort(svGradient.begin(), svGradient.end());
    const char **szGradient = convertToStringArray(svGradient);
    [self fillMenuWithString: it_arr[5] stringValues:szGradient];
    eraseArray(szGradient, svGradient.size());
    std::vector<std::string> svMirror { "NewOne", "MirrorBlend", "Sideways Mirror","Mirror No Blend","Mirror Average", "Mirror Average Mix","Reverse","Double Vision","RGB Shift","RGB Sep","Side2Side","Top2Bottom", "Soft_Mirror", "KanapaTrip", "InterReverse", "InterMirror", "InterFullMirror", "MirrorRGB", "LineByLineReverse", "CycleShiftRGB", "CycleShiftRandomRGB", "CycleShiftRandomRGB_XorBlend", "RGBMirror", "MirrorStrobe", "RandomMirror", "RandomMirrorBlend", "RandomMirrorAlphaBlend", "MirrorXor", "MirrorXorAll", "MirrorXorScale", "EnergyMirror", "MirrorXorAlpha", "MirrorEnergizeSubFilter", "IntertwinedMirror", "BlurredMirror", "DoubleRandomMirror"};
    std::sort(svMirror.begin(), svMirror.end());
    const char **szMirror = convertToStringArray(svMirror);
    [self fillMenuWithString: it_arr[6] stringValues:szMirror];
    eraseArray(szMirror, svMirror.size());
    std::vector<std::string> svStrobe{  "StrobeEffect", "Blank", "Type","Random Flash","Strobe Red Then Green Then Blue","Flash Black","FlashWhite","StrobeScan", "RGBFlash", "ReinterpretDouble", "DiamondStrobe", "BitwiseXorStrobe","FlashBlackAndWhite", "StrobeBlend", "FibFlash", "ScaleFlash", "FadeStrobe", "AndStrobe", "AndStrobeScale", "AndPixelStrobe", "AndOrXorStrobe", "AndOrXorStrobeScale", "BrightStrobe", "DarkStrobe", "RandomXorOpposite", "StrobeTransform", "RandomStrobe", "OrStrobe", "DifferenceStrobe", "BlackAndWhiteDifferenceStrobe", "DifferenceXor", "DifferenceRand", "HalfNegateStrobe", "RandomStrobeFlash", "GaussianStrobe", "StrobeSort", "GlitchSortStrobe", "StrobeXor"};
    std::sort(svStrobe.begin(), svStrobe.end());
    const char **szStrobe = convertToStringArray(svStrobe);
    [self fillMenuWithString: it_arr[7] stringValues:szStrobe];
    eraseArray(szStrobe, svStrobe.size());
    std::vector<std::string> svBlur { "GaussianBlur", "Median Blur", "Blur Distortion", "ColorTrails","TrailsFilter", "TrailsFilterIntense", "TrailsFilterSelfAlpha", "TrailsFilterXor","BlurSim", "TrailsInter", "TrailsBlend", "TrailsNegate", "AcidTrails", "HorizontalTrailsInter" ,"Trails", "BlendTrails", "SmoothTrails", "SmoothTrailsSelfAlphaBlend", "SmoothTrailsRainbowBlend", "MedianBlend", "XorTrails", "RainbowTrails", "NegativeTrails", "IntenseTrails", "GaussianBlend", "RandomAmountMedianBlur", "MedianBlendAnimation", "AlphaAcidTrails", "RandomBlur", "RGBTrails", "RGBTrailsDark", "RGBTrailsAlpha", "RGBTrailsNegativeAlpha", "MovementRGBTrails", "RGBTrailsXor", "DifferenceBrightStrobe", "PsycheTrails", "DarkTrails", "MedianBlurXor", "NegateTrails", "FrameBlurSubFilter"};
    std::sort(svBlur.begin(), svBlur.end());
    const char **szBlur = convertToStringArray(svBlur);
    [self fillMenuWithString: it_arr[8] stringValues:szBlur];
    eraseArray(szBlur, svBlur.size());
    std::vector<std::string> svImage{"Blend with Image", "Blend with Image #2", "Blend with Image #3", "Blend with Image #4", "ImageFile", "ImageXor", "ImageAlphaBlend", "ImageInter", "ImageX", "SmoothRandomImageBlend", "SmoothImageAlphaBlend", "BlendImageOnOff", "ImageShiftUpLeft", "AlphaBlendImageXor", "ExactImage", "BlendImageXor", "BlendImageAround_Median", "ImageBlendTransform", "ImageXorAlpha", "ImageAverageXor", "DarkImageMedianBlend", "ImageBlendSubFilter", "ImageBlendXorSubFilter", "ImageCollectionSubFilter"};
    std::sort(svImage.begin(),svImage.end());
    const char **szImage = convertToStringArray(svImage);
    [self fillMenuWithString: it_arr[9] stringValues:szImage];
    eraseArray(szImage, svImage.size());
    std::vector<std::string> svOther { "Mean", "Laplacian", "Bitwise_XOR", "Bitwise_AND", "Bitwise_OR", "Channel Sort", "Reverse_XOR", "Bitwise_Rotate", "Bitwise_Rotate Diff","Equalize","PixelSort", "GlitchSort", "HPPD", "FuzzyLines","Random Filter", "Alpha Flame Filters","Scanlines", "TV Static","FlipTrip", "Canny", "Inter","Circular","MoveRed","MoveRGB","MoveRedGreenBlue", "Wave","HighWave","VerticalSort","VerticalChannelSort","ScanSwitch","ScanAlphaSwitch", "XorAddMul","RandomIntertwine","RandomFour","RandomTwo","Darken", "AverageRandom","RandomCollectionAverage","RandomCollectionAverageMax","BitwiseXorScale","XorChannelSort","Bitwise_XOR_Average","NotEqual","Sort_Vertical_Horizontal","Sort_Vertical_Horizontal_Bitwise_XOR","Scalar_Average_Multiply","Scalar_Average","Total_Average","VerticalColorBars","inOrder","inOrderBySecond","DarkenFilter","RandomFilterBySecond","ThreeRandom",  "Blend with Source", "Plugin", "Custom", "inOrderAlpha", "XorBackwards", "MoveUpLeft", "Stuck", "StuckStrobe", "SoftFeedback", "SoftFeedbackFrames", "ResizeSoftFeedback","SoftFeedback8","SoftFeedbackFrames8","ResizeSoftFeedback8", "ResizeSoftFeedbackSubFilter", "SoftFeedbackRandFilter","SoftFeedback32","SoftFeedbackFrames32","ResizeSoftFeedback32", "SoftFeedbackRandFilter32", "SoftFeedbackSubFilter","SoftFeedbackResizeSubFilter", "SoftFeedbackResizeSubFilter64", "SoftFeedbackReszieSubFilter64_Negate", "SoftFeedbackReszieSubFilter64_Mirror", "RandomOther", "RandomXorFilter", "Bitwise_XOR_AlphaSubFilter", "XorBlend_SubFilter", "RandomFilterRandomTimes", "RandomSubFilterRandomTimes", "PsycheSort", "Bitwise_XOR_Sort", "BitwiseColorMatrix", "PixelatedSubFilterSort", "RandomPixelOrderSort", "FrameDifference", "SmallDiffference","FilteredDifferenceSubFilter", "GammaDarken5", "GammaDarken10"};
    std::sort(svOther.begin(), svOther.end());
    
    const char **szOther = convertToStringArray(svOther);
    std::vector<std::string> svOther_Custom { "Mean", "Laplacian", "Bitwise_XOR", "Bitwise_AND", "Bitwise_OR", "Channel Sort", "Reverse_XOR","Bitwise_Rotate","Bitwise_Rotate Diff", "Equalize","PixelSort", "GlitchSort","HPPD","FuzzyLines","Random Filter", "Alpha Flame Filters","Scanlines", "TV Static","FlipTrip", "Canny","Inter","Circular","MoveRed","MoveRGB", "MoveRedGreenBlue", "Wave","HighWave","VerticalSort","VerticalChannelSort","ScanSwitch", "ScanAlphaSwitch","XorAddMul", "RandomIntertwine","RandomFour","RandomTwo","Darken", "Blend with Source","AverageRandom","RandomCollectionAverage","RandomCollectionAverageMax","BitwiseXorScale","XorChannelSort","Bitwise_XOR_Average","NotEqual","Sort_Vertical_Horizontal","Sort_Vertical_Horizontal_Bitwise_XOR", "Scalar_Average_Multiply","Scalar_Average","Total_Average","VerticalColorBars","inOrder","inOrderBySecond","DarkenFilter","RandomFilterBySecond","ThreeRandom","inOrderAlpha","XorBackwards", "Plugin", "MoveUpLeft", "Stuck", "StuckStrobe", "SoftFeedback", "SoftFeedbackFrames", "ResizeSoftFeedback", "SoftFeedback8","SoftFeedbackFrames8","ResizeSoftFeedback8", "ResizeSoftFeedbackSubFilter", "SoftFeedbackRandFilter", "SoftFeedback32","SoftFeedbackFrames32","ResizeSoftFeedback32", "SoftFeedbackRandFilter32", "SoftFeedbackSubFilter","SoftFeedbackResizeSubFilter", "SoftFeedbackResizeSubFilter64", "SoftFeedbackReszieSubFilter64_Negate", "SoftFeedbackReszieSubFilter64_Mirror", "RandomOther", "RandomXorFilter", "Bitwise_XOR_AlphaSubFilter", "XorBlend_SubFilter", "RandomFilterRandomTimes", "RandomSubFilterRandomTimes", "PsycheSort", "Bitwise_XOR_Sort", "BitwiseColorMatrix", "PixelatedSubFilterSort", "RandomPixelOrderSort", "FrameDifference", "SmallDiffference", "FilteredDifferenceSubFilter", "GammaDarken5", "GammaDarken10"};
    std::sort(svOther_Custom.begin(), svOther_Custom.end());
    const char **szOther_Custom = convertToStringArray(svOther_Custom);
    
    std::vector<std::string> svSquare {"SquareSwap","SquareSwap4x2","SquareSwap8x4", "SquareSwap16x8","SquareSwap64x32", "SquareBars","SquareBars8","SquareSwapRand16x8","SquareVertical8","SquareVertical16","SquareVertical_Roll","SquareSwapSort_Roll","SquareVertical_RollReverse","SquareSwapSort_RollReverse", "RandomFilteredSquare","RandomQuads","QuadRandomFilter", "RollRandom", "GridFilter8x", "GridFilter16x", "GridFilter8xBlend", "GridRandom", "GridRandomPixel", "PixelatedSquare", "SmoothSourcePixel", "ColorLines", "Curtain", "RandomCurtain", "RandomCurtainVertical", "CurtainVertical", "SlideFilter","SlideFilterXor", "RandomSlideFilter", "SlideUpDown", "SlideUpDownXor", "SlideUpDownRandom", "SlideSubFilter", "SlideSubUpDownFilter", "FourSquare", "EightSquare", "DiagonalSquare", "DiagonalSquareRandom", "SquareStretchDown", "SquareStretchRight", "SquareStretchUp", "SquareStretchLeft", "ExpandSquareBlendSubFilter", "ExpandSquareSubFilter", "ExpandSquareVerticalSubFilter"};
    
    std::sort(svSquare.begin(), svSquare.end());
    const char **szSquare = convertToStringArray(svSquare);
    [self fillMenuWithString: it_arr[10] stringValues:szSquare];
    eraseArray(szSquare, svSquare.size());
    
    const char *szCustom[] = {"Negate","DarkenFilter","Reverse","ReverseFrameBlend", "No Filter", "Blend with Source", "Plugin", "Custom",0};
    const char *szCustom_Spec[] = {"Negate","DarkenFilter","Reverse","ReverseFrameBlend", "No Filter", "Blend with Source", "Plugin",0};
    
    if(cust == NO) {
        [self fillMenuWithString: it_arr[11] stringValues:szOther];
        [self fillMenuWithString: it_arr[12] stringValues:szCustom];
    }
    else {
        [self fillMenuWithString: it_arr[11] stringValues:szOther_Custom];
        [self fillMenuWithString: it_arr[12] stringValues:szCustom_Spec];
    }
    
    eraseArray(szOther, svOther.size());
    eraseArray(szOther_Custom, svOther_Custom.size());
    
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
    [current_filter_custom setMenu: menu_items_custom[index]];
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
    
    if([videoFileInput integerValue] == 0) {
        [up4k setEnabled: NO];
    } else {
        [up4k setEnabled: YES];
    }
    stopCV();
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
    std::cout << add_path << "\n";
    [startProg setEnabled: NO];
    [menuPaused setEnabled: YES];
    [up4k setEnabled: NO];
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
        [startProg setEnabled: YES];
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
            frame = temp_frame;
        }
        if([rotate_h state] == NSOnState) {
            cv::flip(frame, temp_frame, 0);
            frame = temp_frame;
        }
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
        
        if(color_key_set == NSOnState && colorkey_set == true && !color_image.empty()) {
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
            stream << add_path << "-" << (m->tm_year + 1900) << "." << (m->tm_mon + 1) << "." << m->tm_mday << "_" << m->tm_hour << "." << m->tm_min << "." << m->tm_sec <<  "_" << (++index) << ".Acid.Cam.Image." << ac::draw_strings[ac::draw_offset] << ((ac::snapshot_Type == 0) ? ".jpg" : ".png");
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
        return;
    }
    cv::Mat temp_frame;
    if([rotate_v state] == NSOnState) {
        cv::flip(frame, temp_frame, 1);
        frame = temp_frame;
    }
    if([rotate_h state] == NSOnState) {
        cv::flip(frame, temp_frame, 0);
        frame = temp_frame;
    }
    
    cv::Mat up;
    if([up4k state] == NSOnState && frame.size() != cv::Size(3840, 2160)) {
        frame = resizeKeepAspectRatio(frame, cv::Size(3840, 2160), cv::Scalar(0, 0, 0));
    }
    
    if(([color_chk state] == NSOnState) || (ac::draw_strings[ac::draw_offset] == "Blend with Source") || (ac::draw_strings[ac::draw_offset] == "Custom") || (ac::draw_strings[ac::draw_offset] == "HorizontalStripes")) {
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
        stream << add_path << "-" << (m->tm_year + 1900) << "." << (m->tm_mon + 1) << "." << m->tm_mday << "_" << m->tm_hour << "." << m->tm_min << "." << m->tm_sec <<  "_" << (++index) << ".Acid.Cam.Image." << ac::draw_strings[ac::draw_offset] << ((ac::snapshot_Type == 0) ? ".jpg" : ".png");
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
    NSArray *ar = [NSArray arrayWithObjects: @"mov", @"avi", @"mp4", @"mkv",@"m4v", nil];
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
    if( [str isEqualTo:@"Filter"] ) {
        int value = (int)[number integerValue];
        NSString *s = [NSString stringWithFormat:@"%s", ac::draw_strings[value].c_str()];
        return s;
    }
    else {
        NSString *s = [NSString stringWithFormat: @"%d", (int)[number integerValue]];
        return s;
    }
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView {
    return [custom_array count];
}

- (IBAction) addCustomItem: (id) sender {
    NSInteger index = [current_filter_custom indexOfSelectedItem];
    NSInteger cate = [categories_custom indexOfSelectedItem];
    NSMenuItem *item = [menu_items_custom[cate] itemAtIndex: index];
    NSString *title = [item title];
    if(index >= 0 && cate >= 0) {
        int filter_value = ac::filter_map[[title UTF8String]];
        [custom_array addObject: [NSNumber numberWithInt: filter_value]];
        [table_view reloadData];
    }
}

- (IBAction) removeCustomItem: (id) sender {
    NSInteger index = [table_view selectedRow];
    if(index >= 0) {
        [custom_array removeObjectAtIndex:index];
        [table_view reloadData];
    }
}

- (IBAction) moveCustomUp: (id) sender {
    NSInteger index = [table_view selectedRow];
    if(index > 0) {
        NSInteger pos = index-1;
        id obj = [custom_array objectAtIndex:pos];
        id mv = [custom_array objectAtIndex:index];
        [custom_array setObject:obj atIndexedSubscript:index];
        [custom_array setObject:mv atIndexedSubscript: pos];
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
        [custom_array setObject:obj atIndexedSubscript:index];
        [custom_array setObject:mv atIndexedSubscript: pos];
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
    int val = (int)[frame_slider integerValue];
    if(val < [frame_slider maxValue]-1) {
    	jumptoFrame(syphon_enabled, val);
    	std::ostringstream stream;
    	stream << "Jumped to frame: " << val << "\n";
    	flushToLog(stream);
    }
}

- (IBAction) setGoto: (id) sender {
    NSInteger time_val = [frame_slider integerValue];
    double seconds = time_val/ac::fps;
    unsigned int sec = static_cast<unsigned int>(seconds);
    NSString *str_val = [NSString stringWithFormat:@"Jump to Time: %d Seconds @ Frame #%d", sec, (int)time_val];
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
            NSString *s = [NSString stringWithFormat:@"%s", stream.str().c_str(), nil];
            _NSRunAlertPanel(@"Image set", s, @"Ok", nil, nil);
            flushToLog(stream);
        } else {
            color_image = cv::imread([current UTF8String]);
            if(color_image.empty()) {
                _NSRunAlertPanel(@"Image Not set", @"Could Not Set Image...\n", @"Ok", nil, nil);
                return;
            }
            colorkey_set = true;
            std::ostringstream stream;
            stream << "ColorKey Image set to: " << [current UTF8String] << "\n";
            NSString *s = [NSString stringWithFormat:@"%s", stream.str().c_str(), nil];
            _NSRunAlertPanel(@"Image Set", s, @"Ok", nil, nil);
            flushToLog(stream);
        }
    }
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
        [custom_array addObject:num];
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
            _NSRunAlertPanel(@"Blend image released", @"Released Image", @"Ok", nil, nil);
            break;
        case 1:
            colorkey_set = false;
            color_image.release();
            _NSRunAlertPanel(@"Color Key image released", @"Released Image", @"Ok", nil, nil);
            break;
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
    NSInteger num_index = [blend_source_amt indexOfSelectedItem];
    double amount = 0.1 * (num_index+1);
    ac::setBlendPercentage(amount);
    NSInteger num = [proc_change indexOfSelectedItem];
    static double values[] = { 0, 0.001,0.009, 0.01, 0.03, 0.05, 0.07, 0.08, 0.1, 0.3, 0.7, 1.0, 3};
    static std::string str_values[] = { "Default", "0.001","0.009","0.01","0.03", "0.05", "0.07", "0.08","0.1", "0.3"," 0.7", "1.0", "3", ""};
    ac::alpha_increase = values[num];
    log << "Proccess Speed set to: " << str_values[num] << "\n";
    log << "Blend with Source Image set to: " << ((num_index+1)*10) << "%\n";
    
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
    if(sub_chk.find("SubFilter") != std::string::npos) {
        std::ostringstream stream;
        stream << "Could not set Filter: " << sub_chk << " as a SubfFlter because the one you selected itself requires a SubFilter\n";
        flushToLog(stream);
        return;
    }
    NSString *val = [NSString stringWithFormat: @"Filter: %s set as Sub Filter", ac::draw_strings[ac::filter_map[[s UTF8String]]].c_str(), nil];
    int filter_pos = ac::filter_map[[s UTF8String]];
    ac::setSubFilter(filter_pos);
    std::ostringstream stream;
    stream << [val UTF8String] << "\n";
    flushToLog(stream);
}

- (IBAction) setSubSearch: (id) sender {
    NSInteger index = [find_table selectedRow];
    if(index >= 0 && index < [search_results count]) {
        //NSString *val = [find_table ]
        NSNumber *num = [search_results objectAtIndex:index];
        std::string sub_chk = ac::draw_strings[[num integerValue]];
        if(sub_chk.find("SubFilter") != std::string::npos) {
            std::ostringstream stream;
            stream << "Could not set Filter: " << sub_chk << " as a SubfFlter because the one you selected itself requires a SubFilter\n";
            flushToLog(stream);
            return;
        }
        NSString *val = [NSString stringWithFormat: @"Filter: %s set as Sub Filter", ac::draw_strings[[num integerValue]].c_str(), nil];
        int filter_pos = static_cast<int>([num integerValue]);
        ac::setSubFilter(filter_pos);
        std::ostringstream stream;
        stream << [val UTF8String] << "\n";
        flushToLog(stream);
    }
}

- (IBAction) clearSubFilter: (id) sender {
    ac::setSubFilter(-1);
    std::ostringstream stream;
    stream << "Sub Filter cleared\n";
    flushToLog(stream);
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

@end

void custom_filter(cv::Mat &frame) {
    ac::in_custom = true;
    for(NSInteger i = 0; i < [custom_array count]; ++i) {
        if(i == [custom_array count]-1)
            ac::in_custom = false;
        NSNumber *num;
        @try {
            num = [custom_array objectAtIndex:i];
            NSInteger index = [num integerValue];
            if(ac::testSize(frame))
            	ac::draw_func[static_cast<int>(index)](frame);
        } @catch(NSException *e) {
            NSLog(@"%@\n", [e reason]);
        }
    }
    ac::in_custom = false;
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

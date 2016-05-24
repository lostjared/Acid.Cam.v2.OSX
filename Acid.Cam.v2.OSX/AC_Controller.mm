//
//  AC_Controller.m
//  Acid.Cam.2
//
//  Created by Jared Bruni on 6/3/13.
//  Copyright (c) 2013 Jared Bruni. All rights reserved.
//

#import "AC_Controller.h"
#import"videocapture.h"
#import"ac.h"
#include<string>
#include<dlfcn.h>

NSTextView *logView;
NSTextField *frame_count;
NSMutableArray *custom_array;
bool isPaused = false;
NSSlider *frame_slider;
NSMenuItem *stop_prog_i;
AC_Controller *controller;
pixel pix;
bool plugin_loaded = false;
void *library = NULL;

extern int program_main(std::string input_file, bool noRecord, std::string outputFileName, int capture_width, int capture_height, int capture_device, int frame_count, float pass2_alpha, std::string file_path);

void flushToLog(std::ostringstream &sout) {
    NSTextView *sv = logView;
    NSString *value = [[sv textStorage] string];
    NSString *newValue = [[NSString alloc] initWithFormat: @"%@%s", value, sout.str().c_str()];
    [sv setString: newValue];
    [sv scrollRangeToVisible:NSMakeRange([[sv string] length], 0)];
    [newValue release];
    sout.str("");
}

void setFrameLabel(std::ostringstream &text) {
    NSString *str = [NSString stringWithUTF8String: text.str().c_str()];
    [frame_count setStringValue: str];
    text.str("");
}

void setEnabledProg() {
    [stop_prog_i setEnabled: NO];
}

@implementation AC_Controller

- (void) stopCV_prog {
    [startProg setEnabled: YES];
    programRunning = false;
}


- (IBAction) quitProgram: (id) sender {
    if(programRunning == true)
    breakProgram = true;
    else {
        [NSApp terminate:nil];
    }
}

- (void) dealloc {
    [custom_array release];
    [super dealloc];
}

- (void) awakeFromNib {
    controller = self;
    [video_file setEnabled: NO];
    [resolution setEnabled: NO];
    [device_index setEnabled: NO];
    [record_audio setEnabled: NO];
    logView = t_view;
    frame_count = framecount;
    [window1 setLevel: NSStatusWindowLevel];
    [window2 setLevel: NSStatusWindowLevel];
    [custom_window setLevel: NSStatusWindowLevel];
    [alpha_window setLevel: NSStatusWindowLevel];
    for(unsigned int i = 0; i < ac::draw_max-3; ++i){
        NSString *s = [NSString stringWithUTF8String: ac::draw_strings[i].c_str()];
        NSMenuItem *item = [[NSMenuItem alloc] initWithTitle:s action:NULL keyEquivalent:@""];
        [[current_filter menu] addItem:item];
        [item release];
        if(ac::draw_strings[i] != "Custom")
        [filter_combo addItemWithObjectValue:s];
    }
    custom_array = [[NSMutableArray alloc] init];
    [table_view setDelegate:self];
    [table_view setDataSource:self];
    [menuPaused setEnabled: NO];
    stop_prog_i = stop_prog;
    frame_slider = goto_f;
}

- (IBAction) changeFilter: (id) sender {
    NSInteger current = [current_filter indexOfSelectedItem];
    ac::draw_offset = (int)current;
    std::ostringstream strout;
    strout << "Filter set to: " << ac::draw_strings[ac::draw_offset] << "\n";
    flushToLog(strout);
    
    if(ac::draw_strings[ac::draw_offset] == "Custom") {
        [custom_window orderFront:self];
    }
    if(ac::draw_strings[ac::draw_offset] == "Alpha Flame Filters") {
        [alpha_window orderFront:self];
    }
	if(ac::draw_strings[ac::draw_offset] == "Plugin") {
		[plugin_window orderFront:self];
	}
}

- (IBAction) stopProgram: (id) sender {
    stopProgram = true;
    [menuPaused setEnabled: NO];
    stopCV();
}

- (IBAction) selectPlugin: (id) sender {
	
	NSOpenPanel *panel = [NSOpenPanel openPanel];
	
	[panel setCanChooseFiles:YES];
	[panel setCanChooseDirectories:NO];
	[panel setAllowedFileTypes:[NSArray arrayWithObject:@"dylib"]];
	
	if([panel runModal]) {
		NSString *file_type = [[panel URL] path];
		[self closePlugin];
		[plugin_name setStringValue: file_type ];
		pix = [self loadPlugin: file_type];
		plugin_loaded = true;
	}
}




- (pixel) loadPlugin: (NSString *)str {
	
	library = dlopen([str UTF8String], RTLD_LAZY);
	if(library == NULL) {
		std::cerr << "Error could not open: " << [str UTF8String] << "\n";
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
		exit(1);
	}
	return pix;
}


- (void) closePlugin {
	
	if(library != NULL)
		dlclose(library);
}


-(IBAction) startProgram: (id) sender {
    std::string input_file;
    if([videoFileInput integerValue] != 0) {
        input_file = [[video_file stringValue] UTF8String];
        if(input_file.length() == 0) {
            NSRunAlertPanel(@"No Input file selected\n", @"No Input Selected", @"Ok", nil, nil);
            return;
        }
    }
    NSInteger res = [resolution indexOfSelectedItem];
    int res_x[3] = { 640, 1280, 1920 };
    int res_y[3] = { 480, 720, 1080 };
    bool r, raudio;
    if([record_op integerValue] == 1)
        r = false;
    else
        r = true;
    if([record_audio integerValue] == 1)
        raudio = true;
    else
        raudio = false;
    
    static unsigned int counter = 0;
    
    std::ostringstream fname_stream;
    
    std::string filename;
    NSInteger popupType = [output_Type indexOfSelectedItem];
    
    if(!r) {
        ++counter;
    }
    
    if(popupType == 0)
        fname_stream << ".AC2.Output." << (counter) << ".mov";
    else
        fname_stream <<".AC2.Output." << (counter) << ".avi";

    filename = fname_stream.str();
    
    NSArray* paths = NSSearchPathForDirectoriesInDomains( NSMoviesDirectory, NSUserDomainMask, YES );
    
    std::string add_path = std::string([[paths objectAtIndex: 0] UTF8String])+std::string("/")+[[prefix_input stringValue] UTF8String];
    std::cout << add_path << "\n";
    [startProg setEnabled: NO];
    [window1 orderFront:self];
    
    [menuPaused setEnabled: YES];
    
    renderTimer = [NSTimer timerWithTimeInterval:0.001   //a 1ms time interval
                                          target:self
                                        selector:@selector(cvProc:)
                                        userInfo:nil
                                         repeats:YES];

    
    int ret_val = program_main((int)popupType, input_file, r, raudio, filename, res_x[res], res_y[res],(int)[device_index indexOfSelectedItem], 0, 0.75f, add_path);
    
  
    
    if(ret_val != 0) {
        NSRunAlertPanel(@"Failed to initalize camera\n", @"Camera Init Failed\n", @"Ok", nil, nil);
        std::cout << "DeviceIndex: " << (int)[device_index indexOfSelectedItem] << " input file: " << input_file << " filename: " << filename << " res: " << res_x[res] << "x" << res_y[res] << "\n";
    }
}

- (void) cvProc: (id) sender {
    
    
    if(breakProgram == true) { stopCV(); return; }
    if(stopProgram == true)  { stopCV(); return; }
    
    if(isPaused) return;
    
    
    cv::Mat frame;
    
    if(capture.read(frame) == false) {
        stopCV();
        return;
    }
    ProcFrame(frame);
    ++frame_cnt;
    
    
    std::ostringstream ftext;
    ftext.setf(std::ios::fixed, std::ios::floatfield);
    ftext.precision(2);
    ftext << "(Frames/Total Frames/Seconds/MB): " << frame_cnt << "/" << total_frames << "/" << (frame_cnt/ac::fps) << "/" << ((file_size/1024)/1024) << " MB";
    setFrameLabel(ftext);
    //cv::moveWindow("Acid Cam v2", 0, 0);
    imshow("Acid Cam v2", frame);
    if(ac::noRecord == false) {
        // add pause
        writer.write(frame);
        if(file.is_open()) {
            file.seekg(0, std::ios::end);
            file_size = file.tellg();
        }
    }
    if(ac::snapShot == true) {
        static unsigned int index = 0;
        std::ostringstream stream;
        stream << add_path << "_" << (++index) << ".Acid.Cam.Image." << ac::draw_strings[ac::draw_offset] << ((ac::snapshot_Type == 0) ? ".jpg" : ".png");
        imwrite(stream.str(), frame);
        sout << "Took snapshot: " << stream.str() << "\n";
        ac::snapShot = false;
        // flush to log
        flushToLog(sout);
    }
    
    /*
    if( (key = cv::waitKey(1)) ) {
        switch(key) {
            case 't':
                ac::resetAll();
                break;
            case 'a':
            {
                static unsigned int index = 0;
                std::ostringstream stream;
                stream << add_path << "_" <<  (++index) << ".Acid.Cam.Image.Uncompressed." << ac::draw_strings[ac::draw_offset] << ".bmp";
                imwrite(stream.str(), frame);
                sout << "Saved uncompressed image: " << stream.str() << "\n";
                // flush to log
                flushToLog(sout);
            }
                break;
            case 'd':
            case 's':
            {
                static unsigned int index = 0;
                std::ostringstream stream;
                stream << add_path << "_" << (++index) << ".Acid.Cam.Image." << ac::draw_strings[ac::draw_offset] << ((key == 's') ? ".jpg" : ".png");
                imwrite(stream.str(), frame);
                sout << "Took snapshot: " << stream.str() << "\n";
                // flush to log
                flushToLog(sout);
            }
                break;
            case 'x':
                ac::alpha = 1.0f;
                ac::tr = 0.3f;
                sout << "Reset Alpha..\n";
                // flush to log
                flushToLog(sout);
                break;
            case 'l':
                sout << "Draw Offset Increased\n";
                if(ac::draw_offset < ac::draw_max-4)
                    ++ac::draw_offset;
                sout << "Filter set to: " << ac::draw_strings[ac::draw_offset] << "\n";
                // flush to log
                flushToLog(sout);
                break;
            case 'o':
                sout << "Draw Offset Decreased\n";
                if(ac::draw_offset > 0)
                    --ac::draw_offset;
                sout << "Filter set to: " << ac::draw_strings[ac::draw_offset] << "\n";
                // flush to log
                flushToLog(sout);
                break;
            case 'n':
                ac::isNegative = !ac::isNegative;
                sout << "Is Negative: " << boolStr(ac::isNegative) << "\n";
                // flush to log
                flushToLog(sout);
                break;
            case 'r':
                ac::iRev = !ac::iRev;
                if(ac::iRev == true) ac::isNegative = true;
                sout << "Reverse Colors: " << boolStr(ac::iRev) << "\n";
                // flush to log
                flushToLog(sout);
                break;
            case 27:
                sout << "Quitting..\n";
                // flush to log
                flushToLog(sout);
                stopCV();
                
                break;
            case 'p':
                ac::pass2_enabled = !ac::pass2_enabled;
                sout << "Pass2 Enabled: "<< boolStr(ac::pass2_enabled) <<  "\n";
                // flush to log
                flushToLog(sout);
                break;
            case 'h':
                ac::slide_Show = !ac::slide_Show;
                sout << "Slide Show: " <<  boolStr(ac::slide_Show) << "\n";
                // flush to log
                flushToLog(sout);
                break;
            case 'j':
                ac::slide_Rand = !ac::slide_Rand;
                sout << "Slide Show Random: " << boolStr(ac::slide_Rand) << "\n";
                // flush to log
                flushToLog(sout);
                break;
            case 'b':
                ac::strobe_It = !ac::strobe_It;
                sout << "Strobe It: " << boolStr(ac::strobe_It) << "\n";
                // flush to log
                flushToLog(sout);
                break;
            case 'c':
                if(ac::switch_Back == true) {
                    ac::switch_Back = false;
                } else {
                    ac::switch_Back = true;
                    ac::isNegative = false;
                }
                sout << "switch: " << boolStr(ac::switch_Back) << "\n" << "is Negative: " << boolStr(ac::isNegative) << "\n";
                // flush to log
                flushToLog(sout);
                break;
            case 'f':
                ac::blur_First = !ac::blur_First;
                sout << "Blur First: " << boolStr(ac::blur_First) << "\n";
                // flush to log
                flushToLog(sout);
                break;
            case 'g':
                ac::blur_Second = !ac::blur_Second;
                sout << "Blur Second: " << boolStr(ac::blur_Second) << "\n";
                // flush to log
                flushToLog(sout);
                break;
        }
    } */
  
}


- (IBAction) openWebcamDialog: (id) sender {
    if([startaction indexOfSelectedItem] == 0)
        [window1 orderFront: self];
    else { // load video
        [window2 orderFront: self];
    }
}

- (IBAction) startVideoProgram: (id) sender {
    
}
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
        [record_audio setEnabled: YES];
        [selectVideoFile setEnabled: NO];
    }
    else {
        [video_file setEnabled: NO];
        [resolution setEnabled: NO];
        [device_index setEnabled: NO];
        [record_audio setEnabled: NO];
        [selectVideoFile setEnabled: YES];
    }
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex {
    NSString *str =  [[aTableColumn headerCell] stringValue];
    NSNumber *number = [custom_array objectAtIndex:rowIndex];
    if( [str isEqualTo:@"Filter"] ) {
        int value = (int)[number integerValue];
        NSString *s = [NSString stringWithFormat:@"%s", ac::draw_strings[value].c_str()];
        [number release];
        return s;
    }
    else {
        NSString *s = [NSString stringWithFormat: @"%d", (int)[number integerValue]];
        [number release];
        return s;
    }
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView {
    return [custom_array count];
    NSLog(@"count");
}

- (IBAction) addCustomItem: (id) sender {
    NSInteger index = [filter_combo indexOfSelectedItem];
    if(index >= 0) {
        [custom_array addObject: [NSNumber numberWithInt: (int)index]];
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

- (IBAction) selectFileForPrefix: (id) sender {
    NSSavePanel *panel = [NSSavePanel savePanel];
    if([panel runModal]) {
        [prefix_input setStringValue:[[panel URL] path]];
    }
}

- (IBAction) setOption_Negative: (id) sender {
    NSInteger checkedState = [menuNegative state];
    if(checkedState == NSOnState) {
        [menuNegative setState: NSOffState];
        ac::isNegative = false;
        
    }
    else {
        [menuNegative setState: NSOnState];
        ac::isNegative = true;
    }
}

- (IBAction) setOption_NegFlash: (id) sender {
    NSInteger checkedState = [menuNegFlash state];
    if(checkedState == NSOnState) {
        [menuNegFlash setState: NSOffState];
        ac::switch_Back = false;
    }
    else {
        [menuNegFlash setState: NSOnState];
        ac::isNegative = true;
        ac::switch_Back = true;
        ac::isNegative = false;
        [menuNegative setState: NSOffState];
    }
}

- (IBAction) setOption_BlurFirst: (id) sender {
    NSInteger checkedState = [menuBlurFirst state];
    if(checkedState == NSOnState) {
        [menuBlurFirst setState: NSOffState];
        ac::blur_First = false;
    }
    else {
        [menuBlurFirst setState: NSOnState];
        ac::blur_First = true;
    }
}

- (IBAction) setOption_BlurSecond: (id) sender {
    NSInteger checkedState = [menuBlurSecond state];
    if(checkedState == NSOnState) {
        [menuBlurSecond setState: NSOffState];
        ac::blur_Second = false;
    }
    else {
        [menuBlurSecond setState: NSOnState];
        ac::blur_Second = true;
    }
}

- (IBAction) setOption_SecondPass: (id) sender {
    NSInteger checkedState = [menuSecondPass state];
    if(checkedState == NSOnState) {
        [menuSecondPass setState: NSOffState];
        ac::pass2_enabled = false;
    }
    else {
        [menuSecondPass setState: NSOnState];
        ac::pass2_enabled = true;
    }
}

- (IBAction) setOption_ReverseColors: (id) sender {
    NSInteger checkedState = [menuReverseColors state];
    if(checkedState == NSOnState) {
        [menuReverseColors setState: NSOffState];
        ac::iRev = false;
    }
    else {
        [menuReverseColors setState: NSOnState];
        ac::iRev = true;
    }
}

- (IBAction) setOption_SlideShow: (id) sender {
    NSInteger checkedState = [menuSlideShow state];
    if(checkedState == NSOnState) {
        [menuSlideShow setState: NSOffState];
        ac::slide_Show = false;
    }
    else {
        [menuSlideShow setState: NSOnState];
        ac::slide_Show = true;
    }
}

- (IBAction) setOption_SlideRand: (id) sender {
    NSInteger checkedState = [menuSlideRand state];
    if(checkedState == NSOnState) {
        [menuSlideRand setState: NSOffState];
        ac::slide_Rand = false;
    }
    else {
        [menuSlideRand setState: NSOnState];
        ac::slide_Rand = true;
    }
}

- (IBAction) changeOrder: (id) sender {
    ac::color_order = (int) [corder indexOfSelectedItem];
}

- (IBAction) pauseProgram: (id) sender {
    NSInteger checkedState = [menuPaused state];
    if(checkedState == NSOnState) {
        [menuPaused setState: NSOffState];
        isPaused = false;
    } else {
        [menuPaused setState: NSOnState];
        isPaused = true;
    }
}

- (IBAction) goto_Frame: (id) sender {
    int val = (int)[frame_slider integerValue];
    jumptoFrame(val);
}

- (IBAction) setGoto: (id) sender {
    NSInteger time_val = [frame_slider integerValue];
    NSString *str_val = [NSString stringWithFormat:@"Jump to Time: %f Seconds Frame #%d", time_val/ac::fps, (int)time_val];
    [goto_fr setStringValue: str_val];
    
}
- (IBAction) openGoto: (id) sender {
    if(total_frames != 0) {
        [goto_frame orderFront:self];
    } else {
        NSRunAlertPanel(@"Cannot jump to frame from webcam feed", @"Recording from Webcam", @"Ok", nil, nil);
    }
}

- (IBAction) pauseVideo: (id) sender {
    
}

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

@end


void custom_filter(cv::Mat &frame) {
    NSInteger len = [custom_array count];
    for(NSInteger i = 0; i < len; ++i) {
        NSNumber *num = [custom_array objectAtIndex:i];
        NSInteger index = [num integerValue];
        ac::draw_func[(int)index](frame);
        [num release];
    }
}

void setSliders(int frame_count) {
    [frame_slider setMinValue: 0];
    [frame_slider setMaxValue: frame_count];
}

void ac::plugin(cv::Mat &frame) {
	
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
	
	
	
}



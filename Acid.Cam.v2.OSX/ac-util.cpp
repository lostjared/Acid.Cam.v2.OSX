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


#include "ac.h"

cv::Vec3b range_low(40, 40, 40), range_high(40, 40, 40);
cv::Vec3b gray_color(100, 100, 100);
std::vector<ac::Keys> blocked_color_keys;
int pixel_collection_value = 55;

namespace ac {
    int allocated_frames = 0;
    int allocated_max = 1500;
    bool release_frames = false;
    cv::VideoCapture v_cap;
    int color_map_color = 0;
    unsigned int color_value_r[256], color_value_g[256], color_value_b[256];
    bool range_enabled = false;
    int vwait = 5;
    int color_level = 125;
}

void ac::init() {
    fill_filter_map();
    
    for(int red = 0; red < 255; ++red){
        color_value_r[red] = red;
    }
    for(int green = 0; green < 255; ++green){
        color_value_g[green] = green;
    }
    for(int blue = 0; blue < 255; ++blue){
        color_value_b[blue] = blue;
    }

}

void ac::setMaxAllocated(const int &v) {
    allocated_max = v;
}

void ac::setAllocatedFrames(const int &v) {
    allocated_frames = v;
}

int ac::getMaxAllocated() {
    return allocated_max;
}

int ac::getCurrentAllocatedFrames() {
    return allocated_frames;
}

void ac::setPixelCollection(int value) {
    pixel_collection_value = value;
}
int  ac::getPixelCollection() {
    return pixel_collection_value;
}

void ac::setVariableWait(int wait) {
    vwait = wait;
}

int ac::getVariableWait() {
    return vwait;
}


void ac::setColorRangeLowToHigh(cv::Vec3b low, cv::Vec3b high) {
    int start = low[2];
    int diff = high[2]-low[2];
    if(diff <= 0) diff = 1;
    if(diff <= 0) diff = 1;
    diff = 255/diff;
    for(int red = 0; red < 255; ++red) {
        color_value_r[red] = start;
        if((red%diff)==0 && start < high[2]) {
            start ++;
        }
    }
    start = low[1];
    diff = high[1]-low[1];
    if(diff <= 0) diff = 1;
    diff = 255/diff;
    if(diff <= 0) diff = 1;
    
    for(int green = 0; green < 255; ++green){
        color_value_g[green] = start;
        if((green%diff)==0 && start < high[1]) {
            start ++;
        }
    }
    start = low[0];
    diff = high[0]-low[0];
    if(diff <= 0) diff = 1;
    diff = 255/diff;
    if(diff <= 0) diff = 1;
    for(int blue = 0; blue < 255; ++blue){
        color_value_b[blue] = start;
        if((blue%diff)==0 && start < high[0]) {
            start ++;
        }
    }
}

void ac::applyColorRange(cv::Mat &frame) {
    
    if(getColorRangeEnabled() == false)
        return;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            unsigned int val = 0;
            val = cv::saturate_cast<unsigned char>(color_value_r[pixel[2]]);
            pixel[2] = val;
            val = cv::saturate_cast<unsigned char>(color_value_g[pixel[1]]);
            pixel[1] = val;
            val = cv::saturate_cast<unsigned char>(color_value_b[pixel[0]]);
            pixel[0] = val;
        }
    }
    AddInvert(frame);
}

void ac::ApplyColorRangeInverted(cv::Mat &frame) {
    
    if(getColorRangeEnabled() == false)
        return;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            unsigned int val = 0;
            val = cv::saturate_cast<unsigned char>(color_value_r[~pixel[2]]);
            pixel[2] = val;
            val = cv::saturate_cast<unsigned char>(color_value_g[~pixel[1]]);
            pixel[1] = val;
            val = cv::saturate_cast<unsigned char>(color_value_b[~pixel[0]]);
            pixel[0] = val;
        }
    }
    AddInvert(frame);
}

void ac::setColorRangeEnabled(bool e) {
    range_enabled = e;
}

bool ac::getColorRangeEnabled() {
    return range_enabled;
}

void ac::MedianBlur(cv::Mat &frame, unsigned int value) {
    cv::UMat blur, out;
    blur = frame.getUMat(cv::ACCESS_FAST);
    cv::medianBlur(blur, out, value);
    out.copyTo(frame);
}

// Apply color map to cv::Mat
void ac::ApplyColorMap(cv::Mat &frame) {
    if(set_color_map > 0 && set_color_map < 21) {
        cv::Mat output_f1 = frame.clone();
        cv::applyColorMap(output_f1, frame, (int)set_color_map-1);
        const int w = frame.cols;
        const int h = frame.rows;
        color_map_set = true;
        for(int z = 0; z < h; ++z) {
            for(int i = 0; i < w; ++i) {
                ac::swapColors(frame, z, i);
                if(isNegative) ac::invert(frame, z, i);
            }
        }
        color_map_set = false;
    }
}

void ac::setColorMap(int map, cv::Mat &frame) {
    cv::Mat output_f1 = frame.clone();
    cv::applyColorMap(output_f1, frame, (int)map);
}

// set cv::Mat brightness
void ac::setBrightness(cv::Mat &frame, double alpha, int beta) {
    cv::Mat c = frame.clone();
    c.convertTo(frame, -1, alpha, beta);
}

// set cv::Mat gamma
void ac::setGamma(cv::Mat &frame, cv::Mat &outframe, const double gamma) {
    cv::Mat lookUpTable(1, 256, CV_8U);
    uchar* p = lookUpTable.ptr();
    for(int i = 0; i < 256; ++i) {
        p[i] = cv::saturate_cast<unsigned char>(pow(i / 255.0, gamma) * 255.0);
    }
    cv::Mat res = frame.clone();
    LUT(frame, lookUpTable, outframe);
}

// set cv::Mat saturation
void ac::setSaturation(cv::Mat &frame, int saturation) {
    cv::Mat image;
    cv::cvtColor(frame, image, cv::COLOR_BGR2HSV);
    const int w = frame.cols;
    const int h = frame.rows;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = image.at<cv::Vec3b>(z, i);
            pixel[1] = static_cast<unsigned char>(saturation);
        }
    }
    cv::cvtColor(image, frame, cv::COLOR_HSV2BGR);
}

void ac::Negate(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = ~pixel[j];
        }
    }
}

void ac::Add(cv::Mat &src, cv::Mat &add, bool sat) {
    if(src.size() != add.size())
        return;
    if(src.empty() || add.empty())
        return;
    
    for(int z = 0; z < src.rows; ++z) {
        for(int i = 0; i < src.cols; ++i) {
            cv::Vec3b &pixel = src.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = add.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = (sat == true) ? cv::saturate_cast<unsigned char>(pixel[j]+pix[j]) : static_cast<unsigned char>(pixel[j]+pix[j]);
        }
    }
}

void ac::Sub(cv::Mat &src, cv::Mat &sub, bool sat) {
    if(src.size() != sub.size())
        return;
    if(src.empty() || sub.empty())
        return;
    
    for(int z = 0; z < src.rows; ++z) {
        for(int i = 0; i < src.cols; ++i) {
            cv::Vec3b &pixel = src.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = sub.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = (sat == true) ? cv::saturate_cast<unsigned char>(pixel[j]-pix[j]) : static_cast<unsigned char>(pixel[j]-pix[j]);
        }
    }
}



void ac::ScalarAverage(const cv::Mat &frame, cv::Scalar &s) {
    s = cv::Scalar();
    if(frame.empty()) return;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b col = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                s[j] += col[j];
        }
    }
    unsigned long total_pixels = frame.rows * frame.cols;
    for(int j = 0; j < 3; ++j)
        s[j] /= total_pixels;
}

void ac::TotalAverageOffset(cv::Mat &frame, unsigned long &value) {
    if(frame.empty()) return;
    value = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            value += (pixel[0]+pixel[1]+pixel[2]);
        }
    }
    value /= (frame.rows * frame.cols);
}

void ac::setColorKeyRange(cv::Vec3b low, cv::Vec3b high) {
    range_low = low;
    range_high = high;
}

void ac::setBlockedColorKeys(std::vector<ac::Keys> &blocked) {
    blocked_color_keys = blocked;
}


bool ac::colorBounds(const cv::Vec3b &color, const cv::Vec3b &pixel, const cv::Vec3b &range_passed_low, const cv::Vec3b &range_passed_high) {
    bool result = true;
    for(int i = 0; i < 3; ++i) {
        if(!(color[i] <= cv::saturate_cast<unsigned char>(pixel[i]+range_passed_low[i]) && color[i] >= cv::saturate_cast<unsigned char>(pixel[i]-range_passed_high[i]))) {
            result = false;
            break;
        }
    }
    return result;
}


bool ac::compareColor(const cv::Vec3b &color, const cv::Vec3b &low,const cv::Vec3b &high) {
    if((color[0] >= low[0] && color[0] <= high[0]) && (color[1] >= low[1] && color[1] <= high[1]) && (color[2] >= low[2] && color[2] <= high[2]))
        return true;
    return false;
}


ac::SearchType ac::searchColors(const cv::Vec3b &color) {
    for(unsigned int i = 0; i < blocked_color_keys.size(); ++i) {
        if(compareColor(color, blocked_color_keys[i].low, blocked_color_keys[i].high) == true) {
           if(blocked_color_keys[i].spill == true) {
                return SEARCH_GRAY;
            }
            else {
                return SEARCH_PIXEL;
            }
        }
    }
    return SEARCH_NOTFOUND;
}

void ac::setGrayColor(const cv::Vec3b &color) {
    gray_color = color;
}

void ac::filterColorKeyed(const cv::Vec3b &color, const cv::Mat &orig, const cv::Mat &filtered, cv::Mat &output) {
    if(orig.size()!=filtered.size()) {
        std::cerr << "filterColorKeyed: Error not same size...\n";
        return;
    }
    output = orig.clone();
    cv::Mat color_video;
    if(colorkey_replace == true && v_cap.isOpened()) {
        double pos = v_cap.get(cv::CAP_PROP_POS_FRAMES);
        double total_frames = v_cap.get(cv::CAP_PROP_FRAME_COUNT);
        if(pos > total_frames-2) {
            v_cap.set(cv::CAP_PROP_POS_FRAMES,0);
            pos = 0;
        }
        v_cap >> color_video;
    }
    for(int z = 0; z < orig.rows; ++z) {
        for(int i = 0; i < orig.cols; ++i) {
            if(colorkey_replace == true && v_cap.isOpened()) {
                int cX = AC_GetFX(color_video.cols, i, orig.cols);
                int cY = AC_GetFZ(color_video.rows, z, orig.rows);
                cv::Vec3b add_i = color_video.at<cv::Vec3b>(cY, cX);
                cv::Vec3b &dst = output.at<cv::Vec3b>(z, i);
                cv::Vec3b pixel = orig.at<cv::Vec3b>(z, i);
                cv::Vec3b fcolor = filtered.at<cv::Vec3b>(z, i);
                SearchType srch = searchColors(pixel);
                if(colorBounds(color, pixel, range_low, range_high) || srch == SEARCH_PIXEL) {
                    dst = add_i;
                }
                else if(srch == SEARCH_GRAY) {
                    dst = gray_color;
                } else {
                    dst = fcolor;
                }
            }
            else if(colorkey_filter == true) {
                cv::Vec3b &dst = output.at<cv::Vec3b>(z, i);
                cv::Vec3b pixel = orig.at<cv::Vec3b>(z, i);
                cv::Vec3b fcolor = filtered.at<cv::Vec3b>(z, i);
                SearchType srch = searchColors(pixel);
                if(colorBounds(color,pixel,range_low, range_high) || srch == SEARCH_PIXEL) {
                    dst = fcolor;
                }
                else if(srch == SEARCH_GRAY) {
                    dst = gray_color;
                }
                else {
                    dst = pixel;
                }
            } else if(colorkey_set == true && !color_image.empty()) {
                int cX = AC_GetFX(color_image.cols, i, orig.cols);
                int cY = AC_GetFZ(color_image.rows, z, orig.rows);
                cv::Vec3b add_i = color_image.at<cv::Vec3b>(cY, cX);
            	if(add_i == color) {
	                cv::Vec3b pixel = filtered.at<cv::Vec3b>(z, i);
	                cv::Vec3b &dst = output.at<cv::Vec3b>(z, i);
	                dst = pixel;
            	}
            } else if(colorkey_bg == true && !color_bg_image.empty()) {
                int cX = AC_GetFX(color_bg_image.cols, i, orig.cols);
                int cY = AC_GetFZ(color_bg_image.rows, z, orig.rows);
                cv::Vec3b add_i = color_bg_image.at<cv::Vec3b>(cY, cX);
                cv::Vec3b &dst = output.at<cv::Vec3b>(z, i);
                cv::Vec3b pixel = filtered.at<cv::Vec3b>(z, i);
                if(add_i == color)
                    dst = pixel;
                else
                    dst = add_i;
            } else if(colorkey_replace == true && !color_replace_image.empty()) {
                int cX = AC_GetFX(color_replace_image.cols, i, orig.cols);
                int cY = AC_GetFZ(color_replace_image.rows, z, orig.rows);
                cv::Vec3b add_i = color_replace_image.at<cv::Vec3b>(cY, cX);
                cv::Vec3b &dst = output.at<cv::Vec3b>(z, i);
                cv::Vec3b pixel = orig.at<cv::Vec3b>(z, i);
                cv::Vec3b fcolor = filtered.at<cv::Vec3b>(z, i);
                SearchType srch = searchColors(pixel);
                if(colorBounds(color, pixel, range_low, range_high) || srch == SEARCH_PIXEL) {
                	dst = add_i;
                }
                else if(srch == SEARCH_GRAY) {
                    dst = gray_color;
                } else {
                    dst = fcolor;
                }
            }
        }
    }
}
// Alpha Blend function
void ac::AlphaBlend(const cv::Mat &one, const cv::Mat &two, cv::Mat &output,double alpha) {
    if(one.size() != two.size()) {
        return;
    }
    
    if(output.empty() || output.size() != one.size())
        output.create(one.size(), CV_8UC3);
    
    for(int z = 0; z < one.rows; ++z) {
        for(int i = 0; i < one.cols; ++i) {
            cv::Vec3b pix[2];
            cv::Vec3b &pixel = output.at<cv::Vec3b>(z, i);
            pix[0] = one.at<cv::Vec3b>(z, i);
            pix[1] = two.at<cv::Vec3b>(z, i);
            pixel[0] = static_cast<unsigned char>((pix[0][0] * alpha) + (pix[1][0] * alpha));
            pixel[1] = static_cast<unsigned char>((pix[0][1] * alpha) + (pix[1][1] * alpha));
            pixel[2] = static_cast<unsigned char>((pix[0][2] * alpha) + (pix[1][2] * alpha));
        }
    }
}

void ac::AlphaBlendDouble(const cv::Mat &one, const cv::Mat &two, cv::Mat &output, double alpha1, double alpha2) {
    if(one.size() != two.size()) {
        return;
    }
    if(output.empty() || output.size() != one.size())
        output.create(one.size(), CV_8UC3);
    
    for(int z = 0; z < one.rows; ++z) {
        for(int i = 0; i < one.cols; ++i) {
            cv::Vec3b pix[2];
            cv::Vec3b &pixel = output.at<cv::Vec3b>(z, i);
            pix[0] = one.at<cv::Vec3b>(z, i);
            pix[1] = two.at<cv::Vec3b>(z, i);
            pixel[0] = static_cast<unsigned char>((pix[0][0] * alpha1) + (pix[1][0] * alpha2));
            pixel[1] = static_cast<unsigned char>((pix[0][1] * alpha1) + (pix[1][1] * alpha2));
            pixel[2] = static_cast<unsigned char>((pix[0][2] * alpha1) + (pix[1][2] * alpha2));
        }
    }
}

void ac::RealAlphaBlend(const cv::Mat &one, const cv::Mat &two, cv::Mat &output, double alpha) {
    if(one.size() != two.size()) {
        return;
    }
    AlphaBlendDouble(one, two, output, alpha, 1-alpha);
}

void ac::AlphaXorBlend(const cv::Mat &one, const cv::Mat &two, cv::Mat &output, double alpha) {
    if(one.size() != two.size()) {
        return;
    }
    
    if(alpha <= 1)
        alpha = 1;
    
    if(output.empty() || output.size() != one.size())
        output.create(one.size(), CV_8UC3);
    
    for(int z = 0; z < one.rows; ++z) {
        for(int i = 0; i < one.cols; ++i) {
            cv::Vec3b pix[2];
            cv::Vec3b &pixel = output.at<cv::Vec3b>(z, i);
            pix[0] = one.at<cv::Vec3b>(z, i);
            pix[1] = two.at<cv::Vec3b>(z, i);
            pixel[0] = static_cast<unsigned char>((pix[0][0] * static_cast<unsigned char>(alpha)) ^ (pix[1][0] * static_cast<unsigned char>(alpha)));
            pixel[1] = static_cast<unsigned char>((pix[0][1] * static_cast<unsigned char>(alpha)) ^ (pix[1][1] * static_cast<unsigned char>(alpha)));
            pixel[2] = static_cast<unsigned char>((pix[0][2] * static_cast<unsigned char>(alpha)) ^ (pix[1][2] * static_cast<unsigned char>(alpha)));
        }
    }
    
}

void ac::Xor(cv::Mat &dst, const cv::Mat &add) {
    if(dst.size() != add.size()) return;
    for(int z = 0; z < dst.rows; ++z) {
        for(int i = 0; i < dst.cols; ++i) {
            cv::Vec3b &pixel = dst.at<cv::Vec3b>(z, i);
            cv::Vec3b other = add.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = pixel[j]^other[j];
        }
    }
}

void ac::Xor(const cv::Mat &input, const cv::Mat &add, cv::Mat &output) {
   if(input.size() != add.size())
        return;
    
    if(output.empty() || output.size() != input.size())
        output.create(input.size(), CV_8UC3);

    for(int z = 0; z < output.rows; ++z) {
        for(int i = 0; i < output.cols; ++i) {
            cv::Vec3b &pixel = output.at<cv::Vec3b>(z, i);
            cv::Vec3b src = input.at<cv::Vec3b>(z, i);
            cv::Vec3b other = add.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = src[j]^other[j];
        }
    }
}


bool ac::reset_alpha = false;

void ac::resetAlpha(int &dir, double &alpha) {
    if(reset_alpha == true) {
        alpha = 1.0;
        dir = 1;
    }
}

void ac::resetAlpha(double &alpha) {
    if(reset_alpha == true) {
        alpha = 1.0;
    }
}

void ac::AddInvert(cv::Mat &frame) {
    if(in_custom == true) return;
    if(swapColorOn == true) return;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
}

int thread_count = 4;

void ac::setThreadCount(const int &threads) {
    thread_count = threads;
}

int ac::getThreadCount() {
    return thread_count;
}


void ac::ac_resize(const cv::Mat &src, cv::Mat &dst, cv::Size scale) {
    if(dst.empty())
        dst.create(src.rows, src.cols, CV_8UC3);
    cv::UMat copy1 = src.getUMat(cv::ACCESS_FAST), outf;
    cv::resize(copy1, outf, scale);
    outf.copyTo(dst);
}

void ac::ac_resize(const cv::UMat &copy1, cv::Mat &dst, cv::Size scale) {
    cv::UMat out;
    if(dst.empty())
        dst.create(copy1.rows, copy1.cols, CV_8UC3);
    cv::resize(copy1, out, scale);
    out.copyTo(dst);
}

// Make two copies of the current frame, apply filter1 to one, filter2 to the other
// then Alpha Blend them together
void ac::filterFade(cv::Mat &frame, int filter1, int filter2, double alpha, int alpha_mode) {
    const int h = frame.rows; // frame height
    const int w = frame.cols;// framew idth
    // make copies of original frame
    cv::Mat frame1 = frame.clone(), frame2 = frame.clone();
    // apply filters on two copies of original frame
    CallFilter(filter1, frame1);
    CallFilter(filter2, frame2);
    // loop through image setting each pixel with alphablended pixel
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i); // target pixel
            cv::Vec3b frame1_pix = frame1.at<cv::Vec3b>(z, i); // frame1 pixel
            cv::Vec3b frame2_pix = frame2.at<cv::Vec3b>(z, i); // frame2 pixel
            // loop through pixel components and set target pixel to alpha blended pixel of two frames
            for(int q = 0; q < 3; ++q)
                if(alpha_mode == 0)
                    pixel[q] = static_cast<unsigned char>(frame2_pix[q]+(frame1_pix[q]*alpha));
                else if(alpha_mode == 1)
                    pixel[q] = static_cast<unsigned char>((frame2_pix[q] * (1-alpha)) +(frame1_pix[q]* alpha));
                    
        }
    }
}

void ac::copyMat(const cv::Mat &src, const Rect &srcrc, cv::Mat &target, const Rect &rc) {
    for(int i = 0; i < rc.w; ++i) {
        for(int z = 0; z < rc.h; ++z) {
            if(rc.y+z < target.rows && rc.x+i < target.cols) {
                cv::Vec3b &dst = target.at<cv::Vec3b>(rc.y+z, rc.x+i);
                cv::Vec3b srcp = src.at<cv::Vec3b>(srcrc.y+z, srcrc.x+i);
                dst = srcp;
            }
        }
    }
}

void ac::copyMat(cv::Mat &frame, const cv::Mat &cpy, int x, int y) {
    for(int i = x; i < x+cpy.cols && i < frame.cols; ++i) {
        for(int z = y; z < y+cpy.rows && z < frame.rows; ++z) {
            if((i < frame.cols) && (z < frame.rows) && (i >= 0 && z >= 0)) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = cpy.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
    }
}

void ac::copyMatSize(cv::Mat &frame, const cv::Mat &frame_cpy, int offset_x, int offset_y) {
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            int c_x = offset_x+i;
            int c_y = offset_y+z;
            if(c_x < 0 || c_y < 0 || c_x > frame.cols-1 || c_y > frame.rows-1 || c_x > frame_cpy.cols-1 || c_y > frame_cpy.rows-1)
                continue;
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel = frame_cpy.at<cv::Vec3b>(c_y, c_x);
        }
    }
}

void ac::copyMat(cv::Mat &frame, const cv::Mat &cpy) {
    copyMat(frame, cpy, 0, 0);
}


// Copy cv::Mat
void ac::copyMat(const cv::Mat &src,int src_x, int src_y ,cv::Mat &target, const ac::Rect &rc) {
    for(int i = 0; i < rc.w; ++i) {
        for(int z = 0; z < rc.h; ++z) {
            if(src_y+z < src.rows && src_x+i < src.cols && rc.y+z < target.rows && rc.x+i < target.cols) {
                if(src_y+z < src.rows && src_x+i < src.cols && rc.y+z < target.rows && rc.x+i < target.cols) {
                    cv::Vec3b &pixel = target.at<cv::Vec3b>(rc.y+z, rc.x+i);
                    cv::Vec3b src_pixel = src.at<cv::Vec3b>(src_y+z, src_x+i);
                    pixel = src_pixel;
                }
            }
        }
    }
}

void ac::copyMat(const cv::Mat &src, const Point &p, cv::Mat &target, const ac::Rect &rc) {
    copyMat(src, p.x, p.y, target, rc);
}

void ac::copyMat(const cv::Mat &src, int x, int y, cv::Mat &target, int rx, int ry, int rw, int rh) {
    copyMat(src, x,y,target,Rect(rx,ry,rw,rh));
}

void ac::fillRect(cv::Mat &m, const Rect &r, cv::Vec3b pixel) {
    for(int i = r.x; i < r.x+r.w; ++i) {
        for(int z = r.y; z < r.y+r.h; ++z) {
            ASSERT(i < m.cols && z < m.rows);
            cv::Vec3b &pix = m.at<cv::Vec3b>(z, i);
            pix = pixel;
        }
    }
}

ac::Log ac::log_func = 0;

void ac::setLogCallback(Log log) {
    log_func = log;
}
void ac::log_print(std::string s) {
    if(log_func != 0)
        log_func(s);
}


// set custom callback
void ac::setCustom(DrawFunction f) {
    custom_callback = f;
}

// call custom fitler defined elsewhere
void ac::custom(cv::Mat &frame) {
    if(custom_callback != 0)
        custom_callback(frame);
}

void ac::setPlugin(DrawFunction f) {
    plugin_func = f;
}

void ac::plugin(cv::Mat &frame) {
    if(plugin_func != 0) {
        plugin_func(frame);
    }
}

ac::DrawFunction ac::getFilter(std::string name) {
    if(filter_map_str.find(name) != filter_map_str.end())
    	return filter_map_str[name].second;
    return filter_map_str["MedianBlend"].second;
}

bool ac::testSize(cv::Mat &frame) {
    if(frame.cols < frame.cols/64 || frame.rows < frame.rows/64)
        return false;
    
    return true;
}

ac::DrawFunction ac::getRandomFilter() {
    return Random_Filter;
}

int ac::subfilter = -1;

void ac::setSubFilter(int value) {
    subfilter = value;
}

void ac::clearSubFilter() {
    subfilter = -1;
}

void ac::DarkenImage(cv::Mat &frame, unsigned int size) {
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] /= size;
            }
        }
    }
}

void ac::SwapColors(cv::Vec3b &cur) {
    cv::Vec3b temp = cur;
    int color_order = rand()%5;
    switch(color_order) {
        case 1:            cur[0] = temp[2];
            cur[1] = temp[1];
            cur[2] = temp[0];
            break;
        case 2:
            cur[0] = temp[1];
            cur[1] = temp[0];
            break;
        case 3:
            cur[1] = temp[2];
            cur[2] = temp[1];
            break;
        case 4:
            cur[0] = temp[1];
            cur[1] = temp[2];
            cur[2] = temp[0];
            break;
    }
}

void ac::FillRow(cv::Mat &frame, unsigned int row, unsigned char value) {
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[row] = value;
        }
    }
}

void ac::Shuffle(int &index, cv::Mat &frame, std::vector<std::string> &filter_array) {
    static auto rng = std::default_random_engine(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    CallFilter(filter_array[index], frame);
    ++index;
    if(static_cast<unsigned int>(index) > filter_array.size()-1) {
        index = 0;
        std::shuffle(filter_array.begin(), filter_array.end(),rng);
    }
}

void ac::AddMatVector(cv::Mat &frame, std::vector<cv::Mat> &v) {
    for(unsigned int i = 0; i < v.size(); ++i)
        Add(frame, v[i]);
}

unsigned char ac::size_cast(long val) {
    if(val >= 255) return 255;
    if(val < 0) return 0;
    return static_cast<unsigned char>(val);
}

unsigned char ac::size_reset(long val) {
    if(val >= 255 || val <= 0) return rand()%255;
    return static_cast<unsigned char>(val);
}

void ac::blendFilterWithColorMap(int filter, int map, cv::Mat &frame) {
    cv::Mat copyf = frame.clone(), copyi = frame.clone();
    setColorMap(map, copyf);
    CallFilter(filter, copyf);
    AlphaBlend(copyf, copyi, frame, 0.5);
}

void ac::SwitchOrder(cv::Vec3b &cur, int color_order) {
    cv::Vec3b temp = cur;
    switch(color_order) {
        case 1: // RGB
            cur[0] = temp[2];
            cur[1] = temp[1];
            cur[2] = temp[0];
            break;
        case 2:// GBR
            cur[0] = temp[1];
            cur[1] = temp[0];
            break;
        case 3:// BRG
            cur[1] = temp[2];
            cur[2] = temp[1];
            break;
        case 4: // GRB
            cur[0] = temp[1];
            cur[1] = temp[2];
            cur[2] = temp[0];
            break;
    }
}

std::vector<int> subfilters;

void ac::pushSubFilter(int newsub) {
    if(newsub >= 0) {
        subfilters.push_back(subfilter);
        subfilter = newsub;
    }
}

void ac::popSubFilter() {
    auto subsize = subfilters.size();
    if(subsize > 0) {
        subfilter = subfilters[subsize-1];
        subfilters.pop_back();
    }
}

int ac::getFilterByName(const std::string &n) {
    auto it = filter_map.find(n);
    if(it == filter_map.end()) {
        std::cerr << "getFilterByName: Error invalid filter name...\n";
        return -1;
    }
    return it->second;
}


void ac::AlphaMovement(double *alpha, int *dir, double inc) {
    if(alpha_increase != 0) inc = alpha_increase;
    for(int i = 0; i < 2; ++i) {
        if(dir[i] == 0) {
            alpha[i] -= inc;
            if(alpha[i] <= 0.1)
                dir[i] = 1;
        } else {
            alpha[i] += inc;
            if(alpha[i] >= 1.0)
                dir[i] = 0;
        }
        resetAlpha(dir[i], alpha[i]);
    }
}


void ac::AlphaMovementMaxMin(double &alpha, int &dir, double speed, double max, double min) {
    if(alpha_increase != 0) speed = alpha_increase;
    switch(getProcMode()) {
        case MOVEINOUT_INC:
        case MOVEINOUT:
            if(dir == 1) {
                alpha += speed;
                if(alpha > max) {
                    alpha = max;
                    dir = 0;
                }
            } else {
                alpha -= speed;
                if(alpha < min) {
                    alpha = min;
                    dir = 1;
                }
            }
            break;
        case MOVERESET:
            alpha += speed;
            if(alpha >= max) {
                alpha = min;
            }
            break;
    }
    resetAlpha(dir, alpha);
}

void ac::AlphaXorBlendDouble(const cv::Mat &one, const cv::Mat &two, cv::Mat &output, double alpha1, double alpha2) {
    if(one.size() != two.size()) {
        return;
    }
    if(alpha1 <= 1)
        alpha1 = 1;
    if(alpha2 <= 1)
        alpha2 = 1;
    if(output.empty() || output.size() != one.size())
        output.create(one.size(), CV_8UC3);
    
    for(int z = 0; z < one.rows; ++z) {
        for(int i = 0; i < one.cols; ++i) {
            cv::Vec3b pix[2];
            cv::Vec3b &pixel = output.at<cv::Vec3b>(z, i);
            pix[0] = one.at<cv::Vec3b>(z, i);
            pix[1] = two.at<cv::Vec3b>(z, i);
            pixel[0] = static_cast<unsigned char>((pix[0][0] * static_cast<unsigned char>(alpha1)) ^ (pix[1][0] * static_cast<unsigned char>(alpha2)));
            pixel[1] = static_cast<unsigned char>((pix[0][1] * static_cast<unsigned char>(alpha1)) ^ (pix[1][1] * static_cast<unsigned char>(alpha2)));
            pixel[2] = static_cast<unsigned char>((pix[0][2] * static_cast<unsigned char>(alpha1)) ^ (pix[1][2] * static_cast<unsigned char>(alpha2)));
        }
    }
}

void ac::PixelScaleAlpha(cv::Mat &frame, double amt) {
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>(pixel[j]*amt);
            }
        }
    }
}

void ac::Pixelate(cv::Mat &frame, unsigned int size) {
    const int w = frame.cols;// frame width
    const int h = frame.rows;// frame heigh
    int square = size;
    for(int z = 0; z < h; z += square) {
        for(int i = 0; i < w; i += square) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int x = 0; x < square; ++x) {
                for(int y = 0; y < square; ++y) {
                    if(y+z < h && i+x < w) {
                        cv::Vec3b &pix = pixelAt(frame,y+z, i+x);
                        pix = pixel;
                    }
                }
            }
        }
    }
}

void ac::InterlaceFrames(cv::Mat &frame, const cv::Mat &copy1) {
    cv::Mat reimage = copy1.clone();
    static int start_index  = 0;
    int index = 0;
    if(start_index == 0) {
        start_index = 1;
        index = 0;
    } else {
        start_index = 0;
        index = 1;
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            if(z < frame.rows-1 && i < frame.cols-1 && z < reimage.rows-1 && i < frame.cols-1) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            if(index == 0) {
                pixel = reimage.at<cv::Vec3b>(z, i);
            } else {
                continue;
            }
            }
        }
        index = (index == 0) ? 1 : 0;
    }
}

void ac::InterlaceFrames(cv::Mat &frame, cv::Mat *items, const int num_obj) {
    int index = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel = items[index].at<cv::Vec3b>(z, i);
        }
        ++index;
        if(index > num_obj)
            index = 0;
    }
}

void ac::StretchAlphaBlendSelf(cv::Mat &frame, const int speed_x, const int speed_y, int &offset_x, int &offset_y) {
    cv::Size sizeval;
    sizeval.width = frame.cols-offset_x;
    sizeval.height = frame.rows-offset_y;
    if((offset_x >= frame.cols-10) || (offset_y >= frame.rows-10)) {
        offset_x = 1;
        offset_y = 1;
    } else {
        offset_x += speed_x;
        offset_y += speed_y;
    }
    cv::Mat reimage;
    ac_resize(frame, reimage, sizeval);
    static double alpha = 0.5;
    for(int z = 0; z < reimage.rows; ++z) {
        for(int i = 0; i < reimage.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pixi = reimage.at<cv::Vec3b>(z, i);
            
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>(pixel[j]*alpha) + static_cast<unsigned char>(pixi[j]*alpha);
        }
    }
}

void ac::StretchAlphaBlendSelf(cv::Mat &frame, int &dir, const int &speed_x, const int &speed_y, int &offset_x, int &offset_y, const int &size_x, const int &size_y) {
    cv::Size sizeval;
    sizeval.width = frame.cols-offset_x;
    sizeval.height = frame.rows-offset_y;
    if(dir == 1) {
        offset_x += speed_x;
        offset_y += speed_y;
        if((offset_x >= size_x-10 && speed_x > 0) || (offset_y >=size_y-10 && speed_y > 0)) {
            dir = 0;
        }
    } else {
        offset_x -= speed_x;
        offset_y -= speed_y;
        if((offset_x <= 10 && speed_x > 0) || (offset_y <= 10 && speed_y > 0)) {
            dir = 1;
        }
    }
    cv::Mat reimage;
    ac_resize(frame, reimage, sizeval);
    static double alpha = 0.5;
    for(int z = 0; z < reimage.rows; ++z) {
        for(int i = 0; i < reimage.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b pixi = reimage.at<cv::Vec3b>(z, i);
            
            for(int j = 0; j < 3; ++j)
                pixel[j] = static_cast<unsigned char>(pixel[j]*alpha) + static_cast<unsigned char>(pixi[j]*alpha);
        }
    }
    
}

void ac::setChannelToValue(cv::Mat &frame, unsigned int channel, unsigned char value) {
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[channel] = value;
        }
    }
}

void ac::VariableScale(double *alpha,int *dir, double *start, double *start_init, double *start_max, double *stop, double *stop_init, double *stop_max, double inc) {
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            alpha[j] += inc;
            if(alpha[j] >= stop[j]) {
                stop[j] += inc;
                if(stop[j] >= stop_max[j]) {
                    stop[j] = stop_init[j];
                }
                dir[j] = 0;
            }
        } else {
            alpha[j] -= inc;
            if(alpha[j] <= start[j]) {
                start[j] -= inc;
                if(start[j] <= start_max[j]) {
                    start_max[j] = start_init[j];
                }
                dir[j] = 1;
            }
        }
    }
    for(int j = 0; j < 3; ++j) {
        ac::resetAlpha(alpha[j]);
    }
}

void ac::VariableScaleSpeed(double *alpha,int *dir, double *start, double *start_init, double *start_max, double *stop, double *stop_init, double *stop_max, double *inc) {
    for(int j = 0; j < 3; ++j) {
        if(dir[j] == 1) {
            alpha[j] += inc[j];
            if(alpha[j] >= stop[j]) {
                stop[j] += inc[j];
                if(stop[j] >= stop_max[j]) {
                    stop[j] = stop_init[j];
                }
                dir[j] = 0;
            }
        } else {
            alpha[j] -= inc[j];
            if(alpha[j] <= start[j]) {
                start[j] -= inc[j];
                if(start[j] <= start_max[j]) {
                    start_max[j] = start_init[j];
                }
                dir[j] = 1;
            }
        }
    }
    for(int j = 0; j < 3; ++j) {
        ac::resetAlpha(alpha[j]);
    }
}


ac::Pixelated::Pixelated() : x_offset(0), reset_needed(false), is_init(false) {
    
}
void ac::Pixelated::reset(cv::Mat &val) {
    pix_image = val.clone();
    copy_val = pix_image.clone();
    ac::fillRect(copy_val, ac::Rect(0, 0, copy_val.cols, copy_val.rows), cv::Vec3b(0, 0, 0));
    for(int i = 0; i < pix_image.cols; ++i) {
        for(int z = 0; z < pix_image.rows; ++z) {
            value_x.push_back(ImageIndex(i, z));
        }
    }
    std::shuffle(value_x.begin(), value_x.end(), rng);
    is_init = true;
    x_offset = 0;
    reset_needed = false;
    is_init = true;
}

void ac::Pixelated::setPixel(int amount) {
    for(int j = 0; j < amount; ++j) {
        if(resetNeeded()) {
            return;
        }
        setPix();
    }
}

void ac::Pixelated::setPix() {
    if(reset_needed == true)
        return;
    if(pix_image.size() != copy_val.size())
        return;
    ImageIndex &index = value_x[x_offset];
    cv::Vec3b pixel = pix_image.at<cv::Vec3b>(index.y, index.x);
    cv::Vec3b &src_pixel = copy_val.at<cv::Vec3b>(index.y, index.x);
    src_pixel = pixel;
    ++x_offset;
    if(static_cast<unsigned int>(x_offset) > value_x.size()-1)
        reset_needed = true;
}

void ac::Pixelated::drawToMatrix(cv::Mat &frame) {
    if(frame.size() !=  pix_image.size())
        return;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b pix = copy_val.at<cv::Vec3b>(z, i);
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] += pix[j];
        }
    }
}

void ac::InitArrayPosition(int *values, const int &index) {
    switch(index) {
        case 0:
            values[0] = 0;
            values[1] = 1;
            values[2] = 2;
            break;
        case 1:
            values[0] = 2;
            values[1] = 0;
            values[2] = 1;
            break;
        case 2:
            values[0] = 1;
            values[1] = 2;
            values[2] = 0;
            break;
    }
}

void ac::setSourceFrame(const cv::Mat &frame) {
    orig_frame = frame.clone();
}

void ac::IntertwineFrames(IntertwineDir type, const cv::Mat &one, const cv::Mat &two, cv::Mat &dest) {
    if(one.size() != two.size())
        return;
    dest = one.clone();
    if(type == IntertwineDir::VERTICAL) {
        for(int z = 0; z < one.rows; ++z) {
            for(int i = 0; i < one.cols; ++i) {
                cv::Vec3b &pixel = dest.at<cv::Vec3b>(z, i);
                if((z%2) == 0)
                    pixel = two.at<cv::Vec3b>(z, i);
                else
                    pixel = one.at<cv::Vec3b>(one.rows-z-1, i);
                    
            }
        }
    } else if(type == IntertwineDir::HORIZONTAL) {
        for(int i = 0; i < one.cols; ++i) {
            for(int z = 0; z < one.rows; ++z) {
                cv::Vec3b &pixel = dest.at<cv::Vec3b>(z, i);
                if((i%2) == 0)
                    pixel = two.at<cv::Vec3b>(z, i);
                else
                    pixel = one.at<cv::Vec3b>(z, (one.cols-i-1));
            }
        }
    }
}


void ac::AlphaBlendArray(cv::Mat &src, cv::Mat *frames, int num_frames) {
    if(num_frames == 0)
        return;
    if(src.empty())
        return;
    double amt = 1.0/num_frames;
    for(int z = 0; z < src.rows; ++z) {
        for(int i = 0; i < src.cols; ++i) {
            if(i >= 0 && i < src.cols && z >= 0 && z < src.rows) {
                cv::Vec3b &pixel = src.at<cv::Vec3b>(z, i);
                unsigned int rgb_values[3] = {0};
                for(int q = 0; q < num_frames; ++q) {
                    cv::Vec3b value = frames[q].at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        rgb_values[j] += static_cast<unsigned char>(value[j]*amt);
                    }
                }
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(rgb_values[j]);
                }
            }
        }
    }
}

ac::PixelArray2D::~PixelArray2D() {
    erase();
}

void ac::PixelArray2D::create_empty(int w, int h) {
    if(pix_values != 0) {
        erase();
    }
    pix_x = w;
    pix_y = h;
    pix_values = new PixelValues*[pix_x];
    for(int i = 0; i < pix_x; ++i) {
        pix_values[i] = new PixelValues[pix_y];
    }
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            pix_values[i][z].clear();
            pix_values[i][z].speed = 1;
            pix_values[i][z].position_x = i;
            pix_values[i][z].position_y = z;
        }
    }
}

void ac::PixelArray2D::create(cv::Mat &frame, int w, int h, int dir, bool addvec) {
    if(pix_values != 0) {
        erase();
    }
    pix_x = w;
    pix_y = h;
    pix_values = new PixelValues*[pix_x];
    for(int i = 0; i < pix_x; ++i) {
        pix_values[i] = new PixelValues[pix_y];
    }
    int offset = 0;
    int rect = 0;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pix_values[i][z].speed = 1+(rand()%4);
            pix_values[i][z].position_x = i;
            pix_values[i][z].position_y = z;
            for(int j = 0; j < 3; ++j) {
                pix_values[i][z].col[j] = pixel[j];
                pix_values[i][z].add[j] = pixel[j];
                switch(dir) {
                    case 0:
                        pix_values[i][z].dir[j] = 0;
                        break;
                    case -1:
                        pix_values[i][z].dir[j] = rand()%2;
                        break;
                    case -2:
                        pix_values[i][z].dir[j] = rand()%8;
                        break;
                    case -3:
                        if((++offset)%2 == 0)
                            pix_values[i][z].dir[j] = 1;
                        else
                            pix_values[i][z].dir[j] = 0;
                        break;
                    case -4:
                        if((++rect)%25 == 0) {
                            offset = (offset == 0) ? 1 : 0;
                        }
                         pix_values[i][z].dir[j] = offset;
                        break;
                    case -5:
                        pix_values[i][z].col[j] = rand()%255;
                        break;
                    default:
                        pix_values[i][z].dir[j] = dir;
                        break;
                        
                }
            }
        }
    }
    if(addvec == true) {
        set();
    }
}
void ac::PixelArray2D::erase() {
    if(pix_values != 0 && pix_x != 0 && pix_y != 0) {
        for(int j = 0; j < pix_x; ++j) {
            delete [] pix_values[j];
        }
        delete [] pix_values;
        pix_values = 0;
    }
}

void ac::PixelArray2D::set() {
   if(!pixel_index.empty()) {
        pixel_index.erase(pixel_index.begin(), pixel_index.end());
    }
    
    for(int z = 0; z < pix_y; ++z) {
        for(int i = 0; i < pix_x; ++i) {
            pixel_index.push_back(&pix_values[i][z]);
        }
    }
    shuffle();
}

void ac::PixelArray2D::shuffle() {
    std::shuffle(pixel_index.begin(), pixel_index.end(), rng);
}


void ac::PixelArray2D::generateMatrix(cv::Mat &frame) {
    if(frame.empty() || frame.size() != cv::Size(pix_x, pix_y)) {
        frame.create(cv::Size(pix_x, pix_y), CV_8UC3);
    }
    for(int z = 0; z < pix_y; ++z) {
        for(int i = 0; i < pix_x; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pix_values[i][z].col[j];
            }
        }
    }
}

void ac::PixelArray2D::insert(cv::Mat &image) {
    if(image.empty() || image.size() != cv::Size(pix_x, pix_y)) {
        std::cerr << "Invalid size for insert\n";
        return;
    }
    for(int z = 0; z < pix_y; ++z) {
        for(int i = 0; i < pix_x; ++i) {
            cv::Vec3b pix = image.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pix_values[i][z].col[j] = pix[j];
            }
        }
    }
}

void ac::PixelArray2D::setAll(const int &value) {
    for(int z = 0; z < pix_y; ++z) {
        for(int i = 0; i < pix_x; ++i) {
            PixelValues &pix = pix_values[i][z];
            for(int j = 0; j < 3; ++j) {
                pix.col[j] = value;
                pix.add[j] = value;
            }
        }
    }
}

void ac::PixelArray2D::setAllDirection(const int &value) {
    for(int z = 0; z < pix_y; ++z) {
        for(int i = 0; i < pix_x; ++i) {
            PixelValues &pix = pix_values[i][z];
            for(int j = 0; j < 3; ++j) {
                pix.dir[j] = value;
            }
        }
    }
}

bool ac::CopyAudioStream(std::string ffmpeg, std::string file1, std::string file2, std::string output) {
#if defined(__APPLE__) || defined(__linux__)
    std::ostringstream stream;
    stream << ffmpeg << " -y -i \"" << file1 << "\" -i \"" << file2 << "\" -c copy -map 0:v:0 -map 1:a:0? -shortest \"" << output << "\"";
    FILE *fptr = popen(stream.str().c_str(), "r");
    if(fptr == NULL) {
        std::cout << "Error could not open ffmpeg...\n";
        return false;
    }
    char buf[1024] = {0};
    while(!feof(fptr)) {
        size_t bytes = fread(buf, 1024, 1, fptr);
        buf[bytes] = 0;
        std::cout << buf;
    }
    std::cout << buf << "\n";
    if(pclose(fptr) != 0)
        return false;
    return true;
#endif
    return false;
}

bool ac::FFMPEG_Installed(const std::string &ffmpeg) {
    return true;
}

void ac::swapColorState(const bool &b) {
    swapColorOn = b;
}

bool ac::getColorState() {
    return swapColorOn;
}

bool ac::VideoFrame(cv::Mat &frame) {
    if(!v_cap.isOpened())
        return false;
    double pos = v_cap.get(cv::CAP_PROP_POS_FRAMES);
    double total_frames = v_cap.get(cv::CAP_PROP_FRAME_COUNT);
    if(pos > total_frames-100) {
        v_cap.set(cv::CAP_PROP_POS_FRAMES,0);
        pos = 0;
    }
    cv::Mat reframe;
    if(v_cap.read(reframe)) {
        frame = reframe.clone();
        return true;
    }
    return false;
}

void ac::setFilterColorMap(int color) {
    color_map_color = color;
}

int  ac::getColorMapFilter() {
    return color_map_color;
}

void ac::setColorLevel(int color) {
    color_level = color;
}
int ac::getColorLevel() {
    return color_level;
}

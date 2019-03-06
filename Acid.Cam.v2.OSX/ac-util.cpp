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


#include "ac.h"


cv::Vec3b range_low(40, 40, 40), range_high(40, 40, 40);
cv::Vec3b gray_color(100, 100, 100);
std::vector<ac::Keys> blocked_colors;

// Apply color map to cv::Mat
void ac::ApplyColorMap(cv::Mat &frame) {
    if(set_color_map > 0 && set_color_map < 13) {
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
    cv::cvtColor(frame, image, CV_BGR2HSV);
    const int w = frame.cols;
    const int h = frame.rows;
    for(int z = 0; z < h; ++z) {
        for(int i = 0; i < w; ++i) {
            cv::Vec3b &pixel = image.at<cv::Vec3b>(z, i);
            pixel[1] = static_cast<unsigned char>(saturation);
        }
    }
    cv::cvtColor(image, frame, CV_HSV2BGR);
}

void ac::Negate(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
    blocked_colors = blocked;
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
    if(color[0] >= low[0] && color[0] <= high[0] && color[1] >= low[1] && color[1] <= high[1] && color[2] >= low[2] && color[2] <= high[2])
        return true;
    return false;
}


ac::SearchType ac::searchColors(const cv::Vec3b &color) {
    for(int i = 0; i < blocked_colors.size(); ++i) {
        if(compareColor(color, blocked_colors[i].low, blocked_colors[i].high) == true) {
            if(blocked_colors[i].spill == true) {
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
    for(int z = 0; z < orig.rows; ++z) {
        for(int i = 0; i < orig.cols; ++i) {
            if(colorkey_filter == true) {
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
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            swapColors(frame, z, i);// swap colors
            if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
        }
    }
}

// Make two copies of the current frame, apply filter1 to one, filter2 to the other
// then Alpha Blend them together
void ac::filterFade(cv::Mat &frame, int filter1, int filter2, double alpha) {
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i); // target pixel
            cv::Vec3b frame1_pix = frame1.at<cv::Vec3b>(z, i); // frame1 pixel
            cv::Vec3b frame2_pix = frame2.at<cv::Vec3b>(z, i); // frame2 pixel
            // loop through pixel components and set target pixel to alpha blended pixel of two frames
            for(int q = 0; q < 3; ++q)
                pixel[q] = static_cast<unsigned char>(frame2_pix[q]+(frame1_pix[q]*alpha));
        }
    }
}

void ac::copyMat(const cv::Mat &src, const Rect &srcrc, cv::Mat &target, const Rect &rc) {
    for(int i = 0; i < rc.w; ++i) {
        for(int z = 0; z < rc.h; ++z) {
            cv::Vec3b &dst = target.at<cv::Vec3b>(rc.y+z, rc.x+i);
            cv::Vec3b srcp = src.at<cv::Vec3b>(srcrc.y+z, srcrc.x+i);
            dst = srcp;
        }
    }
}



// Copy cv::Mat
void ac::copyMat(const cv::Mat &src,int src_x, int src_y ,cv::Mat &target, const ac::Rect &rc) {
    for(int i = 0; i < rc.w; ++i) {
        for(int z = 0; z < rc.h; ++z) {
            if(src_y+z < src.rows && src_x+i < src.cols && rc.y+z < target.rows && rc.x+i < target.cols) {
                ASSERT(src_y+z < src.rows && src_x+i < src.cols && rc.y+z < target.rows && rc.x+i < target.cols);
                cv::Vec3b &pixel = target.at<cv::Vec3b>(rc.y+z, rc.x+i);
                cv::Vec3b src_pixel = src.at<cv::Vec3b>(src_y+z, src_x+i);
                pixel = src_pixel;
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

ac::DrawFunction ac::getRandomFilter(int &index) {
    static std::string values[] = {"ShuffleAlpha", "ShuffleSelf", "ShuffleMedian", "ShuffleRGB"};
    int r = rand()%4;
    index = filter_map[values[r]];
    return filter_map_str[values[r]].second;
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pixel[row] = value;
        }
    }
}

void ac::Shuffle(int &index, cv::Mat &frame, std::vector<std::string> &filter_array) {
    static auto rng = std::default_random_engine{};
    CallFilter(filter_array[index], frame);
    ++index;
    if(index > filter_array.size()-1) {
        index = 0;
        std::shuffle(filter_array.begin(), filter_array.end(),rng);
    }
}

void ac::AddMatVector(cv::Mat &frame, std::vector<cv::Mat> &v) {
    for(int i = 0; i < v.size(); ++i)
        Add(frame, v[i]);
}

unsigned char ac::size_cast(long val) {
    if(val >= 255) return 255;
    if(val < 0) return 0;
    return val;
}

unsigned char ac::size_reset(long val) {
    if(val >= 255 || val <= 0) return rand()%255;
    return val;
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
    subfilters.push_back(subfilter);
    subfilter = newsub;
}

void ac::popSubFilter() {
    auto subsize = subfilters.size();
    if(subsize > 0) {
        subfilter = subfilters[subsize-1];
        subfilters.pop_back();
    }
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
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]*amt;
            }
        }
    }
}

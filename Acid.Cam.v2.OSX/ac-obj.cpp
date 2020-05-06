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

ac::Point::Point() : x(0), y(0) {}
ac::Point::Point(const ac::Point &p) : x(p.x), y(p.y) {}
ac::Point::Point(int xx, int yy) : x(xx), y(yy) {}

ac::Point &ac::Point::operator=(const ac::Point &p) {
    x = p.x;
    y = p.y;
    return *this;
}

void ac::Point::setPoint(int xx, int yy) {
    x = xx;
    y = yy;
}

ac::Rect::Rect() : x(0), y(0), w(0), h(0) {}
ac::Rect::Rect(const ac::Rect &r) : x(r.x), y(r.y), w(r.w), h(r.h) {}
ac::Rect::Rect(int xx, int yy, int ww, int hh) : x(xx), y(yy), w(ww), h(hh) {}
ac::Rect::Rect(int xx, int yy) : x(xx), y(yy), w(0), h(0) {}
ac::Rect::Rect(int xx, int yy, cv::Size s) : x(xx), y(yy), w(s.width), h(s.height) {}
ac::Rect::Rect(Point pt, int ww, int hh) : x(pt.x), y(pt.y), w(ww), h(hh) {}
ac::Rect::Rect(Point pt, cv::Size s) : x(pt.x), y(pt.y), w(s.width), h(s.height){}

ac::Rect &ac::Rect::operator=(const ac::Rect &r) {
    x = r.x;
    y = r.y;
    w = r.w;
    h = r.h;
    return *this;
}

void ac::Rect::setRect(int xx, int yy, int ww, int hh) {
    x = xx;
    y = yy;
    w = ww;
    h = hh;
}

std::vector<ac::Frames *> ac::all_objects;
bool ac::frames_released = false;

void ac::release_all_objects() {
    int total = getCurrentAllocatedFrames();
    if(total > 0) {
        col_lock.lock();
        for(unsigned int i = 0; i < all_objects.size(); ++i) {
            if(all_objects[i] != 0) all_objects[i]->releaseFrames();
        }
        frames_released = true;
        std::ostringstream stream;
        stream << "acidcam: Released: " << total << " Matrix objects...\nIf this is happening often consider increasing maxmium frames in the Preferences Window.\n";
        std::cout<<stream.str() << "\n";
        log_print(stream.str());
        setAllocatedFrames(0);
        col_lock.unlock();
    }
}

ac::HLine::HLine() : w(0), h(0) {}

void ac::HLine::createLines(int size, int width, int height) {
    w = width;
    h = height;
    for(int i = 0; i < size; ++i) {
        int rand_y = ((rand()%height)/8);
        LineObject l;
        l.line_size = ac::Rect(0,rand_y,width,3);
        l.on = true;
        l.dir = ((rand()%2) == 1) ? 1 : 0;
        lines.push_back(l);
    }
}

void ac::HLine::drawLines(cv::Mat &frame) {
    for(unsigned int l = 0; l < lines.size(); ++l) {
        ac::Rect rc = lines[l].line_size;
        int num = rand()%50, skip = rand()%20;
        int count = 0, skip_count = 0;
        for(int x = rc.x; x < rc.x+rc.w; ++x) {
            if(count < num) {
                if(rc.y >= 0 && rc.y < frame.rows && x >= 0 && x < frame.cols) {
                    cv::Vec3b &pixel = pixelAt(frame,rc.y, x);
                    pixel[0] = pixel[1] = pixel[2] = 255;
                    ++count;
                }
            } else {
                if(skip_count >= skip) {
                    skip_count = 0;
                    count = 0;
                    num = rand()%50;
                    skip = rand()%20;
                } else {
                    ++skip_count;
                }
            }
            if(lines[l].dir == 1) {
                ++lines[l].line_size.y;
                if(lines[l].line_size.y > frame.rows) {
                    lines[l].line_size.y = rand()%frame.rows;
                    lines[l].dir = ((rand()%2) == 1) ? 1 : 0;
                }
                
            } else if(lines[l].dir == 0) {
                --lines[l].line_size.y;
                if(lines[l].line_size.y <= 0) {
                    lines[l].line_size.y = rand()%frame.rows;
                    lines[l].dir = ((rand()%2) == 1) ? 1 : 0;
                    
                }
            }
        }
    }
}

void ac::HLine::clearLines() {
    if(!lines.empty()) {
        lines.erase(lines.begin(), lines.end());
    }
}

















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

/*
 //Basic Multithreaded Filter
 auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
 for(int z = offset; z <  offset+size; ++z) {
 for(int i = 0; i < cols; ++i) {
 }
 }
 };
 UseMultipleThreads(frame, getThreadCount(), callback);
 AddInvert(frame);
 
 */

#include "ac.h"

void ac::LinesAcrossX(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 1; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            cv::Vec3b prev = pixelAt(frame,z, i-1);
            for(int j = 0; j < 3; ++j)
                pixel[j] = pixel[j]^prev[j];
        }
    }
    AddInvert(frame);
}

void ac::XorLineX(cv::Mat &frame) {
    for(int z = 0; z < frame.rows; ++z) {
        static int rgb[3] = {0};
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                rgb[j] += pixel[j];
            }
            
        }
        rgb[0] /= frame.cols;
        rgb[1] /= frame.cols;
        rgb[2] /= frame.cols;
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>(0.5 * (pixel[j]^rgb[j])) + static_cast<unsigned char>(0.5 * pixel[j]);
            }
        }
    }
    AddInvert(frame);
}

void ac::AlphaComponentIncrease(cv::Mat &frame) {
    static int index = 0;
    static double alpha = 0.1;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            pixel[index] ^= static_cast<unsigned char>(alpha * pixel[index]);
        }
    }
    ++index;
    if(index > 2)
        index = 0;
    static int dir = 1;
    if(dir == 1) {
        alpha = alpha+ 0.01;
        if(alpha > 1.0)
            dir = 0;
    } else {
        alpha = alpha- 0.01;
        if(alpha <= 0.1)
            dir = 1;
    }
    AddInvert(frame);
}

void ac::ExpandContract(cv::Mat &frame) {
    static int nw = frame.cols;
    cv::Mat copy1 = frame.clone();
    for(int z = 0;  z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            int col = AC_GetFX(frame.cols, i, nw);
            if(col >= 0 && col < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, col);
                pixel = pix;
            }
        }
        static int dir = 1;
        if(dir == 1) {
            ++nw;
            if(nw > frame.cols * 3)
                dir = 0;
        } else {
            --nw;
            if(nw < frame.cols)
                dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::MoveInThenMoveOut(cv::Mat &frame) {
    static int nw = frame.cols/3;
    cv::Mat copy1 = frame.clone();
    for(int z = 0;  z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            int col = AC_GetFX(frame.cols, i, nw);
            if(col >= 0 && col < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, col);
                pixel = pix;
            }
        }
        static int dir = 1;
        if(dir == 1) {
            ++nw;
            if(nw > frame.cols)
                dir = 0;
        } else {
            --nw;
            if(nw < frame.cols/3)
                dir = 1;
        }
    }
    AddInvert(frame);
}

void ac::MoveInThenMoveOutFast(cv::Mat &frame) {
    static int nw = frame.cols/8;
    cv::Mat copy1 = frame.clone();
    for(int z = 0;  z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            int col = AC_GetFX(frame.cols, i, nw);
            if(col >= 0 && col < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, col);
                pixel = pix;
            }
        }
        static int dir = 1;
        if(dir == 1) {
            nw += (rand()%20);
            if(nw > frame.cols)
                dir = 0;
        } else {
            nw -= (rand()%20);
            if(nw < frame.cols/8)
                dir = 1;
        }
    }
    AddInvert(frame);
}


#if !defined(NO_SCREEN_GRAB) && defined(__APPLE__)
extern void ScreenGrab(cv::Mat &frame);
#endif

void ac::CurrentDesktop(cv::Mat &frame) {
#if !defined(NO_SCREEN_GRAB) && defined(__APPLE__)
    cv::Mat cap;
    ScreenGrab(cap);
    cv::Mat temp;
    ac_resize(cap, temp, frame.size());
    cv::cvtColor(temp, frame, cv::COLOR_RGBA2BGR);
#endif
}

void ac::DistortionFuzz(cv::Mat &frame) {
    static int nw = frame.cols/8;
    cv::Mat copy1 = frame.clone();
    int rand_height = rand()%100;
    int rand_start = rand()%100;
    for(int z = 0;  z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            int col = AC_GetFX(frame.cols, i, nw);
            if(col >= 0 && col < frame.cols) {
                cv::Vec3b &pixel = pixelAt(frame,z, i);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, col);
                pixel = pix;
            }
        }
        static int dir = 1;
        if(dir == 1) {
            nw += (rand()%20);
            if(nw > rand_height) {
                dir = 0;
                rand_height = rand()%100;
            }
        } else {
            nw -= (rand()%20);
            if(nw < (frame.cols/12)+rand_start) {
                dir = 1;
                rand_start = rand()%100;
            }
        }
    }
    AddInvert(frame);
}

void ac::DistortionByRow(cv::Mat &frame) {
    static int nw = frame.rows/16;
    cv::Mat copy1 = frame.clone();
    for(int z = 0;  z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int col = AC_GetFZ(frame.rows, z, nw);
            if(col >= 0 && col < frame.rows) {
                cv::Vec3b pix = copy1.at<cv::Vec3b>(col, i);
                pixel = pix;
            
            }
        }
        ++nw;
        if(nw > frame.rows-1)
            nw = frame.rows/16;
    }
    AddInvert(frame);
}

void ac::DistortionByRowRev(cv::Mat &frame) {
    static int nw = frame.rows/16;
    cv::Mat copy1 = frame.clone();
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0;  z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int col = AC_GetFZ(frame.rows, z, nw);
            if(col >= 0 && col < frame.rows) {
                cv::Vec3b pix = copy1.at<cv::Vec3b>(col, i);
                pixel = pix;
            }
        }
        ++nw;
        if(nw > frame.rows-1)
            nw = frame.rows/16;
    }
    AddInvert(frame);
}

void ac::DistortionByRowVar(cv::Mat &frame) {
    static int nw = frame.rows/16;
    cv::Mat copy1 = frame.clone();
    static int add = 1+(rand()%25);
    for(int z = 0;  z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int col = AC_GetFZ(frame.rows, z, nw);
            if(col >= 0 && col < frame.rows) {
                cv::Vec3b pix = copy1.at<cv::Vec3b>(col, i);
                pixel = pix;
            }
        }
        nw += add;
        if(nw > frame.rows-1) {
            nw = frame.rows/16;
            add = 1+(rand()%25);
        }
    }
    AddInvert(frame);
}

void ac::DistortionByRowRand(cv::Mat &frame) {
    int nw = (frame.rows/16)+(rand()%(frame.rows-(frame.rows/16)));
    cv::Mat copy1 = frame.clone();
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0;  z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int col = AC_GetFZ(frame.rows, z, nw);
            if(col >= 0 && col < frame.rows) {
                cv::Vec3b pix = copy1.at<cv::Vec3b>(col, i);
                pixel = pix;
            }
        }
    }
    AddInvert(frame);
}

void ac::DistortionByCol(cv::Mat &frame) {
    static int nh = frame.cols/16;
    cv::Mat copy1 = frame.clone();
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int col = AC_GetFX(frame.cols, i, nh);
            if(col >= 0 && col < frame.cols) {
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, col);
                pixel = pix;
            }
        }
        nh++;
        if(nh > frame.cols)
            nh = frame.cols/16;
    }
    AddInvert(frame);
}

void ac::DistortionByColRand(cv::Mat &frame) {
    int nh = (frame.cols/16)+(rand()%(frame.cols-(frame.cols/16)));
    cv::Mat copy1 = frame.clone();
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int col = AC_GetFX(frame.cols, i, nh);
            if(col >= 0 && col < frame.cols) {
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, col);
                pixel = pix;
            }
        }
    }
    AddInvert(frame);
}

void ac::DistortionByColVar(cv::Mat &frame) {
    static int nh = frame.cols/16;
    static int add = 1+(rand()%25);
    cv::Mat copy1 = frame.clone();
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            int col = AC_GetFX(frame.cols, i, nh);
            if(col >= 0 && col < frame.cols) {
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, col);
                pixel = pix;
            }
        }
        nh += add;
        if(nh > frame.cols) {
            nh = frame.cols/16;
            add = 1+(rand()%25);
        }
    }
    AddInvert(frame);
}

void ac::LongLines(cv::Mat &frame) {
    
    for(int i = 0; i < frame.cols; ++i) {
        if(rand()%150==0) {
            for(int z = 0; z < frame.rows; ++z) {
                cv::Vec3b &pixel = pixelAt(frame, z, i);
                pixel = 0;
            }
        }
    }
    AddInvert(frame);
}

void ac::TearRight(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static int max = 50;
    
    for(int z = 0; z < frame.rows; ++z) {
        int offset = 25+(rand()%max);
        int total_size = frame.cols+offset;
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            int col = AC_GetFX(frame.cols, i, total_size);
            if(col >= 0 && col < frame.cols) {
                cv::Vec3b pix = pixelAt(copy1, z, col);
                pixel = pix;
            }
        }
    }
    AddInvert(frame);
    static int dir = 1;
    if(dir == 1) {
        max += 20;
        if(max > 400)
            dir = 0;
        
    } else {
        max -= 20;
        if(max < 50)
            dir = 1;
    }
}

namespace ac {
    int xx = 0, yy = 0, ww = 1280, hh = 720;
}

void ac::SetDesktopRect(int x, int y, int w, int h) {
    xx = x;
    yy = y;
    ww = w;
    hh = h;
}

cv::Mat ac::resizeRatio(const cv::Mat &input, const cv::Size &dstSize, const cv::Scalar &bgcolor)
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

#if !defined(NO_SCREEN_GRAB) && defined(__APPLE__)
extern void ScreenGrabRect(int x, int y, int w, int h, cv::Mat &frame);
#endif


void ac::CurrentDesktopRect(cv::Mat &frame) {
#if !defined(NO_SCREEN_GRAB) && defined(__APPLE__)
        cv::Mat cap;
        ScreenGrabRect(xx,yy,ww,hh,cap);
        cv::Mat temp;
        //ac_resize(cap, temp, frame.size());
        cv::Scalar s(0,0,0);
        temp = resizeRatio(cap, frame.size(), s);
        cv::cvtColor(temp, frame, cv::COLOR_RGBA2BGR);
#endif
}

void ac::TearDown(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static int max = 50;
    for(int i = 0; i < frame.cols; ++i) {
        int offset = 25+(rand()%max);
        int total_size = frame.rows+offset;
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            int col = AC_GetFZ(frame.rows, z, total_size);
            if(col >= 0 && col < frame.rows) {
                cv::Vec3b pix = pixelAt(copy1, col, i);
                pixel = pix;
            }
        }
    }
    AddInvert(frame);
    static int dir = 1;
    if(dir == 1) {
        max += 20;
        if(max > 400)
            dir = 0;
        
    } else {
        max -= 20;
        if(max < 50)
            dir = 1;
    }
}

void ac::TearUp(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static int max = 50;
    for(int i = 0; i < frame.cols; ++i) {
        int offset = 25+(rand()%max);
        int total_size = frame.rows-offset;
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            int col = AC_GetFZ(frame.rows, z, total_size);
            if(col >= 0 && col < frame.rows) {
                cv::Vec3b pix = pixelAt(copy1, col, i);
                pixel = pix;
            }
        }
    }
    AddInvert(frame);
    static int dir = 1;
    if(dir == 1) {
        max += 20;
        if(max > 400)
            dir = 0;
        
    } else {
        max -= 20;
        if(max < 50)
            dir = 1;
    }
}

void ac::TearLeft(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    static int max = 50;
    
    for(int z = 0; z < frame.rows; ++z) {
        int offset = 25+(rand()%max);
        int total_size = frame.cols-offset;
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            int col = AC_GetFX(frame.cols, i, total_size);
            if(col >= 0 && col < frame.cols) {
                cv::Vec3b pix = pixelAt(copy1, z, col);
                pixel = pix;
            }
        }
    }
    AddInvert(frame);
    static int dir = 1;
    if(dir == 1) {
        max += 20;
        if(max > 400)
            dir = 0;
        
    } else {
        max -= 20;
        if(max < 50)
            dir = 1;
    }
}

bool new_image_ready = false;
cv::Mat new_image;
bool program_running = false;


std::mutex in_lock;
std::deque<cv::Mat> ac::frames_;

void ac::setNewSyphonImage(cv::Mat &frame) {
    
    if(program_running == false)
        return;
    
    in_lock.lock();
    if((frame.rows <= 720 && frames_.size()<1500) || (frame.rows <= 1080 && frames_.size()<1000) || (frame.rows > 1080 && frames_.size()<200))
        frames_.push_back(frame);
    
    new_image_ready = true;
    in_lock.unlock();
}


void ac::setRunning(bool b) {
    program_running = b;
}

void ac::SyphonInputVideo(cv::Mat &frame) {
    in_lock.lock();
    if(syphon_in_enabled && !frames_.empty()) {
        cv::Scalar s(0,0,0);
        cv::Mat copy;
        cv::Mat &new_imagex = frames_[0];
        ac_resize(new_imagex, copy, frame.size());
        //copy = resizeRatio(new_imagex, frame.size(), s);
        AddInvert(copy);
        copy.copyTo(frame);
        
        if(frames_.size()>1)
            frames_.pop_front();
        
        ac::syphon_in_changed = true;
    }
    in_lock.unlock();
}

void ac::DistortStretch(cv::Mat &frame) {
    cv::Mat copy1 = frame.clone();
    int offset_x = frame.cols;
    int offset_y = frame.rows;
    int off_x = rand()%500, off_y = rand()%500;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            int col_x = AC_GetFX(frame.cols, i, offset_x+off_x);
            int col_y = AC_GetFZ(frame.rows, z, offset_y+off_y);
            if(col_x >= 0 && col_x < frame.cols && col_y >= 0 && col_y < frame.rows) {
                cv::Vec3b pix = pixelAt(copy1, col_y, col_x);
                pixel = pix;
            }
        }
    }
    AddInvert(frame);
}

std::vector<cv::VideoCapture *> capture_devices;
std::vector<std::string> list_of_files;
bool ac::setVideoFiles(std::vector<std::string> &v) {
    
    if(!capture_devices.empty()) {
        for(int i = 0; i < capture_devices.size(); ++i)
            delete capture_devices[i];
        capture_devices.erase(capture_devices.begin(), capture_devices.end());
    }
    for(int i = 0; i < v.size(); ++i) {
        
        cv::VideoCapture *cap = new cv::VideoCapture(v[i]);
        capture_devices.push_back(cap);
    }
    for(int i = 0; i < capture_devices.size(); ++i) {
        if(!capture_devices[i]->isOpened())
            return false;
    }
    list_of_files = v;
    return true;
}

void ac::MultiVideoBlend(cv::Mat &frame) {
    if(capture_devices.size()==0)
        return;
    std::vector<cv::Mat> frames;
    for(int q = 0; q < capture_devices.size(); ++q) {
        cv::Mat temp;
        if(capture_devices[q]->isOpened() && capture_devices[q]->read(temp)) {
            cv::Mat r;
            ac_resize(temp, r, frame.size());
            frames.push_back(r);
        } else {
            capture_devices[q]->open(list_of_files[q]);
        }
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            for(int q = 0; q < frames.size(); ++q) {
                cv::Vec3b pixelx = pixelAt(frames[q], z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] += pixelx[j];
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::MultiVideoAlphaBlend(cv::Mat &frame) {
    if(capture_devices.size()==0)
        return;
    std::vector<cv::Mat> frames;
    for(int q = 0; q < capture_devices.size(); ++q) {
        cv::Mat temp;
        if(capture_devices[q]->isOpened() && capture_devices[q]->read(temp)) {
            cv::Mat r;
            ac_resize(temp, r, frame.size());
            frames.push_back(r);
        } else {
            capture_devices[q]->open(list_of_files[q]);
        }
    }
    double percent = 1.0/(frames.size()+1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            pixel[0] = (pixel[0]*percent);
            pixel[1] = (pixel[1]*percent);
            pixel[2] = (pixel[2]*percent);
            for(int q = 0; q < frames.size(); ++q) {
                cv::Vec3b pixelx = pixelAt(frames[q], z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] += (pixelx[j]*percent);
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::MultiVideoSmooth(cv::Mat &frame) {
    if(capture_devices.size()==0)
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    for(int q = 0; q < capture_devices.size(); ++q) {
        cv::Mat temp;
        if(capture_devices[q]->isOpened() && capture_devices[q]->read(temp)) {
            cv::Mat r;
            ac_resize(temp, r, frame.size());
            collection.shiftFrames(r);
        } else {
            capture_devices[q]->open(list_of_files[q]);
        }
    }
    Smooth(frame, &collection);
    AddInvert(frame);
}

void ac::MultiVideoXor(cv::Mat &frame) {
    if(capture_devices.size()==0)
        return;
    std::vector<cv::Mat> frames;
    for(int q = 0; q < capture_devices.size(); ++q) {
        cv::Mat temp;
        if(capture_devices[q]->isOpened() && capture_devices[q]->read(temp)) {
            cv::Mat r;
            ac_resize(temp, r, frame.size());
            frames.push_back(r);
        } else {
            capture_devices[q]->open(list_of_files[q]);
        }
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            for(int q = 0; q < frames.size(); ++q) {
                cv::Vec3b pixelx = pixelAt(frames[q], z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] ^= pixelx[j];
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::MultiVideoFilter_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "MultiVideoFilter_SubFilter")
        return;
    if(capture_devices.size()==0)
        return;
    std::vector<cv::Mat> frames;
    for(int q = 0; q < capture_devices.size(); ++q) {
        cv::Mat temp;
        if(capture_devices[q]->isOpened() && capture_devices[q]->read(temp)) {
            cv::Mat r;
            ac_resize(temp, r, frame.size());
            CallFilter(subfilter, r);
            frames.push_back(r);
        } else {
            capture_devices[q]->open(list_of_files[q]);
        }
    }
    double percent = 1.0/(frames.size()+1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            pixel[0] = (pixel[0]*percent);
            pixel[1] = (pixel[1]*percent);
            pixel[2] = (pixel[2]*percent);
            for(int q = 0; q < frames.size(); ++q) {
                cv::Vec3b pixelx = pixelAt(frames[q], z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] += (pixelx[j]*percent);
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::FadeOnOff(cv::Mat &frame) {
    static int colors[3] = {0};
    static int offset = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int q = 0; q < 3; ++q) {
                pixel[q] = static_cast<unsigned char>((pixel[q] * 0.5) + (colors[q] * 0.5));
            }
        }
    }
    static int index = 0;
    static int rand_change = 25+rand()%50;
    colors[offset] ++;
    ++index;
    if(index > rand_change) {
        index = 0;
        colors[offset] = 0;
        offset++;
        if(offset > 2)
            offset = 0;
        rand_change = 25+rand()%50;
    }
    AddInvert(frame);
}

void ac::Stereo(cv::Mat &frame) {
    cv::Mat img;
    ac_resize(frame, img, cv::Size(frame.cols/2, frame.rows));
    for(int z = 0; z < frame.rows; ++z) {
        int x = 0;
        for(int i = frame.cols/2; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix =img.at<cv::Vec3b>(z, x);
            ++x;
            pixel = pix;
        }
        for(int i = 0; i < frame.cols/2; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix =img.at<cv::Vec3b>(z, i);
            pixel = pix;
        }
    }
    AddInvert(frame);
}

void ac::MultiVideoSaturateBlend(cv::Mat &frame) {
    if(capture_devices.size()==0)
        return;
    std::vector<cv::Mat> frames;
    for(int q = 0; q < capture_devices.size(); ++q) {
        cv::Mat temp;
        if(capture_devices[q]->isOpened() && capture_devices[q]->read(temp)) {
            cv::Mat r;
            ac_resize(temp, r, frame.size());
            frames.push_back(r);
        } else {
            capture_devices[q]->open(list_of_files[q]);
        }
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            for(int q = 0; q < frames.size(); ++q) {
                cv::Vec3b pixelx = pixelAt(frames[q], z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = cv::saturate_cast<unsigned char>(pixel[j]+pixelx[j]);
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::ImageWithKeys(cv::Mat &frame) {
    if(blend_set==true && !blend_image.empty()) {

        cv::Mat reimage;
        ac_resize(blend_image, reimage, frame.size());
        
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = pixelAt(frame, z, i);
                cv::Vec3b pix = pixelAt(reimage, z, i);
                if(searchColors(pixel) != SEARCH_PIXEL) {
                    pixel = pix;
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::MultiVideoColorKeyOn(cv::Mat &frame) {
    if(capture_devices.size()==0)
        return;
    std::vector<cv::Mat> frames;
    for(int q = 0; q < capture_devices.size(); ++q) {
        cv::Mat temp;
        if(capture_devices[q]->isOpened() && capture_devices[q]->read(temp)) {
            cv::Mat r;
            ac_resize(temp, r, frame.size());
            frames.push_back(r);
        } else {
            capture_devices[q]->open(list_of_files[q]);
        }
    }
    double percent = 1.0/(frames.size()+1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            if(searchColors(pixel) == SEARCH_PIXEL) {
                pixel[0] = (pixel[0]*percent);
                pixel[1] = (pixel[1]*percent);
                pixel[2] = (pixel[2]*percent);
                for(int q = 0; q < frames.size(); ++q) {
                    cv::Vec3b pixelx = pixelAt(frames[q], z, i);
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] += (pixelx[j]*percent);
                    }
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::MultiVideoColorKeyOff(cv::Mat &frame) {
    if(capture_devices.size()==0)
        return;
    std::vector<cv::Mat> frames;
    for(int q = 0; q < capture_devices.size(); ++q) {
        cv::Mat temp;
        if(capture_devices[q]->isOpened() && capture_devices[q]->read(temp)) {
            cv::Mat r;
            ac_resize(temp, r, frame.size());
            frames.push_back(r);
        } else {
            capture_devices[q]->open(list_of_files[q]);
        }
    }
    double percent = 1.0/(frames.size()+1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            if(searchColors(pixel) != SEARCH_PIXEL) {
                pixel[0] = (pixel[0]*percent);
                pixel[1] = (pixel[1]*percent);
                pixel[2] = (pixel[2]*percent);
                for(int q = 0; q < frames.size(); ++q) {
                    cv::Vec3b pixelx = pixelAt(frames[q], z, i);
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] += (pixelx[j]*percent);
                    }
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::MultiVideoMedianBlend(cv::Mat &frame) {
    if(capture_devices.size()==0)
        return;
    std::vector<cv::Mat> frames;
    for(int q = 0; q < capture_devices.size(); ++q) {
        cv::Mat temp;
        if(capture_devices[q]->isOpened() && capture_devices[q]->read(temp)) {
            cv::Mat r;
            ac_resize(temp, r, frame.size());
            frames.push_back(r);
        } else {
            capture_devices[q]->open(list_of_files[q]);
        }
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            int col[3] = {0};
            for(int q = 0; q < frames.size(); ++q) {
                cv::Vec3b pixelx = pixelAt(frames[q], z, i);
                for(int j = 0; j < 3; ++j) {
                    col[j] += pixelx[j];
                }
            }
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^col[j];
            }
        }
    }
    AddInvert(frame);
}

void ac::MultiVideoMirror(cv::Mat &frame) {
    if(capture_devices.size()==0)
        return;
    std::vector<cv::Mat> frames;
    for(int q = 0; q < capture_devices.size(); ++q) {
        cv::Mat temp;
        if(capture_devices[q]->isOpened() && capture_devices[q]->read(temp)) {
            cv::Mat r;
            ac_resize(temp, r, frame.size());
            MirrorRandomNow(r);
            frames.push_back(r);
        } else {
            capture_devices[q]->open(list_of_files[q]);
        }
    }
    MirrorRandomNow(frame);
    double percent = 1.0/(frames.size()+1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            pixel[0] = (pixel[0]*percent);
            pixel[1] = (pixel[1]*percent);
            pixel[2] = (pixel[2]*percent);
            for(int q = 0; q < frames.size(); ++q) {
                cv::Vec3b pixelx = pixelAt(frames[q], z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] += (pixelx[j]*percent);
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::MultiVideoSubtract(cv::Mat &frame) {
    if(capture_devices.size()==0)
        return;
    std::vector<cv::Mat> frames;
    for(int q = 0; q < capture_devices.size(); ++q) {
        cv::Mat temp;
        if(capture_devices[q]->isOpened() && capture_devices[q]->read(temp)) {
            cv::Mat r;
            ac_resize(temp, r, frame.size());
            frames.push_back(r);
        } else {
            capture_devices[q]->open(list_of_files[q]);
        }
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            for(int q = 0; q < frames.size(); ++q) {
                cv::Vec3b pixelx = pixelAt(frames[q], z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] -= pixelx[j];
                }
            }
        }
    }
    AddInvert(frame);
}

void ac::MultiVideoAlphaMedianBlend(cv::Mat &frame) {
    if(capture_devices.size()==0)
        return;
    std::vector<cv::Mat> frames;
    for(int q = 0; q < capture_devices.size(); ++q) {
        cv::Mat temp;
        if(capture_devices[q]->isOpened() && capture_devices[q]->read(temp)) {
            cv::Mat r;
            ac_resize(temp, r, frame.size());
            frames.push_back(r);
        } else {
            capture_devices[q]->open(list_of_files[q]);
        }
    }
    double percent = 0.5;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            double col[3];
            col[0] = (pixel[0]*percent);
            col[1] = (pixel[0]*percent);
            col[2] = (pixel[0]*percent);
            for(int q = 0; q < frames.size(); ++q) {
                cv::Vec3b pixelx = pixelAt(frames[q], z, i);
                for(int j = 0; j < 3; ++j) {
                    col[j] += (percent * pixelx[j]);
                }
            }
            for(int j = 0; j < 3; ++j) {
                pixel[j] = pixel[j]^static_cast<int>(col[j]);
            }
        }
    }
    AddInvert(frame);
}

void ac::MultiVideoInterlace(cv::Mat &frame) {
    if(capture_devices.size()==0)
        return;
    std::vector<cv::Mat> frames;
    for(int q = 0; q < capture_devices.size(); ++q) {
        cv::Mat temp;
        if(capture_devices[q]->isOpened() && capture_devices[q]->read(temp)) {
            cv::Mat r;
            ac_resize(temp, r, frame.size());
            frames.push_back(r);
        } else {
            capture_devices[q]->open(list_of_files[q]);
        }
    }
    int q = 0;
    int counter = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame, z, i);
            cv::Vec3b pixelx = pixelAt(frames[q], z, i);
            for(int j = 0; j < 3; ++j)
                pixel[j] = cv::saturate_cast<unsigned char>(pixel[j]+pixelx[j]);
        }
        ++counter;
        if(counter > 1) {
            counter = 0;
            ++q;
            if(q > frames.size()-1)
                q = 0;
        }
    }
    AddInvert(frame);
}

void ac::MultiVideoChangeLines(cv::Mat &frame) {
    if(capture_devices.size()==0)
        return;
    std::vector<cv::Mat> frames;
    for(int q = 0; q < capture_devices.size(); ++q) {
        cv::Mat temp;
        if(capture_devices[q]->isOpened() && capture_devices[q]->read(temp)) {
            cv::Mat r;
            ac_resize(temp, r, frame.size());
            frames.push_back(r);
        } else {
            capture_devices[q]->open(list_of_files[q]);
        }
    }
    int q = rand()%frames.size();
    int counter = 0;
    int counter_max = 50+(rand()%250);
    bool vid_on = false;
    if(rand()%30==0) {
        for(int z = 0; z < frame.rows; ++z) {
            if(vid_on) {
                for(int i = 0; i < frame.cols; ++i) {
                    cv::Vec3b &pixel = pixelAt(frame, z, i);
                    cv::Vec3b pixelx = pixelAt(frames[q], z, i);
                    for(int j = 0; j < 3; ++j)
                        pixel[j] = pixelx[j];
                }
            }
            ++counter;
            if(counter > counter_max) {
                q = rand()%frames.size();
                counter = 0;
                counter_max = 50+rand()%250;
                vid_on = (vid_on == true) ? false : true;
            }
            
        }
    }
    AddInvert(frame);
}

void ac::MultiVideoDiagSquare(cv::Mat &frame) {
    if(capture_devices.size()==0)
        return;
    
    static MatrixCollection<32> collection;
    static int offset_i = 0;
    if(collection.empty())
        collection.shiftFrames(frame);
    std::vector<cv::Mat> frames;
    for(int q = 0; q < capture_devices.size(); ++q) {
        cv::Mat temp;
        if(capture_devices[q]->isOpened() && capture_devices[q]->read(temp)) {
            cv::Mat r;
            ac_resize(temp, r, frame.size());
            frames.push_back(r);
        } else {
            capture_devices[q]->open(list_of_files[q]);
        }
    }
    
    if(offset_i > frames.size()-1)
        offset_i = 0;
    
    collection.shiftFrames(frames[offset_i]);
    static int offset = 0;
    for(int z = 0; z < frame.rows; z += 32) {
        for(int i = 0; i < frame.cols; i += 32) {
            for(int x = 0; x+i < frame.cols && x < 32; ++x) {
                for(int y = 0; z+y < frame.rows && y < 32; ++y) {
                    cv::Vec3b &pixel = pixelAt(frame,z+y, i+x);
                    cv::Vec3b pix = collection.frames[offset].at<cv::Vec3b>(z+y, i+x);
                    pixel = pix;
                }
            }
            ++offset;
            if(offset > (collection.size()-1)) {
                offset = 0;
                ++offset_i;
                if(offset_i > frames.size()-1)
                    offset_i = 0;
            }
        }
    }
    AddInvert(frame);
}


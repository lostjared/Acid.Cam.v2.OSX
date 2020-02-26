
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

void ac::AddCollectionSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "AddCollectionSubFilter")
        return;
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    collection.shiftFrames(copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            int colors[3] = {0};
            for(int j = 0; j < collection.size(); ++j) {
                cv::Vec3b pix = collection.frames[j].at<cv::Vec3b>(z, i);
                for(int q = 0; q < 3; ++q)
                    colors[q] += pix[q];
            }
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = colors[j]/collection.size();
            }
        }
    }
    AddInvert(frame);
}

void ac::AddCollectionXor_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "AddCollectionXor_SubFilter")
        return;
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    collection.shiftFrames(copy1);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            int colors[3] = {0};
            for(int j = 0; j < collection.size(); ++j) {
                cv::Vec3b pix = collection.frames[j].at<cv::Vec3b>(z, i);
                for(int q = 0; q < 3; ++q)
                    colors[q] += pix[q];
            }
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] ^= colors[j]/collection.size();
            }
        }
    }
    AddInvert(frame);
}

void ac::ProperTrails(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    if(collection.empty())
        collection.shiftFrames(frame);
    cv::Mat out;
    AlphaBlendDouble(frame, collection.frames[7],out,0.5, 0.5);
    collection.shiftFrames(out);
    frame = out.clone();
    AddInvert(frame);
}

void ac::ProperTrails_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || ac::draw_strings[subfilter] == "ProperTrails_SubFilter")
        return;
    static MatrixCollection<8> collection;
    if(collection.empty())
        collection.shiftFrames(frame);
    cv::Mat out;
    AlphaBlendDouble(frame, collection.frames[7],out,0.5, 0.5);
    CallFilter(subfilter, out);
    collection.shiftFrames(out);
    frame = out.clone();
    AddInvert(frame);
}

void ac::StuckFrame_SubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "StuckFrame_SubFilter")
        return;
    static MatrixCollection<8> collection;
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    collection.shiftFrames(copy1);
    cv::Mat out;
    AlphaBlendDouble(frame, collection.frames[7], out, 0.5, 0.6);
    frame = out.clone();
    AddInvert(frame);
}

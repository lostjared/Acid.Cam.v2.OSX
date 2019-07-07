
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

#include"ac.h"


void ac::ColorImageFillMatrix(cv::Mat &frame) {
    
    if(blend_set == false)
        return;
    
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat frame_copy = frame.clone();
    ColorFreezeBlend(frame_copy);
    
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b cpix = reimage.at<cv::Vec3b>(z, i);
                cv::Vec3b cfreeze = frame_copy.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_container.pix_values[i][z].dir[j];
                     PixelValues &pix = pix_container.pix_values[i][z];
                    if(d == 1) {
                        ++pix.col[j];
                        if(pix.col[j] == cpix[j]) {
                            pix.dir[j] = 2;
                        }
                        else if(pix.col[j] >= 255) {
                            pix.col[j] = 255;
                            pix.dir[j] = 0;
                        }
                    } else if(d == 0) {
                        --pix.col[j];
                        if(pix.col[j] == cpix[j]) {
                            pix.dir[j] = 2;
                        } else if(pix.col[j] <= 1) {
                            pix.col[j] = 0;
                            pix.dir[j] = 1;
                        }
                    } else if(d == 2) {
                        pix.col[j] = cfreeze[j];
                    }
                    pixel[j] = static_cast<unsigned char>((pixel[j] * 0.5) + (pix_container.pix_values[i][z].col[j] * 0.5));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

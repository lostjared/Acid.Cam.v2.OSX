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


void ac::BlendWithColor(cv::Mat &frame) {
    cv::Vec3b color(swapColor_b, swapColor_g, swapColor_r);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * color[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    swapColorState(true);
}

void ac::DigitalHazeBlend(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    static int speed[3] = {1,3,5};
    if(pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            PixelValues &pix = pix_container.pix_values[i][z];
            for(int j = 0; j < 3; ++j) {
                if(pix.col[j] > pixel[j])
                    pix.col[j] -= speed[j];
                else if(pix.col[j] < pixel[j])
                    pix.col[j] += speed[j];
                else
                    pix.col[j] = pixel[j];
                pixel[j] = cv::saturate_cast<unsigned char>((pixel[j] * 0.2) + (pix.col[j] * 0.8));
            }
        }
    }
    AddInvert(frame);
}

void ac::VideoAlphaBlend50(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;

    cv::Mat copy1 = frame.clone(), nframe;
    if(VideoFrame(nframe)) {
        cv::Mat reframe;
        ac_resize(nframe, reframe, frame.size());
        AlphaBlendDouble(copy1, reframe, frame, 0.5, 0.5);
    }
    AddInvert(frame);
}

void ac::VideoExactFrame(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;

    cv::Mat temp;
    if(VideoFrame(temp)) {
        cv::Mat reframe;
        ac_resize(temp, reframe, frame.size());
        frame = reframe.clone();
    }
    AddInvert(frame);
}

void ac::VideoAlphaFade(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;

    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat frame_copy,frame2 = frame.clone();
    if(VideoFrame(frame_copy)) {
        cv::Mat reframe;
        ac_resize(frame_copy, reframe, frame.size());
        AlphaBlendDouble(reframe, frame2, frame, alpha, (1-alpha));
    }
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
}

void ac::VideoAlphaBlend25(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;

    cv::Mat copy1 = frame.clone(), nframe;
    if(VideoFrame(nframe)) {
        cv::Mat reframe;
        ac_resize(nframe, reframe, frame.size());
        AlphaBlendDouble(copy1, reframe, frame, 0.75, 0.25);
    }
    AddInvert(frame);
}

void ac::VideoAlphaBlend75(cv::Mat &frame) {
    
    if(v_cap.isOpened() == false)
        return;
    
    cv::Mat copy1 = frame.clone(), nframe;
    if(VideoFrame(nframe)) {
        cv::Mat reframe;
        ac_resize(nframe, reframe, frame.size());
        AlphaBlendDouble(copy1, reframe, frame, 0.25, 0.75);
    }
    AddInvert(frame);
}


void ac::VideoAlphaBlendSubFilter(cv::Mat &frame) {
    if(v_cap.isOpened() == false || subfilter == -1 || draw_strings[subfilter] == "VideoAlphaBlendSubFilter")
        return;
    cv::Mat copy1 = frame.clone(), copy_v;
    if(VideoFrame(copy_v)) {
        cv::Mat reframe;
        ac_resize(copy_v, reframe, frame.size());
        CallFilter(subfilter, reframe);
        CallFilter(subfilter, copy1);
        AlphaBlendDouble(copy1, reframe, frame, 0.5, 0.5);
    }
    AddInvert(frame);
}

void ac::VideoAlphaImage(cv::Mat &frame) {
    if(blend_set == false || v_cap.isOpened() == false)
        return;
    cv::Mat reimage, vframe;
    ac_resize(blend_image, reimage, frame.size());
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        AlphaBlendDouble(reimage, reframe, frame, 0.5, 0.5);
    }
    AddInvert(frame);
}

void ac::VideoAlphaImageFrame(cv::Mat &frame) {
    if(blend_set == false || v_cap.isOpened() == false)
        return;
    cv::Mat reimage, vframe;
    cv::Mat copy1 = frame.clone();
    ac_resize(blend_image, reimage, frame.size());
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        AlphaBlendDouble(reimage, reframe, vframe, 0.5, 0.5);
        AlphaBlendDouble(copy1, vframe, frame, 0.5, 0.5);
    }
    AddInvert(frame);
}

void ac::VideoAlphaImageScale(cv::Mat &frame) {
    if(blend_set == false || v_cap.isOpened() == false)
        return;
    static double alpha1 = 0.1, alpha2 = 1.0;
    static int dir1 = 1, dir2 = 0;
    cv::Mat reimage, vframe;
    cv::Mat copy1 = frame.clone();
    ac_resize(blend_image, reimage, frame.size());
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        AlphaBlendDouble(reimage, reframe, vframe, alpha1,(1-alpha1));
        AlphaBlendDouble(copy1, vframe, frame, alpha2,(1-alpha2));
    }
    AddInvert(frame);
    AlphaMovementMaxMin(alpha1, dir1, 0.01, 1.0, 0.1);
    AlphaMovementMaxMin(alpha2, dir2, 0.05, 1.0, 0.1);
}


void ac::VideoAlphaBlendImage25(cv::Mat &frame) {
    if(blend_set == false || v_cap.isOpened() == false)
        return;
    cv::Mat reimage, nframe;
    ac_resize(blend_image, reimage, frame.size());
    if(VideoFrame(nframe)) {
        cv::Mat reframe;
        ac_resize(nframe, reframe, frame.size());
        cv::Mat copy1 = frame.clone();
        AlphaBlendDouble(copy1, reframe, nframe, 0.25, 0.75);
        AlphaBlendDouble(nframe, reimage, frame, 0.25, 0.75);
    }
    AddInvert(frame);
}

void ac::VideoAlphaBlendImage50(cv::Mat &frame) {
    if(blend_set == false || v_cap.isOpened() == false)
        return;
    cv::Mat reimage, nframe;
    ac_resize(blend_image, reimage, frame.size());
    if(VideoFrame(nframe)) {
        cv::Mat reframe;
        ac_resize(nframe, reframe, frame.size());
        cv::Mat copy1 = frame.clone();
        AlphaBlendDouble(copy1, reframe, nframe, 0.50, 0.50);
        AlphaBlendDouble(nframe, reimage, frame, 0.50, 0.50);
    }
    AddInvert(frame);
}

void ac::VideoAlphaBlendImage75(cv::Mat &frame) {
    if(blend_set == false || v_cap.isOpened() == false)
        return;
    cv::Mat reimage, nframe;
    ac_resize(blend_image, reimage, frame.size());
    if(VideoFrame(nframe)) {
        cv::Mat reframe;
        ac_resize(nframe, reframe, frame.size());
        cv::Mat copy1 = frame.clone();
        AlphaBlendDouble(copy1, reframe, nframe, 0.75, 0.25);
        AlphaBlendDouble(nframe, reimage, frame, 0.75, 0.25);
    }
    AddInvert(frame);
}

void ac::VideoFrameFilterSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "VideoFrameFilterSubFilter" || v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        CallFilter(subfilter, reframe);
        reframe.copyTo(frame);
    }
    AddInvert(frame);
}

void ac::VideoFrameImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "VideoFrameImageSubFilter" || v_cap.isOpened() == false)
        return;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        cv::Mat reimage;
        ac_resize(blend_image, reimage, frame.size());
        CallFilter(subfilter, reframe);
        AlphaBlendDouble(reframe, reimage, frame, alpha, (1-alpha));
    }
    AlphaMovementMaxMin(alpha, dir, 0.005, 1.0, 0.3);
    AddInvert(frame);
}

void ac::VideoAlphaSubFilter(cv::Mat &frame) {
    if(v_cap.isOpened() == false || subfilter == -1 || draw_strings[subfilter] == "VideoAlphaSubFilter")
        return;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat copy1 = frame.clone();
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat copy2;
        ac_resize(vframe, copy2, frame.size());
        AlphaBlendDouble(copy1, copy2, frame, alpha, (1-alpha));
        CallFilter(subfilter, frame);
    }
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.3);
    AddInvert(frame);
}

void ac::VideoRoll(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    int frame_size = frame.rows;
    static constexpr int movement_size = 10;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        static int y = 0;
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                if(z+y < frame_size-1) {
                    cv::Vec3b pix = pixelAt(reframe,z, i);
                    cv::Vec3b &pixel = pixelAt(frame,y+z, i);
                    for(int j = 0; j < 3; ++j) {
                        pixel[j] = static_cast<unsigned char>((pixel[j] * 0.5) + (pix[j] * 0.5));
                    }
                }
            }
        }
        static int dir = 1;
        if(dir == 1) {
            y += movement_size;
            if(y > frame.rows-movement_size) {
                y = frame.rows-movement_size;
                dir = 0;
            }
        } else {
            y -= movement_size;
            if(y < movement_size) {
                y = movement_size;
                dir = 1;
            }
        }
    }
    AddInvert(frame);
}

void ac::VideoRollReverse(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    int frame_size = frame.rows;
    static constexpr int movement_size = 10;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        static int y = 0;
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                if(z+y < frame_size-1) {
                    if(frame.rows-z-1 >= 0 && i >= 0 && i < frame.cols && z >= 0 && z < frame.rows) {
                        cv::Vec3b pix = pixelAt(reframe,(frame.rows-z-1), i);
                        cv::Vec3b &pixel = pixelAt(frame,z, i);
                        for(int j = 0; j < 3; ++j) {
                            pixel[j] = static_cast<unsigned char>((pixel[j] * 0.5) + (pix[j] * 0.5));
                        }
                    }
                }
            }
        }
        static int dir = 1;
        if(dir == 1) {
            y += movement_size;
            if(y > frame.rows-movement_size) {
                y = frame.rows-movement_size;
                dir = 0;
            }
        } else {
            y -= movement_size;
            if(y < movement_size) {
                y = movement_size;
                dir = 1;
            }
        }
    }
    AddInvert(frame);
}

void ac::VideoRollMedianBlend(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    VideoRoll(copy1);
    VideoRollReverse(copy2);
    AlphaBlendDouble(copy1, copy2, frame, 0.5, 0.5);
    MedianBlendMultiThreadScale(frame);
    AddInvert(frame);
}

void ac::VideoRollScaleMedianBlend(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat copy1 = frame.clone(), copy2 = frame.clone();
    VideoRoll(copy1);
    VideoRollReverse(copy2);
    AlphaBlendDouble(copy1, copy2, frame, alpha, (1-alpha));
    MedianBlendMultiThreadScale(frame);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.08, 1.0, 0.1);
}

void ac::VideoSubFilter(cv::Mat &frame) {
    if(v_cap.isOpened() == false || subfilter == -1 || draw_strings[subfilter] == "VideoSubFilter")
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat copy1 = frame.clone();
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        CallFilter(subfilter, reframe);
        AlphaBlendDouble(copy1, reframe, frame, 0.5, 0.5);
    }
    AddInvert(frame);
}

void ac::MedianBlendWithColor(cv::Mat &frame) {
    BlendWithColor(frame);
    MedianBlendMultiThreadScale(frame);
    AddInvert(frame);
}

void ac::VideoImage(cv::Mat &frame) {
    if(blend_set == false || v_cap.isOpened() == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        AlphaBlendDouble(reimage, reframe, frame, 0.5, 0.5);
    }
    AddInvert(frame);
}

void ac::VideoImageFade(cv::Mat &frame) {
    if(blend_set == false || v_cap.isOpened() == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        static double alpha = 1.0;
        static int dir = 1;
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        AlphaBlendDouble(reimage, reframe, frame, alpha, (1-alpha));
        AlphaMovementMaxMin(alpha,dir,0.01, 1.0, 0.1);
    }
    AddInvert(frame);
}

void ac::ColorMovement(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    static int speed[3] = {1,1,1};
    if(pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                ac::PixelValues &pix = pix_container.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    if(pix.dir[j] == 1) {
                        pix.col[j] += speed[j];
                        if(pix.col[j] >= 255) {
                            pix.col[j] = pixel[j];
                            pix.dir[j] = 0;
                        }
                    } else {
                        pix.col[j] -= speed[j];
                        if(pix.col[j] <= 0) {
                            pix.dir[j] = 1;
                            pix.col[j] = pixel[j];
                        }
                    }
                    pixel[j] = static_cast<unsigned char>((pixel[j] * 0.5) + (pix.col[j] * 0.5));
                }
                
            }
        }
    };
    int fps = static_cast<int>(ac::fps);
    static int counter = 0;
    ++counter;
    if(counter >= (fps * 4)) {
        counter = 1;
        for(int j = 0; j < 3; ++j) {
            if(speed[j] >= 10) {
                speed[j] = 1;
            } else ++speed[j];
        }
    }
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::MedianBlendColorMovement(cv::Mat &frame) {
    ColorMovement(frame);
    MedianBlendMultiThreadScale(frame);
}

void ac::ColorMovementRange(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    static int speed[3] = {1,1,1};
    
    if(pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, -1);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                ac::PixelValues &pix = pix_container.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    if(pix.dir[j] == 1) {
                        pix.col[j] += speed[j];
                        if(pix.col[j] >= 255) {
                            pix.dir[j] = 0;
                            pix.col[j] = pixel[j];
                        }
                    } else {
                        pix.col[j] -= speed[j];
                        if(pix.col[j] <= 0) {
                            pix.dir[j] = 1;
                            pix.col[j] = pixel[j];
                        }
                    }
                    pixel[j] = static_cast<unsigned char>((pixel[j] * alpha) + (pix.col[j] * (1-alpha)));
                }
            }
        }
    };
    for(int q = 0; q < 3; ++q) {
        ++speed[q];
        if(speed[q] > 10)
            speed[q] = rand()%5;
    }
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    ac::AlphaMovementMaxMin(alpha,dir, 0.01, 1.0, 0.3);
}

void ac::ColorPixelOrder(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int r = rand()%(collection.size()-1);
                    cv::Vec3b pix = collection.frames[r].at<cv::Vec3b>(z, i);
                    pixel[j] = pix[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}


void ac::ColorPixelOrder32(cv::Mat &frame) {
    static constexpr int size_val = 32;
    static MatrixCollection<size_val> collection;
    collection.shiftFrames(frame);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int r = rand()%(collection.size()-1);
                    cv::Vec3b pix = collection.frames[r].at<cv::Vec3b>(z, i);
                    pixel[j] = pix[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::MedianBlendMuliThread_Pixelate(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    static int speed[3] = {1,1,1};
    if(pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, -1);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                ac::PixelValues &pix = pix_container.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {

                    if(pix.dir[j] == 1) {
                        pix.col[j] += speed[j];
                        if(pix.col[j] >= 255) {
                            pix.col[j] = pixel[j];
                            pix.add[j] = 0;
                            pix.dir[j] = 2;
                        }
                    } else if(pix.dir[j] == 0) {
                        pix.col[j] -= speed[j];
                        if(pix.col[j] <= 0) {
                            pix.col[j] = pixel[j];
                            pix.dir[j] = rand()%2;
                        }
                    } else if(pix.dir[j] == 2) {
                        ++pix.add[j];
                        if(pix.add[j] > 30) {
                            pix.add[j] = 0;
                            pix.dir[j] = rand()%2;
                        }
                    }
                    if(pix.dir[j] == 1) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix.col[j]));
                    } else if(pix.dir[j] == 0) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix.col[3-j-1]));
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MedianBlendMultiThreadScale(frame);
    AddInvert(frame);
}

void ac::SelfScaleGlitch(cv::Mat &frame) {
    static double value[3] = {1,1,1};
    static int dir[3] = {0, 1, 0};
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((pixel[j]/2) * 0.5) + static_cast<unsigned char>(((pixel[j]/2) * value[j]) * 0.5);
                }
            }
        }
    };
    AlphaMovementMaxMin(value[0], dir[0], 1.0, 64.0, 1.0);
    AlphaMovementMaxMin(value[1], dir[1], 1.0, 64.0, 1.0);
    AlphaMovementMaxMin(value[2], dir[2], 1.0, 64.0, 1.0);
    UseMultipleThreads(frame, getThreadCount(), callback);
    MedianBlendMultiThreadScale(frame);
    AddInvert(frame);
}

void ac::PixelGlitch(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, -1);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &pix = pix_container.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    if(pix.dir[j] == 1) {
                        ++pix.col[j];
                        if(pix.col[j] >= 255) {
                            pix.col[j] = pixel[j];
                            pix.dir[j] = 0;
                        }
                        pixel[j] = static_cast<unsigned int>(pixel[j]+pix.col[j]);
                        
                    } else if(pix.dir[j] == 0) {
                        --pix.col[j];
                        if(pix.col[j] <= 1) {
                            pix.col[j] = pixel[j];
                            pix.dir[j] = 1;
                        }
                        pixel[j] = static_cast<unsigned int>(pixel[j]-pix.col[j]);
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::VideoPixelOnOffSwitch(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        static int start = 0;
        int index = start;
        start = (start == 0) ? 1 : 0;
        auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
            for(int z = offset; z <  offset+size; ++z) {
                for(int i = 0; i < cols; ++i) {
                    cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                    cv::Vec3b pix = pixelAt(reframe,z, i);
                    if(index == 0) {
                        index = 1;
                        continue;
                    } else {
                        pixel = pix;
                        index = 0;
                    }
                }
            }
        };
        UseMultipleThreads(frame, getThreadCount(), callback);
        AddInvert(frame);
    }
}

void ac::VideoBlendImageSubFilter(cv::Mat &frame) {
    
    if(blend_set == false || v_cap.isOpened() == false || subfilter == -1 || draw_strings[subfilter] == "VideoBlendImageSubFilter")
        return;
    
    cv::Mat copy1 = frame.clone();
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat vframe;
    static double alpha = 1.0;
    static double alpha_rev = 0.1;
    static int dir = 1;
    static int dir_rev = 1;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        CallFilter(subfilter, reframe);
        CallFilter(subfilter, reimage);
        cv::Mat reout;
        AlphaBlendDouble(reframe, reimage, reout, alpha_rev, (1-alpha_rev));
        AlphaBlendDouble(copy1, reout, frame, alpha, (1-alpha));
    }
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.2);
    AlphaMovementMaxMin(alpha_rev, dir_rev, 0.01, 1.0, 0.2);
    AddInvert(frame);
}

void ac::VideoAlphaImageSubFilter(cv::Mat &frame) {
    if(blend_set == false || v_cap.isOpened() == false || subfilter == -1 || draw_strings[subfilter] == "VideoAlphaImageSubFilter")
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        cv::Mat copy1 = frame.clone();
        cv::Mat outf;
        AlphaBlendDouble(reimage, reframe, outf, 0.5, 0.5);
        AlphaBlendDouble(copy1, outf, frame, 0.5, 0.5);
        CallFilter(subfilter, frame);
    }
    AddInvert(frame);
}

void ac::MedianBlendVideo(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        cv::Mat copy1 = frame.clone();
        MedianBlendMultiThreadScale(reframe);
        AlphaBlendDouble(copy1, reframe, frame, 0.5, 0.5);
    }
    AddInvert(frame);
}

void ac::VideoFrameBlur8(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    static constexpr int Size = 8;
    static MatrixCollection<Size> collection;
    BlurVideoMatrix<Size>(frame, &collection);
    AddInvert(frame);
}

void ac::VideoFrameBlur16(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    static constexpr int Size = 16;
    static MatrixCollection<Size> collection;
    BlurVideoMatrix<Size>(frame, &collection);
    AddInvert(frame);
}

void ac::VideoFrameBlur32(cv::Mat &frame) {
    if(v_cap.isOpened() == false)
        return;
    static constexpr int Size = 32;
    static MatrixCollection<Size> collection;
    BlurVideoMatrix<Size>(frame, &collection);
    AddInvert(frame);
}

void ac::VideoSourceFrameBlur8(cv::Mat &frame) {
    static constexpr int Size = 8;
    static MatrixCollection<Size> collection1, collection2;
    collection1.shiftFrames(frame);
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        collection2.shiftFrames(reframe);
        cv::Mat out1 = frame.clone(), out2 = reframe.clone();
        Smooth(out1, &collection1, false);
        Smooth(out2, &collection2, false);
        AlphaBlendDouble(out1, out2, frame, 0.5, 0.5);
    }
}

void ac::VideoSourceFrameBlur16(cv::Mat &frame) {
    static constexpr int Size = 16;
    static MatrixCollection<Size> collection1, collection2;
    collection1.shiftFrames(frame);
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        collection2.shiftFrames(reframe);
        cv::Mat out1 = frame.clone(), out2 = reframe.clone();
        Smooth(out1, &collection1, false);
        Smooth(out2, &collection2, false);
        AlphaBlendDouble(out1, out2, frame, 0.5, 0.5);
    }
}

void ac::VideoSourceFrameBlur32(cv::Mat &frame) {
    static constexpr int Size = 32;
    static MatrixCollection<Size> collection1, collection2;
    collection1.shiftFrames(frame);
    cv::Mat vframe;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        collection2.shiftFrames(reframe);
        cv::Mat out1 = frame.clone(), out2 = reframe.clone();
        Smooth(out1, &collection1, false);
        Smooth(out2, &collection2, false);
        AlphaBlendDouble(out1, out2, frame, 0.5, 0.5);
    }
}

void ac::VideoImageSourceFrame(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat vframe;
    static double alpha = 1.0;
    static int dir = 1;
    if(VideoFrame(vframe)) {
        cv::Mat reframe;
        ac_resize(vframe, reframe, frame.size());
        cv::Mat out;
        cv::Mat copy1 = frame.clone();
        AlphaBlendDouble(reimage, reframe, out, alpha, (1-alpha));
        AlphaBlendDouble(out, copy1, frame, alpha, (1-alpha));
    }
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.2);
}

void ac::SlowRed(cv::Mat &frame) {
    static int col = 0;
    static int dir = 1;
    static int frame_counter = 0;
    static int increase = 5;
    ++frame_counter;
    if(frame_counter > static_cast<int>(ac::fps)) {
        frame_counter = 0;
        if(dir == 1) {
            col += increase;
            if(col >= 255)
                dir = 0;
        } else {
            col -= increase;
            if(col <= 1)
                dir = 1;
        }
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                pixel[2] = cv::saturate_cast<unsigned char>(pixel[2]+col);
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::SlowGreen(cv::Mat &frame) {
    static int col = 0;
    static int dir = 1;
    static int frame_counter = 0;
    static int increase = 5;
    ++frame_counter;
    if(frame_counter > static_cast<int>(ac::fps)) {
        frame_counter = 0;
        if(dir == 1) {
            col += increase;
            if(col >= 255)
                dir = 0;
        } else {
            col -= increase;
            if(col <= 1)
                dir = 1;
        }
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                pixel[1] = cv::saturate_cast<unsigned char>(pixel[1]+col);
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::SlowBlue(cv::Mat &frame) {
    static int col = 0;
    static int dir = 1;
    static int frame_counter = 0;
    static int increase = 5;
    ++frame_counter;
    if(frame_counter > static_cast<int>(ac::fps)) {
        frame_counter = 0;
        if(dir == 1) {
            col += increase;
            if(col >= 255)
                dir = 0;
        } else {
            col -= increase;
            if(col <= 1)
                dir = 1;
        }
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                pixel[0] = cv::saturate_cast<unsigned char>(pixel[0]+col);
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}


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

void ac::ImageSquareShrinkAlpha(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static int frame_offset_z = 0, frame_offset_i = 0;
    static int dir = 1;
    static int speed = 32;
    static double alpha = 1.0;
    static int dir1 = 1;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    for(int z = (frame.rows-1)-frame_offset_z; z >= frame_offset_z; --z) {
        for(int i = (frame.cols-1)-frame_offset_i; i >= frame_offset_i; --i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((alpha * pixel[j]) + ((1-alpha) * pix[j]));
            }
        }
    }
    if(dir == 1) {
        frame_offset_z += speed;
        frame_offset_i += speed;
        if(frame_offset_z > ((frame.rows/2)-1) || frame_offset_i > ((frame.cols/2)-1)) {
            dir = 0;
        }
    } else {
        frame_offset_z -= speed;
        frame_offset_i -= speed;
        if(frame_offset_z <= 1 || frame_offset_i <= 1) {
            dir = 1;
        }
    }
    AlphaMovementMaxMin(alpha, dir1, 0.01, 1.0, 0.3);
}

void ac::ImageSquareExpand(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static int frame_offset_z = (frame.rows/9), frame_offset_i = (frame.cols/16);
    static int dir = 0;
    static int speed = 32;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    for(int z = (frame.rows-1)-frame_offset_z; z >= frame_offset_z; --z) {
        for(int i = (frame.cols-1)-frame_offset_i; i >= frame_offset_i; --i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix[j]));
            }
        }
    }
    if(dir == 1) {
        frame_offset_z += speed;
        frame_offset_i += speed;
        if(frame_offset_z > ((frame.rows/2)-1) || frame_offset_i > ((frame.cols/2)-1)) {
            dir = 0;
        }
        
    } else {
        frame_offset_z -= speed;
        frame_offset_i -= speed;
        if(frame_offset_z <= 1 || frame_offset_i <= 1) {
            dir = 1;
        }
    }
    
}

void ac::ImageKaleidoscopeNoBlur(cv::Mat &frame) {
    if(blend_set == false)
        return;
    ImageSquareShrinkFast(frame);
    MedianBlendMultiThreadByEight(frame);
    BlendWithSource75(frame);
    Mirror_ReverseColor(frame);
    MirrorRightTopToBottom(frame);
}

void ac::KaleidoscopeSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "KaleidoscopeSubFilter")
        return;
    CallFilter(subfilter, frame);
    BlendWithSource75(frame);
    Mirror_ReverseColor(frame);
    MirrorRightTopToBottom(frame);
}

void ac::ImageSquareShrinkSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageSquareShrinkSubFilter")
        return;
    static int frame_offset_z = 0, frame_offset_i = 0;
    static int dir = 1;
    static int speed = 32;
    static double alpha = 1.0;
    static int dir1 = 1;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    for(int z = (frame.rows-1)-frame_offset_z; z >= frame_offset_z; --z) {
        for(int i = (frame.cols-1)-frame_offset_i; i >= frame_offset_i; --i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((alpha * pixel[j]) + ((1-alpha) * pix[j]));
            }
        }
    }
    if(dir == 1) {
        frame_offset_z += speed;
        frame_offset_i += speed;
        if(frame_offset_z > ((frame.rows/2)-1) || frame_offset_i > ((frame.cols/2)-1)) {
            dir = 0;
        }
    } else {
        frame_offset_z -= speed;
        frame_offset_i -= speed;
        if(frame_offset_z <= 1 || frame_offset_i <= 1) {
            dir = 1;
        }
    }
    AlphaMovementMaxMin(alpha, dir1, 0.01, 1.0, 0.3);
}

void ac::DifferenceReplaceSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "DifferenceReplaceSubFilter")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat value = collection.frames[7].clone();
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b rep = copy1.at<cv::Vec3b>(z, i);
                for(int q = 0; q < collection.size(); ++q) {
                    cv::Vec3b pix = collection.frames[q].at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        int value = (pixel[j] - pix[j]);
                        if(abs(value) > getPixelCollection()) {
                            pixel[j] = rep[j];
                        }
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::InvertedDifferenceReplaceSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "InvertedDifferenceReplaceSubFilter")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat value = collection.frames[7].clone();
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b rep = copy1.at<cv::Vec3b>(z, i);
                for(int q = 0; q < collection.size(); ++q) {
                    cv::Vec3b pix = collection.frames[q].at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        int value = (pixel[j] - pix[j]);
                        if(abs(value) < getPixelCollection()) {
                            pixel[j] = rep[j];
                        } else {
                            pixel[j] = 0;
                        }
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ImageInvertedDifferenceReplaceSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ImageInvertedDifferenceReplaceSubFilter")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat value = collection.frames[7].clone();
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b rep = copy1.at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int q = 0; q < collection.size(); ++q) {
                    cv::Vec3b pix = collection.frames[q].at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        int value = (pixel[j] - pix[j]);
                        if(abs(value) < getPixelCollection()) {
                            pixel[j] = static_cast<unsigned char>((pixel[j] * 0.7) + (0.3 * rep[j]));
                        } else {
                            pixel[j] = static_cast<unsigned char>((pixel[j] * 0.7) + (0.3 * img_pix[j]));
                        }
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ImageDifferenceReplaceSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "DifferenceReplaceSubFilter")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat value = collection.frames[7].clone();
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b rep = copy1.at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int q = 0; q < collection.size(); ++q) {
                    cv::Vec3b pix = collection.frames[q].at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        int value = (pixel[j] - pix[j]);
                        if(abs(value) > getPixelCollection()) {
                            pixel[j] = static_cast<unsigned char>((pixel[j] * 0.7) + (rep[j] * 0.3));
                        } else {
                            pixel[j] = static_cast<unsigned char>((pixel[j] * 0.7) + (img_pix[j] * 0.3));
                        }
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::DifferenceReplaceSubFilterAlphaBlend(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "DifferenceReplaceSubFilterAlphaBlend")
        return;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat value = collection.frames[7].clone();
    cv::Mat copy1 = frame.clone();
    CallFilter(subfilter, copy1);
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b rep = copy1.at<cv::Vec3b>(z, i);
                for(int q = 0; q < collection.size(); ++q) {
                    cv::Vec3b pix = collection.frames[q].at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        int value = (pixel[j] - pix[j]);
                        if(abs(value) > getPixelCollection()) {
                            pixel[j] = static_cast<unsigned char>((pixel[j] * alpha) + (rep[j] * (1-alpha)));
                        }
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.2);
}

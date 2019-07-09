
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

void ac::ColorImageFillSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ColorImageFillSubFilter")
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
    CallFilter(subfilter, frame_copy);
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

void ac::ColorImagePixelsResetSubFilter(cv::Mat &frame) {
    if(blend_set == false)
        return;
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    static int count_index = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0, true);
        pix_x = frame.cols;
        pix_y = frame.rows;
        count_index = 0;
        
    }
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    cv::Mat frame_copy = frame.clone();
    CallFilter(subfilter, frame_copy);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
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
    if(count_index+100 < pix_container.pixel_index.size()) {
        for(int i = count_index; i < pix_container.pixel_index.size() && i < count_index+100; ++i) {
            PixelValues *v = pix_container.pixel_index[i];
            for(int j = 0; j < 3; ++j) {
                pix_container.pix_values[v->position_x][v->position_y].dir[j] = 2;
            }
        }
        count_index += 100;
    }
}

void ac::ColorIncrementReset(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static double alpha = 1.0;
    static int dir = 1;
    static int speed = 5;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pix_container.pix_values[i][z].col[j] += speed;
                    if(pix_container.pix_values[i][z].col[j] >= 255) {
                        pix_container.pix_values[i][z].col[j] = pixel[j];
                    }
                    pixel[j] = static_cast<unsigned char>(1-alpha * pixel[j]) + (alpha * pix_container.pix_values[i][z].col[j]);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AlphaMovementMaxMin(alpha, dir, 0.001, 1.0, 0.5);
    DarkenFilter(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::ColorPixelArray2D(cv::Mat &frame) {
    static PixelArray2D pix_container1, pix_container2;
    static int pix_x = 0, pix_y = 0;
    static int pix2_x = 0, pix2_y = 0;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    cv::Mat copy1 = collection.frames[7].clone();
    static bool reset_matrix = false;
    if(reset_matrix == true || image_matrix_reset == true || pix_container1.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container1.create(frame, frame.cols, frame.rows, 0, true);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    if(reset_matrix == true || image_matrix_reset == true || pix_container2.pix_values == 0 || frame.size() != cv::Size(pix2_x, pix2_y)) {
        pix_container2.create(copy1, frame.cols, frame.rows, 0);
        pix2_x = frame.cols;
        pix2_y = frame.rows;
    }
    reset_matrix = false;
    static double alpha = 1.0;
    static int dir = 1;
    static int speed = 5;
    static int count_index = 0;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container1.pix_values[i][z];
                PixelValues &p2 = pix_container2.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    if(p.dir[j] == 1) {
                        p.col[j] += speed;
                        if(p.col[j] >= 255) {
                            p.dir[j] = 0;
                        }
                    } else if(p.dir[j] == 0) {
                        p.col[j] -= speed;
                        if(p.col[j] <= 0) {
                            p.dir[j] = 1;
                        }
                    } else if(p.dir[j] == 2) {
                        p.col[j] = p2.col[j];
                    }
                    pixel[j] = static_cast<unsigned char>(1-alpha * pixel[j]) + (alpha * pix_container1.pix_values[i][z].col[j]);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    if(count_index+100 < pix_container1.pixel_index.size()) {
        for(int i = count_index; i < pix_container1.pixel_index.size() && i < count_index+100; ++i) {
            PixelValues *v = pix_container1.pixel_index[i];
            for(int j = 0; j < 3; ++j) {
                pix_container1.pix_values[v->position_x][v->position_y].dir[j] = 2;
            }
        }
        count_index += 100;
    } else {
        reset_matrix = true;
    }
    AlphaMovementMaxMin(alpha, dir, 0.001, 1.0, 0.5);
    AddInvert(frame);
}


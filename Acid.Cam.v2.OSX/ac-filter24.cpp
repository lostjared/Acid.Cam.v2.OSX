
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

bool ac::image_matrix_reset = false, ac::image_cycle_reset = false;

void ac::ColorImageMatrixFade(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static int speed = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_container.pix_values[i][z].dir[j];
                    PixelValues &pix = pix_container.pix_values[i][z];
                    if(d == 1) {
                        pix.col[j] += speed;
                        if(pix.col[j] >= 254) {
                            pix.col[j] = static_cast<unsigned char>((img_pix[j] * 0.5) + (pixel[j] * 0.5));
                            pix.dir[j] = 0;
                        }
                    } else {
                        pix.col[j] -= speed;
                        if(pix.col[j] <= 1) {
                            pix.col[j] = 1;
                            pix.dir[j] = 1;
                        }
                    }
                    pixel[j] = pix_container.pix_values[i][z].col[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorImageMatrixFadeFast(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static PixelArray2D pix_container;
    
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static int speed = 5;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_container.pix_values[i][z].dir[j];
                    PixelValues &pix = pix_container.pix_values[i][z];
                    if(d == 1) {
                        pix.col[j] += speed;
                        if(pix.col[j] >= 254) {
                            pix.col[j] = static_cast<unsigned char>((img_pix[j] * 0.5) + (pixel[j] * 0.5));
                            pix.dir[j] = 0;
                        }
                    } else {
                        pix.col[j] -= speed;
                        if(pix.col[j] <= 1) {
                            pix.col[j] = 1;
                            pix.dir[j] = 1;
                        }
                    }
                    pixel[j] = pix_container.pix_values[i][z].col[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorImageMatrixFadeDirection(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, -1);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static int speed = 5;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_container.pix_values[i][z].dir[j];
                    PixelValues &pix = pix_container.pix_values[i][z];
                    if(d == 1) {
                        pix.col[j] += speed;
                        if(pix.col[j] >= 254) {
                            pix.col[j] = static_cast<unsigned char>((img_pix[j] * 0.5) + (pixel[j] * 0.5));
                            pix.dir[j] = 0;
                        }
                    } else {
                        pix.col[j] -= speed;
                        if(pix.col[j] <= 1) {
                            pix.col[j] = pixel[j];
                            pix.dir[j] = rand()%2;
                        }
                    }
                    pixel[j] = pix_container.pix_values[i][z].col[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorImageMatrixFadeDirectionBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, -1);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static int speed = 5;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_container.pix_values[i][z].dir[j];
                    PixelValues &pix = pix_container.pix_values[i][z];
                    if(d == 1) {
                        pix.col[j] += speed;
                        if(pix.col[j] >= 254) {
                            pix.col[j] = static_cast<unsigned char>((img_pix[j] * 0.5) + (pixel[j] * 0.5));
                            pix.dir[j] = 0;
                        }
                    } else {
                        pix.col[j] -= speed;
                        if(pix.col[j] <= 1) {
                            pix.col[j] = static_cast<unsigned char>((img_pix[j] * 0.5) + (pixel[j] * 0.5));
                            pix.dir[j] = 1;
                        }
                    }
                    pixel[j] = static_cast<unsigned char>(pixel[j] * 0.33) + static_cast<unsigned char>(0.33 * pix_container.pix_values[i][z].col[j]) + static_cast<unsigned char>(img_pix[j] * 0.33);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorMatrixCollectionPixelation(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, -2);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &pix_val = pix_container.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = collection.frames[pix_val.dir[j]].at<cv::Vec3b>(z, i);
                    pixel[j] = pix[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorMatrixColllectionImagePixelation(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    ColorTransition(reimage);
    static MatrixCollection<2> collection;
    collection.shiftFrames(frame);
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, -1);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &pix_val = pix_container.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    if(pix_val.dir[j] >= 1) {
                        cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                        pixel[j] = pix[j];
                    }
                    else {
                        cv::Vec3b pix = collection.frames[pix_val.dir[j]].at<cv::Vec3b>(z, i);
                        pixel[j] = pix[j];
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorMatrixCollectionImagePixelationSubFilter(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ColorMatrixCollectionImagePixelationSubFilter")
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    static MatrixCollection<2> collection;
    collection.shiftFrames(frame);
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, -1);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &pix_val = pix_container.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    if(pix_val.dir[j] >= 1) {
                        cv::Vec3b pix = reimage.at<cv::Vec3b>(z, i);
                        pixel[j] = pix[j];
                    }
                    else {
                        cv::Vec3b pix = collection.frames[pix_val.dir[j]].at<cv::Vec3b>(z, i);
                        pixel[j] = pix[j];
                    }
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorImageMatrixMedianBlend(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static PixelArray2D pix_container;
    
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static int speed = 5;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_container.pix_values[i][z].dir[j];
                    PixelValues &pix = pix_container.pix_values[i][z];
                    if(d == 1) {
                        pix.col[j] += speed;
                        if(pix.col[j] >= 254) {
                            pix.col[j] = static_cast<unsigned char>((img_pix[j] * 0.5) + (pixel[j] * 0.5));
                            pix.dir[j] = 0;
                        }
                    } else {
                        pix.col[j] -= speed;
                        if(pix.col[j] <= 1) {
                            pix.col[j] = 1;
                            pix.dir[j] = 1;
                        }
                    }
                    pixel[j] = static_cast<unsigned char>(pixel[j] * 0.33) + static_cast<unsigned char>(pix_container.pix_values[i][z].col[j] * 0.5);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    DarkenFilter(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::ColorImageBlendPixelMedian(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static int speed = 5;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_container.pix_values[i][z].dir[j];
                    PixelValues &pix = pix_container.pix_values[i][z];
                    if(d == 1) {
                        pix.col[j] += speed;
                        if(pix.col[j] >= 254) {
                            pix.col[j] = static_cast<unsigned char>((img_pix[j] * 0.5) + (pixel[j] * 0.5));
                            pix.dir[j] = 0;
                        }
                    } else {
                        pix.col[j] -= speed;
                        if(pix.col[j] <= 1) {
                            pix.col[j] = 1;
                            pix.dir[j] = 1;
                        }
                    }
                    pixel[j] = static_cast<unsigned char>(pixel[j] * 0.25) + static_cast<unsigned char>(img_pix[j] * 0.25) + static_cast<unsigned char>(pix_container.pix_values[i][z].col[j] * 0.5);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    DarkenFilter(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::ColorImageBlendOrigMedian(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static int speed = 5;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_container.pix_values[i][z].dir[j];
                    PixelValues &pix = pix_container.pix_values[i][z];
                    if(d == 1) {
                        pix.col[j] += speed;
                        if(pix.col[j] >= 254) {
                            pix.col[j] = static_cast<unsigned char>((img_pix[j] * 0.5) + (pixel[j] * 0.5));
                            pix.dir[j] = 0;
                        }
                    } else {
                        pix.col[j] -= speed;
                        if(pix.col[j] <= 1) {
                            pix.col[j] = 1;
                            pix.dir[j] = 1;
                        }
                    }
                    pixel[j] = static_cast<unsigned char>(pixel[j] * 0.5) + static_cast<unsigned char>(img_pix[j] * 0.25) + static_cast<unsigned char>(pix_container.pix_values[i][z].col[j] * 0.25);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    DarkenFilter(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::ColorImageBlendSource(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static int speed = 5;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                cv::Vec3b src_pix = pixelAt(orig_frame,z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_container.pix_values[i][z].dir[j];
                    PixelValues &pix = pix_container.pix_values[i][z];
                    if(d == 1) {
                        pix.col[j] += speed;
                        if(pix.col[j] >= 254) {
                            pix.col[j] = static_cast<unsigned char>((img_pix[j] * 0.5) + (pixel[j] * 0.5));
                            pix.dir[j] = 0;
                        }
                    } else {
                        pix.col[j] -= speed;
                        if(pix.col[j] <= 1) {
                            pix.col[j] = 1;
                            pix.dir[j] = 1;
                        }
                    }
                    pixel[j] = static_cast<unsigned char>(pixel[j] * 0.40) + static_cast<unsigned char>(src_pix[j] * 0.30) + static_cast<unsigned char>(img_pix[j] * 0.20) + static_cast<unsigned char>(pix_container.pix_values[i][z].col[j] * 0.50);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    DarkenFilter(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::ColorImageMatrixFadeOnAndOff(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static PixelArray2D pix_container;
    
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, -3);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static int speed = 10;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_container.pix_values[i][z].dir[j];
                    PixelValues &pix = pix_container.pix_values[i][z];
                    if(d == 1) {
                        pix.col[j] += speed;
                        if(pix.col[j] >= 254) {
                            pix.col[j] = static_cast<unsigned char>((img_pix[j] * 0.5) + (pixel[j] * 0.5));
                            pix.dir[j] = 0;
                        }
                    } else {
                        pix.col[j] -= speed;
                        if(pix.col[j] <= 1) {
                            pix.col[j] = 1;
                            pix.dir[j] = 1;
                        }
                    }
                    pixel[j] = static_cast<unsigned char>((pixel[j] * 0.5) + (pix_container.pix_values[i][z].col[j] * 0.5));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorImageMatrixRect(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, -4);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static int speed = 10;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_container.pix_values[i][z].dir[j];
                    PixelValues &pix = pix_container.pix_values[i][z];
                    if(d == 1) {
                        pix.col[j] += speed;
                        if(pix.col[j] >= 254) {
                            pix.col[j] = static_cast<unsigned char>((img_pix[j] * 0.5) + (pixel[j] * 0.5));
                            pix.dir[j] = 0;
                        }
                    } else {
                        pix.col[j] -= speed;
                        if(pix.col[j] <= 1) {
                            pix.col[j] = 1;
                            pix.dir[j] = 1;
                        }
                    }
                    pixel[j] = static_cast<unsigned char>((pixel[j] * 0.5) + (pix_container.pix_values[i][z].col[j] * 0.5));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorImageBackAndForth(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static int speed = 5;
    static double alpha1 = 0.1;
    static int dir1 = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_container.pix_values[i][z].dir[j];
                    PixelValues &pix = pix_container.pix_values[i][z];
                    if(d == 1) {
                        pix.col[j] += speed;
                        if(pix.col[j] >= 254) {
                            pix.col[j] = static_cast<unsigned char>((img_pix[j] * 0.5) + (pixel[j] * 0.5));
                            pix.dir[j] = 0;
                        }
                    } else {
                        pix.col[j] -= speed;
                        if(pix.col[j] <= 1) {
                            pix.col[j] = 1;
                            pix.dir[j] = 1;
                        }
                    }
                    pixel[j] = static_cast<unsigned char>((pixel[j] * alpha1) + (pix_container.pix_values[i][z].col[j] * (1.0-alpha1)));
                }
            }
        }
    };
    AlphaMovementMaxMin(alpha1, dir1, 0.01, 1.0, 0.1);
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorImageFadeReallyFast(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static PixelArray2D pix_container;
    
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static int speed = 15;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_container.pix_values[i][z].dir[j];
                    PixelValues &pix = pix_container.pix_values[i][z];
                    if(d == 1) {
                        pix.col[j] += speed;
                        if(pix.col[j] >= 254) {
                            pix.col[j] = static_cast<unsigned char>((img_pix[j] * 0.5) + (pixel[j] * 0.5));
                            pix.dir[j] = 0;
                        }
                    } else {
                        pix.col[j] -= speed;
                        if(pix.col[j] <= 1) {
                            pix.col[j] = 1;
                            pix.dir[j] = 1;
                        }
                    }
                    pixel[j] = pix_container.pix_values[i][z].col[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorImageSubFilterXor(cv::Mat &frame) {
    if(blend_set == false || subfilter == -1 || draw_strings[subfilter] == "ColorImageSubFilterXor")
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    CallFilter(subfilter, reimage);
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static int speed = 25, dir = 0;
    static double alpha = 1.0;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_container.pix_values[i][z].dir[j];
                    PixelValues &pix = pix_container.pix_values[i][z];
                    if(d == 1) {
                        pix.col[j] += speed;
                        if(pix.col[j] >= 254) {
                            pix.col[j] = static_cast<unsigned char>((img_pix[j] * 0.5) + (pixel[j] * 0.5));
                            pix.dir[j] = 0;
                        }
                    } else {
                        pix.col[j] -= speed;
                        if(pix.col[j] <= 1) {
                            pix.col[j] = 1;
                            pix.dir[j] = 1;
                        }
                    }
                    pixel[j] = static_cast<unsigned char>((img_pix[j]*alpha))^ static_cast<unsigned char>(pix_container.pix_values[i][z].col[j] * 1-alpha);
                }
            }
        }
    };
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorImageAllXor(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static PixelArray2D pix_container;
    static int max_frame = static_cast<int>(ac::fps) * 10;
    static int frame_counter = 0;
    ++frame_counter;
    static int pix_x = 0, pix_y = 0;
    if(frame_counter > max_frame || image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
        frame_counter = 0;
    }
    static int speed = 5;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_container.pix_values[i][z].dir[j];
                    PixelValues &pix = pix_container.pix_values[i][z];
                    if(d == 1) {
                        pix.col[j] += speed;
                        if(pix.col[j] >= 254) {
                            pix.col[j] = static_cast<unsigned char>((img_pix[j] * 0.5) + (pixel[j] * 0.5));
                            pix.dir[j] = 0;
                        }
                    } else {
                        pix.col[j] -= speed;
                        if(pix.col[j] <= 1) {
                            pix.col[j] = 1;
                            pix.dir[j] = 1;
                        }
                    }
                    pixel[j] = img_pix[j]^pixel[j]^pix_container.pix_values[i][z].col[j];
                }
            }
        }
    };
    
    
    
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}


void ac::ColorChannelIteration(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_container.pix_values[i][z].dir[j];
                    PixelValues &pix = pix_container.pix_values[i][z];
                    if(d == 1) {
                        pix.col[j] += pix.speed;
                        if(pix.col[j] >= 254) {
                            pix.col[j] = pixel[j];
                            pix.dir[j] = 0;
                        }
                    } else {
                        pix.col[j] -= pix.speed;
                        if(pix.col[j] <= 1) {
                            pix.col[j] = 1;
                            pix.dir[j] = 1;
                        }
                    }
                    pixel[j] = pix_container.pix_values[i][z].col[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    DarkenFilter(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::ColorImageMatrixFadeVariable(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static PixelArray2D pix_container;
    
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_container.pix_values[i][z].dir[j];
                    PixelValues &pix = pix_container.pix_values[i][z];
                    if(d == 1) {
                        //pix.col[j] += speed;
                        pix.col[j] += pix.speed;
                        if(pix.col[j] >= 254) {
                            pix.col[j] = static_cast<unsigned char>((img_pix[j] * 0.5) + (pixel[j] * 0.5));
                            pix.dir[j] = 0;
                        }
                    } else {
                        //pix.col[j] -= speed;
                        pix.col[j] -= pix.speed;
                        if(pix.col[j] <= 1) {
                            pix.col[j] = 1;
                            pix.dir[j] = 1;
                        }
                    }
                    pixel[j] = pix_container.pix_values[i][z].col[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    DarkenFilter(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::ColorXorImageFade(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static PixelArray2D pix_container;
    static double alpha = 0.1;
    static int dir = 1;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static int speed = 5;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_container.pix_values[i][z].dir[j];
                    PixelValues &pix = pix_container.pix_values[i][z];
                    if(d == 1) {
                        pix.col[j] += speed;
                        if(pix.col[j] >= 254) {
                            pix.col[j] = static_cast<unsigned char>((img_pix[j] * 0.5) + (pixel[j] * 0.5));
                            pix.dir[j] = 0;
                        }
                    } else {
                        pix.col[j] -= speed;
                        if(pix.col[j] <= 1) {
                            pix.col[j] = 1;
                            pix.dir[j] = 1;
                        }
                    }
                    unsigned char ch = static_cast<unsigned char>(pixel[j] * alpha) + static_cast<unsigned char>(img_pix[j] * 1.0-alpha);
                    pixel[j] = ch ^ pix_container.pix_values[i][z].col[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.01, 1.0, 0.1);
}

void ac::ColorFreezeBlend(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static int speed = 5;
    static double alpha = 1.0;
    static int dir = 1;
    static int offset = 0;
    int colors[3] = {0};
    InitArrayPosition(colors, offset);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_container.pix_values[i][z].dir[j];
                    PixelValues &pix = pix_container.pix_values[i][z];
                    if(d == 1) {
                        pix.col[j] += speed;
                        if(pix.col[j] >= 254) {
                            pix.col[j] = pixel[j];
                            pix.dir[j] = 0;
                        }
                    } else {
                        pix.col[j] -= speed;
                        if(pix.col[j] <= 1) {
                            pix.col[j] = 1;
                            pix.dir[j] = 1;
                        }
                    }
                    pixel[j] = pix_container.pix_values[i][z].col[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.05, 2.0, 1.0);;
    ++offset;
    if(offset > 2)
        offset = 0;
}

void ac::ColorImageFill(cv::Mat &frame) {

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
    static int speed = 1;
    InitArrayPosition(colors, offset);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_color = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_container.pix_values[i][z].dir[j];
                    PixelValues &pix = pix_container.pix_values[i][z];
                    if(d == 1) {
                        pix.col[j] += speed;
                        if(pix.col[j] == img_color[j]) {
                            pix.dir[j] = 2;
                        }
                        else if(pix.col[j] >= 254) {
                            pix.col[j] = pixel[j];
                            pix.dir[j] = 0;
                        }
                    } else if(d == 0) {
                        pix.col[j] -= speed;
                        if(pix.col[j] == img_color[j]) {
                            pix.dir[j] = 2;
                        } else if(pix.col[j] <= 1) {
                            pix.col[j] = 1;
                            pix.dir[j] = 1;
                        }
                    } 
                    pixel[j] = pix_container.pix_values[i][z].col[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

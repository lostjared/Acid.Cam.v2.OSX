
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

bool ac::image_matrix_reset = false;

void ac::ColorImageMatrixFade(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static PixelValues** pix_values = 0;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        if(pix_values != 0 && pix_x != 0 && pix_y != 0) {
            for(int j = 0; j < pix_x; ++j) {
                delete [] pix_values[j];
            }
            delete [] pix_values;
            pix_values = 0;
        }
        pix_x = frame.cols;
        pix_y = frame.rows;
        pix_values = new PixelValues*[pix_x];
        for(int i = 0; i < pix_x; ++i) {
            pix_values[i] = new PixelValues[pix_y];
        }
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pix_values[i][z].col[j] = pixel[j];
                    pix_values[i][z].dir[j] = 0;
                }
            }
        }
    }
    static int speed = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_values[i][z].dir[j];
                    PixelValues &pix = pix_values[i][z];
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
                    pixel[j] = pix_values[i][z].col[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorImageMastrixFadeFast(cv::Mat &frame) {
    if(blend_set == false)
        return;
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    static PixelValues** pix_values = 0;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        if(pix_values != 0 && pix_x != 0 && pix_y != 0) {
            for(int j = 0; j < pix_x; ++j) {
                delete [] pix_values[j];
            }
            delete [] pix_values;
            pix_values = 0;
        }
        pix_x = frame.cols;
        pix_y = frame.rows;
        pix_values = new PixelValues*[pix_x];
        for(int i = 0; i < pix_x; ++i) {
            pix_values[i] = new PixelValues[pix_y];
        }
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pix_values[i][z].col[j] = pixel[j];
                    pix_values[i][z].dir[j] = 0;
                }
            }
        }
    }
    static int speed = 5;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_values[i][z].dir[j];
                    PixelValues &pix = pix_values[i][z];
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
                    pixel[j] = pix_values[i][z].col[j];
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
    static PixelValues** pix_values = 0;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        if(pix_values != 0 && pix_x != 0 && pix_y != 0) {
            for(int j = 0; j < pix_x; ++j) {
                delete [] pix_values[j];
            }
            delete [] pix_values;
            pix_values = 0;
        }
        pix_x = frame.cols;
        pix_y = frame.rows;
        pix_values = new PixelValues*[pix_x];
        for(int i = 0; i < pix_x; ++i) {
            pix_values[i] = new PixelValues[pix_y];
        }
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pix_values[i][z].col[j] = pixel[j];
                    pix_values[i][z].dir[j] = rand()%2;
                }
            }
        }
    }
    static int speed = 5;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_values[i][z].dir[j];
                    PixelValues &pix = pix_values[i][z];
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
                    pixel[j] = pix_values[i][z].col[j];
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
    static PixelValues **pix_values = 0;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        if(pix_values != 0 && pix_x != 0 && pix_y != 0) {
            // reset
            for(int j = 0; j < pix_x; ++j) {
                delete [] pix_values[j];
            }
            delete [] pix_values;
            pix_values = 0;
        }
        pix_x = frame.cols;
        pix_y = frame.rows;
        pix_values = new PixelValues*[pix_x];
        for(int i = 0; i < pix_x; ++i) {
            pix_values[i] = new PixelValues[pix_y];
        }
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pix_values[i][z].col[j] = pixel[j];
                    pix_values[i][z].dir[j] = rand()%2;
                }
            }
        }
    }
    static int speed = 5;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    int &d = pix_values[i][z].dir[j];
                    PixelValues &pix = pix_values[i][z];
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
                    pixel[j] = static_cast<unsigned char>(pixel[j] * 0.33) + static_cast<unsigned char>(0.33 * pix_values[i][z].col[j]) + static_cast<unsigned char>(img_pix[j] * 0.33);
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
    static PixelValues **pix_values = 0;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        if(pix_values != 0 && pix_x != 0 && pix_y != 0) {
            // reset
            for(int j = 0; j < pix_x; ++j) {
                delete [] pix_values[j];
            }
            delete [] pix_values;
            pix_values = 0;
        }
        pix_x = frame.cols;
        pix_y = frame.rows;
        pix_values = new PixelValues*[pix_x];
        for(int i = 0; i < pix_x; ++i) {
            pix_values[i] = new PixelValues[pix_y];
        }
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pix_values[i][z].col[j] = pixel[j];
                    pix_values[i][z].dir[j] = rand()%8;
                }
            }
        }
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &pix_val = pix_values[i][z];
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
    static PixelValues **pix_values = 0;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        if(pix_values != 0 && pix_x != 0 && pix_y != 0) {
            // reset
            for(int j = 0; j < pix_x; ++j) {
                delete [] pix_values[j];
            }
            delete [] pix_values;
            pix_values = 0;
        }
        pix_x = frame.cols;
        pix_y = frame.rows;
        pix_values = new PixelValues*[pix_x];
        for(int i = 0; i < pix_x; ++i) {
            pix_values[i] = new PixelValues[pix_y];
        }
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pix_values[i][z].col[j] = pixel[j];
                    pix_values[i][z].dir[j] = rand()%2;
                }
            }
        }
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &pix_val = pix_values[i][z];
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
    static PixelValues **pix_values = 0;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        if(pix_values != 0 && pix_x != 0 && pix_y != 0) {
            // reset
            for(int j = 0; j < pix_x; ++j) {
                delete [] pix_values[j];
            }
            delete [] pix_values;
            pix_values = 0;
        }
        pix_x = frame.cols;
        pix_y = frame.rows;
        pix_values = new PixelValues*[pix_x];
        for(int i = 0; i < pix_x; ++i) {
            pix_values[i] = new PixelValues[pix_y];
        }
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pix_values[i][z].col[j] = pixel[j];
                    pix_values[i][z].dir[j] = rand()%2;
                }
            }
        }
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &pix_val = pix_values[i][z];
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

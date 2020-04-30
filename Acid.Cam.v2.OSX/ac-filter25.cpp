
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
            cv::Vec3b &pixel = pixelAt(frame,z, i);
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
    if(count_index+100 < static_cast<int>(pix_container.pixel_index.size())) {
        for(int i = count_index; i < static_cast<int>(pix_container.pixel_index.size()) && i < count_index+100; ++i) {
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
                    pixel[j] = static_cast<unsigned char>(1-alpha * pixel[j]) + static_cast<unsigned char>(alpha * pix_container.pix_values[i][z].col[j]);
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
                    pixel[j] = static_cast<unsigned char>(1-alpha * pixel[j]) + static_cast<unsigned char>(alpha * pix_container1.pix_values[i][z].col[j]);
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    if(count_index+100 < static_cast<int>(pix_container1.pixel_index.size())) {
        for(int i = count_index; i < static_cast<int>(pix_container1.pixel_index.size()) && i < count_index+100; ++i) {
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


void ac::ManualShell(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int pix_x = 0, pix_y = 0;
    static int speed = 1;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &pix = pix_container.pix_values[i][z];
                cv::Vec3b colors[3];
                colors[0] = collection.frames[1].at<cv::Vec3b>(z, i);
                colors[1][0] = pix.col[0];
                colors[1][1] = pix.col[1];
                colors[1][2] = pix.col[2];
                colors[2] = collection.frames[7].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    switch(pix.dir[j]) {
                        case 1:
                            pix.col[j] += speed;
                            if(pix.col[j] >= 255) {
                                pix.dir[j] = 0;
                                pix.col[j] = colors[j][j];
                            }
                            break;
                        case 0:
                            pix.col[j] -= speed;
                            if(pix.col[j] <= 0) {
                                pix.dir[j] = 1;
                                pix.col[j] = colors[3-j-1][j];
                            }
                            break;
                        case 2:
                            break;
                    }
                    pixel[j] = colors[j][j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorIncrementRandomReset(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int pix_x = 0, pix_y = 0;
    static const int speed = 1;
    static int counter = 0;
    static int frames = 0;
    bool image_reset = false;
    ++frames;
    if(frames > static_cast<int>(ac::fps)) {
        ++counter;
        frames = 0;
    }
    if(counter > 10) {
        image_reset = true;
        counter = 0;
        frames = 0;
    }
    ColorTransition(collection.frames[7]);
    if(image_reset == true || image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                cv::Vec3b matpix = collection.frames[7].at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    switch(p.dir[j]) {
                        case 0:
                            p.col[j] -= speed;
                            if(p.col[j] <= 0) {
                                p.col[j] = matpix[j];
                            }
                            break;
                        case 1:
                            p.col[j] += speed;
                            if(p.col[j] >= 255) {
                                p.col[j] = matpix[j];
                            }
                            break;
                        case 2:
                            break;
                    }
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * p.col[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorImageIncrementResetCollection(cv::Mat &frame) {
    if(blend_set == false)
        return;
    
    static PixelArray2D pix_container;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int pix_x = 0, pix_y = 0;
    static const int speed = 25;
    cv::Mat copy1 = collection.frames[7].clone();
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    static double alpha = 1.0;
    static int dir = 1;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                cv::Vec3b matpix = copy1.at<cv::Vec3b>(z, i);
                cv::Vec3b imgpix = reimage.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    switch(p.dir[j]) {
                        case 0:
                            p.col[j] -= speed;
                            if(p.col[j] <= 0) {
                                p.col[j] = static_cast<unsigned char>((pixel[j] * 0.33) + (matpix[j] * 0.33) + (imgpix[j] * 0.33));
                                p.dir[j] = 1;
                            }
                            break;
                        case 1:
                            p.col[j] += speed;
                            if(p.col[j] >= 255) {
                                p.col[j] = pixel[j];
                                p.dir[j] = 0;
                            }
                            break;
                        case 2:
                            break;
                    }
                    pixel[j] = static_cast<unsigned char>((alpha * pixel[j]) + ((1-alpha) * p.col[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MedianBlur(frame);
    DarkenFilter(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha,dir,0.01,2.0,0.5);
}

void ac::ColorMoveDown(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    static const int speed = 5;
    static int counter = 0;
    static int frames = 0;
    bool image_reset = false;
    ++frames;
    if(frames > static_cast<int>(ac::fps)) {
        ++counter;
        frames = 0;
    }
    if(counter > 5) {
        image_reset = true;
        counter = 0;
        frames = 0;
    }
    if(image_reset == true || image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    p.col[j] -= speed;
                    if(p.col[j] <= 0) {
                        p.col[j] = 255;
                    }
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * p.col[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::ColorMoveDownResetMedianBlend(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static MatrixCollection<32> collection;
    collection.shiftFrames(frame);
    static int pix_x = 0, pix_y = 0;
    static int speed = 1;
    bool reset_value = false;
    static int index = 0;
    if(++index > 30) {
        reset_value = true;
        index = 0;
    }
    if(reset_value == true || image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        cv::Mat copy1 = collection.frames[31].clone();
        pix_container.create(copy1, copy1.cols, copy1.rows, 0);
        pix_x = copy1.cols;
        pix_y = copy1.rows;
        ++speed;
        if(speed > 15) {
            speed = 1;
        }
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    p.col[j] -= speed;
                    if(p.col[j] <= 0) {
                        p.col[j] = 255;
                    }
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * p.col[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::ColorMoveDownSmoothMedianBlend(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int pix_x = 0, pix_y = 0;
    static int speed = 5;
    bool reset_value = false;
    static int frame_counter = 0;
    static int index = 0;
    if(++frame_counter > static_cast<int>(ac::fps)) {
        ++index;
        frame_counter = 0;
    }
    if(index > 10) {
        index = 0;
        reset_value = true;
        ++speed;
        if(speed > 15) {
            speed = 5;
        }
    }
    if(reset_value == true || image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        cv::Mat copy1 = frame.clone();
        Smooth(copy1, &collection);
        pix_container.create(copy1, copy1.cols, copy1.rows, 0);
        pix_x = copy1.cols;
        pix_y = copy1.rows;
    }
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    p.col[j] -= speed;
                    if(p.col[j] <= 0) {
                        p.col[j] = 255;
                    }
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * p.col[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
}

void ac::PixelRandom3(cv::Mat &frame) {
    cv::Mat frames[3];
    for(int j = 0; j < 3; ++j) {
        frames[j] = frame.clone();
        Random_Filter(frames[j]);
    }
    static int index1 = 0;
    static int index2 = rand()%3;
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b  &pixel = frame->at<cv::Vec3b>(z, i);
                int col1[3] = {0}, col2[3] = {0};
                InitArrayPosition(col1, index1);
                InitArrayPosition(col2, index2);
                for(int j = 0; j < 3; ++j) {
                    cv::Vec3b pix = frames[col1[j]].at<cv::Vec3b>(z, i);
                    pixel[j] = pix[col2[j]];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    ++index1;
    if(index1 > 2)
        index1 = 0;
    ++index2;
    if(index2 > 2)
        index2 = 0;
    AddInvert(frame);
}

// pixel moves left
void ac::ShiftMatrixLeft(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols-1; ++i) {
            PixelValues &p = pix_container.pix_values[i][z];
            if(i <= 0) {
                pix_container.pix_values[frame.cols-1][z] = p;
            }
            pix_container.pix_values[i][z] = pix_container.pix_values[i+1][z];
            cv::Vec3b &pixel = pixelAt(frame,z, i);
            for(int j = 0; j < 3; ++j) {
                pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * p.col[j]));
            }
        }
    }
}

void ac::ShiftMatrixLeftSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "ShiftMatrixLeftSubFilter")
        return;
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols-1; ++i) {
            PixelValues &p = pix_container.pix_values[i][z];
            if(i <= 0) {
                pix_container.pix_values[frame.cols-1][z] = p;
            }
            pix_container.pix_values[i][z] = pix_container.pix_values[i+1][z];
        }
    }
    cv::Mat pix_frame;
    pix_container.generateMatrix(pix_frame);
    CallFilter(subfilter, pix_frame);
    cv::Mat frame_copy = frame.clone();
    AlphaBlend(frame_copy, pix_frame, frame, 0.5);
}

void ac::ShiftMatrixUpSubFilter(cv::Mat &frame) {
    if(subfilter == -1 || draw_strings[subfilter] == "ShiftMatrixUpSubFilter")
        return;
    static PixelArray2D pix_container;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    for(int z = 0; z < frame.rows-1; ++z) {
        for(int i = 0; i < frame.cols-1; ++i) {
            PixelValues &p = pix_container.pix_values[i][z];
            if(z <= 0) {
                pix_container.pix_values[i][frame.rows-1] = p;
            }
            pix_container.pix_values[i][z] = pix_container.pix_values[i][z+1];
        }
    }
    cv::Mat pix_frame;
    pix_container.generateMatrix(pix_frame);
    CallFilter(subfilter, pix_frame);
    cv::Mat frame_copy = frame.clone();
    AlphaBlend(frame_copy, pix_frame, frame, 0.5);
}

void ac::PixelatePixelValues(cv::Mat &frame) {
    static PixelArray2D pix_container, pix_container2;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
        pix_container2.create(frame, frame.cols, frame.rows, 0);
    }
    cv::Mat frame_copy = frame.clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                PixelValues &p1 = pix_container.pix_values[i][z];
                PixelValues &p2 = pix_container2.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    if(p1.col[j] < p2.col[j]) {
                        p1.col[j] += 1;
                    } else if(p1.col[j] > p2.col[j]) {
                        p1.col[j] -= 1;
                    } else if(p1.col[j] == p2.col[j]) {
                        cv::Vec3b pix = frame_copy.at<cv::Vec3b>(z, i);
                        p2.col[j] = pix[j];
                        p1.col[j] = static_cast<int>(0.5 * pix[j]);
                    }
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * p1.col[j]));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::StretchCollection(cv::Mat &frame) {
    static MatrixCollection<720> collection;
    cv::Mat resized;
    ac_resize(frame, resized, cv::Size(1280, 720));
    collection.shiftFrames(resized);
    for(int z = 0; z < resized.rows; ++z) {
        int max = 5+(rand()%50);
        for(int q = 0; q < max && z+q < resized.rows; ++q) {
            for(int i = 0; i < resized.cols; ++i) {
                cv::Vec3b &pixel = resized.at<cv::Vec3b>(z+q, i);
                cv::Vec3b value = collection.frames[z].at<cv::Vec3b>(z+q, i);
                pixel = value;
            }
        }
        z += max;
    }
    ac_resize(resized, frame, frame.size());
    AddInvert(frame);
}

void ac::PsychedelicSlitScan(cv::Mat &frame) {
    static MatrixCollection<720> collection;
    cv::Mat resized;
    ac_resize(frame, resized, cv::Size(1280, 720));
    static double alpha = 1.0;
    static int dir = 1;
    collection.shiftFrames(resized);
    for(int z = 0; z < resized.rows; ++z) {
        int max = 5+(rand()%50);
        for(int q = 0; q < max && z+q < resized.rows; ++q) {
            for(int i = 0; i < resized.cols; ++i) {
                cv::Vec3b &pixel = resized.at<cv::Vec3b>(z+q, i);
                cv::Vec3b value = collection.frames[z+q].at<cv::Vec3b>(z+q, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(value[j] * alpha);
                }
            }
        }
        z += max-1;
    }
    ac_resize(resized, frame, frame.size());
    ColorTransition(frame);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha,dir,0.01, 1.0, 0.5);
}

void ac::SineValue(cv::Mat &frame) {
    static double alpha = 1.0;
    static int dir = 1;
    cv::Mat copy1 = frame.clone();
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>(1+static_cast<int>((sin(alpha))*(i+z)));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    cv::Mat copy2 = frame.clone();
    AlphaBlend(copy1, copy2, frame, alpha);
    MedianBlendMultiThread(frame);
    AddInvert(frame);
    AlphaMovementMaxMin(alpha, dir, 0.1, 1.0, 0.01);
}

void ac::SineTransitionUpLeft(cv::Mat &frame) {
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    MedianBlur(frame);
    MedianBlur(frame);
    cv::Mat copy1 = frame.clone();
    Smooth(copy1, &collection);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                int col[3] = {0};
                for(int q = 0; q < collection.size(); ++q) {
                    cv::Vec3b vcol = collection.frames[q].at<cv::Vec3b>(z, i);
                    for(int j = 0; j < 3; ++j) {
                        col[j] += vcol[j];
                    }
                }
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    pixel[j] = static_cast<unsigned char>((pixel[j] * 0.5) + (col[j] * 0.5));
                }
            }
        }
    };
    ColorTransition(frame);
    UseMultipleThreads(frame, getThreadCount(), callback);
    SineValue(frame);
    AddInvert(frame);
}

void ac::TemporaryTrails(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    
    cv::Mat copy1 = collection.frames[collection.size()-1].clone();
    pix_container.insert(copy1);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                PixelValues &p = pix_container.pix_values[i][z];
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j) {
                    p.add[j]++;
                    if(p.add[j] > 150)
                        p.add[j] = pixel[j];
                    pixel[j] = pixel[j]^p.add[j];
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    AddInvert(frame);
}

void ac::StrobingPixelDissolve(cv::Mat &frame) {
    static PixelArray2D pix_container;
    static MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    cv::Mat copy1 = frame.clone();
    Smooth(copy1, &collection);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = copy1.at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    if(pix[j] == pixel[j]) {
                        p.col[j] = pixel[j];
                        p.add[j] = 1;
                    }
                    if(p.add[j] == 1) {
                        p.col[j] += p.speed;
                        if(p.col[j] >= 255) {
                            p.add[j] = 0;
                            p.col[j] = pixel[j];
                        }
                    }
                    if(p.add[j] == 1)
                        pixel[j] = static_cast<unsigned char>((pixel[j] * 0.5) + (p.col[j] * 0.5));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    ColorCollectionReverseStrobe(frame);
    AddInvert(frame);
}

void ac::ImagePixelFrameBlend(cv::Mat &frame) {
    
    if(blend_set == false)
        return;
    
    cv::Mat reimage;
    ac_resize(blend_image, reimage, frame.size());
    
    static PixelArray2D pix_container;
    static MatrixCollection<8> collection;
    static int pix_x = 0, pix_y = 0;
    if(image_matrix_reset == true || pix_container.pix_values == 0 || frame.size() != cv::Size(pix_x, pix_y)) {
        pix_container.create(frame, frame.cols, frame.rows, 0);
        pix_x = frame.cols;
        pix_y = frame.rows;
    }
    cv::Mat copy1 = frame.clone();
    const int max = 1+(rand()%4);
    for(int j = 0; j < max; ++j)
        MedianBlur(copy1);
    
    Smooth(copy1, &collection);
    pix_container.insert(copy1);
    auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b img_pix = reimage.at<cv::Vec3b>(z, i);
                PixelValues &p = pix_container.pix_values[i][z];
                for(int j = 0; j < 3; ++j) {
                    if(p.dir[j] == 1) {
                        p.add[j] += 10;
                        if(p.add[j] > 150) {
                            p.add[j] = img_pix[j];
                            p.dir[j] = 0;
                        }
                    } else {
                        p.add[j] -= 10;
                        if(p.add[j] <= 25) {
                            p.add[j] = img_pix[j];
                            p.dir[j] = 1;
                        }
                    }
                    int pix_val = p.col[j]^p.add[j];
                    pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix_val));
                }
            }
        }
    };
    UseMultipleThreads(frame, getThreadCount(), callback);
    ColorCollectionReverseStrobe(frame);
    AddInvert(frame);
}

void ac::PreviewFilters(cv::Mat &frame) {
    static int filter_num = 0;
    static int frame_count = 0, seconds = 0;
    if (++frame_count > static_cast<int>(fps)) {
        frame_count = 0;
        ++seconds;
    }
    if(seconds > 4) {
        frame_count = 0;
        seconds = 0;
        if(filter_num > ac::getFilterCount()-2) {
            filter_num = 0;
        } else {
            ++filter_num;
        }
        release_frames = true;
    }
    std::string filter_name = ac::draw_strings[filter_num];
    if(filter_name == "PreviewFilters") {
        if(filter_num < ac::getFilterCount()-2)
            filter_name = ac::draw_strings[++filter_num];
        else {
            filter_num = 0;
            filter_name = ac::draw_strings[filter_num];
        }
    }
    //std::cout << filter_name << ": " << filter_num << "/" << getFilterCount()-2 << "\n";
    pushSubFilter(getFilterByName("MedianBlendMultiThread"));
    CallFilter(filter_name, frame);
    popSubFilter();
}


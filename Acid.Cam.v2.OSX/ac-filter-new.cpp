
#include "ac.h"
#include<list>

void ac::FrameSep(cv::Mat &frame) {
    static constexpr int MAX = 16;
    static ac::MatrixCollection<MAX> collection;
    collection.shiftFrames(frame);
    static int off = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Mat &f = collection.frames[off];
            cv::Vec3b pix = f.at<cv::Vec3b>(z, i);
            pixel = pix;
        }
        if(((z%240)==0) && ++off > MAX-1)
            off = 0;
    }
}

void ac::FrameSep2(cv::Mat &frame) {
    static constexpr int MAX = 16;
    static ac::MatrixCollection<MAX> collection;
    
    if(collection.empty()) {
        collection.shiftFrames(frame);
        srand(static_cast<unsigned int>(time(0)));
    } else if(rand()%5==0)
        collection.shiftFrames(frame);
    static bool on = false;
    static int off = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            if(on == false) break;
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Mat &f = collection.frames[off];
            cv::Vec3b pix = f.at<cv::Vec3b>(z, i);
            pixel = pix;
        }
        if(((z%240)==0) && ++off > MAX-1) {
            off = 0;
            static int cnt = 0;
            static int wait = rand()%10;
            ++cnt;
            if(cnt > wait) {
                on = !on;
                cnt = 0;
                wait = rand()%10;
            }
        }
    }
}

void ac::StutterFilter(cv::Mat  &frame) {
    static cv::Mat stored;
    static cv::Size stored_size;
    
    if(stored_size != frame.size()) {
        srand(static_cast<int>(time(0)));
        stored = frame.clone();
        stored_size = frame.size();
    } else {
        if(stored.empty())
            stored = frame.clone();
        
        static bool on = true;
        if(on == true) {
            if((rand()%15)==0) {
                stored = frame.clone();
                on = !on;
            }
            frame = stored.clone();
        } else {
            if((rand()%10) == 0)
                on = !on;
        }
    }
}

void ac::DefStutter(cv::Mat  &frame) {
    static ac::MatrixCollection<8> collection;
    if(collection.empty())
        srand(static_cast<unsigned int>(time(0)));
    collection.shiftFrames(frame);
    cv::Mat frame_copy = frame.clone();
    StutterFilter(frame_copy);
    int num_pixels = rand()%((frame.rows * frame.cols)/320);
    for(int q = 0; q < num_pixels; ++q) {
        int pixel_size = 16+(rand()%64);
        
        int off_x = rand()%(frame.cols-pixel_size-1);
        int off_y = rand()%(frame.rows-pixel_size-1);
        int offset = rand()%(collection.size()-1);
        
        for(int y = off_y; y < off_y + pixel_size; ++y) {
            for(int x = off_x; x < off_x + pixel_size; ++x) {
                cv::Mat &m = collection.frames[offset];
                cv::Vec3b &pix = m.at<cv::Vec3b>(y, x);
                cv::Vec3b &pix_copy = frame_copy.at<cv::Vec3b>(y, x);
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
                for(int j = 0; j < 3; ++j) {
                    if(pixel[j] != pix[j]) {
                        pixel[j] = static_cast<unsigned char>((0.5 * pixel[j]) + (0.5 * pix_copy[j]));
                    }
                }
            }
        }
    }
}

void ac::FrameOffsetRSize(cv::Mat &frame) {
    static ac::MatrixCollection<8> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
    }
    collection.shiftFrames(frame);
    static int rsize = 150;
    int rsize_c = 0;
    int rsize_count = rand()%rsize;
    static int offset = 0;
    int rsize_x = rand()%rsize;
    
    for(int z = 0; z < frame.rows; ++z) {
        int offset_x = 0;
        for(int i = rsize_x; i < frame.cols; ++i) {
            cv::Mat &m = collection.frames[offset];
            cv::Vec3b &pixel_off = m.at<cv::Vec3b>(z, offset_x);
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            
            for(int j = 0; j < 3; ++j) {
                pixel[j] = wrap_cast((pixel[j] * 0.5) + (0.5 * pixel_off[j]));
            }
            
            offset_x++;
        }
        
        ++rsize_c;
        if(rsize_c > rsize_count) {
            rsize = 50+rand()%50;
            rsize_x = rand()%rsize;
            rsize_c = 0;
            rsize_count = 25+rand()%rsize;
            offset++;
            if(offset > (collection.size()-1)) {
                offset = 0;
            }
        }
    }
}

void ac::FrameOffsetRSize2(cv::Mat &frame) {
    static ac::MatrixCollection<8> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
    }
    collection.shiftFrames(frame);
    static int rsize = 150;
    int rsize_c = 0;
    int rsize_count = rand()%rsize;
    static int offset = 0;
    int rsize_x = rand()%rsize;
    
    for(int z = 0; z < frame.rows; ++z) {
        int offset_x = 0;
        for(int i = rsize_x; i < frame.cols; ++i) {
            cv::Mat &m = collection.frames[offset];
            cv::Mat &m2 = collection.frames[7];
            
            cv::Vec3b &pixel_diff = m2.at<cv::Vec3b>(z, i);
            
            cv::Vec3b &pixel_off = m.at<cv::Vec3b>(z, offset_x);
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            
            for(int j = 0; j < 3; ++j) {
                if(abs(pixel[j]-pixel_diff[j]) > 15) {
                    pixel[j] = wrap_cast((pixel[j] * 0.5) + (0.5 * pixel_off[j]));
                }
            }
            offset_x++;
        }
        
        ++rsize_c;
        if(rsize_c > rsize_count) {
            rsize = 50+rand()%50;
            rsize_x = rand()%rsize;
            rsize_c = 0;
            rsize_count = 25+rand()%rsize;
            offset++;
            if(offset > (collection.size()-1)) {
                offset = 0;
            }
        }
    }
}


void ac::FrameOffsetRSize3(cv::Mat &frame) {
    static ac::MatrixCollection<8> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
    }
    collection.shiftFrames(frame);
    static int rsize = 150;
    int rsize_c = 0;
    int rsize_count = rand()%rsize;
    static int offset = 0;
    int rsize_x = rand()%rsize;
    int rgb = rand()%2;
    
    for(int z = 0; z < frame.rows; ++z) {
        int offset_x = 0;
        for(int i = rsize_x; i < frame.cols; ++i) {
            cv::Mat &m = collection.frames[offset];
            cv::Mat &m2 = collection.frames[4];
            cv::Vec3b &pixel_diff = m2.at<cv::Vec3b>(z, i);
            cv::Vec3b &pixel_off = m.at<cv::Vec3b>(z, offset_x);
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            bool add = false;
            for(int j = 0; j < 3; ++j) {
                if(abs(pixel[j]-pixel_diff[j]) > 15) {
                    add = true;
                    break;
                }
            }
            if(add == true)
                pixel[rgb] = wrap_cast((0.5 * pixel[rgb]) + (0.5 * pixel_off[rgb]));
            
            offset_x++;
        }
        ++rsize_c;
        if(rsize_c > rsize_count) {
            rsize = 50+rand()%50;
            rsize_x = rand()%rsize;
            rsize_c = 0;
            rsize_count = 25+rand()%rsize;
            offset++;
            if(offset > (collection.size()-1)) {
                offset = 0;
            }
            rgb++;
            if(rgb > 2)
                rgb = 0;
        }
    }
}

void ac::FrameOffsetRSize4(cv::Mat &frame) {
    static ac::MatrixCollection<8> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
    }
    collection.shiftFrames(frame);
    static int rsize = 150;
    int rsize_c = 0;
    int rsize_count = rand()%rsize;
    static int offset = 0;
    int rsize_x = rand()%rsize;
    bool on = true;
    if( rand()%10 > 5 )
        on = false;
    
    for(int z = 0; z < frame.rows; ++z) {
        int offset_x = 0;
        for(int i = rsize_x; i < frame.cols; ++i) {
            if(on == false) continue;
            
            cv::Mat &m = collection.frames[offset];
            cv::Vec3b &pixel_off = m.at<cv::Vec3b>(z, offset_x);
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            
            for(int j = 0; j < 3; ++j) {
                pixel[j] = wrap_cast((pixel[j] * 0.5) + (0.5 * pixel_off[j]));
            }
            
            offset_x++;
        }
        
        ++rsize_c;
        if(rsize_c > rsize_count) {
            rsize = 150+rand()%50;
            rsize_x = rand()%rsize;
            rsize_c = 0;
            rsize_count = 125+rand()%rsize;
            offset++;
            if(offset > (collection.size()-1)) {
                offset = 0;
            }
            on = (on == true) ? false : true;
        }
    }
}

void ac::SqOnOff(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static constexpr int PIXEL_SIZE=32;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
    }
    collection.shiftFrames(frame);
    
    static int off = 0;
    
    for(int z = 0; z < frame.rows; z += PIXEL_SIZE) {
        for(int i = 0; i < frame.cols; i += PIXEL_SIZE) {
            if((rand()%2)==0) {
                for(int y = z; y < z+PIXEL_SIZE && y < frame.rows; ++y) {
                    for(int x = i; x < i+PIXEL_SIZE && x < frame.cols; ++x) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
                        cv::Mat &m = collection.frames[off];
                        cv::Vec3b &pix_off = m.at<cv::Vec3b>(y, x);
                        pixel = pix_off;
                        
                    }
                }
            }
        }
        ++off;
        if(off > MAX-1)
            off = 0;
    }
    ac::AddInvert(frame);
}

void ac::SqOnOffAlpha(cv::Mat &frame) {
    static constexpr int MAX = 8;
    int PIXEL_SIZE=4+(rand()%28);
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
    }
    collection.shiftFrames(frame);
    static int off = 0;
    static double alpha = 0.1;
    for(int z = 0; z < frame.rows; z += PIXEL_SIZE) {
        for(int i = 0; i < frame.cols; i += PIXEL_SIZE) {
            if((rand()%2)==0) {
                for(int y = z; y < z+PIXEL_SIZE && y < frame.rows; ++y) {
                    for(int x = i; x < i+PIXEL_SIZE && x < frame.cols; ++x) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
                        cv::Mat &m = collection.frames[off];
                        cv::Vec3b &pix_off = m.at<cv::Vec3b>(y, x);
                        for(int q = 0; q < 3; ++q) {
                            pixel[q] = static_cast<unsigned char>((alpha*pixel[q])+((1-alpha)*pix_off[q]));
                        }
                    }
                }
            }
            
        }
        static int adir = 1;
        if(adir == 1) {
            alpha += 0.01;
            if(alpha >= 1.0)
                adir = 0;
        } else {
            alpha -= 0.01;
            if(alpha <= 0.1)
                adir = 1;
        }
        ++off;
        if(off > MAX-1)
            off = 0;
    }
    
    ac::AddInvert(frame);
}

void ac::SqOnOffAlphaClip(cv::Mat &frame) {
    
    static constexpr int MAX = 8;
    int PIXEL_SIZE=4+(rand()%28);
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
    }
    collection.shiftFrames(frame);
    static int off = 0;
    static double alpha = 0.1;
    for(int z = 0; z < frame.rows; z += PIXEL_SIZE) {
        for(int i = 0; i < frame.cols; i += PIXEL_SIZE) {
            if((rand()%2)==0) {
                for(int y = z; y < z+PIXEL_SIZE && y < frame.rows; ++y) {
                    for(int x = i; x < i+PIXEL_SIZE && x < frame.cols; ++x) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
                        cv::Mat &m = collection.frames[off];
                        cv::Vec3b &pix_off = m.at<cv::Vec3b>(y, x);
                        for(int q = 0; q < 3; ++q) {
                            pixel[q] = wrap_cast((alpha*pixel[q])+((1-alpha)*pix_off[q]));
                        }
                    }
                }
            }
            
        }
        static int adir = 1;
        if(adir == 1) {
            alpha += 0.01;
            if(alpha >= 4.0)
                adir = 0;
        } else {
            alpha -= 0.01;
            if(alpha <= 0.1)
                adir = 1;
        }
        ++off;
        if(off > MAX-1)
            off = 0;
    }
    
    ac::AddInvert(frame);
}

void ac::SqOnOffAlphaClipFast(cv::Mat &frame) {
    static constexpr int MAX = 8;
    int PIXEL_SIZE=4+(rand()%36);
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
    }
    collection.shiftFrames(frame);
    static int off = 0;
    static double alpha = 0.1;
    for(int z = 0; z < frame.rows; z += PIXEL_SIZE) {
        for(int i = 0; i < frame.cols; i += PIXEL_SIZE) {
            if((rand()%2)==0) {
                for(int y = z; y < z+PIXEL_SIZE && y < frame.rows; ++y) {
                    for(int x = i; x < i+PIXEL_SIZE && x < frame.cols; ++x) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
                        cv::Mat &m = collection.frames[off];
                        cv::Vec3b &pix_off = m.at<cv::Vec3b>(y, x);
                        for(int q = 0; q < 3; ++q) {
                            pixel[q] = wrap_cast((alpha*pixel[q])+((1-alpha)*pix_off[q]));
                        }
                    }
                }
            }
        }
        static int adir = 1;
        if(adir == 1) {
            alpha += 0.07;
            if(alpha >= 8.0)
                adir = 0;
        } else {
            alpha -= 0.07;
            if(alpha <= 0.1)
                adir = 1;
        }
        ++off;
        if(off > MAX-1)
            off = 0;
    }
    
    ac::AddInvert(frame);
}

void ac::SqOnOffAlphaPixel(cv::Mat &frame) {
    static constexpr int MAX = 8;
    int PIXEL_SIZE=4+(rand()%36);
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
    }
    collection.shiftFrames(frame);
    static int off = 0;
    static double alpha = 0.1;
    for(int z = 0; z < frame.rows; z += PIXEL_SIZE) {
        for(int i = 0; i < frame.cols; i += PIXEL_SIZE) {
            if((rand()%2)==0) {
                
                static int adir = 1;
                if(adir == 1) {
                    alpha += 0.07;
                    if(alpha >= 8.0)
                        adir = 0;
                } else {
                    alpha -= 0.07;
                    if(alpha <= 0.1)
                        adir = 1;
                }
                
                for(int y = z; y < z+PIXEL_SIZE && y < frame.rows; ++y) {
                    for(int x = i; x < i+PIXEL_SIZE && x < frame.cols; ++x) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
                        cv::Mat &m = collection.frames[off];
                        cv::Vec3b &pix_off = m.at<cv::Vec3b>(y, x);
                        for(int q = 0; q < 3; ++q) {
                            pixel[q] = wrap_cast((alpha*pixel[q])+((1-alpha)*pix_off[q]));
                        }
                    }
                }
            }
        }
        ++off;
        if(off > MAX-1)
            off = 0;
    }
    
    ac::AddInvert(frame);
}

void ac::SqOnOffDiff(cv::Mat &frame) {
    static constexpr int MAX = 8;
    int PIXEL_SIZE=4+(rand()%28);
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
    }
    collection.shiftFrames(frame);
    static int off = 0;
    static double alpha = 0.1;
    for(int z = 0; z < frame.rows; z += PIXEL_SIZE) {
        for(int i = 0; i < frame.cols; i += PIXEL_SIZE) {
            if((rand()%2)==0) {
                for(int y = z; y < z+PIXEL_SIZE && y < frame.rows; ++y) {
                    for(int x = i; x < i+PIXEL_SIZE && x < frame.cols; ++x) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
                        cv::Mat &m = collection.frames[off];
                        cv::Vec3b &pix_off = m.at<cv::Vec3b>(y, x);
                        for(int q = 0; q < 3; ++q) {
                            if(abs(pixel[q]-pix_off[q]) > 100) {
                                pixel[q] = wrap_cast((alpha*pixel[q])+((1-alpha)*pix_off[q]));
                            }
                        }
                    }
                }
            }
            
        }
        static int adir = 1;
        if(adir == 1) {
            alpha += 0.01;
            if(alpha >= 4.0)
                adir = 0;
        } else {
            alpha -= 0.01;
            if(alpha <= 0.1)
                adir = 1;
        }
        ++off;
        if(off > MAX-1)
            off = 0;
    }
    
    ac::AddInvert(frame);
}

void ac::SqOnOffSize(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static int PIXEL_SIZE=8;
    static int dir = 1;
    
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
    }
    collection.shiftFrames(frame);
    
    static int off = 0;
    
    for(int z = 0; z < frame.rows; z += PIXEL_SIZE) {
        for(int i = 0; i < frame.cols; i += PIXEL_SIZE) {
            if((rand()%2)==0) {
                for(int y = z; y < z+PIXEL_SIZE && y < frame.rows; ++y) {
                    for(int x = i; x < i+PIXEL_SIZE && x < frame.cols; ++x) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
                        cv::Mat &m = collection.frames[off];
                        cv::Vec3b &pix_off = m.at<cv::Vec3b>(y, x);
                        pixel = pix_off;
                        
                    }
                }
            }
        }
        ++off;
        if(off > MAX-1)
            off = 0;
    }
    
    if(dir == 1) {
        PIXEL_SIZE ++;
        if(PIXEL_SIZE >= 32)
            dir = 0;
    } else {
        PIXEL_SIZE --;
        if(PIXEL_SIZE <= 8)
            dir = 1;
    }
    ac::AddInvert(frame);
}

void ac::Placement(cv::Mat &frame) {
    static ac::MatrixCollection<8> collection;
    collection.shiftFrames(frame);
    
    static int size_x = 24, size_y = 24;
    static auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        static int findex = 0;
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                
                if((i%size_x) == 0 && (z%size_y) == 0) {
                    findex++;
                    if(findex > collection.size()-1)
                        findex = 0;
                }
                
                cv::Mat &m = collection.frames[findex];
                cv::Vec3b &pix = m.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
    };
    ac::UseMultipleThreads(frame, ac::getThreadCount(), callback);
    
    static int dir = 1;
    if(dir == 1) {
        size_x += 2;
        size_y += 2;
        
        if(size_x > frame.cols/2 || size_y > frame.rows/2) {
            dir = 0;
        }
    } else {
        size_x -= 2;
        size_y -= 2;
        if(size_x <= 2 || size_y <= 2) {
            dir = 1;
        }
    }
}

void ac::FrameSep3(cv::Mat &frame) {
    static constexpr int MAX = 16;
    static ac::MatrixCollection<MAX> collection;
    
    if(collection.empty()) {
        collection.shiftFrames(frame);
        srand(static_cast<unsigned int>(time(0)));
    } else if(rand()%5==0)
        collection.shiftFrames(frame);
    static bool on = false;
    static int off = 0;
    static int line_wait = 2+rand()%300;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            if(on == false) break;
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Mat &f = collection.frames[off];
            cv::Vec3b pix = f.at<cv::Vec3b>(z, i);
            pixel = pix;
        }
        if(((z%line_wait)==0) && ++off > MAX-1) {
            off = 0;
            static int cnt = 0;
            static int wait = rand()%10;
            ++cnt;
            if(cnt > wait) {
                on = !on;
                cnt = 0;
                wait = rand()%10;
            }
            line_wait = 2+rand()%300;
        }
    }
}

void ac::FrameSep4(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static ac::MatrixCollection<MAX> collection;
    
    if(collection.empty()) {
        collection.shiftFrames(frame);
        srand(static_cast<unsigned int>(time(0)));
    } else if(rand()%5==0)
        collection.shiftFrames(frame);
    static bool on = false;
    static int off = 0;
    static int line_wait = 2+rand()%(frame.rows/2);
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            if(on == false) break;
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Mat &f = collection.frames[off];
            cv::Vec3b pix = f.at<cv::Vec3b>(z, i);
            pixel = pix;
        }
        if(((z%line_wait)==0) && ++off > MAX-1) {
            off = 0;
            static int cnt = 0;
            static int wait = rand()%40;
            ++cnt;
            if(cnt > wait) {
                on = !on;
                cnt = 0;
                wait = rand()%40;
            }
            line_wait = 2+rand()%(frame.rows/2);
        }
    }
}

void ac::FrameSep5(cv::Mat &frame) {
    static constexpr int MAX = 4;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        collection.shiftFrames(frame);
        srand(static_cast<unsigned int>(time(0)));
    } else if(rand()%10==0)
        collection.shiftFrames(frame);
    static bool on = false;
    static int off = 0;
    static int line_wait = 2+rand()%(frame.rows/2);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            if(on == false) break;
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Mat &f = collection.frames[off];
            cv::Vec3b pix = f.at<cv::Vec3b>(z, i);
            pixel = pix;
        }
        if(((z%line_wait)==0) && ++off > MAX-1) {
            off = 0;
            static int cnt = 0;
            static int wait = rand()%60;
            ++cnt;
            if(cnt > wait) {
                on = !on;
                cnt = 0;
                wait = rand()%60;
            }
            line_wait = 2+rand()%(frame.rows/2);
        }
    }
}

void ac::FrameSepDiff(cv::Mat &frame) {
    static constexpr int MAX = 16;
    static ac::MatrixCollection<MAX> collection;
    
    if(collection.empty()) {
        collection.shiftFrames(frame);
        srand(static_cast<unsigned int>(time(0)));
    } else if(rand()%5==0)
        collection.shiftFrames(frame);
    static bool on = false;
    static int off = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            if(on == false) break;
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Mat &f = collection.frames[off];
            cv::Vec3b pix = f.at<cv::Vec3b>(z, i);
            cv::Mat &old = collection.frames[7];
            cv::Vec3b &old_pix = old.at<cv::Vec3b>(z, i);
            for(int q = 0; q < 3; ++q) {
                if(abs(pixel[q]-old_pix[q]) > 25) {
                    pixel[q] = pix[q];
                }
            }
        }
        if(((z%240)==0) && ++off > MAX-1) {
            off = 0;
            static int cnt = 0;
            static int wait = rand()%10;
            ++cnt;
            if(cnt > wait) {
                on = !on;
                cnt = 0;
                wait = rand()%10;
            }
        }
    }
}

void ac::FrameSepResize(cv::Mat &frame) {
    static constexpr int MAX = 16;
    static ac::MatrixCollection<MAX> collection;
    
    if(collection.empty()) {
        collection.shiftFrames(frame);
        srand(static_cast<unsigned int>(time(0)));
    } else if(rand()%5==0)
        collection.shiftFrames(frame);
    static bool on = false;
    static int off = 0;
    
    cv::Mat &old = collection.frames[7];
    ac::Square_Block_Resize_Vertical(old);
    
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            if(on == false) break;
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Mat &f = collection.frames[off];
            cv::Vec3b pix = f.at<cv::Vec3b>(z, i);
            cv::Vec3b &old_pix = old.at<cv::Vec3b>(z, i);
            for(int q = 0; q < 3; ++q) {
                if(abs(pixel[q]-old_pix[q]) > 25) {
                    pixel[q] = pix[q];
                }
            }
        }
        if(((z%240)==0) && ++off > MAX-1) {
            off = 0;
            static int cnt = 0;
            static int wait = rand()%10;
            ++cnt;
            if(cnt > wait) {
                on = !on;
                cnt = 0;
                wait = rand()%10;
            }
        }
    }
}
void ac::FrameSepResize2(cv::Mat &frame) {
    static constexpr int MAX = 16;
    static ac::MatrixCollection<MAX> collection;
    
    if(collection.empty()) {
        collection.shiftFrames(frame);
        srand(static_cast<unsigned int>(time(0)));
    } else if(rand()%5==0)
        collection.shiftFrames(frame);
    static bool on = false;
    static int off = 0;
    
    cv::Mat &old = collection.frames[7];
    ac::Square_Block_Resize_Vertical_RGB(old);
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            if(on == false) break;
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Mat &f = collection.frames[off];
            cv::Vec3b pix = f.at<cv::Vec3b>(z, i);
            cv::Vec3b &old_pix = old.at<cv::Vec3b>(z, i);
            for(int q = 0; q < 3; ++q) {
                if(abs(pixel[q]-old_pix[q]) > 5) {
                    pixel[q] = pix[q];
                }
            }
        }
        if(((z%240)==0) && ++off > MAX-1) {
            off = 0;
            static int cnt = 0;
            static int wait = rand()%10;
            ++cnt;
            if(cnt > wait) {
                on = !on;
                cnt = 0;
                wait = rand()%10;
            }
        }
    }
}
void ac::FrameSepSquare(cv::Mat &frame) {
    static constexpr int MAX = 16;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        collection.shiftFrames(frame);
        srand(static_cast<unsigned int>(time(0)));
    } else if(rand()%5==0)
        collection.shiftFrames(frame);
    static bool on = false;
    static int off = 0;
    cv::Mat &old = collection.frames[7];
    ac::SquareShift(old);
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            if(on == false) break;
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Mat &f = collection.frames[off];
            cv::Vec3b pix = f.at<cv::Vec3b>(z, i);
            cv::Vec3b &old_pix = old.at<cv::Vec3b>(z, i);
            for(int q = 0; q < 3; ++q) {
                if(abs(pixel[q]-old_pix[q]) > 5) {
                    pixel[q] = pix[q];
                }
            }
        }
        if(((z%240)==0) && ++off > MAX-1) {
            off = 0;
            static int cnt = 0;
            static int wait = rand()%10;
            ++cnt;
            if(cnt > wait) {
                on = !on;
                cnt = 0;
                wait = rand()%10;
            }
        }
    }
}
void ac::FrameSepH(cv::Mat &frame) {
    static constexpr int MAX = 16;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        collection.shiftFrames(frame);
        srand(static_cast<unsigned int>(time(0)));
    } else if(rand()%5==0)
        collection.shiftFrames(frame);
    static bool on = false;
    static int off = 0;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            if(on == false) break;
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Mat &f = collection.frames[off];
            cv::Vec3b pix = f.at<cv::Vec3b>(z, i);
            pixel = pix;
        }
        if(((i%240)==0) && ++off > MAX-1) {
            off = 0;
            static int cnt = 0;
            static int wait = rand()%10;
            ++cnt;
            if(cnt > wait) {
                on = !on;
                cnt = 0;
                wait = rand()%10;
            }
        }
    }
}

void ac::FrameSkip(cv::Mat &frame) {
    static constexpr int MAX = 16;
    static ac::MatrixCollection<MAX> collection;
    
    if(collection.empty()) {
        collection.shiftFrames(frame);
        srand(static_cast<unsigned int>(time(0)));
    } else if(rand()%5==0)
        collection.shiftFrames(frame);
    static bool on = false;
    static int off = 0;
    if(on == true) {
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Mat &f = collection.frames[off];
                cv::Vec3b pix = f.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
    }
    
    off++;
    if(off > MAX-1)
        off = 0;
    static int cnt = 0;
    static int wait = rand()%20;
    ++cnt;
    if(cnt > wait) {
        on = !on;
        cnt = 0;
        wait = rand()%20;
    }
}

void ac::FrameSkipResize(cv::Mat &frame) {
    static constexpr int MAX = 16;
    static ac::MatrixCollection<MAX> collection;
    
    if(collection.empty()) {
        collection.shiftFrames(frame);
        srand(static_cast<unsigned int>(time(0)));
    } else if(rand()%5==0)
        collection.shiftFrames(frame);
    static bool on = false;
    static int off = 0;
    if(on == true) {
        cv::Mat &f = collection.frames[off];
        ac::Square_Block_Resize_Vertical(f);
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b pix = f.at<cv::Vec3b>(z, i);
                pixel = pix;
            }
        }
    }
    
    off++;
    if(off > MAX-1)
        off = 0;
    static int cnt = 0;
    static int wait = rand()%20;
    ++cnt;
    if(cnt > wait) {
        on = !on;
        cnt = 0;
        wait = rand()%20;
    }
}

void ac::FrameReverse(cv::Mat &frame) {
    static constexpr int MAX = 16;
    static ac::MatrixCollection<MAX> collection;
    
    if(collection.empty()) {
        collection.shiftFrames(frame);
        srand(static_cast<unsigned int>(time(0)));
    } else if(rand()%2 == 0)
        collection.shiftFrames(frame);
    
    static int off = 0;
    cv::Mat &f = collection.frames[off];
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Vec3b pix = f.at<cv::Vec3b>(z, i);
            for(int q = 0; q < 3; ++q)
                pixel[q] = ac::wrap_cast((0.5 * pixel[q]) + (0.5 * pix[q]));
        }
    }
    static int dir = 1;
    if(dir == 1) {
        if(++off > MAX-1) {
            off = MAX-1;
            dir = 0;
        }
    } else {
        if(--off <= 0) {
            off = 0;
            dir = 1;
        }
    }
}

void ac::FrameStretch_X(cv::Mat &frame) {
    static int frame_x_off = 0;
    static int frame_y_off = 0;
    
    static auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                int off_x = AC_GetFX(frame->cols, i, frame->cols+frame_x_off);
                 if(z >= 0 && z < frame->rows && off_x >= 0 && off_x < frame->cols) {
                    cv::Vec3b pix = frame->at<cv::Vec3b>(z, off_x);
                    pixel[0] = ac::wrap_cast((0.5 * pixel[0]) + (0.5 * pix[0]));
                    pixel[1] = ac::wrap_cast((0.5 * pixel[1]) + (0.5 * pix[1]));
                    pixel[2] = ac::wrap_cast((0.5 * pixel[2]) + (0.5 * pix[2]));
                }
            }
        }
    };
    ac::UseMultipleThreads(frame, ac::getThreadCount(), callback);
    
    static int dir = 1;
    if(dir == 1) {
        frame_x_off += 4;
        frame_y_off += 4;
        if((frame_x_off > (frame.cols/2)) || (frame_y_off > (frame.rows/2)))  {
            dir = 0;
        }
    } else {
        frame_x_off -= 4;
        frame_y_off -= 4;
        if(frame_x_off <= 0 || frame_y_off <= 0) {
            dir = 1;
            frame_x_off = 0;
            frame_y_off = 0;
        }
    }
}

void ac::FrameStretch_Y(cv::Mat &frame) {
    static int frame_x_off = 0;
    static int frame_y_off = 0;
    
    static auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                
                // int off_x = AC_GetFX(frame->cols-1, i, frame->cols+frame_x_off);
                int off_y = AC_GetFZ(frame->rows-1, z, frame->rows+frame_y_off);
                cv::Vec3b pix = frame->at<cv::Vec3b>(off_y, i);
                
                pixel[0] = ac::wrap_cast((0.5 * pixel[0]) + (0.5 * pix[0]));
                pixel[1] = ac::wrap_cast((0.5 * pixel[1]) + (0.5 * pix[1]));
                pixel[2] = ac::wrap_cast((0.5 * pixel[2]) + (0.5 * pix[2]));
                
            }
        }
    };
    ac::UseMultipleThreads(frame, ac::getThreadCount(), callback);
    
    static int dir = 1;
    if(dir == 1) {
        frame_x_off += 4;
        frame_y_off += 4;
        if((frame_x_off > (frame.cols/2)) || (frame_y_off > (frame.rows/2)))  {
            dir = 0;
        }
    } else {
        frame_x_off -= 4;
        frame_y_off -= 4;
        if(frame_x_off <= 0 || frame_y_off <= 0) {
            dir = 1;
            frame_x_off = 0;
            frame_y_off = 0;
        }
    }
}

void ac::FrameStretch_XY(cv::Mat &frame) {
    static int frame_x_off = 0;
    static int frame_y_off = 0;
    
    static auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                
                int off_x = AC_GetFX(frame->cols-1, i, frame->cols+frame_x_off);
                int off_y = AC_GetFZ(frame->rows-1, z, frame->rows+frame_y_off);
                
                cv::Vec3b pix = frame->at<cv::Vec3b>(off_y, off_x);
                
                pixel[0] = ac::wrap_cast((0.5 * pixel[0]) + (0.5 * pix[0]));
                pixel[1] = ac::wrap_cast((0.5 * pixel[1]) + (0.5 * pix[1]));
                pixel[2] = ac::wrap_cast((0.5 * pixel[2]) + (0.5 * pix[2]));
                
            }
        }
    };
    ac::UseMultipleThreads(frame, ac::getThreadCount(), callback);
    
    static int dir = 1;
    if(dir == 1) {
        frame_x_off += 4;
        frame_y_off += 4;
        if((frame_x_off > (frame.cols/2)) || (frame_y_off > (frame.rows/2)))  {
            dir = 0;
        }
    } else {
        frame_x_off -= 4;
        frame_y_off -= 4;
        if(frame_x_off <= 0 || frame_y_off <= 0) {
            dir = 1;
            frame_x_off = 0;
            frame_y_off = 0;
            
        }
    }
}

void ac::FrameStretch_X_IO(cv::Mat &frame) {
    static int frame_x_off = 0;
    static int frame_y_off = 0;
    
    static auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                
                int off_x = AC_GetFX(frame->cols-1, i, frame->cols+frame_x_off);
                
                cv::Vec3b pix = frame->at<cv::Vec3b>(z, off_x);
                
                pixel[0] = ac::wrap_cast((0.5 * pixel[0]) + (0.5 * pix[0]));
                pixel[1] = ac::wrap_cast((0.5 * pixel[1]) + (0.5 * pix[1]));
                pixel[2] = ac::wrap_cast((0.5 * pixel[2]) + (0.5 * pix[2]));
                
            }
        }
    };
    ac::UseMultipleThreads(frame, ac::getThreadCount(), callback);
    
    static int dir = 1;
    static int frame_max = 1;
    if(dir == 1) {
        frame_x_off += 4;
        frame_y_off += 4;
        if((frame_x_off > frame_max) || (frame_y_off > frame_max))  {
            dir = 0;
            static int dir_max = 1;
            if(dir_max == 1) {
                frame_max += 50;
                if(frame_max >= frame.cols/2)
                    dir_max = 0;
            } else {
                frame_max -= 50;
                if(frame_max <= 0) {
                    frame_max = 0;
                    dir_max = 1;
                }
            }
            
        }
    } else {
        frame_x_off -= 4;
        frame_y_off -= 4;
        if(frame_x_off <= 0 || frame_y_off <= 0) {
            dir = 1;
            frame_x_off = 0;
            frame_y_off = 0;
        }
    }
}

void ac::FrameStretch_Y_IO(cv::Mat &frame) {
    static int frame_x_off = 0;
    static int frame_y_off = 0;
    
    static auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                
                int off_y = AC_GetFZ(frame->rows-1, z, frame->rows+frame_y_off);
                
                cv::Vec3b pix = frame->at<cv::Vec3b>(off_y, i);
                
                pixel[0] = ac::wrap_cast((0.5 * pixel[0]) + (0.5 * pix[0]));
                pixel[1] = ac::wrap_cast((0.5 * pixel[1]) + (0.5 * pix[1]));
                pixel[2] = ac::wrap_cast((0.5 * pixel[2]) + (0.5 * pix[2]));
                
            }
        }
    };
    ac::UseMultipleThreads(frame, ac::getThreadCount(), callback);
    
    static int dir = 1;
    static int frame_max = 1;
    if(dir == 1) {
        frame_x_off += 4;
        frame_y_off += 4;
        if((frame_x_off > frame_max) || (frame_y_off > frame_max))  {
            dir = 0;
            static int dir_max = 1;
            if(dir_max == 1) {
                frame_max += 50;
                if(frame_max >= frame.cols/2)
                    dir_max = 0;
            } else {
                frame_max -= 50;
                if(frame_max <= 0) {
                    frame_max = 0;
                    dir_max = 1;
                }
            }
            
        }
    } else {
        frame_x_off -= 4;
        frame_y_off -= 4;
        if(frame_x_off <= 0 || frame_y_off <= 0) {
            dir = 1;
            frame_x_off = 0;
            frame_y_off = 0;
        }
    }
}

void ac::FrameStretch_XY_IO(cv::Mat &frame) {
    static int frame_x_off = 0;
    static int frame_y_off = 0;
    
    static auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                
                int off_x = AC_GetFX(frame->cols-1, i, frame->cols+frame_x_off);
                int off_y = AC_GetFZ(frame->rows-1, z, frame->rows+frame_y_off);
                
                cv::Vec3b pix = frame->at<cv::Vec3b>(off_y, off_x);
                
                pixel[0] = ac::wrap_cast((0.5 * pixel[0]) + (0.5 * pix[0]));
                pixel[1] = ac::wrap_cast((0.5 * pixel[1]) + (0.5 * pix[1]));
                pixel[2] = ac::wrap_cast((0.5 * pixel[2]) + (0.5 * pix[2]));
                
            }
        }
    };
    ac::UseMultipleThreads(frame, ac::getThreadCount(), callback);
    
    static int dir = 1;
    static int frame_max = 1;
    if(dir == 1) {
        frame_x_off += 4;
        frame_y_off += 4;
        if((frame_x_off > frame_max) || (frame_y_off > frame_max))  {
            dir = 0;
            static int dir_max = 1;
            if(dir_max == 1) {
                frame_max += 50;
                if(frame_max >= frame.cols/2)
                    dir_max = 0;
            } else {
                frame_max -= 50;
                if(frame_max <= 0) {
                    frame_max = 0;
                    dir_max = 1;
                }
            }
            
        }
    } else {
        frame_x_off -= 4;
        frame_y_off -= 4;
        if(frame_x_off <= 0 || frame_y_off <= 0) {
            dir = 1;
            frame_x_off = 0;
            frame_y_off = 0;
        }
    }
}

void ac::FrameSetGamma(cv::Mat &frame) {
    ac::setGamma(frame, frame, 4);
}

struct Pixel {
    int x,y;
    cv::Vec3b color;
    int timeout;
    Pixel() : x{0}, y{0}, timeout{0} {}
    Pixel(int xx, int yy, cv::Vec3b c) : x{xx}, y{yy}, color{c}, timeout{0} {}
    
};

void ac::FramePixelsLeftOver(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static ac::MatrixCollection<MAX> collection;
    static std::list<Pixel> pixels;
    
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    } else {
        collection.shiftFrames(frame);
    }
    
    for(int z = 0; z < frame.rows-20; z += 20) {
        for(int i = 0; i < frame.cols-20; i += 20) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Mat &m = collection.frames[2];
            cv::Vec3b pix = m.at<cv::Vec3b>(z, i);
            bool set = false;
            for(int q = 0; q < 3; ++q) {
                if(abs(pixel[q]-pix[q]) > 40) {
                    set = true;
                    break;
                }
            }
            if(set == true) {
                pixels.push_back(Pixel(i, z, cv::Vec3b(rand()%255, rand()%255, rand()%255)));
            }
        }
    }
    
    for(std::list<Pixel>::iterator i = pixels.begin(); i != pixels.end(); ) {
        if(++i->timeout > 3) {
            i = pixels.erase(i);
        } else {
            
            i->color = cv::Vec3b(rand()%255, rand()%255, rand()%255);
            
            for(int y = i->y; y < i->y + 25; ++y)
                for(int x = i->x; x < i->x + 25; ++x) {
                    if(x >= 0 && x < frame.cols && y >= 0 && y < frame.rows) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
                        cv::Vec3b &pix = i->color;
                        pixel[0] = ac::wrap_cast((0.5 * pixel[0]) + (0.5 * pix[0]));
                        pixel[1] = ac::wrap_cast((0.5 * pixel[1]) + (0.5 * pix[1]));
                        pixel[2] = ac::wrap_cast((0.5 * pixel[2]) + (0.5 * pix[2]));
                        //pixel = pix;
                    }
                }
            
            i ++;
        }
    }
}
void ac::FramePixelsLeftOverBlend(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static ac::MatrixCollection<MAX> collection;
    static std::list<Pixel> pixels;

    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    } else {
        collection.shiftFrames(frame);
    }
    
    for(int z = 0; z < frame.rows-20; z += 20) {
        for(int i = 0; i < frame.cols-20; i += 20) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            cv::Mat &m = collection.frames[2];
            cv::Vec3b pix = m.at<cv::Vec3b>(z, i);
            bool set = false;
            for(int q = 0; q < 3; ++q) {
                if(abs(pixel[q]-pix[q]) > 40) {
                    set = true;
                    break;
                }
            }
            if(set == true) {
                pixels.push_back(Pixel(i, z, pixel));
            }
        }
    }
    
    for(std::list<Pixel>::iterator i = pixels.begin(); i != pixels.end(); ) {
        if(++i->timeout > 3) {
            i = pixels.erase(i);
        } else {
            
            for(int y = i->y; y < i->y + 25; ++y)
                for(int x = i->x; x < i->x + 25; ++x) {
                    if(x >= 0 && x < frame.cols && y >= 0 && y < frame.rows) {
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
                        cv::Vec3b &pix = i->color;
                        
                        pixel[0] = ac::wrap_cast((0.5 * pixel[0]) + (0.5 * pix[0]));
                        pixel[1] = ac::wrap_cast((0.5 * pixel[1]) + (0.5 * pix[1]));
                        pixel[2] = ac::wrap_cast((0.5 * pixel[2]) + (0.5 * pix[2]));
                        //pixel = pix;
                    }
                }
            
            i ++;
        }
    }
}

void ac::FramePixels(cv::Mat &frame) {
    static constexpr int MAX = 16;
    static ac::MatrixCollection<MAX> collection;
    
    if(collection.empty()) {
        collection.shiftFrames(frame);
        srand(static_cast<unsigned int>(time(0)));
    } else collection.shiftFrames(frame);

    static int off = 0;
    static constexpr int SIZE_W=64;
    
    for(int z = 0; z < frame.rows; z += SIZE_W+1) {
            for(int i = 0; i < frame.cols; i += SIZE_W+1) {
                for(int x = 0; i+x < frame.cols && x < SIZE_W; ++x) {
                    for(int y = 0; z+y < frame.rows && y < SIZE_W; ++y) {
                        
                        cv::Mat &f = collection.frames[off];

                        if(x%8 == 0 || y%8 == 0) {
                            ++off;
                            if(off > MAX-1)
                                off = 0;
                        }
                        
                        cv::Vec3b pix = f.at<cv::Vec3b>(y+z, x+i);
                        cv::Vec3b &pixel = frame.at<cv::Vec3b>(y+z, x+i);
                        pixel = pix;
                    }
            }
        }
    }
}

void ac::FrameStopStart(cv::Mat &frame) {
    static constexpr int MAX = 16;
    static ac::MatrixCollection<MAX> collection;
    static int max = 20;
    
    if(collection.empty()) {
        collection.shiftFrames(frame);
        srand(static_cast<unsigned int>(time(0)));
    } else if(rand()%max == 0) {
        collection.shiftFrames(frame);
        max = 1+(rand()%30);
    }
    
    static int off = 0;
    cv::Mat &f = collection.frames[off];
    
    static auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = f.at<cv::Vec3b>(z, i);
                pixel[0] = ac::wrap_cast ((0.5 * pixel[0]) + (0.5 * pix[0]));
                
                pixel[1] = ac::wrap_cast ((0.5 * pixel[1]) + (0.5 * pix[1]));
                
                pixel[2] = ac::wrap_cast ((0.5 * pixel[2]) + (0.5 * pix[2]));
            }
        }
    };
    ac::UseMultipleThreads(frame, ac::getThreadCount(), callback);
    
    static int dir = 1;
    if(dir == 1) {
        if(++off > MAX-1) {
            off = MAX-1;
            dir = 0;
        }
    } else {
        if(--off <= 0) {
            off = 0;
            dir = 1;
        }
    }
}

void ac::FrameStopStart2(cv::Mat &frame) {
    static constexpr int MAX = 16;
    static ac::MatrixCollection<MAX> collection;
    static int max = 20;
    static int count = 0;
    
    if(collection.empty()) {
        collection.shiftFrames(frame);
        srand(static_cast<unsigned int>(time(0)));
    } else if(++count > max == 0) {
        collection.shiftFrames(frame);
        max = 1+(rand()%30);
        count = 0;
    }
    
    cv::Mat &f = collection.frames[15];
    static bool on = true;
    
    static auto callback = [&](cv::Mat *frame, int offset, int cols, int size) {
        for(int z = offset; z <  offset+size; ++z) {
            for(int i = 0; i < cols; ++i) {
                cv::Vec3b &pixel = frame->at<cv::Vec3b>(z, i);
                cv::Vec3b pix = f.at<cv::Vec3b>(z, i);
                if(on == true) {
                    pixel[0] = ac::wrap_cast ((0.5 * pixel[0]) + (0.5 * pix[0]));
                    
                    pixel[1] = ac::wrap_cast ((0.5 * pixel[1]) + (0.5 * pix[1]));
                    
                    pixel[2] = ac::wrap_cast ((0.5 * pixel[2]) + (0.5 * pix[2]));
                } else {
                    pixel = pix;
                }
            }
        }
    };
    ac::UseMultipleThreads(frame, ac::getThreadCount(), callback);

    
    static int wait = rand()%30;
    static int counter = 0;
    if(++counter > wait) {
        counter = 0;
        wait = rand()%30;
        on = !on;
    }
}

void ac::FrameXBlend(cv::Mat &frame) {
    cv::Vec3b pix;
    static int offset_x = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pix = frame.at<cv::Vec3b>(z, offset_x);
            pixel[0] = ac::wrap_cast((0.5 * pixel[0]) + (0.5 * pix[0]));
            pixel[1] = ac::wrap_cast((0.5 * pixel[1]) + (0.5 * pix[1]));
            pixel[2] = ac::wrap_cast((0.5 * pixel[2]) + (0.5 * pix[2]));
        }
        offset_x++;
        if(offset_x > frame.cols-1) offset_x = 0;
    }
}

void ac::FrameYBlend(cv::Mat &frame) {
    cv::Vec3b pix;
    static int offset_y = 0;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pix = frame.at<cv::Vec3b>(offset_y, i);
            pixel[0] = ac::wrap_cast((0.5 * pixel[0]) + (0.5 * pix[0]));
            pixel[1] = ac::wrap_cast((0.5 * pixel[1]) + (0.5 * pix[1]));
            pixel[2] = ac::wrap_cast((0.5 * pixel[2]) + (0.5 * pix[2]));
        }
        offset_y++;
        if(offset_y > frame.rows-1) offset_y = 0;
    }
}

void ac::FrameXBlendXor(cv::Mat &frame) {
    cv::Vec3b pix;
    static int offset_x = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pix = frame.at<cv::Vec3b>(z, offset_x);
            pixel[0] = pixel[0]^pix[0];
            pixel[1] = pixel[1]^pix[1];
            pixel[2] = pixel[2]^pix[2];
        }
        offset_x++;
        if(offset_x > frame.cols-1) offset_x = 0;
    }
}
void ac::FrameYBlendXor(cv::Mat &frame) {
    cv::Vec3b pix;
    static int offset_y = 0;
    for(int i = 0; i < frame.cols; ++i) {
        for(int z = 0; z < frame.rows; ++z) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            pix = frame.at<cv::Vec3b>(offset_y, i);
            pixel[0] = pixel[0]^pix[0];
            pixel[1] = pixel[1]^pix[1];
            pixel[2] = pixel[2]^pix[2];
        }
        offset_y++;
        if(offset_y > frame.rows-1) offset_y = 0;
    }
}

void ac::StrobeCycle(cv::Mat &frame) {
    static ac::MatrixCollection<8> collection;
    if(collection.empty())
        collection.shiftFrames(frame);
    collection.shiftFrames(frame);
    static bool strobe = false;
    static int offset = 0;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = ac::pixelAt(frame, z, i);
            cv::Vec3b pix[4];
            pix[0] = ac::pixelAt(collection.frames[1], z, i);
            pix[1] = ac::pixelAt(collection.frames[4], z, i);
            pix[2] = ac::pixelAt(collection.frames[7], z, i);
            for(int j = 0; j < 3; ++j) {
                if(strobe == false)
                    pixel[j] = pix[offset][j];
                else
                    pixel[j] = pix[j][j];
            }
        }
    }
    strobe = (strobe == true) ? false : true;
    static int cnt = 0;
    if(++cnt > 10) {
        ++offset;
        if(offset > 2)
            offset = 0;
        cnt = 0;
    }
}

void ac::StrobeCycleSt(cv::Mat &frame) {
    static ac::MatrixCollection<8> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    }
    else if(rand()%4 == 0)
        collection.shiftFrames(frame);
    static bool strobe = false;
    static int offset = 0;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = ac::pixelAt(frame, z, i);
            cv::Vec3b pix[4];
            pix[0] = ac::pixelAt(collection.frames[1], z, i);
            pix[1] = ac::pixelAt(collection.frames[4], z, i);
            pix[2] = ac::pixelAt(collection.frames[7], z, i);
            for(int j = 0; j < 3; ++j) {
                if(strobe == false)
                    pixel[j] = pix[offset][j];
                else
                    pixel[j] = pix[j][j];
            }
        }
    }
    strobe = (strobe == true) ? false : true;
    static int cnt = 0;
    if(++cnt > 10) {
        ++offset;
        if(offset > 2)
            offset = 0;
        cnt = 0;
    }
}

void ac::StrobeCycleSt2(cv::Mat &frame) {
    static ac::MatrixCollection<8> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    }
    else if(rand()%2 == 0)
        collection.shiftFrames(frame);
    static bool strobe = false;
    static int offset = 0;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = ac::pixelAt(frame, z, i);
            cv::Vec3b pix[4];
            pix[0] = ac::pixelAt(collection.frames[1], z, i);
            pix[1] = ac::pixelAt(collection.frames[4], z, i);
            pix[2] = ac::pixelAt(collection.frames[7], z, i);
            for(int j = 0; j < 3; ++j) {
                if(strobe == false)
                    pixel[j] = pix[offset][j];
                else
                    pixel[j] = pix[j][j];
            }
        }
    }
    strobe = (strobe == true) ? false : true;
    static int cnt = 0;
    if(++cnt > 4) {
        ++offset;
        if(offset > 2)
            offset = 0;
        cnt = 0;
    }
}

void ac::StrobeCycleSt3(cv::Mat &frame) {
    static ac::MatrixCollection<8> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    }
    else if(rand()%3 == 0)
        collection.shiftFrames(frame);
    
    static bool strobe = false;
    static int offset = 0;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = ac::pixelAt(frame, z, i);
            cv::Vec3b pix[4];
            pix[0] = ac::pixelAt(collection.frames[1], z, i);
            pix[1] = ac::pixelAt(collection.frames[4], z, i);
            pix[2] = ac::pixelAt(collection.frames[7], z, i);
            for(int j = 0; j < 3; ++j) {
                if(strobe == false)
                    pixel[j] = pix[offset][j];
                else
                    pixel[j] = pix[j][j];
            }
        }
    }
    strobe = (strobe == true) ? false : true;
    static int cnt = 0;
    static int dir = 1;
    if(++cnt > 6) {
        if(dir == 1) {
        ++offset;
            if(offset > 2) {
                dir = 0;
                offset = 2;
            }
            cnt = 0;
        } else {
            --offset;
            if(offset < 0) {
                offset = 0;
                dir = 1;
            }
        }
    }
}

void ac::OutOfOrder(cv::Mat &frame) {
    static constexpr int MAX = 16;
    static ac::MatrixCollection<16> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    }
    else if(rand()%3 == 0)
        collection.shiftFrames(frame);
    
    static bool strobe = false;
    static int offset = 0;
    /*
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = ac::pixelAt(frame, z, i);
         
        }
    }*/
    
    if(strobe) {
        frame = collection.frames[offset].clone();
    }
    
    strobe = (strobe == true) ? false : true;
    static int cnt = 0;
    static int dir = 1;
    if(++cnt > 6) {
        if(dir == 1) {
        ++offset;
            if(offset > MAX-1) {
                dir = 0;
                offset = 2;
            }
            cnt = 0;
        } else {
            --offset;
            if(offset < 0) {
                offset = rand()%(MAX-1);
                dir = 1;
            }
        }
    }
}

void drawSquare(cv::Mat &frame, int x, int y, int w, int h, cv::Mat &off) {
    for(int i = x; i < w; ++i) {
        for(int z = y; z < h; ++z) {
            if(i < frame.cols && z < frame.rows && i >= 0 && z >= 0) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                pixel = off.at<cv::Vec3b>(z, i);
            }
        }
    }
}

void ac::RSquare(cv::Mat  &frame) {
    static constexpr int MAX = 8;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    }
    collection.shiftFrames(frame);
    for(int i = 0; i < (50+rand()%50); ++i) {
        int x = rand()%frame.cols;
        int y = rand()%frame.rows;
        int w = 10+rand()%(frame.cols-10);
        int h = 10+rand()%(frame.rows-10);
        drawSquare(frame, x, y, w, h, collection.frames[rand()%(MAX-1)]);
    }
    ac::FrameSep2(frame);
}

void ac::TrackingDown(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    }
    else
        collection.shiftFrames(frame);
    static int dir = 1;
    static int offset = 0;
    static int div = 2;
    static int size_y = frame.rows/16;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            int cy = AC_GetFZ(frame.rows-1, z, size_y);
            if(cy >= 0 && cy < frame.rows && i >= 0 && i < frame.cols) {
                cv::Vec3b &pix = collection.frames[offset].at<cv::Vec3b>(cy, i);
                pixel = pix;
            }
        }
        size_y ++;
        if(size_y > frame.rows*2)
            size_y = frame.rows/16;
    }
    
    if(++offset > (MAX-1)) {
        offset = 0;
    }
}

void ac::RSquareEven(cv::Mat &frame) {
    static constexpr int MAX = 16;
      static ac::MatrixCollection<MAX> collection;
      if(collection.empty()) {
          srand(static_cast<unsigned int>(time(0)));
          collection.shiftFrames(frame);
      }
      collection.shiftFrames(frame);
      for(int i = 0; i < (50+rand()%50); ++i) {
          int x = rand()%frame.cols;
          int y = rand()%frame.rows;
          int w = 10+rand()%(frame.cols-10);
          //int h = 10+rand()%(frame.rows-10);
          drawSquare(frame, x, y, w, w, collection.frames[rand()%(MAX-1)]);
      }
      ac::FrameSep2(frame);
}

struct AC_Rect {
    int x,y,w,h;
    int offset;
    bool dir;
    int ex_w, ex_h;
    AC_Rect() : x{0}, y{0}, w{0}, h{0}, offset{0} {}
    void setRect(int xx, int yy, int ww, int hh) {
        x = xx;
        y = yy;
        w = ww;
        h = hh;
        ex_w = ww+50;
        ex_h = hh+50;
        dir = (rand()%2 == 0) ? true : false;
    }
};

class RectContainer {
    int size_w, size_h;
public:
    std::list<AC_Rect> cont;
    RectContainer() = default;
    void init(int MAX, int max, int w, int h) {
        size_w = w;
        size_h = h;
        for(int i = 0; i < max; ++i) {
            AC_Rect rc;
            rc.setRect(rand()%w, rand()%h, rand()%w, rand()%h);
            rc.offset = rand()%MAX;
            cont.push_back(rc);
        }
    }
    void update(int MAX, int w, int h, int step) {
        for(auto it = cont.begin(); it != cont.end(); ++it) {
            it->x += rand()%(1+step);
            it->y += rand()%(1+step);
            it->w += rand()%(1+step);
            it->h += rand()%(1+step);
            
            if(it->dir == true) {
                if(it->offset ++ > (MAX-1)) {
                    it->offset = MAX-1;
                    it->dir = false;
                }
            } else {
                if(it->offset-- <= 0) {
                    it->offset = 0;
                    it->dir = true;
                }
            }
            if(it->w > 50 || it->h > h) {
                it->x = rand()%size_w;
                it->y = rand()%size_h;
                it->w = rand()%size_w;
                it->h = rand()%size_h;
                continue;
            }
        }
    }
    
};


void ac::StretchRand(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static ac::MatrixCollection<MAX> collection;
    static RectContainer cont;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        cont.init(MAX, 20, frame.cols, frame.rows);
        collection.shiftFrames(frame);
    }
    else
        collection.shiftFrames(frame);
    
    cont.update(MAX, frame.cols, frame.rows, rand()%10);
    for(auto i = cont.cont.begin(); i != cont.cont.end(); ++i) {
        for(int x = i->x; x < i->x + i->w && x < frame.cols; ++x) {
            for(int y = i->y; y < i->y + i->h && y < frame.rows; ++y) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(y, x);
                cv::Vec3b &pix = collection.frames[i->offset].at<cv::Vec3b>(y, x);
                for(int q = 0; q < 3; ++q) {
                    pixel[q] = ac::wrap_cast((0.5 * pixel[q]) + (0.5 * pix[q]));
                }
            }
        }
    }
}

void ac::TrackingDownBlend(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    }
    else
        collection.shiftFrames(frame);
    static int dir = 1;
    static int offset = 0;
    static int div = 2;
    static int size_y = frame.rows/16;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            int cy = AC_GetFZ(frame.rows-1, z, size_y);
            if(cy >= 0 && cy < frame.rows && i >= 0 && i < frame.cols) {
                cv::Vec3b &pix = collection.frames[offset].at<cv::Vec3b>(cy, i);
                for(int q = 0; q < 3; ++q) {
                    pixel[q] = ac::wrap_cast((0.2 * pixel[q]) + (0.8 * pix[q]));
                }
            }
        }
        size_y ++;
        if(size_y > frame.rows*2)
            size_y = frame.rows/16;
    }
    
    if(++offset > (MAX-1)) {
        offset = 0;
    }
}

void ac::FrameWobble1(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    }
    else
        collection.shiftFrames(frame);
    static int dir = 1;
    static int offset = 0;
    static int div = 2;
    static int size_x = frame.cols/16;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            int cx = AC_GetFX(frame.cols-1, i, size_x);
            if(cx >= 0 && cx < frame.cols && z >= 0 && z < frame.rows) {
                cv::Vec3b &pix = collection.frames[offset].at<cv::Vec3b>(z, cx);
                pixel = pix;
            }
        }
        size_x ++;
        if(size_x > frame.cols*2)
            size_x = frame.cols/16;
    }
    
    if(++offset > (MAX-1)) {
        offset = 0;
    }
}

void ac::TrackingRev(cv::Mat &frame) {
    static constexpr int MAX = 8;
     static ac::MatrixCollection<MAX> collection;
     if(collection.empty()) {
         srand(static_cast<unsigned int>(time(0)));
         collection.shiftFrames(frame);
     }
     else
         collection.shiftFrames(frame);
     static int dir = 1;
     static int offset = 0;
     static int div = 2;
     static int size_y = frame.rows/16;
     
     for(int z = 0; z < frame.rows; ++z) {
         for(int i = 0; i < frame.cols; ++i) {
             cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
             int cy = AC_GetFZ(frame.rows-1, z, size_y);
             if(cy >= 0 && cy < frame.rows && i >= 0 && i < frame.cols) {
                 cv::Vec3b &pix = collection.frames[offset].at<cv::Vec3b>(cy, i);
                 pixel = pix;
             }
         }
         if(dir == 1) {
             size_y ++;
             if(size_y > frame.rows*2)
                 dir = 0;
         } else {
             size_y --;
             if(size_y <= 2)
                 dir = 1;
         }
     }
     
     if(++offset > (MAX-1)) {
         offset = 0;
     }
}

void ac::TrackingMirror(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    }
    else
        collection.shiftFrames(frame);
    static int dir = 1;
    static int offset = 0;
    static int div = 2;
    static int size_y = frame.rows/16;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            int cy = AC_GetFZ(frame.rows-1, z, size_y);
            
            cy = frame.rows-cy-1;
            
            if(cy >= 0 && cy < frame.rows && i >= 0 && i < frame.cols) {
                cv::Vec3b &pix = collection.frames[offset].at<cv::Vec3b>(cy, i);
                pixel = pix;
            }
        }
        size_y ++;
        if(size_y > frame.rows*2)
            size_y = frame.rows/16;
    }
    
    if(++offset > (MAX-1)) {
        offset = 0;
    }
}

void ac::TrackingMirrorRev(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    }
    else
        collection.shiftFrames(frame);
    static int dir = 1;
    static int offset = 0;
    static int div = 2;
    static int size_y = frame.rows/16;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            int cy = AC_GetFZ(frame.rows-1, z, size_y);
            
            cy = frame.rows-cy-1;
            
            if(cy >= 0 && cy < frame.rows && i >= 0 && i < frame.cols) {
                cv::Vec3b &pix = collection.frames[offset].at<cv::Vec3b>(cy, i);
                pixel = pix;
            }
        }
        size_y ++;
        if(size_y > frame.rows*2)
            size_y = frame.rows/16;
    }
    
    if(++offset > (MAX-1)) {
        offset = 0;
    }
}

void ac::TrackingMirrorRevBlend(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    }
    else
        collection.shiftFrames(frame);
    
    static bool flash = true;
    if(flash) {
        static int dir = 1;
        static int offset = 0;
        static int div = 2;
        static int size_y = frame.rows/16;
        
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                int cy = AC_GetFZ(frame.rows-1, z, size_y);
                if(cy >= 0 && cy < frame.rows && i >= 0 && i < frame.cols) {
                    cv::Vec3b &pix = collection.frames[offset].at<cv::Vec3b>(cy, i);
                    pixel = pix;
                }
            }
            size_y ++;
            if(size_y > frame.rows*2)
                size_y = frame.rows/16;
        }
        
        if(++offset > (MAX-1)) {
            offset = 0;
        }
    } else {
        static int dir = 1;
        static int offset = 0;
        static int div = 2;
        static int size_y = frame.rows/16;
        
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                int cy = AC_GetFZ(frame.rows-1, z, size_y);
                
                cy = frame.rows-cy-1;
                
                if(cy >= 0 && cy < frame.rows && i >= 0 && i < frame.cols) {
                    cv::Vec3b &pix = collection.frames[offset].at<cv::Vec3b>(cy, i);
                    pixel = pix;
                }
            }
            size_y ++;
            if(size_y > frame.rows*2)
                size_y = frame.rows/16;
        }
        
        if(++offset > (MAX-1)) {
            offset = 0;
        }
    }

    flash = (flash == true) ? false : true;
}

void ac::FrameWobble2(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    }
    else if(rand()%2 == 0)
        collection.shiftFrames(frame);
    
    static int flash = 0;
    switch(flash) {
        case 0: {
            static int dir = 1;
            static int offset = 0;
            static int div = 2;
            static int size_x = frame.cols/16;
            
            for(int z = 0; z < frame.rows; ++z) {
                for(int i = 0; i < frame.cols; ++i) {
                    cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                    int cx = AC_GetFX(frame.cols-1, i, size_x);
                    if(cx >= 0 && cx < frame.cols && z >= 0 && z < frame.rows) {
                        cv::Vec3b &pix = collection.frames[offset].at<cv::Vec3b>(z, cx);
                        pixel = pix;
                    }
                }
                size_x ++;
                if(size_x > frame.cols*2)
                    size_x = frame.cols/16;
            }
            
            if(++offset > (MAX-1)) {
                offset = 0;
            }
        }
            break;
        case 1: {
            int width = 1+(rand()%frame.cols);
            int skip = 0;
            static int offset = 0;
            
            for(int z = 0; z < frame.rows; ++z) {
                for(int i = 0; i < frame.cols; ++i) {
                    cv::Vec3b &pixel = ac::pixelAt(frame, z, i);
                    
                    int offx = AC_GetFX(frame.cols-1, i, width);
                    
                    cv::Vec3b &cpix = collection.frames[offset].at<cv::Vec3b>(z, offx);
                
                    pixel = cpix;
                }
                width += 1+(rand()%5);
                if(width%10) {
                    if(++offset > (MAX-1))
                        offset = 0;
                }
                
            }
        }
            break;
        case 2: {
            static int dir = 1;
            static int offset = 0;
            
            frame = collection.frames[offset].clone();
            
            if(dir == 1) {
                static int wait = 0;
                static int timeout = rand()%10;
                if(++offset > (MAX-1)) {
                    offset = 0;
                }
                if(++wait > timeout) {
                    wait = 0;
                    timeout = rand()%10;
                    dir = 0;
                }
            } else {
                static int wait = 0;
                static int timeout = rand()%10;
                if(--offset <= 0) {
                    offset = MAX-1;
                }
                if(++wait > timeout) {
                    wait = 0;
                    timeout = rand()%10;
                    dir = 1;
                }
            }
        }
            break;
    }
    
    if(++flash > 2)
        flash = 0;
    
}

void ac::BlockPixels(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    }
    else
        collection.shiftFrames(frame);
    
    
    int row = 0;
    static int offset = 0;
    static double index = 0.1;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            
            cv::Vec3b &pix = collection.frames[offset].at<cv::Vec3b>(z, i);
            
            
            pixel[row] = ac::wrap_cast((index * pixel[row]) + (0.5-index * pix[row]));
            
            if(++row > 2)
                row = 0;
            
            index += 0.01;
        }
        
        if(index >= 0.5)
            index = 0.1;
        
        if(++offset > (MAX-1))
            offset = rand()%(MAX-1);
        
    }
}

void ac::RGB_Lr(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    }
    else
        collection.shiftFrames(frame);
    
    static int start_x[2] = {0, frame.cols};
    static int start_y[2] = {0, frame.rows};
    static int offset = 0;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            int xpos = start_x[0]+i;
            int xpos2 = i-start_x[1];
            
            if(xpos >= 0 && xpos < frame.cols) {
                cv::Vec3b &pix = collection.frames[offset].at<cv::Vec3b>(z, xpos);
                pixel[0] = ac::wrap_cast((0.5 * pixel[0]) + (0.5 * pix[0]));
            }
            
            if(xpos2 >= 0 && xpos2 < frame.cols) {
                cv::Vec3b &pix = collection.frames[offset].at<cv::Vec3b>(z, xpos2);
                pixel[0] = ac::wrap_cast((0.5 * pixel[0]) + (0.5 * pix[0]));
            }
            
            int ypos = start_y[0]+z;
            int ypos2 = z-start_y[1];

            if(ypos >= 0 && ypos < frame.rows) {
                cv::Vec3b &pix = collection.frames[offset].at<cv::Vec3b>(ypos, i);
                pixel[1] = ac::wrap_cast((0.5 * pixel[1]) + (0.5 * pix[1]));

            }
            if(ypos2 >= 0 && ypos2 < frame.rows) {
                cv::Vec3b &pix = collection.frames[offset].at<cv::Vec3b>(ypos2, i);
                pixel[2] = ac::wrap_cast((0.5 * pixel[2]) + (0.5 * pix[2]));

            }
        }
    }
    
    start_x[0] += 50;
    if(start_x[0] > frame.cols) {
        start_x[0] = 0;
    }
    start_x[1] -= 50;
    if(start_x[1] <= 0) {
        start_x[1] = frame.cols;
    }
    start_y[0] += 50;
    if(start_y[0] > frame.rows) {
        start_y[0] = 0;
    }
    start_y[1] -= 50;
    if(start_y[1] <= 0) {
        start_y[1] = frame.rows;
    }
    
    if(++offset > (MAX-1))
        offset = rand()%(MAX-1);
}

void ac::TrackRGB1(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    }
    else
        collection.shiftFrames(frame);
    static int dir = 1;
    static int offset = 0;
    static int div = 2;
    static int size_y = frame.rows/16;
    static int color_offset = 0;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            int cy = AC_GetFZ(frame.rows-1, z, size_y);
            if(cy >= 0 && cy < frame.rows && i >= 0 && i < frame.cols) {
                cv::Vec3b &pix = collection.frames[offset].at<cv::Vec3b>(cy, i);
                pixel[color_offset] = pix[color_offset];
            }
        }
        size_y ++;
        if(size_y > frame.rows*2) {
            size_y = frame.rows/16;
            if(++color_offset > 2)
                color_offset = 0;
        }
    }
    
    if(++offset > (MAX-1)) {
        offset = 0;
    }
}

void ac::TrackRGB2(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    }
    else
        collection.shiftFrames(frame);
    static int dir = 1;
    static int offset = 0;
    static int div = 2;
    static int size_y = frame.rows/16;
    static int color_offset = 0;
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            int cy = AC_GetFZ(frame.rows-1, z, size_y);
            
            cy = frame.rows-cy-1;
            
            if(cy >= 0 && cy < frame.rows && i >= 0 && i < frame.cols) {
                cv::Vec3b &pix = collection.frames[offset].at<cv::Vec3b>(cy, i);
                pixel[color_offset] = pix[color_offset];
            }
        }
        size_y ++;
        if(size_y > frame.rows*2) {
            size_y = frame.rows/16;
            if(++color_offset > 2)
                color_offset = 0;
        }
    }
    
    if(++offset > (MAX-1)) {
        offset = 0;
    }
}

void ac::FrameChop(cv::Mat &frame) {
    static int offset_x = 0;
    static int max_x = frame.cols+(frame.cols/2);
    
    static ac::MatrixCollection<2> collection;
    collection.shiftFrames(frame);
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int x = 0; x < frame.cols; x += 100) {
            for(int i = 0; i < 100; ++i) {
                int pos_x = x+i;
                int px = AC_GetFZ(frame.cols, pos_x, offset_x);
                if(px >= 0 && px < frame.cols && x+i < frame.cols) {
                    cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, x+i);
                    cv::Vec3b &pix = collection.frames[1].at<cv::Vec3b>(z, px);
                    pixel = pix;
                }
            }
            
        }
        
        offset_x --;
        if(offset_x <= max_x) {
            offset_x = rand()%(frame.cols+(frame.cols/2));
            static int dir = 0;
            if(dir == 1) {
                max_x += 10;
                if(max_x > (frame.cols+(frame.cols/2)))
                    dir = 0;
            } else {
                max_x -= 10;
                if(max_x <= 100)
                    dir = 1;
            }
        }
    }
}

struct Impl {
    int *rows = nullptr;
    int *row_speed = nullptr;
    
    Impl() = default;
    ~Impl() {
        delete [] rows;
        delete [] row_speed;
    }
};



void ac::YLineDown(cv::Mat  &frame) {
    static ac::MatrixCollection<2> collection;
    collection.shiftFrames(frame);
    static int row_w = 0;
    static Impl row;
    
    if(row_w == 0 || row_w != frame.cols) {
        
        if(row.rows != nullptr) {
            delete [] row.rows;
            delete [] row.row_speed;
        }
        
        row.rows = new int[frame.cols+1];
        row.row_speed =new int[frame.cols+1];
        for(int i = 0; i < frame.cols; ++i) {
            row.rows[i] = rand()%50;
            row.row_speed[i] = 2+rand()%25;
        }
        row_w = frame.cols;
    }
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            int row_y = row.rows[i]+z;
            if(row_y >= 0 && row_y < frame.rows) {
                cv::Vec3b &pix = collection.frames[1].at<cv::Vec3b>(row_y, i);
                pixel = pix;
            }
        }
    }
    
    for(int i = 0; i < frame.cols; ++i) {
        row.rows[i] += row.row_speed[i];
        if(row.rows[i] > frame.rows) {
            row.rows[i] = rand()%50;
        }
    }
}

void ac::YLineDownBlend(cv::Mat &frame) {
    static ac::MatrixCollection<2> collection;
     collection.shiftFrames(frame);
     static int row_w = 0;
     static Impl row;
     
     if(row_w == 0 || row_w != frame.cols) {
         
         if(row.rows != nullptr) {
             delete [] row.rows;
             delete [] row.row_speed;
         }
         
         row.rows = new int[frame.cols+1];
         row.row_speed =new int[frame.cols+1];
         for(int i = 0; i < frame.cols; ++i) {
             row.rows[i] = rand()%50;
             row.row_speed[i] = 2+rand()%25;
         }
         row_w = frame.cols;
     }
     
     for(int z = 0; z < frame.rows; ++z) {
         for(int i = 0; i < frame.cols; ++i) {
             cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
             int row_y = row.rows[i]+z;
             if(row_y >= 0 && row_y < frame.rows) {
                 cv::Vec3b &pix = collection.frames[1].at<cv::Vec3b>(row_y, i);
                 for(int q = 0; q < 3; ++q)
                     pixel[q] = ac::wrap_cast((0.5 * pixel[q]) + (0.5 * pix[q]));
             }
         }
     }
     
     for(int i = 0; i < frame.cols; ++i) {
         row.rows[i] += row.row_speed[i];
         if(row.rows[i] > frame.rows) {
             row.rows[i] = rand()%50;
         }
     }
}

void ac::FrameBreakX1(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    }
    else
    collection.shiftFrames(frame);
    
    auto drawRect = [&](cv::Mat &col_frame, int x, int y, int w, int h) {
      
        for(int z = y; z < y+h && z < frame.rows; ++z) {
            for(int i = x; i < x+w && i < frame.cols; ++i) {
                if(z >= 0 && z < frame.rows && i >= 0 && i < frame.cols) {
                frame.at<cv::Vec3b>(z, i) = col_frame.at<cv::Vec3b>(z, i);
                }
            }
        }
    };
    
    int size = frame.rows/4;
    static int offset = 0;
    
    for(int j = 0; j < 4; ++j) {
  
        if(!(rand()%4 == 0)) continue;
        
        drawRect(collection.frames[offset], 0, size*j, frame.cols, size);
    
        if(++offset > (MAX-1))
            offset = 0;
    }
    
}

void ac::RGBLagFrame(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        collection.shiftFrames(frame);
    }
    
    static int counter = 0;
    if(++counter > 2) {
        counter = 0;
        collection.shiftFrames(frame);
    }
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = ac::pixelAt(frame, z, i);
            cv::Vec3b *off[3];
            off[0] = &collection.frames[1].at<cv::Vec3b>(z, i);
            off[1] = &collection.frames[3].at<cv::Vec3b>(z, i);
            off[2] = &collection.frames[7].at<cv::Vec3b>(z, i);
            cv::Vec3b pix = pixel;
            for(int q = 0; q < 3; ++q) {
                pixel[q] = (*off[q])[q];
                pixel[q] = ac::wrap_cast((0.5 * pixel[q]) + (0.5 * pix[q]));
            }
        }
    }
}

void ac::LagFrame(cv::Mat &frame) {
    static constexpr int MAX = 2;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    }
    
    static int counter = 0;
    if(++counter > (24*3)) {
        counter = 0;
        collection.shiftFrames(frame);
    }
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = ac::pixelAt(frame, z, i);
            cv::Vec3b &pix = collection.frames[1].at<cv::Vec3b>(z, i);
            
            for(int q = 0; q < 3; ++q)
                pixel[q] = ac::wrap_cast((0.5 * pixel[q]) + (0.5 * pix[q]));
        }
    }
}

void ac::FrameStoreStutter(cv::Mat &frame) {
    static constexpr int MAX = 4;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    }
    static int counter = 0;
    if(++counter > 4) {
        counter = 0;
        collection.shiftFrames(frame);
    }
    static int offset = 0;
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = ac::pixelAt(frame, z, i);
            cv::Vec3b &pix = ac::pixelAt(collection.frames[offset], z, i);
            pixel = pix;
        }
    }
    if(++offset > (MAX-1)) {
        offset = 0;
    }
}

void ac::SetFramePixelate1(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    }
    else
        collection.shiftFrames(frame);
    
    static int wait = rand()%15;
    static int count = 0;
    static int size_x = 8;
    static int num = 2;
    static int offset = 0;
    
    if(++count > wait) {
        count = 0;
        wait = rand()%15;
        for(int z = 0; z < frame.rows; z += size_x) {
            for(int i = 0; i < frame.cols; i += size_x) {
                if(rand()%num == 0) {
                    for(int x = 0; x < size_x && x+i < frame.cols; ++x) {
                        for(int y = 0; y < size_x && y+z < frame.rows; ++y) {
                            cv::Vec3b &pix = collection.frames[offset].at<cv::Vec3b>(z+y, i+x);
                            
                            cv::Vec3b &pixel = ac::pixelAt(frame, z+y, i+x);
                            pixel = pix;
                        }
                    }
                }
            }
        }
    }
    static int dir = 1;
    if(dir == 1) {
        if(++size_x >= 32) {
            dir = 0;
        }
    } else {
        if(--size_x <= 8) {
            dir = 1;
        }
    }
    
    if(++offset > (MAX-1)) {
        offset = 0;
        ++num;
        if(num > 8)
            num = 2;
    }
}

void ac::SetFramePixelate2(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    }
    else
        collection.shiftFrames(frame);
    
    static int wait = rand()%15;
    static int count = 0;
    static int size_x = 8;
    static int num = 2;
    if(++count > wait) {
        count = 0;
        wait = rand()%15;
        for(int z = 0; z < frame.rows; z += size_x) {
            for(int i = 0; i < frame.cols; i += size_x) {
                if(rand()%num == 0) {
                    for(int x = 0; x < size_x && x+i < frame.cols; ++x) {
                        for(int y = 0; y < size_x && y+z < frame.rows; ++y) {
                            cv::Vec3b &pix = collection.frames[6].at<cv::Vec3b>(z+y, i+x);
                            
                            cv::Vec3b &pixel = ac::pixelAt(frame, z+y, i+x);
                            for(int q = 0; q < 3; ++q) {
                                pixel[q] = ac::wrap_cast((0.5 * pixel[q]) + (0.5 * pix[q]));
                            }
                        }
                    }
                }
            }
        }
        
    }
    static int dir = 1;
    if(dir == 1) {
        if(++size_x >= 32) {
            dir = 0;
        }
    } else {
        if(--size_x <= 8) {
            dir = 1;
        }
    }
}

void ac::SetFramePixelate3(cv::Mat &frame) {
    static constexpr int MAX = 2;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        srand(static_cast<unsigned int>(time(0)));
        collection.shiftFrames(frame);
    }
    
    static int counter = 0;
    if(++counter > 24) {
        counter = 0;
        collection.shiftFrames(frame);
    }
    
    static int num = 8;
    static double alpha = 0.2;
    
    for(int z = 0; z < frame.rows; z += num) {
        for(int i = 0; i < frame.cols; i += num) {
        
            if(rand()%3 == 0) continue;
            
            for(int x = 0; x < num && x+i < frame.cols; ++x) {
                for(int y = 0; y < num && y+z < frame.rows; ++y) {
                
                    
                    cv::Vec3b &pixel = ac::pixelAt(frame, z+y, i+x);
                    cv::Vec3b &pix = collection.frames[1].at<cv::Vec3b>(z+y, i+x);
                    
                    for(int q = 0; q < 3; ++q)
                        pixel[q] = ac::wrap_cast((alpha * pixel[q]) + ((1-alpha)*pix[q]));
                    
                    static int dir = 1;
                    
                    if(dir == 1) {
                        alpha -= 0.05;
                        if(alpha < 0.2)
                            dir = 0;
                    }
                    else {
                        alpha += 0.05;
                        if(alpha >= 1.0)
                            dir = 1;
                    }
                }
            }
        }
    }
}

void ac::SquareDiff1(cv::Mat &frame) {
    static constexpr int MAX = 8;
    static ac::MatrixCollection<MAX> collection;
    if(collection.empty()) {
        collection.shiftFrames(frame);
        srand(static_cast<unsigned int>(time(0)));
    }
    else
        collection.shiftFrames(frame);
    
    auto drawSquare = [&](cv::Mat sframe, int x, int y, int w, int h) {
        for(int i = x; i < x+w && i < frame.cols; ++i) {
            for(int z = y; z < y+h && z < frame.rows; ++z) {
                
                if(i >= 0 && i < frame.cols && z >= 0 && z < frame.cols) {
                    cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                    cv::Vec3b &pix = sframe.at<cv::Vec3b>(z, i);
                    pixel = pix;
                }
            }
        }
    };
    
    int square_h = 16;
    int square_w = 64;
    static int offset = 0;
    
    for(int z = 0; z < frame.rows; z += square_h) {
        square_w = 1+rand()%32;
        for(int i = 0; i < frame.cols; i += square_w) {
            drawSquare(collection.frames[offset], i, z, square_w, square_h);
        }
        if(++offset > (MAX-1)) {
            offset = rand()%(MAX-1);
        }
        square_h = 1+rand()%128;
    }
}

struct LineAcrossImpl {
    struct Item {
        int col, speed;
        bool on;
        Item() : col{rand()%50}, speed{rand()%25}, on{true}  {}
    };
    Item *item = nullptr;
    LineAcrossImpl() = default;
    ~LineAcrossImpl() {
        if(item != nullptr)
            delete [] item;
        
    }
};


void ac::LineAcrossX(cv::Mat  &frame) {
    static ac::MatrixCollection<2> collection;
    collection.shiftFrames(frame);
    static int row_w = 0;
    static LineAcrossImpl row;
    
    if(row_w == 0 || row_w != frame.rows) {
        if(row.item != nullptr) {
            delete [] row.item;
        }
        row_w = frame.rows;
        row.item = new LineAcrossImpl::Item[row_w+1];
    }
    
    for(int z = 0; z < frame.rows; ++z) {
        for(int i = 0; i < frame.cols; ++i) {
            cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
            
            int row_col = row.item[z].col+i;
            int row_x = AC_GetFX(frame.cols-1, i, row_col);
            
            if(row_x >= 0 && row_x < frame.cols) {
                cv::Vec3b &pix = collection.frames[1].at<cv::Vec3b>(z, row_x);
                pixel = pix;
            }
        }
    }
    
    for(int i = 0; i < row_w; ++i) {
        if(row.item[i].on) {
            row.item[i].col += 25;
            if(row.item[i].col > frame.cols)
                row.item[i].on = false;
        } else {
            row.item[i].col -= 25;
            if(row.item[i].col <= 1) {
                row.item[i].col = 1;
                row.item[i].on = true;
            }
        }
    }
}

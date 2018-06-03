#include"ac.h"

// Image blend
// cv::Mat as reference
void ac::imageBlend(cv::Mat &frame) {
    static double pos = 1.0f;// static pos set to 1
    if(blend_set == true) {// if image is set
        int i,z;
        for(i = 0; i < frame.cols; ++i) { // top to bottom
            for(z = 0; z < frame.rows; ++z) {// left to right
                // get resized x,y
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                cv::Vec3b &current = frame.at<cv::Vec3b>(z, i);// get current pixel
                cv::Vec3b im = blend_image.at<cv::Vec3b>(cY, cX);// get pixel to blend from resized x,y
                // set pixel values
                current[0] = static_cast<unsigned char>(current[0]+(im[0]*pos));
                current[1] = static_cast<unsigned char>(current[1]+(im[1]*pos));
                current[2] = static_cast<unsigned char>(current[2]+(im[2]*pos));
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i);// invert pixel
            }
        }
    }
    static double pos_max = 7.0f;// max pos value
    static int direction = 1;
    procPos(direction, pos, pos_max);
}
// takes cv::Mat reference
void ac::imageBlendTwo(cv::Mat &frame) {
    static double pos = 1.0f; // static pos equal 1.0
    if(blend_set == true) {// if image is set to blend with
        int i,z;// loop variables
        for(i = 0; i < frame.cols; ++i) { // left to right
            for(z = 0; z < frame.rows; ++z) {// top to bottom
                // resize x,y
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                // grab pixels
                cv::Vec3b &current = frame.at<cv::Vec3b>(z, i);
                cv::Vec3b im = blend_image.at<cv::Vec3b>(cY, cX);
                // set pixel values
                current[0] = static_cast<unsigned char>(im[0]+(current[0]*pos));
                current[1] = static_cast<unsigned char>(im[1]+(current[1]*pos));
                current[2] = static_cast<unsigned char>(im[2]+(current[2]*pos));
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i); // invert pixel
            }
        }
    }
    static double pos_max = 7.0f;// max position set to 7.0
    static int direction = 1;
    procPos(direction, pos, pos_max);
}
// blend with Image
// takes cv::Mat reference
void ac::imageBlendThree(cv::Mat &frame) {
    if(blend_set == true) { // if blend_set is true (image selected)
        static double pos = 1.0f;// static pos equals 1.0
        for(int i = 0; i < frame.cols; ++i) { // from top to bottom
            for(int z = 0; z < frame.rows; ++z) {// from left to right
                // calculate x,y pixel position
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                // get pixel to manipulate reference
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                // get image pixel from calculated x,y positions
                cv::Vec3b im = blend_image.at<cv::Vec3b>(cY, cX);
                // calculate pixel data
                pixel[0] += (pixel[0]^im[0]);
                pixel[1] += (pixel[1]^im[1]);
                pixel[2] += static_cast<unsigned char>((pixel[2]^im[2])*pos);
                swapColors(frame, z, i);//swap colors
                if(isNegative) invert(frame, z, i);// if isNegative invert pixel
                
            }
        }
        // static int directione quals 1
        static int direction = 1;
        // static pos_max equals 7.0
        static double pos_max = 7.0f;
        if(direction == 1) {// if direction equals 1
            pos += 0.005;// pos plus equal 0.005
            if(pos > pos_max) {// pos greater than pos_max
                pos = pos_max;// pos set to pos_max
                direction = 0;// direction set to zero
                pos_max += 0.5f;// pos_max plus equal 0.5
            }
        } else if(direction == 0) {// direction is set to 0
            pos -= 0.005;// pos minus equal 0.005
            if(pos <= 1) {/// pos less than equal 1
                if(pos_max > 15) pos_max = 1.0f;//reset pos_max if greater than 15
                direction = 1;// direction set to 1
            }
        }
    }
}

// imageblend4
void ac::imageBlendFour(cv::Mat &frame) {
    if(blend_set == true) {
        static unsigned int state = 0;
        static double pos = 1.0;
        int w = frame.cols;// frame width
        int h = frame.rows;// frame height
        int cw = blend_image.cols;
        int ch = blend_image.rows;
        for(int z = 3; z < h-3; ++z) {// top to bottom
            for(int i = 3; i < w-3; ++i) {// left to right
                // current pixel by reference
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                // calculate resized image based x,y positions
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                // grab pixel refernces from blend_image
                cv::Vec3b &pr = blend_image.at<cv::Vec3b>((ch-cY), (cw-cX));
                cv::Vec3b &pg = blend_image.at<cv::Vec3b>((ch-cY), cX);
                cv::Vec3b &pb = blend_image.at<cv::Vec3b>(cY, (cw-cX));
                // perform operation based on current state variable
                switch(state) {
                    case 0:
                        pixel[0] += static_cast<unsigned char>((pr[0]+pg[1]+pb[2])*pos);
                        pixel[1] += static_cast<unsigned char>((pr[2]+pg[1]+pb[0])*pos);
                        break;
                    case 1:
                        pixel[1] += static_cast<unsigned char>((pr[2]+pg[1]+pb[0])*pos);
                        pixel[2] += static_cast<unsigned char>((pr[0]+pg[1]+pb[2])*pos);
                        break;
                    case 2:
                        pixel[2] += static_cast<unsigned char>((pr[0]+pg[1]+pb[2])*pos);
                        pixel[0] += static_cast<unsigned char>((pr[2]+pg[1]+pb[0])*pos);
                        break;
                }
                
            }
            
        }
        ++state;// increase state
        if(state > 2) state = 0; // greater than 2 reset state
        static int direction = 1;
        // static pos_max equals 7.0
        static double pos_max = 3.0f;
        if(direction == 1) {// if direction equals 1
            pos += 0.005;// pos plus equal 0.005
            if(pos > pos_max) {// pos greater than pos_max
                pos = pos_max;// pos set to pos_max
                direction = 0;// direction set to zero
                pos_max += 0.5f;// pos_max plus equal 0.5
            }
        } else if(direction == 0) {// direction is set to 0
            pos -= 0.005;// pos minus equal 0.005
            if(pos <= 1) {/// pos less than equal 1
                if(pos_max > 3) pos_max = 1.0f;//reset pos_max if greater than 15
                direction = 1;// direction set to 1
            }
        }
    }
}

void ac::ImageFile(cv::Mat &frame) {
    if(blend_set == true) {
        const int w = frame.cols;
        const int h = frame.rows;
        for(int z = 0;  z < h; ++z) {
            for(int i = 0; i < w; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                cv::Vec3b add_i = blend_image.at<cv::Vec3b>(cY, cX);
                pixel[0] += add_i[0];
                pixel[1] += add_i[1];
                pixel[1] += add_i[2];
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i); // invert pixel
            }
        }
    }
    
}
void ac::ImageXor(cv::Mat &frame) {
    if(blend_set == true) {
        const int w = frame.cols;
        const int h = frame.rows;
        static double alpha = 1.0, alpha_max = 4.0;
        for(int z = 0;  z < h; ++z) {
            for(int i = 0; i < w; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                cv::Vec3b add_i = blend_image.at<cv::Vec3b>(cY, cX);
                for(int j = 0; j < 3; ++j)
                    pixel[j] = cv::saturate_cast<unsigned char>(pixel[j]^add_i[j])*alpha;
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i); // invert pixel
            }
        }
        static int direction = 1;
        procPos(direction, alpha, alpha_max);
    }
}

void ac::ImageAlphaBlend(cv::Mat &frame) {
    if(blend_set == true) {
        const int w = frame.cols;
        const int h = frame.rows;
        static double alpha = 1.0, alpha_max = 2.0;
        for(int z = 0;  z < h; ++z) {
            for(int i = 0; i < w; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                cv::Vec3b add_i = blend_image.at<cv::Vec3b>(cY, cX);
                for(int j = 0; j < 3; ++j)
                    pixel[j] = static_cast<unsigned char>((pixel[j]*alpha) + (add_i[j] * alpha));
                
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i); // invert pixel
            }
        }
        static int direction = 1;
        procPos(direction, alpha, alpha_max);
    }
}

void ac::ImageInter(cv::Mat &frame) {
    if(blend_set == true) {
        static int start = 0, restart = 0;
        const int w = frame.cols;
        const int h = frame.rows;
        for(int z = 0;  z < h; ++z) {
            for(int i = 0; i < w; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                cv::Vec3b add_i = blend_image.at<cv::Vec3b>(cY, cX);
                if(start == 0) {
                    pixel = add_i;
                }
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i); // invert pixel
            }
            start = (start == 0) ? 1 : 0;
        }
        if(restart == 0) {
            start = 1;
            restart = 1;
        } else {
            start = 0;
            restart = 0;
        }
    }
}

// blend with image
void ac::ImageX(cv::Mat &frame) {
    if(blend_set == true) {
        static double alpha = 1.0, alpha_max = 8.0;
        static cv::Mat frame_blend = blend_image.clone();
        for(int i = 0; i < frame.cols-1; ++i) {
            for(int z = 0; z < frame.rows-1; ++z) {
                int cX = AC_GetFX(frame_blend.cols, i, frame.cols);
                int cY = AC_GetFZ(frame_blend.rows, z, frame.rows);
                
                if(cX >= frame_blend.cols || cY >= frame_blend.rows)
                    continue;
                
                cv::Vec3b &pixel = blend_image.at<cv::Vec3b>(cY, cX);
                cv::Vec3b pix = blend_image.at<cv::Vec3b>(cY+1, cX+1);
                pixel = pix;
                cv::Vec3b &pix_value = frame.at<cv::Vec3b>(z, i);
                for(int j = 0; j < 3; ++j)
                    pix_value[j] = static_cast<unsigned char>(pixel[j]+(pix_value[j]*alpha));
            }
        }
        static int direction = 1;
        procPos(direction, alpha, alpha_max);
    }
}

void ac::SmoothRandomImageBlend(cv::Mat &frame) {
    if(blend_set == true) {
        static MatrixCollection<8> collection;
        unsigned int index = 0;
        DrawFunction rfunc = getRandomFilter(index);
        cv::Mat temp_frame;
        cv::resize(blend_image, temp_frame, frame.size());
        rfunc(temp_frame);
        collection.shiftFrames(temp_frame);
        Smooth(frame, &collection);
    }
}

void ac::SmoothImageAlphaBlend(cv::Mat &frame) {
    if(blend_set == true) {
        static double alpha = 1.0, alpha_max = 2.0;
        static MatrixCollection<8> collection;
        cv::Mat temp_frame;
        cv::Mat temp_image;
        cv::Mat blend_image_scaled;
        cv::resize(blend_image, blend_image_scaled, frame.size());
        temp_frame = frame.clone();
        AlphaBlend(temp_frame,blend_image_scaled,frame,alpha);
        collection.shiftFrames(frame);
        Smooth(temp_frame, &collection);
        frame = temp_frame;
        static int direction = 1;
        procPos(direction, alpha, alpha_max, 8, 0.05);
    }
}

void ac::BlendImageOnOff(cv::Mat &frame) {
    if(blend_set == true) {
        static double alpha = 1.0, alpha_max = 4.0;
        static int index = 0;
        for(int z = 3; z < frame.rows-3; ++z) {
            for(int i = 3; i < frame.cols-3; ++i) {
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                cv::Vec3b pix[4];
                pix[0] = blend_image.at<cv::Vec3b>(cY, cX);
                pix[1] = blend_image.at<cv::Vec3b>(cY+1, cX);
                pix[2] = blend_image.at<cv::Vec3b>(cY, cX+1);
                pix[3] = blend_image.at<cv::Vec3b>(cY+1, cX+1);
                cv::Scalar value;
                for(unsigned int j = 0; j < 4; ++j) {
                    for(unsigned int q = 0; q < 3; ++q) {
                        value[q] += pix[j][q];
                    }
                }
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                for(unsigned int j = 0; j < 3; ++j) {
                    value[j] /= 4;
                    unsigned char val = static_cast<unsigned char>(value[j]);
                    switch(index) {
                        case 0:
                            pixel[j] += static_cast<unsigned char>(val*alpha);
                            break;
                        case 1:
                            pixel[j] -= static_cast<unsigned char>(val*alpha);
                            break;
                    }
                }
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
            }
        }
        ++index;
        if(index > 1) index = 0;
        
        static int dir = 1;
        procPos(dir, alpha, alpha_max);
    }
}

void ac::XorSelfAlphaImage(cv::Mat &frame) {
    if(blend_set == true) {
        static double alpha = 1.0, alpha_max = 2.0;
        static double alpha_r = 14.0;
        for(int z = 0; z < frame.rows; ++z) {
            for(int i = 0; i < frame.cols; ++i) {
                cv::Vec3b &pixel = frame.at<cv::Vec3b>(z, i);
                int cX = AC_GetFX(blend_image.cols, i, frame.cols);
                int cY = AC_GetFZ(blend_image.rows, z, frame.rows);
                cv::Vec3b pix = blend_image.at<cv::Vec3b>(cY, cX);
                for(unsigned int j = 0; j < 3; ++j) {
                    //pixel[j] ^= (1-((pixel[j] + pix[j])) * (2+static_cast<unsigned char>(alpha)));
                    pixel[j] = static_cast<unsigned char>(pixel[j] * alpha) + (pix[j] * alpha_r);
                }
                swapColors(frame, z, i);// swap colors
                if(isNegative) invert(frame, z, i);// if isNegative invert pixel */
            }
        }
        static int dir = 1, dir_r = 0;
        procPos(dir, alpha, alpha_max);
        procPos(dir_r, alpha_r, alpha_max);
    }
}

void ac::AlphaBlendImageXor(cv::Mat &frame) {
    if(blend_set == true) {
        static MatrixCollection<8> collection;
        SmoothImageAlphaBlend(frame);
        Bitwise_XOR(frame);
        collection.shiftFrames(frame);
        Smooth(frame, &collection);
    }
}

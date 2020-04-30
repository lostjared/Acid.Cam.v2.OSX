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


#include "ac.h"

// variables for changePixel
int current_filterx = 0;
int bytesPerSample = 0;
int bytesPerRow = 0;
int width = 0;
int height = 0;
int red = 0;
int green = 0;
int blue = 0;
int offset = 0;
int randomNumber = 0;
int reverse = 0;
bool negate = false;

// changePixel for Alpha Flame Filters
// this function is called once for each pixel in the source image
void changePixel(cv::Mat &full_buffer, int i, int z, cv::Vec3b &buffer, double pos, double *count) {
    //each case is a different operation on the RGB pixel values stored in buffer
    switch(current_filterx) {
        case 0:
        {
            double value = pos;
            buffer[0] = static_cast<unsigned char>(value*buffer[0]);
            buffer[1] = static_cast<unsigned char>(value*buffer[1]);
            buffer[2] = static_cast<unsigned char>(value*buffer[2]);
        }
            break;
        case 1:
        {
            double value = pos;
            buffer[0] = static_cast<unsigned char>(value*buffer[0]);
            buffer[1] = static_cast<unsigned char>((-value)*buffer[1]);
            buffer[2] = static_cast<unsigned char>(value*buffer[2]);
        }
            break;
        case 2:
        {
            buffer[0] += static_cast<unsigned char>(buffer[0]*-pos);
            buffer[1] += static_cast<unsigned char>(buffer[1]*pos);
            buffer[2] += static_cast<unsigned char>(buffer[2]*-pos);
        }
            break;
        case 3:
        {
            int current_pos = static_cast<int>(pos*0.2f);
            buffer[0] = static_cast<unsigned char>((i*current_pos)+buffer[0]);
            buffer[2] = static_cast<unsigned char>((z*current_pos)+buffer[2]);
            buffer[1] = static_cast<unsigned char>((current_pos*buffer[1]));
        }
            break;
        case 4:
        {
            int current_pos = static_cast<int>(pos*0.2f);
            buffer[0] = static_cast<unsigned char>((z*current_pos)+buffer[0]);
            buffer[1] = static_cast<unsigned char>((i*current_pos)+buffer[1]);
            buffer[2] = static_cast<unsigned char>(((i+z)*current_pos)+buffer[2]);
        }
            break;
        case 5:
        {
            int current_pos = static_cast<int>(pos*0.2f);
            buffer[0] = static_cast<unsigned char>(-(z*current_pos)+buffer[0]);
            buffer[1] = static_cast<unsigned char>(-(i*current_pos)+buffer[1]);
            buffer[2] = static_cast<unsigned char>(-((i+z)*current_pos)+buffer[2]);
        }
            break;
            
        case 6:
        {
            int zq = z+1, iq = i+1;
            if(zq > height-1 || iq > width-1) return;
            cv::Vec3b &temp = full_buffer.at<cv::Vec3b>(zq, iq);
            buffer[0] += static_cast<unsigned char>((i*pos)+temp[0]);
            buffer[1] += static_cast<unsigned char>((z*pos)+temp[1]);
            buffer[2] += static_cast<unsigned char>((i/(z+1))+temp[2]);
        }
            break;
        case 7:
        {
            unsigned char colv[4], cola[4];
            colv[0] = buffer[0];
            colv[1] = buffer[1];
            colv[2] = buffer[2];
            cola[0] = buffer[2];
            cola[1] = buffer[1];
            cola[2] = buffer[0];
            int alpha = (int)pos;
            int red_values[] = { colv[0]+cola[2], colv[1]+cola[1], colv[2]+cola[0], 0 };
            int green_values[] = { colv[2]+cola[0], colv[1]+cola[1], colv[0]+cola[2], 0 };
            int blue_values[] = { colv[1]+cola[1], colv[0]+cola[2], colv[2]+cola[0], 0 };
            unsigned char R = 0,G = 0,B = 0;
            for(int iq = 0; iq <= 2; ++iq) {
                R += red_values[iq];
                R /= 3;
                G += green_values[iq];
                G /= 3;
                B += blue_values[iq];
                B /= 3;
            }
            buffer[0] += static_cast<unsigned char>(alpha*R);
            buffer[1] += static_cast<unsigned char>(alpha*G);
            buffer[2] += static_cast<unsigned char>(alpha*B);
        }
            break;
        case 8:
        {
            unsigned char colv[4], cola[4];
            colv[0] = buffer[0];
            colv[1] = buffer[1];
            colv[2] = buffer[2];
            int iq = (width-i-1);
            int zq = (height-z-1);
            cv::Vec3b &t = full_buffer.at<cv::Vec3b>(zq, iq);
            cola[0] = t[0];
            cola[1] = t[1];
            cola[2] = t[2];
            int alpha = (int)pos;
            int red_values[] = { colv[0]+cola[2], colv[1]+cola[1], colv[2]+cola[0], 0 };
            int green_values[] = { colv[2]+cola[0], colv[1]+cola[1], colv[0]+cola[2], 0 };
            int blue_values[] = { colv[1]+cola[1], colv[0]+cola[2], colv[2]+cola[0], 0 };
            unsigned char R = 0,G = 0,B = 0;
            for(int iq = 0; iq <= 2; ++iq) {
                R += red_values[iq];
                R /= 3;
                G += green_values[iq];
                G /= 3;
                B += blue_values[iq];
                B /= 3;
            }
            buffer[0] += static_cast<unsigned char>(alpha*R);
            buffer[1] += static_cast<unsigned char>(alpha*G);
            buffer[2] += static_cast<unsigned char>(alpha*B);
        }
            break;
        case 9:
        {
            double alpha = pos;
            unsigned char colorz[3][3];
            colorz[0][0] = buffer[0];
            colorz[0][1] = buffer[1];
            colorz[0][2] = buffer[2];
            int total_r = colorz[0][0] +colorz[0][1]+colorz[0][2];
            total_r /= 3;
            total_r *= static_cast<int>(alpha);
            int iq = i+1;
            if(iq > width) return;
            int zq = z;
            cv::Vec3b &temp = full_buffer.at<cv::Vec3b>(zq, iq);
            colorz[1][0] = temp[0];
            colorz[1][1] = temp[1];
            colorz[1][2] = temp[2];
            int total_g = colorz[1][0]+colorz[1][1]+colorz[1][2];
            total_g /= 3;
            total_g *= static_cast<int>(alpha);
            buffer[0] = static_cast<unsigned char>(total_r);
            buffer[1] = static_cast<unsigned char>(total_g);
            buffer[2] = static_cast<unsigned char>(total_r+total_g*alpha);
            
        }
            break;
        case 10:
        {
            buffer[0] = static_cast<unsigned char>(((i+z)*pos)/(i+z+1)+buffer[0]*pos);
            buffer[1] += static_cast<unsigned char>(((i*pos)/(z+1))+buffer[1]);
            buffer[2] += static_cast<unsigned char>(((z*pos)/(i+1))+buffer[2]);
        }
            break;
        case 11:
        {
            buffer[0] += static_cast<unsigned char>((buffer[2]+(i*pos))/(pos+1));
            buffer[1] += static_cast<unsigned char>((buffer[1]+(z*pos))/(pos+1));
            buffer[2] += buffer[0];
        }
            break;
        case 12:
        {
            buffer[0] += static_cast<unsigned char>((i/(z+1))*pos+buffer[0]);
            buffer[1] += static_cast<unsigned char>((z/(i+1))*pos+buffer[1]);
            buffer[2] += static_cast<unsigned char>(((i+z)/(pos+1)+buffer[2]));
        }
            break;
        case 13:
        {
            buffer[0] += static_cast<unsigned char>((pos*(i/(pos+1))+buffer[2]));
            buffer[1] += static_cast<unsigned char>((pos*(z/(pos+1))+buffer[1]));
            buffer[2] += static_cast<unsigned char>((pos*((i*z)/(pos+1)+buffer[0])));
        }
            break;
        case 14:
        {
            buffer[0] = static_cast<unsigned char>(((i+z)*pos)/(i+z+1)+buffer[0]*pos);
            buffer[1] += static_cast<unsigned char>((buffer[1]+(z*pos))/(pos+1));
            buffer[2] += static_cast<unsigned char>(((i+z)/(pos+1)+buffer[2]));
        }
            break;
        case 15:
        {
            buffer[0] = static_cast<unsigned char>((i%(z+1))*pos+buffer[0]);
            buffer[1] = static_cast<unsigned char>((z%(i+1))*pos+buffer[1]);
            buffer[2] = static_cast<unsigned char>((i+z%((int)pos+1))+buffer[2]);
        }
            break;
        case 16:
        {
            int r = 0;
            r = (buffer[0]+buffer[1]+buffer[2])/3;
            buffer[0] += static_cast<unsigned char>(pos*r);
            buffer[1] += static_cast<unsigned char>(-(pos*r));
            buffer[2] += static_cast<unsigned char>(pos*r);
        }
            break;
        case 17:
        {
            unsigned long r = 0;;
            r = static_cast<unsigned char>((buffer[0]+buffer[1]+buffer[2])/(pos+1));
            buffer[0] += static_cast<unsigned char>(r*pos);
            r = (buffer[0]+buffer[1]+buffer[2])/3;
            buffer[1] += static_cast<unsigned char>(r*pos);
            r = (buffer[0]+buffer[1]+buffer[2])/5;
            buffer[2] += static_cast<unsigned char>(r*pos);
        }
            break;
        case 18:
        {
            buffer[0] += static_cast<unsigned char>(1+(sin(pos))*z);
            buffer[1] += static_cast<unsigned char>((1+cos(pos))*i);
            buffer[2] += static_cast<unsigned char>(((buffer[0]+buffer[1]+buffer[2])/3));
        }
            break;
        case 19:
        {
            buffer[0] += static_cast<unsigned char>(((buffer[2]-i)*((((int)pos+1)%15)+2)));
            buffer[1] += static_cast<unsigned char>(((buffer[1]-z)*((((int)pos+1)%15)+2)));
            buffer[2] += static_cast<unsigned char>((buffer[0]-(i+z)*((((int)pos+1)%15)+2)));
        }
            break;
        case 20:
        {
            buffer[0] += static_cast<unsigned char>((buffer[0]+buffer[1]-buffer[2])/3*pos);
            buffer[1] -= static_cast<unsigned char>((buffer[0]-buffer[1]+buffer[2])/6*pos);
            buffer[2] += static_cast<unsigned char>((buffer[0]-buffer[1]-buffer[2])/9*pos);
        }
            break;
        case 21:
        {
            int iq = i, zq = z+1;
            if(zq > height-2) return;
            cv::Vec3b &temp = full_buffer.at<cv::Vec3b>(zq, iq);
            zq = z+2;
            if(zq > height-2) return;
            cv::Vec3b &temp2 = full_buffer.at<cv::Vec3b>(zq, iq);
            int ir, ig, ib;
            ir = buffer[0]+temp[0]-temp2[0];
            ig = buffer[1]-temp[1]+temp2[1];
            ib = buffer[2]-temp[2]-temp2[2];
            if(z%2 == 0) {
                if(i%2 == 0) {
                    buffer[0] = static_cast<unsigned char>(ir+(0.5*pos));
                    buffer[1] = static_cast<unsigned char>(ig+(0.5*pos));
                    buffer[2] = static_cast<unsigned char>(ib+(0.5*pos));
                } else {
                    buffer[0] = static_cast<unsigned char>(ir+(1.5*pos));
                    buffer[1] = static_cast<unsigned char>(ig+(1.5*pos));
                    buffer[2] = static_cast<unsigned char>(ib+(1.5*pos));
                }
            } else {
                if(i%2 == 0) {
                    buffer[0] += static_cast<unsigned char>(ir+(0.1*pos));
                    buffer[1] += static_cast<unsigned char>(ig+(0.1*pos));
                    buffer[2] += static_cast<unsigned char>(ib+(0.1*pos));
                } else {
                    buffer[0] -= static_cast<unsigned char>(ir+(i*pos));
                    buffer[1] -= static_cast<unsigned char>(ig+(z*pos));
                    buffer[2] -= static_cast<unsigned char>(ib+(0.1*pos));
                }
            }
        }
            break;
        case 22:
        {
            if((i%2) == 0) {
                if((z%2) == 0) {
                    buffer[0] = static_cast<unsigned char>(1-pos*buffer[0]);
                    buffer[2] = static_cast<unsigned char>((i+z)*pos);
                } else {
                    buffer[0] = static_cast<unsigned char>(pos*buffer[0]-z);
                    buffer[2] = static_cast<unsigned char>((i-z)*pos);
                }
            } else {
                if((z%2) == 0) {
                    buffer[0] = static_cast<unsigned char>(pos*buffer[0]-i);
                    buffer[2] = static_cast<unsigned char>((i-z)*pos);
                } else {
                    buffer[0] = static_cast<unsigned char>(pos*buffer[0]-z);
                    buffer[2] = static_cast<unsigned char>((i+z)*pos);
                }
            }
        }
            break;
        case 23:
        {
            buffer[0] = static_cast<unsigned char>(buffer[0]+buffer[1]*2+pos);
            buffer[1] = static_cast<unsigned char>(buffer[1]+buffer[0]*2+pos);
            buffer[2] = static_cast<unsigned char>(buffer[2]+buffer[0]+pos);
        }
            break;
        case 24:
        {
            buffer[0] += static_cast<unsigned char>(buffer[2]+pos);
            buffer[1] += static_cast<unsigned char>(buffer[1]+pos);
            buffer[2] += static_cast<unsigned char>(buffer[0]+pos);
        }
            break;
        case 25:
        {
            buffer[0] += static_cast<unsigned char>((buffer[2]*pos));
            buffer[1] += static_cast<unsigned char>((buffer[0]*pos));
            buffer[2] += static_cast<unsigned char>((buffer[1]*pos));
        }
            break;
        case 26:
        {
            buffer[0] += static_cast<unsigned char>((buffer[2]*pos)+i);
            buffer[1] += static_cast<unsigned char>((buffer[0]*pos)+z);
            buffer[2] += static_cast<unsigned char>((buffer[1]*pos)+i-z);
        }
            break;
        case 27:
        {
            buffer[0] = static_cast<unsigned char>((-buffer[2])+z);
            buffer[1] = static_cast<unsigned char>((-buffer[0])+i);
            buffer[2] = static_cast<unsigned char>((-buffer[1])+pos);
        }
            break;
        case 28:
        {
            buffer[0] = static_cast<unsigned char>(buffer[2]+(1+(i*z)/pos));
            buffer[1] = static_cast<unsigned char>(buffer[1]+(1+(i*z)/pos));
            buffer[2] = static_cast<unsigned char>(buffer[0]+(1+(i*z)/pos));
        }
            break;
        case 29:
        {
            int iq = i, zq = z+1;
            if(zq > height-2) return;
            cv::Vec3b &temp = full_buffer.at<cv::Vec3b>(zq, iq);
            zq = z+2;
            if(zq > height-2) return;
            cv::Vec3b &temp2 = full_buffer.at<cv::Vec3b>(zq, iq);
            zq = z+3;
            if(zq > height-2) return;
            cv::Vec3b &temp3 = full_buffer.at<cv::Vec3b>(zq, iq);
            zq = z+4;
            if(zq > height-2) return;
            cv::Vec3b &temp4 = full_buffer.at<cv::Vec3b>(zq, iq);
            unsigned char col[4];
            col[0] = static_cast<unsigned char>((temp[0]+temp2[0]+temp3[0]+temp4[0])/4);
            col[1] = static_cast<unsigned char>((temp[1]+temp2[1]+temp3[1]+temp4[1])/4);
            col[2] = static_cast<unsigned char>((temp[2]+temp2[2]+temp3[2]+temp4[2])/4);
            buffer[0] = static_cast<unsigned char>(col[0]*pos);
            buffer[1] = static_cast<unsigned char>(col[1]*pos);
            buffer[2] = static_cast<unsigned char>(col[2]*pos);
        }
            break;
        case 30:
        {
            double rad = 100.0;
            double degree = 0.01*pos;
            int x = static_cast<int>(rad * (cos(degree)));
            int y = static_cast<int>(rad * sin(degree));
            int z = static_cast<int>(rad * tan(degree));
            buffer[0] = static_cast<unsigned char>(buffer[0]+x);
            buffer[2] = static_cast<unsigned char>(buffer[1]+y);
            buffer[1] = static_cast<unsigned char>(buffer[1]+z);
        }
            break;
        case 31:
        {
            int average= static_cast<unsigned char>((buffer[0]+buffer[1]+buffer[2]+1)/3);
            buffer[0] += static_cast<unsigned char>(buffer[2]+average*(pos));
            buffer[1] += static_cast<unsigned char>(buffer[0]+average*(pos));
            buffer[2] += static_cast<unsigned char>(buffer[1]+average*(pos));
        }
            break;
        case 32:
        {
            int value = 0;
            value  = ~buffer[0] + ~buffer[1] + ~buffer[2];
            value /= 2;
            buffer[0] = static_cast<unsigned char>(buffer[0]+value*pos);
            value /= 2;
            buffer[1] = static_cast<unsigned char>(buffer[1]+value*pos);
            value /= 2;
            buffer[2] = static_cast<unsigned char>(buffer[2]+value*pos);
        }
            break;
        case 33:
        {
            buffer[0] += static_cast<unsigned char>(*count*pos);
            buffer[1] += static_cast<unsigned char>(*count*pos);
            buffer[2] += static_cast<unsigned char>(*count*pos);
            *count += 0.00001f;
            if(*count > 255) *count = 0;
        }
            break;
        case 34:
        {
            buffer[0] += static_cast<unsigned char>(pos*(randomNumber+pos));
            buffer[1] += static_cast<unsigned char>(pos*(randomNumber+z));
            buffer[2] += static_cast<unsigned char>(pos*(randomNumber+i));
        }
            break;
        case 35:
        {
            buffer[0] += static_cast<unsigned char>(*count *z);
            buffer[1] += static_cast<unsigned char>(*count *pos);
            buffer[2] += static_cast<unsigned char>(*count *z);
            *count += 0.0000001f;
        }
            break;
        case 36:
        {
            buffer[0] += static_cast<unsigned char>(sin(3.14+pos)*pos);
            buffer[1] += static_cast<unsigned char>(cos(3.14+pos)*pos);
            buffer[2] += static_cast<unsigned char>(tan(3.14+pos)*pos);
        }
            break;
    }
    buffer[0] += red;
    buffer[1] += green;
    buffer[2] += blue;
    if(negate == true) {
        buffer[0] = ~buffer[0];
        buffer[1] = ~buffer[1];
        buffer[2] = ~buffer[2];
    }
    unsigned char buf[3];
    buf[0] = buffer[0];
    buf[1] = buffer[1];
    buf[2] = buffer[2];
    switch(reverse) {
        case 0://normal
            break;
        case 1:
            buffer[0] = buf[2];
            buffer[1] = buf[1];
            buffer[2] = buf[0];
            break;
        case 2:
            buffer[0] = buf[1];
            buffer[1] = buf[2];
            buffer[2] = buf[0];
            break;
        case 3:
            buffer[0] = buf[2];
            buffer[1] = buf[0];
            buffer[2] = buf[1];
            break;
        case 4:
            buffer[0] = buf[1];
            buffer[1] = buf[0];
            buffer[2] = buf[2];
            break;
    }
}

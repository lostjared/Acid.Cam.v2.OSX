/* Acid Cam Functions for OpenCV
 * written by Jared Bruni https://github.com/lostjared
 
 This software is dedicated to all the people that struggle with mental illness.
 
 Website: http://lostsidedead.com
 YouTube: http://youtube.com/LostSideDead
 Instagram: http://instagram.com/jaredbruni
 Twitter: http://twitter.com/jaredbruni
 Facebook: http://facebook.com/LostSideDead0x
 
 You can use this program free of charge and redistrubute as long
 as you do not charge anything for this program. This program is 100%
 Free.
 
 BSD 2-Clause License
 
 Copyright (c) 2018, Jared Bruni
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

#ifndef __TOKENIZE_H__
#define __TOKENIZE_H__

#include<iostream>
#include<vector>
#include<string>

namespace token {
    
    template<typename type>
    type substr(type t, size_t start, size_t stop) {
        type temp;
        for(size_t i = start; i < stop; i++)
            temp += t[i];
        return temp;
    }
    
    template<> char* substr(char *t, size_t start, size_t stop) {
        char *temp = new char [ stop-start+1 ];
        size_t pos = 0;
        for(size_t i = start; i < stop; i++)
            temp[pos++] = t[i];
        temp[pos] = 0;
        return temp;
    }
    
    template<typename type>
    size_t len(type &t) {
        size_t c = 0;
        for(c = 0; t[c] != 0; c++);
        return c;
    }
    
    template<typename type>
    size_t find(size_t start, type& source, type& sub) {
        for(size_t i = start; source[i] != 0; i++) {
            bool add = true;
            for(size_t z = 0; sub[z] != 0; z++) {
                if(source[i+z] != sub[z]) {
                    add = false;
                    break;
                }
            }
            if(add == true)
                return i;
        }
        return 0;
    }
    
    template<typename type>
    size_t tokenize(type source, type delim, std::vector<type> &v) {
        size_t i = find<type>(0,source,delim),z=0;
        if(i == 0) i = find<type>(i+1, source,delim);
        size_t lenz = len<type>(source), dlen = len<type>(delim);
        while (i != 0 && i < lenz && z < lenz ) {
            type s = substr(source,z,i);
            if(len(s) > 0 && s[0] != delim[0])
                v.push_back(s);
            z = i+dlen;
            i = find<type>(i+1, source, delim);
        }
        if( z < lenz ) v.push_back( substr(source,z,lenz) );
        return v.size();
    }
}

#endif


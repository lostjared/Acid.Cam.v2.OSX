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

#ifndef __FILTER_CAT_H__
#define __FILTER_CAT_H__

#include<vector>
#include<string>
#include<unordered_map>
namespace ac {
    class FilterItem {
    public:
        FilterItem() = default;
        std::vector<std::string> *menu_list;
        std::string menu_name;
        FilterItem(std::vector<std::string> &v, std::string name) : menu_list(&v), menu_name(name) {}
        FilterItem(const FilterItem &f) : menu_list(f.menu_list), menu_name(f.menu_name) {}
        FilterItem &operator=(const FilterItem &f) {
            menu_list = f.menu_list;
            menu_name = f.menu_name;
            return *this;
        }
    };
    extern std::default_random_engine random;
    extern std::vector<std::string> vzBlend;
    extern std::vector<std::string> svDistort;
    extern std::vector<std::string> svPattern;
    extern std::vector<std::string> svGradient;
    extern std::vector<std::string> svMirror;
    extern std::vector<std::string> svStrobe;
    extern std::vector<std::string> svBlur;
    extern std::vector<std::string> svImage;
    extern std::vector<std::string> svOther;
    extern std::vector<std::string> svOther_Custom;
    extern std::vector<std::string> svSquare;
    extern std::vector<std::string> vSub;
    extern std::vector<std::string> svCustom_Spec;
    extern std::vector<std::string> svAll;
    extern std::vector<std::string> svAllSorted;
    extern std::vector<std::string> svUser;
    extern const char *szCustom[];
    extern const char *szCustom_Spec[];
    extern std::unordered_map<std::string, FilterItem> filter_menu_map;
    extern std::vector<std::string> median;
    extern void SortFilters();
    extern void init_filter_menu_map();
}


#endif


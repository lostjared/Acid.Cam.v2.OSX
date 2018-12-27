#ifndef __FILTER_CAT_H__
#define __FILTER_CAT_H__


#include<vector>
#include<string>

namespace ac {
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
    extern const char *szCustom[];
    extern const char *szCustom_Spec[];
    extern void SortFilters();
}

#endif


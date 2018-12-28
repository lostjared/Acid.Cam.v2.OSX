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
    extern void SortFilters();
    extern void init_filter_menu_map();
}

#endif


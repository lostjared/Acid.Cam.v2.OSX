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
            if(len(s) > 0 && s != delim)
                v.push_back(s);
            z = i+dlen;
            i = find<type>(i+1, source, delim);
        }
        if( z < lenz ) v.push_back( substr(source,z,lenz) );
        return v.size();
    }
}

#endif


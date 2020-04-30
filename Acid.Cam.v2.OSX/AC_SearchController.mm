
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

#import "AC_Controller.h"
#import "tokenize.hpp"

std::string Lower(const std::string &s) {
    std::string tmp;
    for(int i = 0; i < s.length(); ++i) {
        tmp += tolower(s[i]);
    }
    return tmp;
}

void SearchForString(NSString *s) {
    [search_results removeAllObjects];
    std::string search = Lower([s UTF8String]);
    std::vector<std::string> tokens;
    token::tokenize(search, std::string(" "), tokens);
    std::vector<int> used;
    int num = 0;
    for(int i = 0; i < ac::draw_max-4; ++i) {
        std::string search_items = Lower(ac::draw_strings[i]);
        for(unsigned q = 0; q < tokens.size(); ++q) {
            if(search_items.find(tokens[q]) != std::string::npos) {
                if(std::find(used.begin(), used.end(), i) == std::end(used)) {
                    [search_results addObject: [NSNumber numberWithInt:i]];
                    used.push_back(i);
                    ++num;
                }
            }
        }
    }
    std::ostringstream stream;
    stream << "Searched for: " << [s UTF8String] << " matched " << num << " filters.\n";
    flushToLog(stream);
    [controller reloadTable];
}


// search Delegate
@implementation SearchController

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex {
    NSString *str =  [[aTableColumn headerCell] stringValue];
    NSNumber *number = [search_results objectAtIndex:rowIndex];
    if( [str isEqualTo:@"Filter"] ) {
        int value = (int)[number integerValue];
        NSString *s = [NSString stringWithFormat:@"%s", ac::draw_strings[value].c_str()];
        return s;
    }
    else if([str isEqualTo:@"Index"]) {
        NSString *s = [NSString stringWithFormat: @"%d", (int)[number integerValue]];
        return s;
    }
    return @"";
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView {
    return [search_results count];
}

@end

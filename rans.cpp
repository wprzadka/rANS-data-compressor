//
// Created by viking on 18.04.22.
//

#include "rans.h"

std::map<char, uint32_t> RANS::get_frequencies(const std::string& word){
    std::map<char, uint32_t> freq{};

    for (auto v : word){
        if(freq.find(v) == freq.end()){
            freq[v] = 0;
        }
        ++freq[v];
    }

    return freq;
}

std::map<char, uint32_t> RANS::compute_cumulative_freq(){
    std::map<char, uint32_t> acc;
    uint32_t prev_sum = 0;
    for (const auto& iter : frequencies){
        acc[iter.first] = prev_sum;
        prev_sum += iter.second;
    }
    return acc;
}

char RANS::get_symbol(uint32_t value){
    auto iter = accumulated.begin();
    char symbol = iter->first;
    while(iter->second <= value && iter != accumulated.end()){
        symbol = iter->first;
        ++iter;
    }
    return symbol;
}

uint64_t RANS::encode(const std::string &data) {
    int64_t code = 0;
    for (auto s = data.rbegin(); s != data.rend(); ++s){
        uint32_t freq = frequencies[*s];
        code = ((code / freq) << SYMBOLS) + (code % freq) + accumulated[*s];
    }
    return code;
}

std::string RANS::decode(uint64_t code) {
    std::string decoded;
    while(code > 0){
        char s = get_symbol(code & MASK);
        decoded += s;
        code = frequencies[s] * (code >> SYMBOLS) + (code & MASK) - accumulated[s];
    }
    return decoded;
}

void RANS::init(const std::string &data) {
    frequencies = get_frequencies(data);
    accumulated = compute_cumulative_freq();
}


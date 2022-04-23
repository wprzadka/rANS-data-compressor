//
// Created by viking on 18.04.22.
//

#include <fstream>
#include "rans.h"

std::map<char, uint32_t> RANS::get_frequencies(const char* word, uint16_t size){
    std::map<char, uint32_t> freq{};

    for (int i = 0; i < size; ++i) {
        if(freq.find(word[i]) == freq.end()){
            freq[word[i]] = 0;
        }
        ++freq[word[i]];
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

void RANS::prepare_frequencies(const char* data, uint16_t size){
    frequencies = get_frequencies(data, size);
    normalize_symbol_frequencies();
    compute_cumulative_freq();
}

std::string RANS::encode(const char* data, uint16_t size) {
    uint32_t code = 0;
    std::string encoded;
    for (int i = size - 1; i >= 0; --i) {
        uint32_t freq = frequencies[data[i]];
        if(code > freq * (1 << (STATE_BITS - N_VALUE))){
            encoded += static_cast<char>(code & ((1 << 8) - 1));
            code >>= 8;
        }
        code = ((code / freq) << N_VALUE) + (code % freq) + accumulated[data[i]];
    }
    return encoded;
}

std::string RANS::decode(const char* code, uint16_t size) {
    std::string decoded;
    int idx = 0;
    uint32_t state = 0;
    for (int i = 0; i < 4; ++i){
        state <<= 8;
        state += code[idx];
        ++idx;
    }
    while(state > 0){
        char s = get_symbol(state & MASK);
        decoded += s;
        state = frequencies[s] * (state >> N_VALUE) + (state & MASK) - accumulated[s];
        if(state < (1 << HALF_STATE_BITS) && idx < size){
            state <<= 8;
            state += code[idx];
        }
    }
    return decoded;
}

//void RANS::compute_symbol_frequencies(std::ifstream& input_file){
//    std::map<char, uint32_t> freq{};
//
//    static const uint8_t BUFFOR_SIZE = 32;
//    char* buffor = new char[BUFFOR_SIZE];
//
//    while(!input_file.eof()){
//        input_file.read(buffor, BUFFOR_SIZE);
//        int i = 0;
//        while (i < BUFFOR_SIZE && buffor[i] != '\0') {
//            if(freq.find(buffor[i]) == freq.end()){
//                freq[buffor[i]] = 0;
//            }
//            ++freq[buffor[i]];
//        }
//    }
//
//    delete[] buffor;
//    frequencies = freq;
//}

void RANS::normalize_symbol_frequencies(){
    // Find probabilities of symbols occurrences
    uint32_t sum_freq = 0;
    for (auto& pair : frequencies){
        sum_freq += pair.second;
    }
    std::map<char, double> probabilities{};
    for (auto& pair : frequencies){
        probabilities[pair.first] = static_cast<double>(pair.second) / sum_freq;
    }
    // normalize occurrence probabilities to fractions of 2^N_VALUE
    sum_freq = 0;
    for (auto& pair: probabilities){
        uint32_t new_freq = static_cast<uint32_t>(pair.second * (1 << N_VALUE));
        frequencies[pair.first] = new_freq;
        sum_freq += new_freq;
    }
    // TODO MAKE frequencies sum to 2^N_VALUE
}


// (1)
// pr = fs / sum(fs)
// fs = pr * 2^n

// (2)
// read in blocks forwardly
// and read every block backwards
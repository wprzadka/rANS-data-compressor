//
// Created by viking on 18.04.22.
//

#include "rans.h"
#include <bits/stdc++.h>

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
    accumulated = compute_cumulative_freq();
}

std::string RANS::encode(const char* data, uint16_t size) {
    uint32_t state = (1 << HALF_STATE_BITS);
    std::string encoded;

    // Encode data
    for (int i = size - 1; i >= 0; --i) {
        uint32_t freq = frequencies[data[i]];
        while (state >= freq * (1 << (STATE_BITS - N_VALUE))){
            encoded += static_cast<char>(state & 255);
            state >>= 8;
            encoded += static_cast<char>(state & 255);
            state >>= 8;
        }
        state = ((state / freq) << N_VALUE) + (state % freq) + accumulated[data[i]];
    }

    // Write state at the end of encoding
    uint8_t state_bits = STATE_BITS;
    while (state_bits > 0) {
        encoded += static_cast<char>(state & 255);
        state >>= 8;
        state_bits -= 8;
    }
    std::reverse(encoded.begin(), encoded.end());
    return encoded;
}

std::string RANS::decode(const char* code, uint16_t size) {
    std::string decoded;
    int idx = 0;
    uint32_t state = 0;

    // Reconstruct state of rANS at end of encoding
    uint8_t state_bits = STATE_BITS;
    while (state_bits > 0) {
        state <<= 8;
        state += code[idx++] & 255;
        state_bits -= 8;
    }
    // Decode data
    while(state > (1 << HALF_STATE_BITS)){
        char s = get_symbol(state & MASK);

        decoded += s;
        state = frequencies[s] * (state >> N_VALUE) + (state & MASK) - accumulated[s];

        while (state < (1 << HALF_STATE_BITS) && idx < size) {
            state <<= 8;
            state += code[idx++] & 255;
            state <<= 8;
            state += code[idx++] & 255;
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
    // Normalize occurrence probabilities to fractions of 2^N_VALUE
    sum_freq = 0;
    for (auto& pair: probabilities){
        uint32_t new_freq = static_cast<uint32_t>(pair.second * (1 << N_VALUE));
        new_freq = new_freq == 0 ? 1 : new_freq;
        frequencies[pair.first] = new_freq;
        sum_freq += new_freq;
    }
    // Ensure that frequencies sums to 2^N
    auto fst = frequencies.begin();
    frequencies[fst->first] += (1 << N_VALUE) - sum_freq;
    // Check if all frequencies are in valid range
    for(auto p : frequencies){
        assert(p.second > 0 && p.second <= (1 << N_VALUE));
    }
}

void RANS::init_frequencies(const std::map<char, uint32_t>& freqs) {
    frequencies = freqs;
    accumulated = compute_cumulative_freq();
}

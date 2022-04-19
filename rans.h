//
// Created by viking on 18.04.22.
//

#ifndef ANS_RANS_H
#define ANS_RANS_H


#include <cstdint>
#include <map>

class RANS {
    const uint8_t SYMBOLS = 4;
    const uint32_t MASK = (1 << SYMBOLS) - 1;

public:
    std::map<char, uint32_t> frequencies;
    std::map<char, uint32_t> accumulated;

    void init(const std::string& data);
    void init(const std::map<char, uint32_t> freq);
    uint64_t encode(const std::string& data);
    std::string decode(uint64_t code);

protected:
    std::map<char, uint32_t> get_frequencies(const std::string& word);
    std::map<char, uint32_t> compute_cumulative_freq();
    char get_symbol(uint32_t value);
};


#endif //ANS_RANS_H

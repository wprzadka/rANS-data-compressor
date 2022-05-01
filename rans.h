//
// Created by viking on 18.04.22.
//

#ifndef ANS_RANS_H
#define ANS_RANS_H


#include <cstdint>
#include <map>
#include "gtest/gtest.h"

class RANS {
    FRIEND_TEST(RANS_Test, get_symbol);
    FRIEND_TEST(RANS_Test, prepare_frequencies);

public:
    const static uint8_t N_VALUE = 12;
    const static uint32_t MASK = (1 << N_VALUE) - 1;

    const static uint8_t STATE_BITS = 32;
    const static uint8_t HALF_STATE_BITS = STATE_BITS >> 1;

    const static uint8_t MAX_SYMBOL = 255;
    const static uint8_t NEGATIVE_SYMBOLS_NUM = 127;
    const static uint16_t BLOCK_SIZE = 8192;

    std::array<uint32_t, MAX_SYMBOL> frequencies{};
    std::array<uint32_t, MAX_SYMBOL> accumulated{};

    inline uint32_t get_frequency(char symbol) {return frequencies[static_cast<uint16_t>(symbol) + NEGATIVE_SYMBOLS_NUM];};
    inline uint32_t get_accumulated(char symbol) {return accumulated[static_cast<uint16_t>(symbol) + NEGATIVE_SYMBOLS_NUM];};

    void prepare_frequencies(const char *data, uint16_t size);
    void init_frequencies(const std::array<uint32_t, MAX_SYMBOL> &freqs);

    std::string encode(const char* data, uint16_t size);
    std::string decode(const char* state, uint16_t size);

protected:
    std::array<uint32_t, MAX_SYMBOL> compute_frequencies(const char *word, uint16_t size);
    std::array<uint32_t, MAX_SYMBOL> compute_cumulative_freq();
    void normalize_symbol_frequencies();
    char get_symbol(uint32_t value);
};


#endif //ANS_RANS_H

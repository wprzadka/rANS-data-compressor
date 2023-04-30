//
// Created by viking on 18.04.22.
//

#ifndef ANS_RANS_H
#define ANS_RANS_H


#include <cstdint>
#include <map>
#include <climits>
#include "gtest/gtest.h"

#define USE_LOOKUP_TABLE

class RANS {
    FRIEND_TEST(RANS_Test, get_symbol);
    FRIEND_TEST(RANS_Test, prepare_frequencies);

public:
    const static uint8_t N_VALUE = 12;
    const static uint32_t MASK = (1 << N_VALUE) - 1;

    const static uint8_t STATE_BITS = 32;
    const static uint8_t HALF_STATE_BITS = STATE_BITS >> 1;

    const static uint16_t SYMBOLS_NUM = 256;
    const static uint16_t BLOCK_SIZE = 8192;

    std::array<uint32_t, SYMBOLS_NUM> frequencies{};
    std::array<uint32_t, SYMBOLS_NUM> accumulated{};

    void prepare_frequencies(const unsigned char *data, uint16_t size);
    void init_frequencies(const std::array<uint32_t, SYMBOLS_NUM> &freqs);

    std::string encode(const unsigned char* data, uint16_t size);
    std::string decode(const unsigned char* state, uint16_t size);

protected:
#ifdef USE_LOOKUP_TABLE
    std::array<unsigned char, 1 << N_VALUE> symbols_lookup{};
#endif
    std::array<uint32_t, SYMBOLS_NUM> compute_frequencies(const unsigned char *word, uint16_t size);
    std::array<uint32_t, SYMBOLS_NUM> compute_cumulative_freq();
    void normalize_symbol_frequencies();
    unsigned char get_symbol(uint32_t value);
};


#endif //ANS_RANS_H

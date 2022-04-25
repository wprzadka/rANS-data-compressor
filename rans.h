//
// Created by viking on 18.04.22.
//

#ifndef ANS_RANS_H
#define ANS_RANS_H


#include <cstdint>
#include <map>

class RANS {
    const uint8_t N_VALUE = 12;
    const uint32_t MASK = (1 << N_VALUE) - 1;

    const uint8_t STATE_BITS = 32;
    const uint8_t HALF_STATE_BITS = STATE_BITS >> 1;

public:
    const uint16_t BLOCK_SIZE = 8192;

    std::map<char, uint32_t> frequencies;
    std::map<char, uint32_t> accumulated;

    void prepare_frequencies(const char *data, uint16_t size);
    void init_frequencies(const std::map<char, uint32_t> &freqs);

    std::string encode(const char* data, uint16_t size);
    std::string decode(const char* state, uint16_t size);

protected:
    std::map<char, uint32_t> get_frequencies(const char *word, uint16_t size);
    std::map<char, uint32_t> compute_cumulative_freq();
    void normalize_symbol_frequencies();
    char get_symbol(uint32_t value);

//    void compute_symbol_frequencies(std::ifstream& input_file);
};


#endif //ANS_RANS_H

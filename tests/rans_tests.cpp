//
// Created by viking on 26.04.22.
//

#include <fstream>
#include "gtest/gtest.h"
#include "../rans.h"

static const uint8_t MIN_CHAR_ABS = 128;


class RANS_Coding_Test: public ::testing::Test{
public:
    const uint16_t plain_text_size = 1080;
    const char* plain_text =
            "One morning, when Gregor Samsa woke from troubled dreams, he found himself transformed in his bed into a horrible vermin.\n"
            "He lay on his armour-like back, and if he lifted his head a little he could see his brown belly, slightly domed and divided by arches into stiff sections.\n"
            "The bedding was hardly able to cover it and seemed ready to slide off any moment.\n"
            "His many legs, pitifully thin compared with the size of the rest of him, waved about helplessly as he looked.\n"
            "\"What's happened to me? \" he thought. It wasn't a dream.\n"
            "His room, a proper human room although a little too small, lay peacefully between its four familiar walls.\n"
            "A collection of textile samples lay spread out on the table - Samsa was a travelling salesman - and above it there hung a picture that he had recently cut out of an illustrated magazine and housed in a nice, gilded frame.\n"
            "It showed a lady fitted out with a fur hat and fur boa who sat upright, raising a heavy fur muff that covered the whole of her lower arm towards the viewer.\n"
            "Gregor then turned to look out the window at the dull weather. Drops";

    std::array<uint32_t, RANS::SYMBOLS_NUM> frequencies;

    RANS_Coding_Test(){
        const std::map<unsigned char, uint32_t> temp_freqs = {
                {'\n', 52}, {' ', 728}, {'"', 7}, {'\'', 7}, {',', 37}, {'-', 11},
                {'.', 37}, {'?', 3}, {'A', 3}, {'D', 3}, {'G', 7}, {'H', 11},
                {'I', 7}, {'O', 3}, {'S', 7}, {'T', 3}, {'W', 3}, {'a', 292},
                {'b', 53}, {'c', 53}, {'d', 166}, {'e', 379}, {'f', 98}, {'g', 53},
                {'h', 197}, {'i', 197}, {'k', 18}, {'l', 201}, {'m', 109}, {'n', 159},
                {'o', 238}, {'p', 49}, {'r', 189}, {'s', 166}, {'t', 269}, {'u', 102},
                {'v', 34}, {'w', 75}, {'x', 3}, {'y', 60}, {'z', 7},
        };

        frequencies.fill(0);
        for (auto p : temp_freqs){
            frequencies[p.first] = p.second;
        }
    }
};

TEST_F(RANS_Coding_Test, encode_decode_consistency){
    RANS rans{};
    auto* symbols = reinterpret_cast<const unsigned char*>(plain_text);
    rans.prepare_frequencies(symbols, plain_text_size);

    // encode data
    std::string encoded = rans.encode(symbols, plain_text_size);

    // decode data
    auto* encoded_symbols = reinterpret_cast<const unsigned char*>(encoded.c_str());
    rans.init_frequencies(frequencies);
    std::string decoded = rans.decode(encoded_symbols, encoded.size());

    ASSERT_EQ(plain_text_size, decoded.size());
    for (int i = 0; i < decoded.size(); ++i){
        ASSERT_EQ(plain_text[i], decoded[i]);
    }
}

TEST(RANS_Test, prepare_frequencies){
    RANS rans{};
    const uint8_t SIZE = 8;
    const uint32_t POW = 1 << RANS::N_VALUE;
    std::vector<std::pair<unsigned char, uint32_t>> symbols = {{'A', POW / 4}, {'B', POW / 2}, {'C', POW / 4}};
    auto* data = new unsigned char[SIZE] {'A', 'B', 'B', 'B', 'C', 'A', 'C', 'B'};

    rans.prepare_frequencies(data, SIZE);

    for (const auto& s : symbols){
        ASSERT_EQ(s.second, rans.frequencies[s.first]);
    }
    uint32_t accumulated = 0;
    for (const auto& s : symbols){
        ASSERT_EQ(accumulated, rans.accumulated[s.first]);
        accumulated += s.second;
    }
    delete[] data;
}

TEST(RANS_Test, init_frequencies){
    unsigned char symbols[] = {'A', 'B', 'C'};
    std::array<uint32_t, RANS::SYMBOLS_NUM> frequencies({});
    frequencies['A'] = 2;
    frequencies['B'] = 4;
    frequencies['C'] = 2;
    RANS rans{};

    rans.init_frequencies(frequencies);

    for (auto s : symbols){
        ASSERT_EQ(frequencies[s], rans.frequencies[s]);
    }
    uint32_t accumulated = 0;
    for (auto s : symbols){
        ASSERT_EQ(accumulated, rans.accumulated[s]);
        accumulated += frequencies[s];
    }
}

TEST(RANS_Test, get_symbol){
    std::array<uint32_t, RANS::SYMBOLS_NUM> frequencies({});
    frequencies['A'] = 2;
    frequencies['B'] = 4;
    frequencies['C'] = 2;
    RANS rans{};
    rans.init_frequencies(frequencies);

    char symbols[] = {'A', 'A', 'B', 'B', 'B', 'B', 'C', 'C'};
    for (int i = 0; i < 8; ++i) {
        ASSERT_EQ(symbols[i], rans.get_symbol(i));
    }
}

int main(int argc, char** argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

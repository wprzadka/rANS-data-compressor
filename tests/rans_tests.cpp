//
// Created by viking on 26.04.22.
//

#include <fstream>
#include "gtest/gtest.h"
#include "../rans.h"

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
    const uint16_t encoded_text_size = 588;

    std::array<uint32_t, 255> frequencies{};

    RANS_Coding_Test(){
        const std::map<char, uint32_t> temp_freqs = {
                {'\n', 52}, {' ', 728}, {'"', 7}, {'\'', 7}, {',', 37}, {'-', 11},
                {'.', 37}, {'?', 3}, {'A', 3}, {'D', 3}, {'G', 7}, {'H', 11},
                {'I', 7}, {'O', 3}, {'S', 7}, {'T', 3}, {'W', 3}, {'a', 292},
                {'b', 53}, {'c', 53}, {'d', 166}, {'e', 379}, {'f', 98}, {'g', 53},
                {'h', 197}, {'i', 197}, {'k', 18}, {'l', 201}, {'m', 109}, {'n', 159},
                {'o', 238}, {'p', 49}, {'r', 189}, {'s', 166}, {'t', 269}, {'u', 102},
                {'v', 34}, {'w', 75}, {'x', 3}, {'y', 60}, {'z', 7},
        };
        for (auto p : temp_freqs){
            frequencies[p.first + 127] = p.second;
        }
    }
};

TEST_F(RANS_Coding_Test, encode){
    RANS rans{};
    rans.prepare_frequencies(plain_text, plain_text_size);

    std::string encoded = rans.encode(plain_text, plain_text_size);
    ASSERT_EQ(encoded_text_size, encoded.size());

    std::ifstream exact("encoded.bin", std::ios::binary | std::ios::in);
    if (!exact){
        printf("Can't open file\n");
        FAIL();
    }
    char* encoded_text = new char[encoded_text_size];
    exact.read(encoded_text, encoded_text_size);
    uint32_t read_bytes = exact.gcount();
    exact.close();
    if (read_bytes != encoded_text_size) {
        printf("Not all input has been read\n");
        FAIL();
    }

    for (int i = 0; i < encoded_text_size; ++i){
        ASSERT_EQ(encoded_text[i], encoded[i]);
    }
    delete[] encoded_text;
}


TEST_F(RANS_Coding_Test, decode){
    RANS rans{};

    std::ifstream input("encoded.bin", std::ios::binary | std::ios::in);
    if (!input){
        printf("Can't open file\n");
        FAIL();
    }
    char* encoded_text = new char[encoded_text_size];
    input.read(encoded_text, encoded_text_size);
    uint32_t read_bytes = input.gcount();
    input.close();
    if (read_bytes != encoded_text_size) {
        printf("Not all input has been read\n");
        FAIL();
    }
    rans.init_frequencies(frequencies);

    std::string decoded = rans.decode(encoded_text, encoded_text_size);

    delete[] encoded_text;
    ASSERT_EQ(plain_text_size, decoded.size());
    for (int i = 0; i < encoded_text_size; ++i){
        ASSERT_EQ(plain_text[i], decoded[i]);
    }
}

TEST(RANS_Test, prepare_frequencies){
    RANS rans{};
    const uint8_t SIZE = 8;
    const uint32_t POW = 1 << rans.N_VALUE;
    std::vector<std::pair<char, uint32_t>> symbols = {{'A', POW / 4}, {'B', POW / 2}, {'C', POW / 4}};
    char* data = new char[SIZE] {'A', 'B', 'B', 'B', 'C', 'A', 'C', 'B'};

    rans.prepare_frequencies(data, SIZE);

    for (const auto& s : symbols){
        ASSERT_EQ(s.second, rans.get_frequency(s.first));
    }
    uint32_t accumulated = 0;
    for (const auto& s : symbols){
        ASSERT_EQ(accumulated, rans.get_accumulated(s.first));
        accumulated += s.second;
    }
    delete[] data;
}

TEST(RANS_Test, init_frequencies){
    char symbols[] = {'A', 'B', 'C'};
    std::array<uint32_t, RANS::MAX_SYMBOL> frequencies{};
    frequencies['A' + 127] = 2;
    frequencies['B' + 127] = 4;
    frequencies['C' + 127] = 2;
    RANS rans{};

    rans.init_frequencies(frequencies);

    for (char s : symbols){
        ASSERT_EQ(frequencies[s + 127], rans.get_frequency(s));
    }
    uint32_t accumulated = 0;
    for (char s : symbols){
        ASSERT_EQ(accumulated, rans.get_accumulated(s));
        accumulated += frequencies[s + 127];
    }
}

TEST(RANS_Test, get_symbol){
    std::array<uint32_t, RANS::MAX_SYMBOL> frequencies{};
    frequencies['A' + 127] = 2;
    frequencies['B' + 127] = 4;
    frequencies['C' + 127] = 2;
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

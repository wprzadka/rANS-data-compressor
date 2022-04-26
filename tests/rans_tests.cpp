//
// Created by viking on 26.04.22.
//

#include "gtest/gtest.h"
#include "../rans.h"

class RANS_Test: public ::testing::Test{

};

TEST(RANS_Test, prepare_frequencies){
    RANS rans{};
    const uint8_t SIZE = 8;
    const uint32_t POW = 1 << rans.N_VALUE;
    std::vector<std::pair<char, uint32_t>> symbols = {{'A', POW / 4}, {'B', POW / 2}, {'C', POW / 4}};
    char* data = new char[SIZE] {'A', 'B', 'B', 'B', 'C', 'A', 'C', 'B'};

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
    std::map<char, uint32_t> frequencies = {
            {'A', 2}, {'B', 4}, {'C', 2}
    };
    RANS rans{};

    rans.init_frequencies(frequencies);

    for (const auto& pair : frequencies){
        ASSERT_EQ(pair.second, rans.frequencies[pair.first]);
    }
    uint32_t accumulated = 0;
    for (const auto& pair : frequencies){
        ASSERT_EQ(accumulated, rans.accumulated[pair.first]);
        accumulated += pair.second;
    }
}

TEST(RANS_Test, get_symbol){
    std::map<char, uint32_t> frequencies = {
            {'A', 2}, {'B', 4}, {'C', 2}
    };
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

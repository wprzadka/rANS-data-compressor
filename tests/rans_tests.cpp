//
// Created by viking on 26.04.22.
//

#include "gtest/gtest.h"
#include "../rans.h"

class RANS_Test: public ::testing::Test{

};

TEST(RANS_Test, GetSymbolValuesTest){
    std::map<char, uint32_t> frequencies = {
            {'A', 2}, {'B', 4}, {'C', 2}
    };
    RANS rans{};
    rans.init_frequencies(frequencies);

    char symbols[] = {'A', 'A', 'B', 'B', 'B', 'B', 'C', 'C'};
    for (int i = 0; i < 8; ++i) {
        ASSERT_EQ(symbols[i], rans.get_symbol(i));
    };
}

int main(int argc, char** argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

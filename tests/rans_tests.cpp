//
// Created by viking on 26.04.22.
//

#include "gtest/gtest.h"
#include "../rans.h"

class RANS_Test: public ::testing::Test{

};

TEST(RANS_Test, GetSymbolValuesTest){
    std::map<char, uint32_t> frequencies = {
            {'A', 2}, {'B', 4}, {'C', 1}
    };
    RANS rans{};
    rans.init_frequencies(frequencies);

    ASSERT_EQ('A', rans.get_symbol(1));
    ASSERT_EQ('B', rans.get_symbol(5));
    ASSERT_EQ('C', rans.get_symbol(7));
}

int main(int argc, char** argv){
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

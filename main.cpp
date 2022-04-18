//
// Created by viking on 17.04.22.
//

#include <string>
#include "rans.h"


int main(){
    RANS rans{};

    std::string message = "AABAA4CCAAD12";
    rans.init(message);

    uint64_t enc = rans.encode(message);
    printf("encoded\n%lu\n", enc);

    std::string dec = rans.decode(enc);
    printf("decoded\n%s\n", dec.c_str());
}

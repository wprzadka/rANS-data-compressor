//
// Created by viking on 17.04.22.
//

#include <string>
#include <fstream>
#include "rans.h"


int main(){

    std::ifstream file_reader("example.txt", std::ios::binary);
    if(!file_reader.is_open()){
        printf("File can't be opened\n");
        return 1;
    }

    uint8_t size = 16;
    char* mem_block = new char [size];
    file_reader.read(mem_block, size);
    printf("input = %s\n", mem_block);

    std::string message = mem_block;

    RANS rans{};
    rans.init(message);

    uint64_t enc = rans.encode(message);
    printf("encoded\n%lu\n", enc);

    std::string dec = rans.decode(enc);
    printf("decoded\n%s\n", dec.c_str());

    delete[] mem_block;
}

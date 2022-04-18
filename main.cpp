//
// Created by viking on 17.04.22.
//

#include <string>
#include <fstream>
#include "rans.h"
#include <getopt.h>

int encode_file(char* file_name){

    std::ifstream file_reader(file_name, std::ios::binary);
    if(!file_reader.is_open()){
        printf("file \"%s\" can't be opened.\n", file_name);
    }
    uint8_t size = 16;
    char* mem_block = new char [size];
    file_reader.read(mem_block, size);

    std::string message = mem_block;

    RANS rans{};
    rans.init(message);

    uint64_t enc = rans.encode(message);
    printf("encoded\n%lu\n", enc);
    return 0;
}

int decode_file(char* file_name){

//    std::string dec = rans.decode(enc);
//    printf("decoded\n%s\n", dec.c_str());
    printf("not implemented\n");
    return 0;
}

int main(int argc, char** argv){

    option option_names[] = {
            {"version", no_argument, nullptr, 'v'},
            {"help", no_argument, nullptr, 'h'},
            {"encode", required_argument, nullptr, 'e'},
            {"decode", required_argument, nullptr, 'd'}
    };

    int opt;
    opt = getopt_long(argc, argv, "vh:e:d", option_names, nullptr);
    switch (opt) {
        case 'v':
            printf("0.1");
            return 0;
        case 'h':
            printf("possible operations:\n"
                   "-v --version\n"
                   "-h --help\n"
                   "-e --encode file_name\n"
                   "-d --decode file_name\n");
            return 0;
        case 'e':
            return encode_file(optarg);
        case 'd':
            return decode_file(optarg);
        case ':':
            printf("Option requires an argument.\n");
            return 1;
        case '?':
        default:
            printf("Unknown argument \"%c\" provided.\n", optopt);
            return 1;
    }
}

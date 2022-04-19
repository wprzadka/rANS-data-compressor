//
// Created by viking on 17.04.22.
//

#include <string>
#include <fstream>
#include <getopt.h>
#include "rans.h"


void write_symbol_freqencies(const std::map<char, uint32_t>& freqs, std::ofstream& file){

//    write number of symbols
    char* buffor = new char[2];
    buffor[0] = static_cast<char>(freqs.size() & 255);
    file.write(buffor, 1);

//    write symbols - frequency pairs
    for (const auto& pair : freqs){
        buffor[0] = pair.first;
        buffor[1] = static_cast<char>(pair.second & 255);
        file.write(buffor, 2);
    }
    delete[] buffor;
}

int encode_file(const char* input_file, const char* output_file = "out.bin"){

    std::ifstream file_reader(input_file, std::ios::binary);
    if(!file_reader.is_open()){
        printf("file \"%s\" can't be opened.\n", input_file);
    }
    uint8_t size = 16;
    char* mem_block = new char [size];
    file_reader.read(mem_block, size);
    file_reader.close();

    std::string message = mem_block;
    delete[] mem_block;

    RANS rans{};
    rans.init(message);

    uint64_t enc = rans.encode(message);
    printf("encoded\n%lu\n", enc);

    std::ofstream file_writer(output_file, std::ios::binary | std::ios::out);
    write_symbol_freqencies(rans.frequencies, file_writer);
    char* enc_str = new char[8];
    for (int i = 0; i < 8; ++i){
        printf("(%u) = %u\n", i, static_cast<unsigned char>((enc >> (i * 8)) & 255) << (i * 8));
        enc_str[i] = static_cast<char>((enc >> (i * 8)) & 255);
    }
    file_writer.write(enc_str, 8);
    printf("\'%s\'\n", enc_str);

    file_writer.close();
    delete[] enc_str;

    return 0;
}

std::map<char, uint32_t> read_symbol_frequencies(std::ifstream& file){
    char* mem_buffor = new char[2];
    file.read(mem_buffor, 1);
    char symbols = mem_buffor[0];

    std::map<char, uint32_t> freqs{};
    printf("symbols: %u\n", symbols);
    while(symbols > 0){
        file.read(mem_buffor, 2);
        freqs[mem_buffor[0]] = static_cast<uint32_t>(mem_buffor[1]);
        printf("%c -> %u\n", mem_buffor[0], mem_buffor[1]);
        --symbols;
    }

    delete[] mem_buffor;
    return freqs;
}

int decode_file(const char* input_file, const char* output_file = "decoded.txt"){

    std::ifstream file_reader(input_file, std::ios::binary | std::ios::in);
    RANS rans;

    rans.init(read_symbol_frequencies(file_reader));

    char* mem_buffor = new char[8];
    file_reader.read(mem_buffor, 8);
    file_reader.close();
    printf("\'%s\'\n", mem_buffor);
    uint64_t enc = 0;
    for (int i = 0; i < 8; ++i) {
        printf("(%u) = %u\n", i, static_cast<unsigned char>(mem_buffor[i]) << (i * 8));
        enc += (static_cast<unsigned char>(mem_buffor[i]) << (i * 8));
    }
    delete[] mem_buffor;

    printf("encoded = %lu\n", enc);
    std::string dec = rans.decode(enc);
    printf("decoded\n%s\n", dec.c_str());

    std::ofstream file_writer(output_file, std::ios::binary | std::ios::out);
    file_writer.write(dec.c_str(), dec.size());
    file_writer.close();
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

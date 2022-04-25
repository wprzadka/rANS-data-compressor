//
// Created by viking on 17.04.22.
//

#include <string>
#include <fstream>
#include <getopt.h>
#include <cassert>
#include "rans.h"


void write_symbol_freqencies(const std::map<char, uint32_t>& freqs, std::ofstream& file){

//    write number of symbols
    char* buffor = new char[3];
    buffor[0] = static_cast<char>(freqs.size() & 255);
    file.write(buffor, 1);

//    write symbols - frequency pairs
    for (const auto& pair : freqs){
        buffor[0] = pair.first;
        assert(pair.second < (1 << 16));
        buffor[1] = static_cast<char>((pair.second >> 8) & 255);
        buffor[2] = static_cast<char>(pair.second & 255);
        file.write(buffor, 3);
    }
    delete[] buffor;
}

std::map<char, uint32_t> read_symbol_frequencies(std::ifstream& file){
    char* mem_buffor = new char[3];
    file.read(mem_buffor, 1);
    char symbols = mem_buffor[0];

    std::map<char, uint32_t> freqs{};
    while(symbols > 0){
        file.read(mem_buffor, 3);
        uint32_t freq = (static_cast<unsigned char>(mem_buffor[1] & 255) << 8) + static_cast<unsigned char>((mem_buffor[2] & 255));
        freqs[mem_buffor[0]] = freq;
        --symbols;
    }

    delete[] mem_buffor;
    return freqs;
}

static const uint8_t BLOCK_SIZE_BYTES = 2;

void write_size_of_block(std::ofstream& file, uint32_t size){
    assert(size < (1 << (BLOCK_SIZE_BYTES * 8)));
    char* mem_buff = new char[BLOCK_SIZE_BYTES];

    for (int i = BLOCK_SIZE_BYTES - 1; i >= 0; --i) {
        mem_buff[i] = static_cast<char>(size & 255);
        size >>= 8;
    }
    file.write(mem_buff, BLOCK_SIZE_BYTES);

    delete[] mem_buff;
}

uint32_t read_size_of_block(std::ifstream& file){
    char* mem_buff = new char[BLOCK_SIZE_BYTES];
    file.read(mem_buff, BLOCK_SIZE_BYTES);

    uint32_t size = 0;
    for (int i = 0; i < BLOCK_SIZE_BYTES; ++i) {
        size <<= 8;
        size += mem_buff[i] & 255;
    }
    return size;
}

int encode_file(const std::string& input_file, const std::string& output_file = "out.bin"){
    std::ifstream file_reader(input_file, std::ios::binary | std::ios::in);
    std::ofstream file_writer(output_file, std::ios::binary | std::ios::out);
    if(!file_reader.is_open() || !file_writer.is_open()){
        return 1;
    }
    RANS rans{};
    char* mem_buff = new char[rans.BLOCK_SIZE];
    while(!file_reader.eof()){
        // Read next block
        file_reader.read(mem_buff, rans.BLOCK_SIZE);
        uint32_t bits_read = file_reader.gcount();
        // Prepare and frequencies of symbol occurrence
        rans.prepare_frequencies(mem_buff, bits_read);
        // encode block
        std::string enc = rans.encode(mem_buff, bits_read);
        // save block with frequencies to file
        write_symbol_freqencies(rans.frequencies, file_writer);
        write_size_of_block(file_writer, enc.size());
        file_writer.write(enc.c_str(), static_cast<long>(enc.size()));
    }
    delete[] mem_buff;
    file_reader.close();
    file_writer.close();
    return 0;
}

int decode_file(const std::string& input_file, const std::string& output_file = "decoded.bin"){
    std::ifstream file_reader(input_file, std::ios::binary | std::ios::in);
    std::ofstream file_writer(output_file, std::ios::binary | std::ios::out);
    if(!file_reader.is_open() || !file_writer.is_open()){
        return 1;
    }
    RANS rans{};
    char* mem_buff = new char[rans.BLOCK_SIZE];
    while(file_reader){
        // Read frequencies
        std::map<char, uint32_t> freqs = read_symbol_frequencies(file_reader);
        rans.init_frequencies(freqs);
        // Read number of bytes in block
        uint32_t bytes_num = read_size_of_block(file_reader);
        // Read next block
        file_reader.read(mem_buff, bytes_num);
        uint32_t bits_read = file_reader.gcount();
        // decode block
        std::string dec = rans.decode(mem_buff, bits_read);
        // save decoded block to file
        file_writer.write(dec.c_str(), static_cast<long>(dec.size()));
    }
    delete[] mem_buff;
    file_reader.close();
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

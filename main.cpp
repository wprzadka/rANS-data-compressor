//
// Created by viking on 17.04.22.
//

#include <string>
#include <fstream>
#include <getopt.h>
#include <cassert>
#include <numeric>
#include "rans.h"

static const uint8_t BLOCK_SIZE_BYTES = 2;
static const uint8_t SYMBOL_FREQ_BYTES = 3;

void write_symbol_freqencies(const std::array<uint32_t, RANS::MAX_SYMBOL>& freqs, std::ofstream& file){

    // Write number of symbols
    char* mem_buff = new char[SYMBOL_FREQ_BYTES];
    uint8_t non_zero_freqs_num = std::accumulate(
            freqs.begin(),
            freqs.end(),
            0,
            [](uint8_t acc, uint32_t elem){ return acc + (elem > 0 ? 1 : 0); }
            );
    mem_buff[0] = static_cast<char>(non_zero_freqs_num & 255);
    file.write(mem_buff, 1);

    // Write symbols - frequency pairs
    for (unsigned char symbol = 0; symbol < RANS::MAX_SYMBOL; ++symbol){
        if (freqs[symbol] == 0) {
            continue;
        }
        mem_buff[0] = static_cast<int>(symbol) - RANS::NEGATIVE_SYMBOLS_NUM;
        assert(freqs[symbol] < (1 << RANS::N_VALUE));
        for (int i = 1; i < SYMBOL_FREQ_BYTES; ++i) {
            mem_buff[i] = static_cast<char>((freqs[symbol] >> ((SYMBOL_FREQ_BYTES - 1 - i) << 3)) & 255);
        }
        file.write(mem_buff, SYMBOL_FREQ_BYTES);
    }
    delete[] mem_buff;
}

std::array<uint32_t, RANS::MAX_SYMBOL> read_symbol_frequencies(std::ifstream& file){
    char* mem_buff = new char[SYMBOL_FREQ_BYTES];
    std::array<uint32_t, RANS::MAX_SYMBOL> freqs{};

    file.read(mem_buff, 1);
    if(file.eof()){
        throw "EOF";
    }
    unsigned char symbols = static_cast<unsigned char>(mem_buff[0]);

    while(symbols > 0){
        file.read(mem_buff, SYMBOL_FREQ_BYTES);
        uint32_t freq = 0;
        for (int i = 1; i < SYMBOL_FREQ_BYTES; ++i){
            freq <<= 8;
            freq += static_cast<unsigned char>(mem_buff[i] & 255);
        }
        freqs[mem_buff[0] + RANS::NEGATIVE_SYMBOLS_NUM] = freq;
        --symbols;
    }

    delete[] mem_buff;
    return freqs;
}

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
    while(file_reader){
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
        std::array<uint32_t, RANS::MAX_SYMBOL> freqs{};
        try {
            // TODO Implement sane way of checking EOF
            freqs = read_symbol_frequencies(file_reader);
        }catch (...) {
            break;
        }
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

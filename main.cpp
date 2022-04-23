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
        // Prepare and write frequencies of symbol occurance
        rans.prepare_frequencies(mem_buff, bits_read);
        // encode block
        std::string enc = rans.encode(mem_buff, bits_read);
        // save block with frequencies to file
        write_symbol_freqencies(rans.frequencies, file_writer);
        file_writer.write(enc.c_str(), bits_read);
    }
    delete[] mem_buff;
    file_reader.close();
    file_writer.close();
    return 0;
}

int decode_file(const std::string& input_file){
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

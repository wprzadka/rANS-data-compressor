//
// Created by viking on 17.04.22.
//

#include <string>
#include <map>

std::map<char, uint32_t> get_frequencies(const std::string& word){
    std::map<char, uint32_t> freq{};

    for (auto v : word){
        if(freq.find(v) == freq.end()){
            freq[v] = 0;
        }
        ++freq[v];
    }

    return freq;
}

std::map<char, uint32_t> compute_cumulative_freq(const std::map<char, uint32_t>& freq){
    std::map<char, uint32_t> accumulated;
    uint32_t prev_sum = 0;
    for (const auto& iter : freq){
        accumulated[iter.first] = prev_sum;
        prev_sum += iter.second;
    }
    return accumulated;
}

char get_symbol(uint32_t value, std::map<char, uint32_t> accumulative_freqs){
    auto iter = accumulative_freqs.begin();
    char symbol = iter->first;
    while(iter->second <= value && iter != accumulative_freqs.end()){
        symbol = iter->first;
        ++iter;
    }
    return symbol;
}


int main(){

    std::string message = "AABAAACCABAAACCB";
    const uint8_t SYMBOLS = 4;
    const uint32_t MASK = (1 << SYMBOLS) - 1;

    auto frequencies = get_frequencies(message);
    auto accum = compute_cumulative_freq(frequencies);

//    for (auto & iter : frequencies){
//        printf("%c: %u\n", iter.first, iter.second);
//    }
//    for (auto & iter : accum){
//        printf("%c: %u\n", iter.first, iter.second);
//    }

    int64_t x = 0;
    for (auto s = message.rbegin(); s != message.rend(); ++s){
        uint32_t freq = frequencies[*s];
        x = ((x / freq) << SYMBOLS) + (x % freq) + accum[*s];
    }
    printf("encoded\n%lu\n", x);

    std::string dec;
    while(x > 0){
        char s = get_symbol(x & MASK, accum);
        dec += s;
        x = frequencies[s] * (x >> SYMBOLS) + (x & MASK) - accum[s];
    }
    printf("decoded\n%s\n", dec.c_str());
}

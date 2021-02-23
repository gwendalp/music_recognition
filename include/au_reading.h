#ifndef START_SVM_AU_READING_H
#define START_SVM_AU_READING_H


//32 bit word (unsigned) 	field 	Description/Content Hexadecimal numbers in C notation
//0 	                    magic number 	the value 0x2e736e64 (four ASCII characters ".snd")
//1 	                    data offset 	the offset to the data in bytes, must be divisible by 8. The minimum valid number is 24 (decimal), since this is the header length (six 32-bit words) with no space reserved for extra information (the annotation field). The minimum valid number with an annotation field present is 32 (decimal).
//2 	                    data size 	data size in bytes. If unknown, the value 0xffffffff should be used.
//3 	                    encoding 	Data encoding format:
//
//1 = 8-bit G.711 μ-law
//2 = 8-bit linear PCM
//3 = 16-bit linear PCM
//4 = 24-bit linear PCM
//5 = 32-bit linear PCM
//6 = 32-bit IEEE floating point
//7 = 64-bit IEEE floating point
//8 = Fragmented sample data
//9 = DSP program
//10 = 8-bit fixed point
//11 = 16-bit fixed point
//12 = 24-bit fixed point
//13 = 32-bit fixed point
//18 = 16-bit linear with emphasis
//19 = 16-bit linear compressed
//20 = 16-bit linear with emphasis and compression
//21 = Music kit DSP commands
//23 = 4-bit compressed using the ITU-T G.721 ADPCM voice data encoding scheme
//24 = ITU-T G.722 SB-ADPCM
//25 = ITU-T G.723 3-bit ADPCM
//26 = ITU-T G.723 5-bit ADPCM
//27 = 8-bit G.711 A-law
//
//4 	sample rate 	the number of samples/second, e.g., 8000
//5 	channels 	the number of interleaved channels, e.g., 1 for mono, 2 for stereo; more channels possible, but may not be supported by all readers.



#include <iostream>
#include <fstream>
#include <cstdio>
#include <cmath>

#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include "global.h"

namespace fs = std::filesystem;


struct AU_header {

    uint32_t magic_number;
    uint32_t data_offset;
    uint32_t data_size;
    uint32_t encoding;
    uint32_t sample_rate;
    uint32_t channels;

};

std::ostream &operator<<(std::ostream &s, const AU_header *h) {
    return s << "AU Header:\n"
             << std::hex << "\tMagic Number:\t 0x" << std::hex << h->magic_number << "\n"
             << std::dec << "\tData Offset:\t " << std::dec << h->data_offset << "\n"
             << std::dec << "\tData size:\t " << (double) (h->data_size) / (1.0 * (1 << 20)) << " MiB\n"
             << std::dec << "\tEncoding:\t " << h->encoding << "\n"
             << std::dec << "\tSample Rate:\t " << h->sample_rate << " sample/s \n"
             << std::dec << "\tChannels:\t " << h->channels
             << std::dec;
}

uint32_t read_w(std::ifstream &file, bool bigEndian=true) {
    uint32_t word = 0;
    uint8_t b;
    for (std::size_t i = 0; i < 4; i++) {
        file.read(reinterpret_cast<char *>(&b), sizeof(uint8_t));
        if (bigEndian)
            word = word | (b << ((3 - i) * 8));
        else
            word = word | (b << (i * 8));
    }
    return word;
}

void auFileDetails(const std::string fileName) {
    FILE *fin = fopen(fileName.c_str(), "rb");
    std::ifstream myFile(fileName);
    std::unique_ptr<AU_header> header = std::make_unique<AU_header>();

    header->magic_number = read_w(myFile);
    std::cout << "(0-3) Magic number:  " << std::hex << header->magic_number << std::endl;
    header->data_offset = read_w(myFile);
    std::cout << "(4-7) Data Offset:  " << std::dec << header->data_offset << std::endl;
    header->data_size = read_w(myFile);
    std::cout << "(8-11) Data size:  "
              << header->data_size
              << " B, "
              << header->data_size / (1.0 * (1 << 20))
              << " MiB"
              << std::endl;
    header->encoding = read_w(myFile);
    std::cout << "(12-15) Encoding:  " << header->encoding << std::endl;
    header->sample_rate = read_w(myFile);
    std::cout << "(16-19) Sample rate:  " << header->sample_rate << std::endl;
    header->channels = read_w(myFile);
    std::cout << "(20-23) Channels:  " << header->channels << std::endl;
    fclose(fin);
}

DataVector readAuFile(const std::string fileName) {
    FILE *fin = fopen(fileName.c_str(), "rb");
    std::ifstream myFile(fileName);
    DataVector data;
    std::unique_ptr<AU_header> header = std::make_unique<AU_header>();
    header->magic_number = read_w(myFile);
    //std::cout << "(0-3) Magic number:  " << std::hex << header->magic_number << std::endl;
    header->data_offset = read_w(myFile);
    //std::cout << "(4-7) Data Offset:  " << std::dec << header->data_offset << std::endl;
    header->data_size = read_w(myFile);
    //std::cout << "(8-11) Data size:  "
    //          << header->data_size
    //          << " B, "
    //          << header->data_size / (1.0 * (1 << 20))
    //          << " MiB"
    //          << std::endl;
    header->encoding = read_w(myFile);
    //std::cout << "(12-15) Encoding:  " << header->encoding << std::endl;
    header->sample_rate = read_w(myFile);
    //std::cout << "(16-19) Sample rate:  " << header->sample_rate << std::endl;
    header->channels = read_w(myFile);
    //std::cout << "(20-23) Channels:  " << header->channels << std::endl;
    myFile.seekg(header->data_offset, std::ios_base::beg);
    uint8_t lower_bits;
    uint8_t higher_bits;
    for (std::size_t k = 0; k < header->data_size/2; k++) {
        myFile.read(reinterpret_cast<char *>(&higher_bits), sizeof(uint8_t));
        myFile.read(reinterpret_cast<char *>(&lower_bits), sizeof(uint8_t));
        data.push_back(static_cast<real>((signed short) ((higher_bits << 8) + lower_bits)));
    }
    myFile.close();
    fclose(fin);
    return data;
}


#endif //START_SVM_AU_READING_H

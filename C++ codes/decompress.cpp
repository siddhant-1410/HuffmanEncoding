#include <iostream>
#include <vector>
#include <map>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

using namespace std;

struct Tree {
    int frequency;
    unsigned char character;
    Tree *left = nullptr;
    Tree *right = nullptr;
};

string toBinary(unsigned char a) {
    string output = "";
    while (a != 0) {
        string bit = a % 2 == 0 ? "0" : "1";
        output += bit;
        a /= 2;
    }
    if (output.size() < 8) {
        int deficit = 8 - output.size();
        for (int i = 0; i < deficit; i++) {
            output += "0";
        }
    }
    reverse(output.begin(), output.end());
    return output;
}

unsigned char *readFileIntoBuffer(const char *path, int &sz) {
    FILE *fp = fopen(path, "rb");
    if (fp == nullptr) {
        cerr << "Error opening file" << endl;
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    unsigned char *buffer = (unsigned char *) malloc(sz);
    fread(buffer, 1, sz, fp);
    fclose(fp);
    return buffer;
}

void writeFileFromBuffer(const char *path, unsigned char *buffer, int sz, int flag) {
    FILE *fp;
    if (flag == 0) {
        fp = fopen(path, "wb");
    } else {
        fp = fopen(path, "ab");
    }
    if (fp == nullptr) {
        cerr << "Error opening file" << endl;
        exit(1);
    }
    fwrite(buffer, 1, sz, fp);
    fclose(fp);
}

string getStringFromBuffer(unsigned char *buffer, int sz) {
    string bitstring = "";
    for (int i = 0; i < sz; i++) {
        bitstring += toBinary(buffer[i]);
    }
    return bitstring;
}

unsigned char *getDecodedBuffer(string bitstring, vector<unsigned char> &buffer, map<unsigned char, string> codes,
                                int &sz, int paddedBits) {
    string bit = "";
    map<string, unsigned char> reversecodes;
    for (map<unsigned char, string>::iterator i = codes.begin(); i != codes.end(); i++) {
        reversecodes[i->second] = i->first;
    }
    for (int i = 0; i < bitstring.size() - paddedBits; i++) {
        bit += string(1, bitstring[i]);
        if (reversecodes.find(bit) != reversecodes.end()) {
            buffer.push_back(reversecodes[bit]);
            bit = "";
        }
    }
    sz = buffer.size();
    return buffer.data();
}

unsigned char *readHeader(unsigned char *buffer, map<unsigned char, string> &codes, int &paddedBits, int &sz) {
    paddedBits = *((int *) buffer);
    buffer = buffer + 4;
    sz -= 4;
    int size = *((int *) buffer);
    buffer = buffer + 4;
    sz -= 4;
    for (int i = 0; i < size; i++) {
        unsigned char key = buffer[0];
        buffer++;
        sz--;
        int len = *((int *) buffer);
        buffer += 4;
        sz -= 4;
        char *value = (char *) malloc(len + 1);
        for (int j = 0; j < len; j++) {
            value[j] = buffer[j];
        }
        buffer += len;
        sz -= len;
        value[len] = '\0';
        codes[key] = value;
    }
    return buffer;
}

void decompressFile(const char *inputPath, const char *outputPath) {
    int sz = 0;
    map<unsigned char, string> codes;
    int paddedBits = 0;
    unsigned char *fileBuffer = readFileIntoBuffer(inputPath, sz);
    fileBuffer = readHeader(fileBuffer, codes, paddedBits, sz);

    string fileBitString = getStringFromBuffer(fileBuffer, sz);

    vector<unsigned char> outputBufferV;
    unsigned char *outputBuffer;
    getDecodedBuffer(fileBitString, outputBufferV, codes, sz, paddedBits);
    outputBuffer = outputBufferV.data();

    writeFileFromBuffer(outputPath, outputBuffer, sz, 0);
}

int main() {
    const char *compressedFile = "test1.txt";
    const char *decompressedFile = "decoded.txt";
    
    decompressFile(compressedFile, decompressedFile);
    
    cout << "Decompression completed successfully!" << endl;
    return 0;
}

#include <iostream>
#include <vector>
#include <queue>
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

class TreeComparator {
public:
    bool operator()(Tree *a, Tree *b) {
        return a->frequency > b->frequency;
    }
};

Tree *buildHuffmanTree(vector<pair<unsigned char, int>> freqtable) {
    priority_queue<Tree *, vector<Tree *>, TreeComparator> huffqueue;
    for (int i = 0; i < freqtable.size(); i++) {
        Tree *node = new Tree();
        node->frequency = freqtable[i].second;
        node->character = freqtable[i].first;
        huffqueue.push(node);
    }
    while (huffqueue.size() > 1) {
        Tree *a, *b;
        a = huffqueue.top();
        huffqueue.pop();
        b = huffqueue.top();
        huffqueue.pop();
        Tree *c = new Tree();
        c->frequency = a->frequency + b->frequency;
        c->left = a;
        c->right = b;
        huffqueue.push(c);
    }
    return huffqueue.top();
}

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

void traverseHuffmanTree(Tree *root, string prev, string toAppend, map<unsigned char, string> &codemap) {
    prev += toAppend;
    if (root->right == nullptr && root->left == nullptr) {
        codemap[root->character] = prev;
    }
    if (root->right != nullptr) {
        traverseHuffmanTree(root->right, prev, "1", codemap);
    }
    if (root->left != nullptr) {
        traverseHuffmanTree(root->left, prev, "0", codemap);
    }
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

vector<pair<unsigned char, int>> convertToVector(map<unsigned char, int> codes) {
    vector<pair<unsigned char, int>> codesV;
    for (map<unsigned char, int>::iterator i = codes.begin(); i != codes.end(); i++) {
        codesV.push_back(make_pair(i->first, i->second));
    }
    return codesV;
}

string getHuffmanBitstring(unsigned char *buffer, map<unsigned char, string> codes, int sz, int &paddedBits) {
    string outputBuffer = "";
    for (int i = 0; i < sz; i++) {
        outputBuffer = outputBuffer + codes[buffer[i]];
    }
    if (outputBuffer.size() % 8 != 0) {
        int deficit = 8 * ((outputBuffer.size() / 8) + 1) - outputBuffer.size();
        paddedBits = deficit;
        for (int i = 0; i < deficit; i++) {
            outputBuffer += "0";
        }
    }
    return outputBuffer;
}

unsigned char *getBufferFromString(string bitstring, vector<unsigned char> &outputBuffer, int &sz) {
    int interval = 0;
    unsigned char bit = 0;
    for (int i = 0; i < sz; i++) {
        bit = (bit << 1) | (bitstring[i] - '0');
        interval++;
        if (interval == 8) {
            interval = 0;
            outputBuffer.push_back(bit);
            bit = 0;
        }
    }
    sz = outputBuffer.size();
    return outputBuffer.data();
}

void writeHeader(const char *path, map<unsigned char, string> codes, int paddedBits) {
    int size = codes.size();
    writeFileFromBuffer(path, (unsigned char *) &paddedBits, sizeof(int), 0);
    writeFileFromBuffer(path, (unsigned char *) &size, sizeof(int), 1);
    for (map<unsigned char, string>::iterator i = codes.begin(); i != codes.end(); i++) {
        unsigned char key = i->first;
        int len = i->second.size();
        writeFileFromBuffer(path, (unsigned char *) &key, 1, 1);
        writeFileFromBuffer(path, (unsigned char *) &len, sizeof(int), 1);
        writeFileFromBuffer(path, (unsigned char *) i->second.c_str(), len, 1);
    }
}

void compressFile(const char *path, const char *output_path, map<unsigned char, string> &codes) {
    int sz = 0;
    int paddedBits = 0;
    map<unsigned char, int> freqtable;
    unsigned char *buffer = readFileIntoBuffer(path, sz);

    for (int i = 0; i < sz; i++) {
        freqtable[buffer[i]]++;
    }

    Tree *root = buildHuffmanTree(convertToVector(freqtable));
    traverseHuffmanTree(root, "", "", codes);

    string outputString = getHuffmanBitstring(buffer, codes, sz, paddedBits);
    sz = outputString.size();

    vector<unsigned char> outputBufferV;
    getBufferFromString(outputString, outputBufferV, sz);
    unsigned char *outputBuffer = outputBufferV.data();

    writeHeader(output_path, codes, paddedBits);
    writeFileFromBuffer(output_path, outputBuffer, sz, 1);
}

int main() {
    const char *inputFile = "file.txt";
    const char *compressedFile = "test1.txt";
    
    map<unsigned char, string> codes;
    compressFile(inputFile, compressedFile, codes);
    
    cout << "Compression completed successfully!" << endl;
    return 0;
}
    
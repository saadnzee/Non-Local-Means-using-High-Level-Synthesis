#include "nlm.h"
#include <cstdio>
#include <fstream>
#include <string>
#include <bitset>

int main() {
    std::string infile = "C:\\Users\\Saad.DESKTOP-05D0E3M\\AppData\\Roaming\\Xilinx\\Vivado\\nlmHLS\\Image6.txt";
    std::string outfile = "C:\\Users\\Saad.DESKTOP-05D0E3M\\AppData\\Roaming\\Xilinx\\Vivado\\nlmHLS\\Image6_nlm.txt";
    std::ifstream fin(infile);
    if (!fin) {
        std::printf("Error: cannot open %s", infile);
        return -1;
    }

    // Input and Output Buffers
    unsigned char in_buf[ROWS * COLS];
    unsigned char out_buf[ROWS * COLS];

    int idx = 0;
    std::string line;
    while (idx < ROWS * COLS && std::getline(fin, line)) {
        int val = std::stoi(line, nullptr, 2);
        if (val < 0) val = 0;
        if (val > 255) val = 255;
        in_buf[idx++] = static_cast<unsigned char>(val);
    }
    fin.close();
    if (idx != ROWS * COLS) {
        std::printf("Error: expected %d pixels, got %d", ROWS*COLS, idx);
        return -1;
    }

    nlm_core(in_buf,  out_buf);

    std::ofstream fout(outfile);
    if (!fout) {
        std::printf("Error: cannot open %s for writing", outfile);
        return -1;
    }

    // displaying! this is just for debugging
    std::printf("pixels after nlm in out_buf:\n");
    for (int i = 0; i < ROWS * COLS; i++) {
        std::bitset<8> bits(out_buf[i]);
        std::printf("%d: %s\n", i, bits.to_string().c_str());
        fout << bits.to_string() << "\n";
    }
    fout.close();

    std::printf("NLM complete!");
    return 0;
}



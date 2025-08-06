#include <iostream>
#include <string>
#include <unordered_map>
#include <functional>
#include <sstream>
#include <cctype>
#include <fstream>
#include <filesystem>
#ifdef _WIN32
#include <windows.h>
#endif

std::string getArchitecture() {
#ifdef _WIN32
    SYSTEM_INFO si;
    GetNativeSystemInfo(&si);
    switch (si.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_AMD64: return "64 bit (AMD64)";
    case PROCESSOR_ARCHITECTURE_INTEL: return "32 bit (x86)";
    case PROCESSOR_ARCHITECTURE_ARM64: return "ARM64";
    default: return "Unknown architecture";
    }
#else
    return sizeof(void*) == 8 ? "64 bit" : "32 bit";
#endif
}

std::string getPlatform() {
#ifdef _WIN32
    return "windows";
#elif __linux__
    return "linux";
#else
    return "unknown";
#endif
}

#include "lexer.hpp"

constexpr auto VERSION = "0.0.1-alpha";
constexpr auto VERSION_CODE = "xxxxxx";
std::string formatBuildDateTime() {
    std::istringstream iss(__DATE__);
    std::string month, day, year;
    iss >> month >> day >> year;
    if (day.size() == 2 && day[0] == ' ') day = day.substr(1);
    return month + " " + day + " " + year + ", " + __TIME__;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Base" << " " << VERSION << " " << "(tags/" << VERSION << ":" 
            << VERSION_CODE << "," << " " << formatBuildDateTime() << ")" << " "
            << "[MSC v.1943" << " " << getArchitecture() << "]" << " " << "on" << " " << getPlatform() << std::endl;
        std::cerr << "Usage: base <filename> [--v | --version]" << std::endl;
        return 1;
    }
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--v" || arg == "--version") {
            std::cout << "Base lang " << VERSION << std::endl;
            return 0;
        }
    }
    std::string filename = argv[1];
    std::string ext = std::filesystem::path(filename).extension().string();
    if (ext != ".a" && ext != ".b") {
        std::cerr << "Unsupported file extension: " << ext << std::endl;
        return 1;
    }
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "File not found: " << filename << std::endl;
        return 1;
    }
    std::string code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    Lexer lexer(code);
    Token tok;
    do {
        tok = lexer.nextToken();
        std::cout << "Line " << tok.line << ": ";
        std::cout << static_cast<int>(tok.type) << " -> " << tok.value << "\n";
    } while (tok.type != TokenTypeEnum::EndOfFile);
    return 0;
}

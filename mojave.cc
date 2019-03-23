#include <iostream>
#include <tuple>
#include <cstdio>
#include <string>
#include <cstring>
#include <fstream>
#include <chrono>
#include "StemBundle.hpp"
#include "mojaveStorage.hpp"
#include "args/args.hpp"

struct mojave: args::group<mojave>
{
    static const char* help() {
        return ":mojave-cli to manage the inverted index";
    }
};

struct create: mojave::command<create>
{
    std::string input;
    std::string output;
    std::string swl;
    create(): swl(std::string("")), output(std::string("")){}
    static const char* help() {
        return ":create a mojave index";
    }

    template<class F>
    void parse(F f)
    {
        f(input, "--input", "-I", args::help("Input file of the index."), args::required());
        f(output, "--output", "-O", args::help("Output file of the index."));
        f(swl, "--stopword", "-S", args::help("Stopword file of the index."));
    }

    void run() {
        using namespace std::chrono;
        milliseconds ms0 = duration_cast< milliseconds >(
                system_clock::now().time_since_epoch()
        );
        std::ifstream file(input.c_str());
        std::string str, file_contents, str0;
        while (std::getline(file, str)) {
            file_contents += str;
            file_contents.push_back('\n');
        }
        StemBundle bundle;
        if(swl.length() != 0) {
            std::ifstream filestop("Eng.swl");
            while (std::getline(filestop, str0)) {
                bundle.AddStop(str0.c_str(), str0.length());
            }
        }
        bundle.Stem(file_contents.c_str());
        if(output.length() == 0){
            mojaveStorage store;
            for(auto i = bundle.begin(); !i.end(); i++) {
                uint32_t count;
                uint64_t hval;
                char *str;
                WordIndexList *list;
                std::tie(hval, str, count, list) = i.get();
                printf("%llx | %20s | %8d | %8d\n", hval, str, count, strlen(str));
                store.insert(hval, *list);
            }
        } else {
            mojaveStorage store(output.c_str());
            for(auto i = bundle.begin(); !i.end(); i++) {
                uint32_t count;
                uint64_t hval;
                char *str;
                WordIndexList *list;
                std::tie(hval, str, count, list) = i.get();
                printf("%llx | %20s | %8d | %8d\n", hval, str, count, strlen(str));
                store.insert(hval, *list);
            }
        }
        milliseconds ms1 = duration_cast< milliseconds >(
                system_clock::now().time_since_epoch()
        );
        printf("THE INDEX SET UP IN %llu Ms\n", ms1.count() - ms0.count());
    }
};

struct find: mojave::command<find>
{
    std::string filename;
    std::string str;
    find() {}
    static const char* help() {
        return ":find index from a mojave index";
    }

    template<class F>
    void parse(F f)
    {
        f(filename, "--input", "-I", args::help("Input file of the index."), args::required());
        f(str, "--str", "-s", args::help("string need to find"), args::required());
    }

    void run() {
        using namespace std::chrono;
        milliseconds ms0 = duration_cast< milliseconds >(
                system_clock::now().time_since_epoch()
        );
        mojaveStorage store(1);
        store.sync(filename.c_str(), SYNC_TO_MEM);
        bool finded;
        IndexList *list1;
        std::tie(finded, list1) = store.find(str.c_str(), str.length());
        for(auto i: *list1) {
            uint32_t doc = i >> 32;
            uint32_t pos = i & 0x00000000FFFFFFFF;
            printf("|%4d %4d|\n", doc, pos);
        }
        milliseconds ms1 = duration_cast< milliseconds >(
                system_clock::now().time_since_epoch()
        );
        printf("WORD SEARCH IN %llu Ms\n", ms1.count() - ms0.count());
    }
};

struct watch: mojave::command<watch>
{
    std::string filename;
    std::string input;
    watch() : input(std::string("")){}
    static const char* help() {
        return ":persistent search from a mojave index";
    }

    template<class F>
    void parse(F f)
    {
        f(filename, "--input", "-I", args::help("Input file of the index."), args::required());
        f(input, "--flle", "-F", args::help("Input standard input file for searching"));
    }

    void run() {
        using namespace std::chrono;
        uint32_t count = 0;
        std::string str;
        std::istream *inStream = &std::cin;
        if(input.length() != 0)
            inStream = new std::ifstream(input);

        milliseconds ms0 = duration_cast< milliseconds >(
                system_clock::now().time_since_epoch()
        );
        mojaveStorage store(1);
        store.sync(filename.c_str(), SYNC_TO_MEM);
        milliseconds ms1 = duration_cast< milliseconds >(
                system_clock::now().time_since_epoch()
        );
        printf("INDEX SYNCED IN %llu Ms\n", ms1.count() - ms0.count());

        milliseconds ms2 = duration_cast< milliseconds >(
                system_clock::now().time_since_epoch()
        );
        while(std::getline(*inStream, str)) {
            if(str == "\\q")break;
            count++;
            milliseconds msx = duration_cast< milliseconds >(
                    system_clock::now().time_since_epoch()
            );
            bool finded;
            IndexList *list1;
            std::tie(finded, list1) = store.find(str.c_str(), str.length());
            for (auto i: *list1) {
                uint32_t doc = i >> 32;
                uint32_t pos = i & 0x00000000FFFFFFFF;
                printf("|%4d %4d|\n", doc, pos);
            }
            milliseconds msy = duration_cast< milliseconds >(
                    system_clock::now().time_since_epoch()
            );
            printf("QUERY %09d FINISHED IN %llu Ms\n", count, msy.count() - msx.count());
        }
        milliseconds ms3 = duration_cast< milliseconds >(
                system_clock::now().time_since_epoch()
        );
        printf("ALL QUERY FINISHED IN %llu Ms\n", ms3.count() - ms2.count());
    }
};


int main(int argc, char const *argv[]) {
    args::parse<mojave>(argc, argv);
    return 0;
}
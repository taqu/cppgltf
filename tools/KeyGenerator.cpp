#include <utility>
#include <cassert>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;

typedef char Char;

u32 hash_FNV1a(const u8* v, u32 count)
{
    u32 hash = 2166136261U;

    for(u32 i=0; i<count; ++i){
        hash ^= v[i];
        hash *= 16777619U;
    }
    return hash;
}


int main(int argc, char** argv)
{
    if(argc<=2){
        return 0;
    }
    std::ifstream file(argv[1], std::ios::binary);
    if(!file.is_open()){
        return 0;
    }
    std::vector<std::string> words;
    std::string line;
    while(!file.eof()){
        if(std::getline(file, line)){
            while(0<line.size()){
                if('\n' == line[line.size()-1] || '\r' == line[line.size()-1]){
                    line.pop_back();
                }else{
                    break;
                }
            }
            if(line.size()<=0){
                continue;
            }
            if(2<=line.size() &&  line[0]=='/' && line[1]=='/'){
                continue;
            }
            for(std::size_t j=0; j<line.size(); ++j){
                line[j] = toupper(line[j]);
            }
            bool found=false;
            for(std::size_t j=0; j<words.size(); ++j){
                if(words[j] == line){
                    found = true;
                    break;
                }
            }
            if(found){
                continue;
            }
            words.push_back(line);
        }
    }
    file.close();

    std::ofstream outfile;

    outfile.open(argv[2], std::ios::binary);
    if(!outfile.is_open()){
        return 0;
    }
    for(std::size_t i=0; i<words.size(); ++i){
        outfile << "    static const s32 GLTF_" << words[i].c_str() << " = " << i << ";" << std::endl;
    }
    outfile.close();

    outfile.open(argv[3], std::ios::binary);
    if(!outfile.is_open()){
        return 0;
    }

    outfile << "namespace {" << std::endl;
    outfile << "    struct Word{ u32 hash_; const char* str_;};" << std::endl;
    outfile << "    static const s32 NumWords = " << words.size() << ";" << std::endl;
    outfile << "    const Word words[] =" << std::endl;
    outfile << "    {" << std::endl;
    for(std::size_t i=0; i<words.size(); ++i){
        u32 hash = hash_FNV1a(reinterpret_cast<const u8*>(words[i].c_str()), words[i].size());
        outfile << "        {" << hash << "U,\"" << words[i] << "\"}," << std::endl;
    }
    outfile << "    };" << std::endl;
    outfile << "}" << std::endl;
    outfile.close();
    return 0;
}

#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <Windows.h>
#include "cppgltf.h"

struct Test
{
	enum class Type
    {
		Accept,
		Reject,
		Undefined,
    };

	Type type_;
	std::string path_;
};

bool start_with(const std::string& filename, char c)
{
	if(filename.length()<=0){
		return false;
	}
	return c == filename[0];
}

void gather(std::vector<Test>& result, const char* directory)
{
    std::string path = directory;
	path += "*.json";
	WIN32_FIND_DATA findFileData;
   HANDLE handle = FindFirstFile(path.c_str(), &findFileData);
   if(nullptr == handle){
	   return;
   }
   for(;;){
	   if(FALSE == FindNextFileA(handle, &findFileData)){
		   break;
	   }
	   Test test;
	   test.path_ = findFileData.cFileName;
	   if(start_with(test.path_, 'i')){
		   test.type_ = Test::Type::Undefined;
	   }else if(start_with(test.path_, 'y')){
		   test.type_ = Test::Type::Accept;
	   }else if(start_with(test.path_, 'n')){
		   test.type_ = Test::Type::Reject;
	   }
	   result.push_back(test);
   }
   FindClose(handle);
}

struct Data
{
	size_t size_;
	void* data_;
};
Data read(std::ifstream& file)
{
	Data data;
	file.seekg(0, std::ios::end);
	data.size_ = file.tellg();
	file.seekg(0, std::ios::beg);
	data.data_ = ::malloc(data.size_);
	file.read(reinterpret_cast<char*>(data.data_), data.size_);
	return data;
}

void test(const std::vector<Test>& tests, const char* directory)
{
	cppgltf::JsonParser parser;
	std::ifstream file;
	std::string path;
	size_t skip = 227;
	for(size_t i=0; i<tests.size(); ++i){
		if(i<skip){
			continue;
		}
		const Test& test = tests[i];
		path = directory;
		path += test.path_;
		file.open(path, std::ios::binary);
		if(!file.is_open()){
			continue;
		}
		Data data = read(file);
		file.close();
		char* begin = reinterpret_cast<char*>(data.data_);
		bool result = parser.parse(begin, begin+data.size_);

		switch(test.type_){
        case Test::Type::Accept:
			if(!result){
				assert(false);
			}
			break;
        case Test::Type::Reject:
			if(result){
				assert(false);
			}
			break;
		case Test::Type::Undefined:
			if(result){
				std::cout << "accept" << std::endl;
			}else{
				std::cout<< "reject" << std::endl;
			}
			break;
		}
		::free(data.data_);
	}
}

int main(void)
{
	std::vector<Test> tests;
	std::string directory = "../../test/JSONTestSuite/test_parsing/";
	gather(tests, directory.c_str());
	test(tests, directory.c_str());
	return 0;
}
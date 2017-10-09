#include "config.h"

TEST_CASE("sample Box can be loaded", "[Box]"){
    static const char* normalDir = DATA_ROOT"Box/glTF/";
    static const char* normal = DATA_ROOT"Box/glTF/Box.gltf";
    static const char* binaryDir = DATA_ROOT"Box/glTF-Binary/";
    static const char* binary = DATA_ROOT"Box/glTF-Binary/Box.glb";

    SECTION("load normal"){
        FILE* file = gltf::fopen_s(normal, "rb");
        if(NULL == file){
            return;
        }

        gltf::IFStream ifstream = std::move(gltf::IFStream(file));
        gltf::glTFHandler gltfHandler(normalDir);
        gltf::JSONReader gltfJsonReader(ifstream, gltfHandler);
        bool result = gltfJsonReader.read();
        REQUIRE(result);
        fclose(file);

        bool checkRequirements = gltfHandler.get().checkRequirements();
        REQUIRE(checkRequirements);

        gltf::glTF& gltf = gltfHandler.get();
        REQUIRE(("COLLADA2GLTF" == gltf.asset_.generator_));

#ifdef TEST_OUT
        {
            FILE* outfile = gltf::fopen_s("..\\test_Box_normal.gltf", "wb");
            if(NULL == outfile){
                return;
            }
            gltf.setDirectory("../");
            gltf::OFStream ofstream(outfile);
            gltf::glTFWriter writer(ofstream);
            writer.write(gltf, gltf::GLTF_FILE_AsIs, gltf::glTFWriter::Flag_Format);
            fclose(outfile);
        }
#endif
    }

    SECTION("load binary"){
        FILE* file = gltf::fopen_s(binary, "rb");
        if(NULL == file){
            return;
        }

        gltf::IFStream ifstream = std::move(gltf::IFStream(file));
        gltf::GLBEventHandler glbHandler;
        gltf::GLBReader glbReader(ifstream, glbHandler);
        bool result = glbReader.read();
        REQUIRE(result);
        fclose(file);

        bool checkRequirements = glbHandler.get().checkRequirements();
        REQUIRE(checkRequirements);
    }
}

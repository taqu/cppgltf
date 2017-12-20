#include "config.h"

TEST_CASE("sample Box can be loaded", "[Box]"){
    static const char* textDir = DATA_ROOT"Box/glTF/";
    static const char* text = DATA_ROOT"Box/glTF/Box.gltf";
    static const char* binaryDir = DATA_ROOT"Box/glTF-Binary/";
    static const char* binary = DATA_ROOT"Box/glTF-Binary/Box.glb";

    SECTION("load text"){
        cppgltf::IFStream ifstream;
        if(!ifstream.open(text)){
            return;
        }
        cppgltf::glTFHandler gltfHandler(textDir);
        cppgltf::JSONReader gltfJsonReader(ifstream, gltfHandler);
        bool result = gltfJsonReader.read();
        REQUIRE(result);
        ifstream.close();

        bool checkRequirements = gltfHandler.get().checkRequirements();
        REQUIRE(checkRequirements);

        cppgltf::glTF& gltf = gltfHandler.get();

        //asset
        REQUIRE(("COLLADA2GLTF" == gltf.asset_.generator_));
        REQUIRE(("2.0" == gltf.asset_.version_));

        //scene
        REQUIRE((0 == gltf.scene_));

        //scenes
        REQUIRE((1 == gltf.scenes_.size()));
        REQUIRE((1 == gltf.scenes_[0].nodes_.size()));
        REQUIRE((0 == gltf.scenes_[0].nodes_[0]));

        //nodes
        REQUIRE((2 == gltf.nodes_.size()));
        REQUIRE((1 == gltf.nodes_[0].children_.size()));
        REQUIRE((1 == gltf.nodes_[0].children_[0]));
        REQUIRE(Approx(1.0f) == gltf.nodes_[0].matrix_[0]);
        REQUIRE(Approx(0.0f) == gltf.nodes_[0].matrix_[1]);
        REQUIRE(Approx(0.0f) == gltf.nodes_[0].matrix_[2]);
        REQUIRE(Approx(0.0f) == gltf.nodes_[0].matrix_[3]);
        REQUIRE(Approx(0.0f) == gltf.nodes_[0].matrix_[4]);
        REQUIRE(Approx(0.0f) == gltf.nodes_[0].matrix_[5]);
        REQUIRE(Approx(-1.0f) == gltf.nodes_[0].matrix_[6]);
        REQUIRE(Approx(0.0f) == gltf.nodes_[0].matrix_[7]);
        REQUIRE(Approx(0.0f) == gltf.nodes_[0].matrix_[8]);
        REQUIRE(Approx(1.0f) == gltf.nodes_[0].matrix_[9]);
        REQUIRE(Approx(0.0f) == gltf.nodes_[0].matrix_[10]);
        REQUIRE(Approx(0.0f) == gltf.nodes_[0].matrix_[11]);
        REQUIRE(Approx(0.0f) == gltf.nodes_[0].matrix_[12]);
        REQUIRE(Approx(0.0f) == gltf.nodes_[0].matrix_[13]);
        REQUIRE(Approx(0.0f) == gltf.nodes_[0].matrix_[14]);
        REQUIRE(Approx(1.0f) == gltf.nodes_[0].matrix_[15]);
        REQUIRE((0 == gltf.nodes_[1].mesh_));

        //meshes
        REQUIRE((1 == gltf.meshes_.size()));
        REQUIRE(("Mesh" == gltf.meshes_[0].name_));
        REQUIRE((1 == gltf.meshes_[0].primitives_.size()));
        REQUIRE((2 == gltf.meshes_[0].primitives_[0].attributes_.size()));
        REQUIRE((cppgltf::GLTF_ATTRIBUTE_NORMAL == gltf.meshes_[0].primitives_[0].attributes_[0].semantic_));
        REQUIRE((1 == gltf.meshes_[0].primitives_[0].attributes_[0].index_));
        REQUIRE((cppgltf::GLTF_ATTRIBUTE_POSITION == gltf.meshes_[0].primitives_[0].attributes_[1].semantic_));
        REQUIRE((2 == gltf.meshes_[0].primitives_[0].attributes_[1].index_));
        REQUIRE((0 == gltf.meshes_[0].primitives_[0].indices_));
        REQUIRE((cppgltf::GLTF_PRIMITIVE_TRIANGLES == gltf.meshes_[0].primitives_[0].mode_));
        REQUIRE((0 == gltf.meshes_[0].primitives_[0].material_));

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
        cppgltf::IFStream ifstream;
        if(!ifstream.open(binary)){
            return;
        }
        cppgltf::GLBEventHandler glbHandler;
        cppgltf::GLBReader glbReader(ifstream, glbHandler);
        bool result = glbReader.read();
        REQUIRE(result);
        ifstream.close();

        bool checkRequirements = glbHandler.get().checkRequirements();
        REQUIRE(checkRequirements);
    }
}

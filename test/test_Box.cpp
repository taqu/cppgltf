#include "config.h"

void common_check_Box(const cppgltf::glTF& gltf)
{
    bool checkRequirements = gltf.checkRequirements();
    REQUIRE(checkRequirements);

    //asset
    REQUIRE("COLLADA2GLTF" == gltf.asset_.generator_);
    REQUIRE("2.0" == gltf.asset_.version_);

    //scene
    REQUIRE(0 == gltf.scene_);

    {//scenes
        REQUIRE(1 == gltf.scenes_.size());
        REQUIRE(1 == gltf.scenes_[0].nodes_.size());
        REQUIRE(0 == gltf.scenes_[0].nodes_[0]);
    }

    {//nodes
        REQUIRE(2 == gltf.nodes_.size());
        REQUIRE(1 == gltf.nodes_[0].children_.size());
        REQUIRE(1 == gltf.nodes_[0].children_[0]);
        REQUIRE(gltf.nodes_[0].flags_.check(cppgltf::Node::Flag_Matrix));
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
    }

    {//meshes
        REQUIRE(1 == gltf.meshes_.size());
        REQUIRE("Mesh" == gltf.meshes_[0].name_);
        REQUIRE(1 == gltf.meshes_[0].primitives_.size());
        REQUIRE(2 == gltf.meshes_[0].primitives_[0].attributes_.size());
        REQUIRE(cppgltf::GLTF_ATTRIBUTE_NORMAL == gltf.meshes_[0].primitives_[0].attributes_[0].semanticType_);
        REQUIRE(1 == gltf.meshes_[0].primitives_[0].attributes_[0].accessor_);
        REQUIRE(0 == gltf.meshes_[0].primitives_[0].attributes_[0].semanticIndex_);
        REQUIRE(cppgltf::GLTF_ATTRIBUTE_POSITION == gltf.meshes_[0].primitives_[0].attributes_[1].semanticType_);
        REQUIRE(2 == gltf.meshes_[0].primitives_[0].attributes_[1].accessor_);
        REQUIRE(0 == gltf.meshes_[0].primitives_[0].attributes_[1].semanticIndex_);
        REQUIRE(0 == gltf.meshes_[0].primitives_[0].indices_);
        REQUIRE(cppgltf::GLTF_PRIMITIVE_TRIANGLES == gltf.meshes_[0].primitives_[0].mode_);
        REQUIRE(0 == gltf.meshes_[0].primitives_[0].material_);
        REQUIRE(0 == gltf.meshes_[0].primitives_[0].indices_);
    }

    {//accessors
        REQUIRE(3 == gltf.accessors_.size());

        REQUIRE(0 == gltf.accessors_[0].bufferView_);
        REQUIRE(0 == gltf.accessors_[0].byteOffset_);
        REQUIRE(cppgltf::GLTF_TYPE_UNSIGNED_SHORT == gltf.accessors_[0].componentType_);
        REQUIRE(36 == gltf.accessors_[0].count_);
        REQUIRE(0 == gltf.accessors_[0].min_[0].fvalue_);
        REQUIRE(23 == gltf.accessors_[0].max_[0].cast<cppgltf::s32>());
        REQUIRE(cppgltf::GLTF_TYPE_SCALAR == gltf.accessors_[0].type_);

        REQUIRE(1 == gltf.accessors_[1].bufferView_);
        REQUIRE(0 == gltf.accessors_[1].byteOffset_);
        REQUIRE(cppgltf::GLTF_TYPE_FLOAT == gltf.accessors_[1].componentType_);
        REQUIRE(24 == gltf.accessors_[1].count_);
        REQUIRE(Approx(-1.0f) == gltf.accessors_[1].min_[0].fvalue_);
        REQUIRE(Approx(-1.0f) == gltf.accessors_[1].min_[1].fvalue_);
        REQUIRE(Approx(-1.0f) == gltf.accessors_[1].min_[2].fvalue_);
        REQUIRE(Approx(1.0f) == gltf.accessors_[1].max_[0].fvalue_);
        REQUIRE(Approx(1.0f) == gltf.accessors_[1].max_[1].fvalue_);
        REQUIRE(Approx(1.0f) == gltf.accessors_[1].max_[2].fvalue_);
        REQUIRE(cppgltf::GLTF_TYPE_VEC3 == gltf.accessors_[1].type_);

        REQUIRE(1 == gltf.accessors_[2].bufferView_);
        REQUIRE(288 == gltf.accessors_[2].byteOffset_);
        REQUIRE(cppgltf::GLTF_TYPE_FLOAT == gltf.accessors_[2].componentType_);
        REQUIRE(24 == gltf.accessors_[2].count_);
        REQUIRE(Approx(-0.5f) == gltf.accessors_[2].min_[0].fvalue_);
        REQUIRE(Approx(-0.5f) == gltf.accessors_[2].min_[1].fvalue_);
        REQUIRE(Approx(-0.5f) == gltf.accessors_[2].min_[2].fvalue_);
        REQUIRE(Approx(0.5f) == gltf.accessors_[2].max_[0].fvalue_);
        REQUIRE(Approx(0.5f) == gltf.accessors_[2].max_[1].fvalue_);
        REQUIRE(Approx(0.5f) == gltf.accessors_[2].max_[2].fvalue_);
        REQUIRE(cppgltf::GLTF_TYPE_VEC3 == gltf.accessors_[2].type_);
    }

    {//materials
        REQUIRE(1 == gltf.materials_.size());

        REQUIRE(Approx(0.800000011920929f) == gltf.materials_[0].pbrMetallicRoughness_.baseColorFactor_[0]);
        REQUIRE(Approx(0.0f) == gltf.materials_[0].pbrMetallicRoughness_.baseColorFactor_[1]);
        REQUIRE(Approx(0.0f) == gltf.materials_[0].pbrMetallicRoughness_.baseColorFactor_[2]);
        REQUIRE(Approx(1.0f) == gltf.materials_[0].pbrMetallicRoughness_.baseColorFactor_[3]);
        REQUIRE(Approx(0.0f) == gltf.materials_[0].pbrMetallicRoughness_.metallicFactor_);
        REQUIRE("Red" == gltf.materials_[0].name_);
    }

    {//bufferViews
        REQUIRE(2 == gltf.bufferViews_.size());

        REQUIRE(0 == gltf.bufferViews_[0].buffer_);
        REQUIRE(576 == gltf.bufferViews_[0].byteOffset_);
        REQUIRE(72 == gltf.bufferViews_[0].byteLength_);
        REQUIRE(34963 == gltf.bufferViews_[0].target_);

        REQUIRE(0 == gltf.bufferViews_[1].buffer_);
        REQUIRE(0 == gltf.bufferViews_[1].byteOffset_);
        REQUIRE(576 == gltf.bufferViews_[1].byteLength_);
        REQUIRE(12 == gltf.bufferViews_[1].byteStride_);
        REQUIRE(34962 == gltf.bufferViews_[1].target_);
    }
    //buffer
    REQUIRE(NULL != gltf.buffers_[0].data_);
}

TEST_CASE("A sample Box can be loaded", "[Box]"){
    static const char* textDir = DATA_ROOT"Box/glTF/";
    static const char* text = DATA_ROOT"Box/glTF/Box.gltf";
    static const char* binary = DATA_ROOT"Box/glTF-Binary/Box.glb";
    static const char* embedded = DATA_ROOT"Box/glTF-Embedded/Box.gltf";

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

        cppgltf::glTF& gltf = gltfHandler.get();
        common_check_Box(gltf);
        //buffer
        REQUIRE(648 == gltf.buffers_[0].byteLength_);
        REQUIRE("Box0.bin" == gltf.buffers_[0].uri_);

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

        cppgltf::glTF& gltf = glbHandler.get();
        common_check_Box(gltf);

        REQUIRE("" == gltf.buffers_[0].uri_);
    }

    SECTION("load embedded"){
        cppgltf::IFStream ifstream;
        if(!ifstream.open(embedded)){
            return;
        }
        cppgltf::glTFHandler gltfHandler(textDir);
        cppgltf::JSONReader gltfJsonReader(ifstream, gltfHandler);
        bool result = gltfJsonReader.read();
        REQUIRE(result);
        ifstream.close();

        cppgltf::glTF& gltf = gltfHandler.get();
        common_check_Box(gltf);
    }
}

#include "config.h"

void common_check_BoomBox(const cppgltf::glTF& gltf)
{
    bool checkRequirements = gltf.checkRequirements();
    REQUIRE(checkRequirements);

    //asset
    REQUIRE("glTF Tools for Unity" == gltf.asset_.generator_);
    REQUIRE("2.0" == gltf.asset_.version_);

    //scene
    REQUIRE(0 == gltf.scene_);

    {//scenes
        REQUIRE(1 == gltf.scenes_.size());
        REQUIRE(1 == gltf.scenes_[0].nodes_.size());
        REQUIRE(0 == gltf.scenes_[0].nodes_[0]);
    }

    {//nodes
        REQUIRE(1 == gltf.nodes_.size());
        REQUIRE(0 == gltf.nodes_[0].children_.size());
        REQUIRE(gltf.nodes_[0].flags_.check(cppgltf::Node::Flag_Rotation));
        REQUIRE(Approx(0.0f) == gltf.nodes_[0].rotation_[0]);
        REQUIRE(Approx(1.0f) == gltf.nodes_[0].rotation_[1]);
        REQUIRE(Approx(0.0f) == gltf.nodes_[0].rotation_[2]);
        REQUIRE(Approx(0.0f) == gltf.nodes_[0].rotation_[3]);
        REQUIRE(0 == gltf.nodes_[0].mesh_);
        REQUIRE("BoomBox" == gltf.nodes_[0].name_);
    }

    {//meshes
        REQUIRE(1 == gltf.meshes_.size());
        REQUIRE("BoomBox" == gltf.meshes_[0].name_);
        REQUIRE(1 == gltf.meshes_[0].primitives_.size());
        REQUIRE(4 == gltf.meshes_[0].primitives_[0].attributes_.size());

        REQUIRE(cppgltf::GLTF_ATTRIBUTE_TEXCOORD == gltf.meshes_[0].primitives_[0].attributes_[0].semanticType_);
        REQUIRE(0 == gltf.meshes_[0].primitives_[0].attributes_[0].accessor_);
        REQUIRE(0 == gltf.meshes_[0].primitives_[0].attributes_[0].semanticIndex_);

        REQUIRE(cppgltf::GLTF_ATTRIBUTE_NORMAL == gltf.meshes_[0].primitives_[0].attributes_[1].semanticType_);
        REQUIRE(1 == gltf.meshes_[0].primitives_[0].attributes_[1].accessor_);
        REQUIRE(0 == gltf.meshes_[0].primitives_[0].attributes_[1].semanticIndex_);

        REQUIRE(cppgltf::GLTF_ATTRIBUTE_TANGENT == gltf.meshes_[0].primitives_[0].attributes_[2].semanticType_);
        REQUIRE(2 == gltf.meshes_[0].primitives_[0].attributes_[2].accessor_);
        REQUIRE(0 == gltf.meshes_[0].primitives_[0].attributes_[2].semanticIndex_);

        REQUIRE(cppgltf::GLTF_ATTRIBUTE_POSITION == gltf.meshes_[0].primitives_[0].attributes_[3].semanticType_);
        REQUIRE(3 == gltf.meshes_[0].primitives_[0].attributes_[3].accessor_);
        REQUIRE(0 == gltf.meshes_[0].primitives_[0].attributes_[3].semanticIndex_);

        REQUIRE(4 == gltf.meshes_[0].primitives_[0].indices_);
        REQUIRE(cppgltf::GLTF_PRIMITIVE_TRIANGLES == gltf.meshes_[0].primitives_[0].mode_);
        REQUIRE(0 == gltf.meshes_[0].primitives_[0].material_);
    }

    {//accessors
        REQUIRE(5 == gltf.accessors_.size());

        REQUIRE(0 == gltf.accessors_[0].bufferView_);
        REQUIRE(0 == gltf.accessors_[0].byteOffset_);
        REQUIRE(cppgltf::GLTF_TYPE_FLOAT == gltf.accessors_[0].componentType_);
        REQUIRE(3575 == gltf.accessors_[0].count_);
        REQUIRE(cppgltf::GLTF_TYPE_VEC2 == gltf.accessors_[0].type_);

        REQUIRE(1 == gltf.accessors_[1].bufferView_);
        REQUIRE(0 == gltf.accessors_[1].byteOffset_);
        REQUIRE(cppgltf::GLTF_TYPE_FLOAT == gltf.accessors_[1].componentType_);
        REQUIRE(3575 == gltf.accessors_[1].count_);
        REQUIRE(cppgltf::GLTF_TYPE_VEC3 == gltf.accessors_[1].type_);

        REQUIRE(2 == gltf.accessors_[2].bufferView_);
        REQUIRE(0 == gltf.accessors_[2].byteOffset_);
        REQUIRE(cppgltf::GLTF_TYPE_FLOAT == gltf.accessors_[2].componentType_);
        REQUIRE(3575 == gltf.accessors_[2].count_);
        REQUIRE(cppgltf::GLTF_TYPE_VEC4 == gltf.accessors_[2].type_);

        REQUIRE(3 == gltf.accessors_[3].bufferView_);
        REQUIRE(0 == gltf.accessors_[3].byteOffset_);
        REQUIRE(cppgltf::GLTF_TYPE_FLOAT == gltf.accessors_[3].componentType_);
        REQUIRE(3575 == gltf.accessors_[3].count_);
        REQUIRE(Approx(-0.009921154f) == gltf.accessors_[3].min_[0].fvalue_);
        REQUIRE(Approx(-0.00977163f) == gltf.accessors_[3].min_[1].fvalue_);
        REQUIRE(Approx(-0.0100762453f) == gltf.accessors_[3].min_[2].fvalue_);
        REQUIRE(Approx(0.009921154f) == gltf.accessors_[3].max_[0].fvalue_);
        REQUIRE(Approx(0.00977163f) == gltf.accessors_[3].max_[1].fvalue_);
        REQUIRE(Approx(0.0100762453f) == gltf.accessors_[3].max_[2].fvalue_);
        REQUIRE(cppgltf::GLTF_TYPE_VEC3 == gltf.accessors_[3].type_);

        REQUIRE(4 == gltf.accessors_[4].bufferView_);
        REQUIRE(0 == gltf.accessors_[4].byteOffset_);
        REQUIRE(cppgltf::GLTF_TYPE_UNSIGNED_SHORT == gltf.accessors_[4].componentType_);
        REQUIRE(18108 == gltf.accessors_[4].count_);
        REQUIRE(cppgltf::GLTF_TYPE_SCALAR == gltf.accessors_[4].type_);
    }

    {//materials
        REQUIRE(1 == gltf.materials_.size());

        REQUIRE(Approx(1.0f) == gltf.materials_[0].pbrMetallicRoughness_.baseColorFactor_[0]);
        REQUIRE(Approx(1.0f) == gltf.materials_[0].pbrMetallicRoughness_.baseColorFactor_[1]);
        REQUIRE(Approx(1.0f) == gltf.materials_[0].pbrMetallicRoughness_.baseColorFactor_[2]);
        REQUIRE(Approx(1.0f) == gltf.materials_[0].pbrMetallicRoughness_.baseColorFactor_[3]);
        REQUIRE(Approx(1.0f) == gltf.materials_[0].pbrMetallicRoughness_.metallicFactor_);

        REQUIRE(0 == gltf.materials_[0].pbrMetallicRoughness_.baseColorTexture_.index_);
        REQUIRE(0 == gltf.materials_[0].pbrMetallicRoughness_.baseColorTexture_.texCoord_);
        REQUIRE(1 == gltf.materials_[0].pbrMetallicRoughness_.metallicRoughnessTexture_.index_);
        REQUIRE(0 == gltf.materials_[0].pbrMetallicRoughness_.metallicRoughnessTexture_.texCoord_);

        REQUIRE(2 == gltf.materials_[0].normalTexture_.index_);
        REQUIRE(0 == gltf.materials_[0].normalTexture_.texCoord_);
        REQUIRE(1 == gltf.materials_[0].occlusionTexture_.index_);
        REQUIRE(0 == gltf.materials_[0].occlusionTexture_.texCoord_);

        REQUIRE(Approx(1.0f) == gltf.materials_[0].emissiveFactor_[0]);
        REQUIRE(Approx(1.0f) == gltf.materials_[0].emissiveFactor_[1]);
        REQUIRE(Approx(1.0f) == gltf.materials_[0].emissiveFactor_[2]);

        REQUIRE(3 == gltf.materials_[0].emissiveTexture_.index_);
        REQUIRE(0 == gltf.materials_[0].emissiveTexture_.texCoord_);

        REQUIRE("BoomBox_Mat" == gltf.materials_[0].name_);
    }

    {//bufferViews
        REQUIRE(5 == gltf.bufferViews_.size());

        REQUIRE(0 == gltf.bufferViews_[0].buffer_);
        REQUIRE(0 == gltf.bufferViews_[0].byteOffset_);
        REQUIRE(28600 == gltf.bufferViews_[0].byteLength_);
        REQUIRE(-1 == gltf.bufferViews_[0].byteStride_);
        REQUIRE(-1 == gltf.bufferViews_[0].target_);

        REQUIRE(0 == gltf.bufferViews_[1].buffer_);
        REQUIRE(28600 == gltf.bufferViews_[1].byteOffset_);
        REQUIRE(42900 == gltf.bufferViews_[1].byteLength_);
        REQUIRE(-1 == gltf.bufferViews_[1].byteStride_);
        REQUIRE(-1 == gltf.bufferViews_[1].target_);

        REQUIRE(0 == gltf.bufferViews_[2].buffer_);
        REQUIRE(71500 == gltf.bufferViews_[2].byteOffset_);
        REQUIRE(57200 == gltf.bufferViews_[2].byteLength_);
        REQUIRE(-1 == gltf.bufferViews_[2].byteStride_);
        REQUIRE(-1 == gltf.bufferViews_[2].target_);

        REQUIRE(0 == gltf.bufferViews_[3].buffer_);
        REQUIRE(128700 == gltf.bufferViews_[3].byteOffset_);
        REQUIRE(42900 == gltf.bufferViews_[3].byteLength_);
        REQUIRE(-1 == gltf.bufferViews_[3].byteStride_);
        REQUIRE(-1 == gltf.bufferViews_[3].target_);

        REQUIRE(0 == gltf.bufferViews_[4].buffer_);
        REQUIRE(171600 == gltf.bufferViews_[4].byteOffset_);
        REQUIRE(36216 == gltf.bufferViews_[4].byteLength_);
        REQUIRE(-1 == gltf.bufferViews_[4].byteStride_);
        REQUIRE(-1 == gltf.bufferViews_[4].target_);
    }

    {//images
        REQUIRE(4 == gltf.images_.size());

        REQUIRE("BoomBox_baseColor.png" == gltf.images_[0].uri_);
        REQUIRE("BoomBox_occlusionRoughnessMetallic.png" == gltf.images_[1].uri_);
        REQUIRE("BoomBox_normal.png" == gltf.images_[2].uri_);
        REQUIRE("BoomBox_emissive.png" == gltf.images_[3].uri_);
    }

    {//textures
        REQUIRE(4 == gltf.textures_.size());

        REQUIRE(0 == gltf.textures_[0].source_);
        REQUIRE(1 == gltf.textures_[1].source_);
        REQUIRE(2 == gltf.textures_[2].source_);
        REQUIRE(3 == gltf.textures_[3].source_);
    }

    {//buffers
        REQUIRE(1 == gltf.buffers_.size());
        REQUIRE(207816 == gltf.buffers_[0].byteLength_);
        REQUIRE(NULL != gltf.buffers_[0].data_);
    }
}

void binary_check_BoomBox(const cppgltf::glTF& gltf)
{
    bool checkRequirements = gltf.checkRequirements();
    REQUIRE(checkRequirements);

    //asset
    REQUIRE("glTF Tools for Unity" == gltf.asset_.generator_);
    REQUIRE("2.0" == gltf.asset_.version_);

    //scene
    REQUIRE(0 == gltf.scene_);

    {//scenes
        REQUIRE(1 == gltf.scenes_.size());
        REQUIRE(1 == gltf.scenes_[0].nodes_.size());
        REQUIRE(0 == gltf.scenes_[0].nodes_[0]);
    }

    {//nodes
        REQUIRE(1 == gltf.nodes_.size());
        REQUIRE(0 == gltf.nodes_[0].children_.size());
        REQUIRE(gltf.nodes_[0].flags_.check(cppgltf::Node::Flag_Rotation));
        REQUIRE(Approx(0.0f) == gltf.nodes_[0].rotation_[0]);
        REQUIRE(Approx(1.0f) == gltf.nodes_[0].rotation_[1]);
        REQUIRE(Approx(0.0f) == gltf.nodes_[0].rotation_[2]);
        REQUIRE(Approx(0.0f) == gltf.nodes_[0].rotation_[3]);
        REQUIRE(0 == gltf.nodes_[0].mesh_);
        REQUIRE("BoomBox" == gltf.nodes_[0].name_);
    }

    {//meshes
        REQUIRE(1 == gltf.meshes_.size());
        REQUIRE("BoomBox" == gltf.meshes_[0].name_);
        REQUIRE(1 == gltf.meshes_[0].primitives_.size());
        REQUIRE(4 == gltf.meshes_[0].primitives_[0].attributes_.size());

        REQUIRE(cppgltf::GLTF_ATTRIBUTE_TEXCOORD == gltf.meshes_[0].primitives_[0].attributes_[0].semanticType_);
        REQUIRE(0 == gltf.meshes_[0].primitives_[0].attributes_[0].accessor_);
        REQUIRE(0 == gltf.meshes_[0].primitives_[0].attributes_[0].semanticIndex_);

        REQUIRE(cppgltf::GLTF_ATTRIBUTE_NORMAL == gltf.meshes_[0].primitives_[0].attributes_[1].semanticType_);
        REQUIRE(1 == gltf.meshes_[0].primitives_[0].attributes_[1].accessor_);
        REQUIRE(0 == gltf.meshes_[0].primitives_[0].attributes_[1].semanticIndex_);

        REQUIRE(cppgltf::GLTF_ATTRIBUTE_TANGENT == gltf.meshes_[0].primitives_[0].attributes_[2].semanticType_);
        REQUIRE(2 == gltf.meshes_[0].primitives_[0].attributes_[2].accessor_);
        REQUIRE(0 == gltf.meshes_[0].primitives_[0].attributes_[2].semanticIndex_);

        REQUIRE(cppgltf::GLTF_ATTRIBUTE_POSITION == gltf.meshes_[0].primitives_[0].attributes_[3].semanticType_);
        REQUIRE(3 == gltf.meshes_[0].primitives_[0].attributes_[3].accessor_);
        REQUIRE(0 == gltf.meshes_[0].primitives_[0].attributes_[3].semanticIndex_);

        REQUIRE(4 == gltf.meshes_[0].primitives_[0].indices_);
        REQUIRE(cppgltf::GLTF_PRIMITIVE_TRIANGLES == gltf.meshes_[0].primitives_[0].mode_);
        REQUIRE(0 == gltf.meshes_[0].primitives_[0].material_);
    }

    {//accessors
        REQUIRE(5 == gltf.accessors_.size());

        REQUIRE(4 == gltf.accessors_[0].bufferView_);
        REQUIRE(0 == gltf.accessors_[0].byteOffset_);
        REQUIRE(cppgltf::GLTF_TYPE_FLOAT == gltf.accessors_[0].componentType_);
        REQUIRE(3575 == gltf.accessors_[0].count_);
        REQUIRE(cppgltf::GLTF_TYPE_VEC2 == gltf.accessors_[0].type_);

        REQUIRE(5 == gltf.accessors_[1].bufferView_);
        REQUIRE(0 == gltf.accessors_[1].byteOffset_);
        REQUIRE(cppgltf::GLTF_TYPE_FLOAT == gltf.accessors_[1].componentType_);
        REQUIRE(3575 == gltf.accessors_[1].count_);
        REQUIRE(cppgltf::GLTF_TYPE_VEC3 == gltf.accessors_[1].type_);

        REQUIRE(6 == gltf.accessors_[2].bufferView_);
        REQUIRE(0 == gltf.accessors_[2].byteOffset_);
        REQUIRE(cppgltf::GLTF_TYPE_FLOAT == gltf.accessors_[2].componentType_);
        REQUIRE(3575 == gltf.accessors_[2].count_);
        REQUIRE(cppgltf::GLTF_TYPE_VEC4 == gltf.accessors_[2].type_);

        REQUIRE(7 == gltf.accessors_[3].bufferView_);
        REQUIRE(0 == gltf.accessors_[3].byteOffset_);
        REQUIRE(cppgltf::GLTF_TYPE_FLOAT == gltf.accessors_[3].componentType_);
        REQUIRE(3575 == gltf.accessors_[3].count_);
        REQUIRE(Approx(-0.009921154f) == gltf.accessors_[3].min_[0].fvalue_);
        REQUIRE(Approx(-0.00977163f) == gltf.accessors_[3].min_[1].fvalue_);
        REQUIRE(Approx(-0.0100762453f) == gltf.accessors_[3].min_[2].fvalue_);
        REQUIRE(Approx(0.009921154f) == gltf.accessors_[3].max_[0].fvalue_);
        REQUIRE(Approx(0.00977163f) == gltf.accessors_[3].max_[1].fvalue_);
        REQUIRE(Approx(0.0100762453f) == gltf.accessors_[3].max_[2].fvalue_);
        REQUIRE(cppgltf::GLTF_TYPE_VEC3 == gltf.accessors_[3].type_);

        REQUIRE(8 == gltf.accessors_[4].bufferView_);
        REQUIRE(0 == gltf.accessors_[4].byteOffset_);
        REQUIRE(cppgltf::GLTF_TYPE_UNSIGNED_SHORT == gltf.accessors_[4].componentType_);
        REQUIRE(18108 == gltf.accessors_[4].count_);
        REQUIRE(cppgltf::GLTF_TYPE_SCALAR == gltf.accessors_[4].type_);
    }

    {//materials
        REQUIRE(1 == gltf.materials_.size());

        REQUIRE(Approx(1.0f) == gltf.materials_[0].pbrMetallicRoughness_.baseColorFactor_[0]);
        REQUIRE(Approx(1.0f) == gltf.materials_[0].pbrMetallicRoughness_.baseColorFactor_[1]);
        REQUIRE(Approx(1.0f) == gltf.materials_[0].pbrMetallicRoughness_.baseColorFactor_[2]);
        REQUIRE(Approx(1.0f) == gltf.materials_[0].pbrMetallicRoughness_.baseColorFactor_[3]);
        REQUIRE(Approx(1.0f) == gltf.materials_[0].pbrMetallicRoughness_.metallicFactor_);

        REQUIRE(0 == gltf.materials_[0].pbrMetallicRoughness_.baseColorTexture_.index_);
        REQUIRE(0 == gltf.materials_[0].pbrMetallicRoughness_.baseColorTexture_.texCoord_);
        REQUIRE(1 == gltf.materials_[0].pbrMetallicRoughness_.metallicRoughnessTexture_.index_);
        REQUIRE(0 == gltf.materials_[0].pbrMetallicRoughness_.metallicRoughnessTexture_.texCoord_);

        REQUIRE(2 == gltf.materials_[0].normalTexture_.index_);
        REQUIRE(0 == gltf.materials_[0].normalTexture_.texCoord_);
        REQUIRE(1 == gltf.materials_[0].occlusionTexture_.index_);
        REQUIRE(0 == gltf.materials_[0].occlusionTexture_.texCoord_);

        REQUIRE(Approx(1.0f) == gltf.materials_[0].emissiveFactor_[0]);
        REQUIRE(Approx(1.0f) == gltf.materials_[0].emissiveFactor_[1]);
        REQUIRE(Approx(1.0f) == gltf.materials_[0].emissiveFactor_[2]);

        REQUIRE(3 == gltf.materials_[0].emissiveTexture_.index_);
        REQUIRE(0 == gltf.materials_[0].emissiveTexture_.texCoord_);

        REQUIRE("BoomBox_Mat" == gltf.materials_[0].name_);
    }

    {//bufferViews
        REQUIRE(9 == gltf.bufferViews_.size());

        REQUIRE(0 == gltf.bufferViews_[0].buffer_);
        REQUIRE(0 == gltf.bufferViews_[0].byteOffset_);
        REQUIRE(3285844 == gltf.bufferViews_[0].byteLength_);
        REQUIRE(-1 == gltf.bufferViews_[0].byteStride_);
        REQUIRE(-1 == gltf.bufferViews_[0].target_);

        REQUIRE(0 == gltf.bufferViews_[1].buffer_);
        REQUIRE(3285844 == gltf.bufferViews_[1].byteOffset_);
        REQUIRE(4471451 == gltf.bufferViews_[1].byteLength_);
        REQUIRE(-1 == gltf.bufferViews_[1].byteStride_);
        REQUIRE(-1 == gltf.bufferViews_[1].target_);

        REQUIRE(0 == gltf.bufferViews_[2].buffer_);
        REQUIRE(7757295 == gltf.bufferViews_[2].byteOffset_);
        REQUIRE(2845923 == gltf.bufferViews_[2].byteLength_);
        REQUIRE(-1 == gltf.bufferViews_[2].byteStride_);
        REQUIRE(-1 == gltf.bufferViews_[2].target_);

        REQUIRE(0 == gltf.bufferViews_[3].buffer_);
        REQUIRE(10603218 == gltf.bufferViews_[3].byteOffset_);
        REQUIRE(132833 == gltf.bufferViews_[3].byteLength_);
        REQUIRE(-1 == gltf.bufferViews_[3].byteStride_);
        REQUIRE(-1 == gltf.bufferViews_[3].target_);

        REQUIRE(0 == gltf.bufferViews_[4].buffer_);
        REQUIRE(10736052 == gltf.bufferViews_[4].byteOffset_);
        REQUIRE(28600 == gltf.bufferViews_[4].byteLength_);
        REQUIRE(-1 == gltf.bufferViews_[4].byteStride_);
        REQUIRE(-1 == gltf.bufferViews_[4].target_);

        REQUIRE(0 == gltf.bufferViews_[5].buffer_);
        REQUIRE(10764652 == gltf.bufferViews_[5].byteOffset_);
        REQUIRE(42900 == gltf.bufferViews_[5].byteLength_);
        REQUIRE(-1 == gltf.bufferViews_[5].byteStride_);
        REQUIRE(-1 == gltf.bufferViews_[5].target_);

        REQUIRE(0 == gltf.bufferViews_[6].buffer_);
        REQUIRE(10807552 == gltf.bufferViews_[6].byteOffset_);
        REQUIRE(57200 == gltf.bufferViews_[6].byteLength_);
        REQUIRE(-1 == gltf.bufferViews_[6].byteStride_);
        REQUIRE(-1 == gltf.bufferViews_[6].target_);

        REQUIRE(0 == gltf.bufferViews_[7].buffer_);
        REQUIRE(10864752 == gltf.bufferViews_[7].byteOffset_);
        REQUIRE(42900 == gltf.bufferViews_[7].byteLength_);
        REQUIRE(-1 == gltf.bufferViews_[7].byteStride_);
        REQUIRE(-1 == gltf.bufferViews_[7].target_);

        REQUIRE(0 == gltf.bufferViews_[8].buffer_);
        REQUIRE(10907652 == gltf.bufferViews_[8].byteOffset_);
        REQUIRE(36216 == gltf.bufferViews_[8].byteLength_);
        REQUIRE(-1 == gltf.bufferViews_[8].byteStride_);
        REQUIRE(-1 == gltf.bufferViews_[8].target_);
    }

    {//images
        REQUIRE(4 == gltf.images_.size());

        REQUIRE("image/png" == gltf.images_[0].mimeType_);
        REQUIRE(0 == gltf.images_[0].bufferView_);
        REQUIRE("image/png" == gltf.images_[1].mimeType_);
        REQUIRE(1 == gltf.images_[1].bufferView_);
        REQUIRE("image/png" == gltf.images_[2].mimeType_);
        REQUIRE(2 == gltf.images_[2].bufferView_);
        REQUIRE("image/png" == gltf.images_[3].mimeType_);
        REQUIRE(3 == gltf.images_[3].bufferView_);
    }

    {//textures
        REQUIRE(4 == gltf.textures_.size());

        REQUIRE(0 == gltf.textures_[0].source_);
        REQUIRE(1 == gltf.textures_[1].source_);
        REQUIRE(2 == gltf.textures_[2].source_);
        REQUIRE(3 == gltf.textures_[3].source_);
    }

    {//buffers
        REQUIRE(1 == gltf.buffers_.size());
        REQUIRE(10943868 == gltf.buffers_[0].byteLength_);
        REQUIRE(NULL != gltf.buffers_[0].data_);
    }
}

TEST_CASE("A sample BoomBox can be loaded", "[BoomBox]"){
    static const char* textDir = DATA_ROOT"BoomBox/glTF/";
    static const char* text = DATA_ROOT"BoomBox/glTF/BoomBox.gltf";
    static const char* binary = DATA_ROOT"BoomBox/glTF-Binary/BoomBox.glb";
    static const char* embedded = DATA_ROOT"BoomBox/glTF-Embedded/BoomBox.gltf";

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
        common_check_BoomBox(gltf);
        //buffer
        REQUIRE("BoomBox.bin" == gltf.buffers_[0].uri_);
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
        binary_check_BoomBox(gltf);
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
        common_check_BoomBox(gltf);
    }
}

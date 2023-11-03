#pragma once
#include <json.hpp>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "geometry.h"
#include "math/vector3.h"
#include "engine/quaternion.h"

using nlohmann::json;
using math::Vector3;
using physics::Quaternion;

struct ObjectData {//78 bytes
    Vector3 pos{}; //12 bytes
    Vector3 scl{}; //12 bytes
    Vector3 vel{}; //12 bytes
    Vector3 angVel{};//12 bytes
    Quaternion orientation{}; //16 bytes, (don't need rotation cuz this can fully represents the object's orientation in 3D space)
    unsigned int textureID{}; //4 bytes
    float mass{};//4 bytes
    bool IsFixed {false}; //1 bytes
    ObjectType type;  // enum Sphere or Box, 1 byte
};
 //Function to save object data to a JSON file
static void SaveObjectsToJson(const std::vector<ObjectData>& objects, int fileIdx) {
    json j = json::array();
    for (const auto& obj : objects) {
        json objJson;
        objJson["pos"] = { obj.pos.x, obj.pos.y, obj.pos.z };
        objJson["scl"] = { obj.scl.x, obj.scl.y, obj.scl.z };
        objJson["vel"] = { obj.vel.x, obj.vel.y, obj.vel.z };
        objJson["angVel"] = { obj.angVel.x, obj.angVel.y, obj.angVel.z };
        objJson["ori"] = { obj.orientation.w,obj.orientation.x,obj.orientation.y,obj.orientation.z };
        objJson["textureID"] = obj.textureID;
        objJson["mass"] = obj.mass;
        objJson["isFixed"] = obj.IsFixed ;
        objJson["type"] = static_cast<int>(obj.type);
        j.push_back(objJson);
    }
    std::string filePath = "PhysicsEngine/presets/preset_" + std::to_string(fileIdx) + ".json";
    std::ofstream outputFile(filePath);

    std::ifstream ifs(filePath);
    outputFile << j.dump(4);
    outputFile.close();
}

// Function to load object data from a JSON file
static void LoadObjectsFromJson(std::vector<ObjectData>& objects,unsigned fileIdx) {
    std::string filePath = "PhysicsEngine/presets/preset_" + std::to_string(fileIdx) + ".json";
    std::ifstream inputFile(filePath);
    if (!std::filesystem::exists(filePath)) {
        std::cerr << "Error: File not found: " << filePath << std::endl;
        return;
    }
    if (!inputFile.is_open()) {
        std::cerr << "Error: Failed to open the JSON file." << std::endl;
        return;
    }


    json j = json::parse(inputFile);

    for (const auto& objJson : j) {
        ObjectData objData;
        objData.pos = { objJson["pos"][0], objJson["pos"][1], objJson["pos"][2] };
        objData.scl = { objJson["scl"][0], objJson["scl"][1], objJson["scl"][2] };
        objData.vel = { objJson["vel"][0], objJson["vel"][1], objJson["vel"][2] };
        objData.angVel = { objJson["angVel"][0], objJson["angVel"][1], objJson["angVel"][2] };
        objData.orientation = { objJson["ori"][0],objJson["ori"][1], objJson["ori"][2], objJson["ori"][3] };
        objData.textureID = objJson["textureID"];
        objData.mass = objJson["mass"];
        objData.IsFixed = objJson["isFixed"];
        objData.type = static_cast<ObjectType>(objJson["type"]);
        objects.push_back(objData);
    }
}

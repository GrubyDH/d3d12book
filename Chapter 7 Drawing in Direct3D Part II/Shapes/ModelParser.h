#pragma once
#include "../../Common/GeometryGenerator.h"

#include <string>
#include <fstream>
#include <sstream>
#include <limits>

class ModelParser {
public:
    static GeometryGenerator::MeshData Parse(const std::string & filename) {
        GeometryGenerator::MeshData geom;

        std::ifstream file(filename);
        std::string line;
        std::istringstream iss;

        std::getline(file, line);
        iss.str(line);
        iss.ignore(std::numeric_limits<std::streamsize>::max(), ':');
        int vertexCount = 0;
        iss >> vertexCount;

        std::getline(file, line);
        iss.clear();
        iss.str(line);
        iss.ignore(std::numeric_limits<std::streamsize>::max(), ':');
        int indexCount = 0;
        iss >> indexCount;

        std::getline(file, line);
        std::getline(file, line);

        geom.Vertices.resize(vertexCount);
        for (int i = 0; i < vertexCount; ++i) {
            std::getline(file, line);
            iss.clear();
            iss.str(line);
            GeometryGenerator::Vertex v;
            iss >> v.Position.x >> v.Position.y >> v.Position.z >> v.Normal.x >> v.Normal.y >> v.Normal.z;
            geom.Vertices[i] = v;
        }

        std::getline(file, line);
        std::getline(file, line);
        std::getline(file, line);

        geom.Indices32.resize(indexCount * 3);
        for (int i = 0; i < indexCount; ++i) {
            std::getline(file, line);
            iss.clear();
            iss.str(line);
            int baseIdx = i * 3;
            uint32_t idx[3];
            iss >> idx[0] >> idx[1] >> idx[2];
            geom.Indices32[baseIdx] = idx[0];
            geom.Indices32[baseIdx + 1] = idx[1];
            geom.Indices32[baseIdx + 2] = idx[2];
        }

        return geom;
    }
};

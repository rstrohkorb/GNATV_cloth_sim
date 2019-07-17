#include <iostream>
#include <fstream>
#include <numeric>
#include <boost/algorithm/string.hpp>
#include "Cloth.h"

void Cloth::init(std::string _filename, initOrientation _o)
{
    // read in object data
    readObj(_filename);
    // finish creating triangles, compute r-weights
    for(auto& tr : m_triangles)
    {
        tr.tri.setVertices(m_mspts[tr.a].pos(), m_mspts[tr.b].pos(), m_mspts[tr.c].pos());
        tr.tri.computeR(_o);
    }
    // determine mass of the masspoints
    std::vector<std::vector<float>> massCollect;
    massCollect.resize(m_mspts.size());
    // accumulate masses of triangles connected to each masspoint
    for(auto t : m_triangles)
    {
        auto tmass = t.tri.surface_area() * m_material.mass();
        massCollect[t.a].push_back(tmass);
        massCollect[t.b].push_back(tmass);
        massCollect[t.c].push_back(tmass);
    }
    // for each masspoint, sum over collected values to get mass
    for(size_t i = 0; i < m_mspts.size(); ++i)
    {
        auto totalMass = std::accumulate(massCollect[i].begin(), massCollect[i].end(), 0.0f);
        m_mspts[i].setMass(totalMass/3);
    }
}

void Cloth::render(std::vector<ngl::Vec3> &o_vertexData)
{
    o_vertexData.reserve(m_triangles.size());
    // spit out current triangle positions
    for(auto t : m_triangles)
    {
        o_vertexData.push_back(t.tri.v1());
        o_vertexData.push_back(t.tri.v2());
        o_vertexData.push_back(t.tri.v3());
    }
}

void Cloth::readObj(std::string _filename)
{
    // import cloth data from obj
    std::ifstream clothFile;
    std::string line;
    clothFile.open(_filename);
    // read each line
    while(std::getline(clothFile, line))
    {
        switch(line[0])
        {
        case 'v':
        {
            // get vertex data
            std::vector<std::string> res;
            boost::split(res, line, [](char c){return c == ' ';});
            float p0, p1, p2;
            p0 = std::stof(res[1]);
            p1 = std::stof(res[2]);
            p2 = std::stof(res[3]);
            // Create masspoint with that position
            MassPoint m (ngl::Vec3(p0, p1, p2));
            m_mspts.push_back(m);
        }
        break;
        case 'f':
        {
            // get triangle index data
            Triref tr;
            std::vector<std::string> res;
            boost::split(res, line, [](char c){return c == ' ';});
            tr.a = stoul(res[1]) - 1;  // subtract 1 since obj files index faces at 1
            tr.b = stoul(res[2]) - 1;
            tr.c = stoul(res[3]) - 1;
            m_triangles.push_back(tr);
        }
        break;
        default: break;
        }
    }
    clothFile.close();
}

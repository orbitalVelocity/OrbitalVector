//
//  includes.cpp
//  GLFW3_test
//
//  Created by Si Li on 9/16/14.
//  Copyright (c) 2014 Si Li. All rights reserved.
//

#include "includes.h"
#include "rk547m.h"
using namespace std;

bool globalShowFXAAAAirection = 0;
bool globalReload;
int renderStage = 0x3F;
const int stage1  = 0x0001;     //shadowMap
const int stage2  = 0x0002;     //forward
const int stage3  = 0x0004;     //highpass
const int stage4  = 0x0008;     //blur1
const int stage5  = 0x0010;     //blur2
const int stage6  = 0x0020;     //composite/tonemap
const int stage7  = 0x0040;     //FXAA
const int stage8  = 0x0080;
const int stage9  = 0x0010;
const int stage10 = 0x0200;
//const int stage11 = 0x0400;
//const int stage12 = 0x0800;
string get_file_contents(string filename)
{
    std::ifstream in(filename, std::ios::in | std::ios::binary);
    if (in)
    {
        std::string contents;
        in.seekg(0, std::ios::end);
        contents.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&contents[0], contents.size());
        in.close();
        return(contents);
    }
    throw(errno);
}

string printVec3(glm::vec3 v)
{
    stringstream out;
    out << v.x << ", " << v.y << ", " << v.z;
    return out.str();
}

glm::vec2 getVec2(glm::mat4 vp, glm::vec3 _pos)
{
    glm::vec4 pos = vp * glm::vec4(_pos, 1.0f);
    pos.x /= pos.z;
    pos.y /= pos.z;
    pos.x = ( pos.x+1.0f) / 2;
    pos.y = (-pos.y+1.0f) / 2; //FIXME: why y has to be negative?
    return glm::vec2(pos.x, pos.y);
}

/*
void computeNormals(vector<glm::vec3> &normals,
                    vector<unsigned> &normCount,
                    const vector<GLuint> &triangles,
                    const vector<glm::vec3> &vertices) {
    normals = vector<glm::vec3>(vertices.size(), glm::vec3(0));
    normCount = vector<unsigned>(vertices.size(), 0);
    
    for (size_t idx = 0; idx < triangles.size(); ++idx) {
        glm::vec3 v0(triangles[idx].x),
            v1(triangles[idx].y),
            v2(triangles[idx].z),
            norm = (v1-v0)*(v2-v0);
        
            ++normCount[triangles[idx].x];
            normals[triangles[idx].x] += glm::normalize(norm);
            ++normCount[triangles[idx].y];
            normals[triangles[idx].y] += glm::normalize(norm);
            ++normCount[triangles[idx].z];
            normals[triangles[idx].z] += glm::normalize(norm);
    }
    
    for (size_t idx = 0; idx < vertices.size(); ++idx) {
        normals[idx] /= normCount[idx];
        normals[idx] /= normals[idx].length();
    }
}

void permuteNormals(vector<glm::vec3> &normals, vector<glm::vec3> &triangles,
                    vector<glm::vec3> &vertices, vector<unsigned> &normIndex)
{
    vector<glm::vec3> newNormals, newVertices;
    vector<glm::vec3> newTriangles;
    for (size_t i = 0; i < triangles.size(); ++i) {
        newVertices.push_back(vertices[triangles[i].x]);
        newNormals.push_back(normals[normIndex[i*3+0]]);
        newVertices.push_back(vertices[triangles[i].y]);
        newNormals.push_back(normals[normIndex[i*3+1]]);
        newVertices.push_back(vertices[triangles[i].z]);
        newNormals.push_back(normals[normIndex[i*3+2]]);
        newTriangles.push_back(glm::vec3(i*3, i*3+1, i*3+2));
    }
    
    vertices = newVertices;
    normals = newNormals;
    triangles = newTriangles;
}

void readObj(string filename, Obj &obj) {
    ifstream in(filename.c_str());
    if (!in) {
        cerr << "Invalid input file." << endl;
        exit(1);
    };
    
    unsigned maxVertex(0);
    vector<glm::vec3> &vertices = obj.vertices;
    vector<glm::vec3> &normals = obj.normals;
    vector<unsigned> &normCount = obj.normCount;
    vector<unsigned> &normIndex = obj.normIndex;
    vector<GLuint> &triangles = obj.triangles;
    
    
    while (!!in) {
        char c;
        in.get(c);
        if (c == 'v' && in.peek() == ' ') {                            // Vertices
            float x, y, z;
            in >> x >> y >> z;
            vertices.push_back(glm::vec3(x, y, z));
        } else if (c == 'v' && in.peek() == 'n') {
            in.get(c);
            float x, y, z;
            in >> x >> y >> z;
            glm::vec3 v(x, y, z);
            normals.push_back(glm::normalize(v));
        } else if (c == 'f' && in.peek() == ' ')
        {                     // Faces
            unsigned x[3];
            for (unsigned i = 0; i < 3; ++i) {
                in >> x[i]; --x[i]; // The input is sadly 1-indexed
                if (x[i] > maxVertex) maxVertex = x[i];
                
                // Ignore texture coordinates, extract normal indices.
                unsigned a(0);
                if (in.peek() == '/') {
                    in.get(c);
                    if (in.peek() != '/') in >> a;
                    in.get(c);
                    in >> a; --a;
                    normIndex.push_back(a);
                }
            }
            for (int i=0; i<3; i++) {
                triangles.push_back(x[i]);
            }
            
            // Support quadrilaterals.
            while (in.peek() == ' ') in.get(c);
            if (isdigit(in.peek())) {
                unsigned x;
                in >> x; --x;
                triangles.push_back(triangles[triangles.size()-1]*3+0);
                triangles.push_back(triangles[triangles.size()-1]*3+1);
                triangles.push_back(x);
                
                if (x > maxVertex) maxVertex = x;
            }
        } else {
            while (!!in && c != '\n') { in.get(c); } // Comments, etc.
        }
#if 0
        if (c == 'm') {
            // mtllib
            string s;
            in >> s;
            if (s != "tllib") cout << "Unexpected text following m" << endl;
            
            while (in.peek() == ' ' || in.peek() == '\t') in.get(c);
            in >> s;
            readMaterials((string(dirname(filename)) + "/" + s).c_str());
        } else if (c == 'u') {
            // usemtl
            string s;
            in >> s;
            if (s != "semtl") cout << "Unexpected text following u" << endl;
            while (!!in && in.peek() == ' ' || in.peek() == '\t') in.get(c);
            if (in.peek() == '\n') curMat = 0;
            else if (isdigit(in.peek())) {
                changeMat(curMat, matStartTri);
                in >> curMat;
                matStartTri = triangles.size();
            }
        }
#endif 
    }
//    changeMat(curMat, matStartTri);
    
    if (maxVertex >= vertices.size()) {
        cerr << "Undefined vertex " << maxVertex << endl;
        exit(1);
    }
    
    if (normals.size() == 0) computeNormals(normals, normCount, triangles, vertices);
    else {
        cout << "permuting normals...\n";
        permuteNormals(normals, triangles, vertices, normIndex);
    }
//    centerVertices(vertices);
//    drawDist = computeDrawDist(vertices);
}
*/

#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "shader.h"
#include "texture.h"

using namespace std;

struct Vertex 
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec2 tex_coords;
};

class Mesh 
{
    vector <Vertex> vertices;
    vector <GLuint> indexes;
    vector <Texture2D> textures;
    GLuint vertex_array, vertex_buffer, element_buffer;
public:
    Mesh(vector<Vertex> vertices, vector<GLuint> indexes, vector<Texture2D> textures);
    ~Mesh();
    void render(Shader &shader);
};

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indexes, vector<Texture2D> textures) : vertices(vertices), indexes(indexes), textures(textures)
{
    glGenVertexArrays(1, &vertex_array);
    glGenBuffers(1, &vertex_buffer);
    glGenBuffers(1, &element_buffer);

    glBindVertexArray(vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(GLuint), &indexes[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(GLfloat)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(GLfloat)));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(9 * sizeof(GLfloat)));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(12 * sizeof(GLfloat)));

    glBindVertexArray(0);
}
Mesh::~Mesh()
{
    //glDeleteVertexArrays(1, &vertex_array);
    //glDeleteBuffers(1, &vertex_buffer);
}
void Mesh::render(Shader &shader)
{
    string type;
    int dif_count = 0, spec_count = 0, norm_count = 0;
    for (int i = 0; i < textures.size(); ++i)
    {
        type = textures[i].getType();
        if (type == "diffuse_map")
            shader.setUniform(("material." + type + "[" + to_string(dif_count++) + "]").c_str(), i);
        else if (type == "specular_map")
            shader.setUniform(("material." + type + "[" + to_string(spec_count++) + "]").c_str(), i);
        else if (type == "normal_map")
            shader.setUniform(("material." + type + "[" + to_string(norm_count++) + "]").c_str(), i);

        textures[i].active(GL_TEXTURE0 + i);
    }
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(vertex_array);
    glDrawElements(GL_TRIANGLES, indexes.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


class Model
{
private:
    vector <Mesh> meshes;
    string directory;
    void loadNode(aiNode *node, const aiScene *scene);
    Mesh loadMesh(aiMesh *mesh, const aiScene *scene);
    vector <Texture2D> loadMaterialTextures(aiMaterial *material, aiTextureType type, string type_name);
public:
    Model(const string &path);
    void render(Shader &shader);
};

Model::Model(const string &path) 
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        cout << "Error while loading model: " << importer.GetErrorString() << endl;
        throw -1;
    }
    directory = path.substr(0, path.find_last_of('/'));

    loadNode(scene->mRootNode, scene);
}
void Model::loadNode(aiNode *node, const aiScene *scene)
{
    aiMesh *mesh;
    for (int i = 0; i < node->mNumMeshes; ++i)
    {
        mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(loadMesh(mesh, scene));
    }
    for (int i = 0; i < node->mNumChildren; i++)
        loadNode(node->mChildren[i], scene);
}
Mesh Model::loadMesh(aiMesh* mesh, const aiScene* scene) 
{
    vector <Vertex> vertices;
    vector <GLuint> indexes;
    vector <Texture2D> textures;
    
    for (int i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex;
        glm::vec3 vector;

        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;

        vector.x = mesh->mNormals[i].x;
        vector.y = mesh->mNormals[i].y;
        vector.z = mesh->mNormals[i].z;
        vertex.normal = vector;

        vector.x = mesh->mTangents[i].x;
        vector.y = mesh->mTangents[i].y;
        vector.z = mesh->mTangents[i].z;
        vertex.tangent = vector;

        vector.x = mesh->mBitangents[i].x;
        vector.y = mesh->mBitangents[i].y;
        vector.z = mesh->mBitangents[i].z;
        vertex.bitangent = vector;

        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.tex_coords = vec;
        }
        else
            vertex.tex_coords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    for (int i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; ++j)
            indexes.push_back(face.mIndices[j]);
    }

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
        vector <Texture2D> diffuse_maps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "diffuse_map");
        textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
        vector <Texture2D> specular_maps = loadMaterialTextures(material, aiTextureType_SPECULAR, "specular_map");
        textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
        vector <Texture2D> normal_maps = loadMaterialTextures(material, aiTextureType_HEIGHT, "normal_map");
        textures.insert(textures.end(), normal_maps.begin(), normal_maps.end());
    }
    
    return Mesh(vertices, indexes, textures);
}
vector <Texture2D> Model::loadMaterialTextures(aiMaterial *material, aiTextureType type, string type_name)
{
    vector <Texture2D> textures;
    aiString str;
    for (int i = 0; i < material->GetTextureCount(type); ++i)
    {
        material->GetTexture(type, i, &str);
        Texture2D texture(str.C_Str(), type_name, directory);
        textures.push_back(texture);
    }
    return textures;
}
void Model::render(Shader &shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].render(shader);
}
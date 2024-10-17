#define _CRT_SECURE_NO_WARNINGS (1)
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stddef.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>

#define log(...) printf(__VA_ARGS__)

#define fail(...) do {printf(__VA_ARGS__);exit(0);} while(0)

#define ARRAY_INIT_CAPACITY 8
#define append(array, elem) \
    do { \
        assert((array)->len >= 0); \
        assert((array)->capacity >= 0); \
        assert((array)->len <= (array)->capacity); \
        assert(sizeof(*((array)->data)) == sizeof(elem)); \
        if (((array))->len >= (array)->capacity) { \
            (array)->capacity = (array)->capacity == 0 ? ARRAY_INIT_CAPACITY : (array)->capacity*2; \
            (array)->data = realloc((array)->data, sizeof(elem)*(array)->capacity); \
            assert((array)->data); \
        } \
        ((array))->data[((array))->len] = (elem); \
        ((array))->len += 1; \
    } while (0);

#pragma pack(1)
typedef struct {
    float x, y, z;
} Vector3;

#pragma pack(1)
typedef struct {
    Vector3 normal;
    Vector3 v1;
    Vector3 v2;
    Vector3 v3;
    uint16_t attribute;
} Triangle;

typedef struct {
    Vector3* data;
    size_t len;
    size_t capacity;
} Vertices;

typedef struct {
    Vector3* data;
    size_t len;
    size_t capacity;
} Normals;

typedef struct {
    size_t v1, v2, v3, n;
} Face;

typedef struct {
    Face* data;
    size_t len;
    size_t capacity;
} Faces;

_Static_assert(sizeof(Vector3) == 3*4, "Unsupported pragma pack");
_Static_assert(offsetof(Triangle, attribute) == 48, "Unsupported pragma pack");
_Static_assert(sizeof(Triangle) == 50, "Unsupported pragma pack");

int main(int argc, char** argv) {
    if (argc != 4) {
        fail("Usage: stl2obj <stlfile.stl> <objfile.obj> <modelname>\n");
    }
    FILE* stlfile = fopen(argv[1], "rb");
    if (stlfile == 0) {
        fail("Cannot open stl input file: %s", argv[1]);
    }
    char header[80] = {0};
    if (fread(header, 1, 80, stlfile) != 80) {
        fail("Unsupported file format: 80 byte header missing\n");
    }
    uint32_t number_of_triangles; // Little endian, though unspecified and native maybe?
    if (fread(&number_of_triangles, sizeof(number_of_triangles), 1, stlfile) != 1) {
        fail("Number of triangles not present\n");
    }
    log("Number of triangles %" PRIu32 "\n", number_of_triangles);
    Triangle* triangles = calloc(number_of_triangles, sizeof(Triangle));
    if (!triangles) {
        fail("We are OOMed");
    }
    size_t parsed_triangles;
    if ((parsed_triangles = fread(triangles, sizeof(Triangle), number_of_triangles, stlfile)) != number_of_triangles) {
        fail("Unexpected EOF while parsing\n");
    }
    log("Parsed triangles %zu\n", parsed_triangles);
    fclose(stlfile);
    Vertices vertices = {0};
    Normals normals = {0};
    Faces faces = {0};
    for (size_t i = 0; i < parsed_triangles; i++) {
        append(&vertices, triangles[i].v1);
        size_t v1 = vertices.len-1;
        append(&vertices, triangles[i].v2);
        size_t v2 = vertices.len-1;
        append(&vertices, triangles[i].v3);
        size_t v3 = vertices.len-1;
        append(&normals, triangles[i].normal);
        size_t n = normals.len-1;
        Face f = {v1, v2, v3, n};
        append(&faces, f);
    }
    FILE* embedfile = fopen(argv[2], "wb+");
    log("Outfile %s\n", argv[2]);
    assert(embedfile);
    /*
    typedef struct Model {
        Matrix transform;       // Local transform matrix

        int meshCount;          // Number of meshes
        int materialCount;      // Number of materials
        Mesh *meshes;           // Meshes array
        Material *materials;    // Materials array
        int *meshMaterial;      // Mesh material number

        // Animation data
        int boneCount;          // Number of bones
        BoneInfo *bones;        // Bones information (skeleton)
        Transform *bindPose;    // Bones base transformation (pose)
    } Model;
    */

    /*
    typedef struct Mesh {
    int vertexCount;        // Number of vertices stored in arrays
    int triangleCount;      // Number of triangles stored (indexed or not)

    // Vertex attributes data
    float *vertices;        // Vertex position (XYZ - 3 components per vertex) (shader-location = 0)
    float *texcoords;       // Vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
    float *texcoords2;      // Vertex texture second coordinates (UV - 2 components per vertex) (shader-location = 5)
    float *normals;         // Vertex normals (XYZ - 3 components per vertex) (shader-location = 2)
    float *tangents;        // Vertex tangents (XYZW - 4 components per vertex) (shader-location = 4)
    unsigned char *colors;      // Vertex colors (RGBA - 4 components per vertex) (shader-location = 3)
    unsigned short *indices;    // Vertex indices (in case vertex data comes indexed)

    // Animation vertex data
    float *animVertices;    // Animated vertex positions (after bones transformations)
    float *animNormals;     // Animated normals (after bones transformations)
    unsigned char *boneIds; // Vertex bone ids, max 255 bone ids, up to 4 bones influence by vertex (skinning)
    float *boneWeights;     // Vertex bone weight, up to 4 bones influence by vertex (skinning)

    // OpenGL identifiers
    unsigned int vaoId;     // OpenGL Vertex Array Object id
    unsigned int *vboId;    // OpenGL Vertex Buffer Objects id (default vertex data)
    } Mesh;
    */
    char* modelname = argv[3];
    fprintf(embedfile, "static float %s_mesh_vertices[] = {\n", modelname);
    for (size_t i = 0; i < vertices.len; i++) {
        Vector3 v = vertices.data[i];
        fprintf(embedfile, "%f, %f, %f,\n", v.x, v.y, v.z);
    }
    fprintf(embedfile, "};\n"); //vertices

    fprintf(embedfile, "static float %s_mesh_normals[] = {\n", modelname);
    for (size_t i = 0; i < normals.len; i++) {
        Vector3 vn = normals.data[i];
        fprintf(embedfile, "%f, %f, %f,\n", vn.x, vn.y, vn.z);
    }
    fprintf(embedfile, "};\n"); //normals

    fprintf(embedfile, "Mesh %s_mesh = {\n", modelname);
    fprintf(embedfile, ".vertexCount=%zu,\n",vertices.len);
    fprintf(embedfile, ".triangleCount=%zu,\n", faces.len);
    fprintf(embedfile, ".vertices = %s_mesh_vertices,", modelname);
    fprintf(embedfile, "};\n"); // mesh

    fprintf(embedfile, "%s_mesh.normals=calloc(%zu, %zu);\n", modelname, normals.len*3*3, sizeof(float));

    fprintf(embedfile, "size_t %s_mesh_normals_it = 0;\n", modelname);

    fprintf(embedfile, "for (size_t %s_i = 0; %s_i < %zu; %s_i += 3) {\n", modelname, modelname, 3*normals.len, modelname);
    fprintf(embedfile, "     %s_mesh.normals[%s_mesh_normals_it] =  %s_mesh_normals[%s_i];\n", modelname, modelname, modelname, modelname);
    fprintf(embedfile, "     %s_mesh.normals[%s_mesh_normals_it+1] =  %s_mesh_normals[%s_i+1];\n", modelname, modelname, modelname, modelname);
    fprintf(embedfile, "     %s_mesh.normals[%s_mesh_normals_it+2] =  %s_mesh_normals[%s_i+2];\n", modelname, modelname, modelname, modelname);
    fprintf(embedfile, "     %s_mesh_normals_it += 3;\n", modelname);
    fprintf(embedfile, "     %s_mesh.normals[%s_mesh_normals_it] =  %s_mesh_normals[%s_i];\n", modelname, modelname, modelname, modelname);
    fprintf(embedfile, "     %s_mesh.normals[%s_mesh_normals_it+1] =  %s_mesh_normals[%s_i+1];\n", modelname, modelname, modelname, modelname);
    fprintf(embedfile, "     %s_mesh.normals[%s_mesh_normals_it+2] =  %s_mesh_normals[%s_i+2];\n", modelname, modelname, modelname, modelname);
    fprintf(embedfile, "     %s_mesh_normals_it += 3;\n", modelname);
    fprintf(embedfile, "     %s_mesh.normals[%s_mesh_normals_it] =  %s_mesh_normals[%s_i];\n", modelname, modelname, modelname, modelname);
    fprintf(embedfile, "     %s_mesh.normals[%s_mesh_normals_it+1] =  %s_mesh_normals[%s_i+1];\n", modelname, modelname, modelname, modelname);
    fprintf(embedfile, "     %s_mesh.normals[%s_mesh_normals_it+2] =  %s_mesh_normals[%s_i+2];\n", modelname, modelname, modelname, modelname);
    fprintf(embedfile, "     %s_mesh_normals_it += 3;\n", modelname);
    fprintf(embedfile, "};\n"); // for loop

    fprintf(embedfile, "UploadMesh(&%s_mesh, false);\n", modelname);
    fprintf(embedfile, "Model %s = LoadModelFromMesh(%s_mesh);\n", modelname, modelname);
    fclose(embedfile);
}

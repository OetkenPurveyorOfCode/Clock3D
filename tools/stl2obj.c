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
    if (argc != 3) {
        fail("Usage: stl2obj <stlfile.stl> <objfile.obj>\n");
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
        size_t v1 = vertices.len;
        append(&vertices, triangles[i].v2);
        size_t v2 = vertices.len;
        append(&vertices, triangles[i].v3);
        size_t v3 = vertices.len;
        append(&normals, triangles[i].normal);
        size_t n = normals.len;
        Face f = {v1, v2, v3, n};
        append(&faces, f);
    }
    FILE* objfile = fopen(argv[2], "wb+");
    log("Outfile %s\n", argv[2]);
    assert(objfile);
    for (size_t i = 0; i < vertices.len; i++) {
        Vector3 v = vertices.data[i];
        fprintf(objfile, "v %f %f %f\n", v.x, v.y, v.z);
    }
    for (size_t i = 0; i < normals.len; i++) {
        Vector3 vn = normals.data[i];
        fprintf(objfile, "vn %f %f %f\n", vn.x, vn.y, vn.z);
    }
    for (size_t i = 0; i < faces.len; i++) {
        Face f = faces.data[i];
        fprintf(objfile, "f %zu//%zu %zu//%zu %zu//%zu\n", f.v1, f.n, f.v2, f.n, f.v3, f.n);
    }

    fclose(objfile);
}

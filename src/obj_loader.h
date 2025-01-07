#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: Open the file
//       Read in all of the data into structs or sth
//       Make the data available to the caller

struct Vertex
{
    float x, y, z;
};

struct Face
{
    v3 vertex;
    v3 texcoord;
    v3 normal;
};

struct Mesh
{
    Vertex vertices[10000];
    Face faces[10000];
    int vertices_count;
    int faces_count;
};

internal void LoadObj(char *filepath, Mesh *mesh)
{
    FILE *file = fopen("../../build/mesh/sting.obj", "r");
    if(file == NULL) return;

    int num_vertices = 0;
    int num_faces = 0;

    char buffer[256];
    while(fgets(buffer, sizeof(buffer), file)) 
    {
        if(buffer[0] == 'v' && buffer[1] == ' ')
        {
            sscanf(buffer, "v %f %f %f", &mesh->vertices[num_vertices].x, &mesh->vertices[num_vertices].y, &mesh->vertices[num_vertices].z);
        }
    }
}
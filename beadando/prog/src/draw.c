#include "draw.h"
#include "model.h"

#include <GL/glut.h>

#include <stdio.h>

void draw_model(const struct Model* model, float offsetX, float offsetY, float offsetZ, float scaleX, float scaleY, float scaleZ, float rotX, float rotY, float rotZ)
{
    draw_triangles(model, offsetX, offsetY, offsetZ, scaleX, scaleY, scaleZ, rotX, rotY, rotZ);
    // draw_quads(model);
}

int rand_with_minmax(int minimum_number, int max_number)
{
	return rand() % (max_number + 1 - minimum_number) + minimum_number;
}

void draw_triangles(const struct Model* model, float offsetX, float offsetY, float offsetZ, float scaleX, float scaleY, float scaleZ, float rotX, float rotY, float rotZ)
{
	srand(time(0));
	
    int i, k;
    int vertex_index, texture_index, normal_index;
    double x, y, z, normal_x, normal_y, normal_z, u, v;
	
	int randNum = rand_with_minmax(1, 2);

    for (i = 0; i < model->n_triangles; ++i) 
	{
		glBindTexture(GL_TEXTURE_2D, model->triangles[i].points[0].material_index);
		
		glBegin(GL_TRIANGLES);
		
        for (k = 0; k < 3; ++k) 
		{
            normal_index = model->triangles[i].points[k].normal_index;
            normal_x = model->normals[normal_index].x;
            normal_y = model->normals[normal_index].y;
            normal_z = model->normals[normal_index].z;
            glNormal3d(offsetX + normal_x, offsetY + normal_y, offsetZ + normal_z);
			
            texture_index = model->triangles[i].points[k].texture_index;
			
            u = model->texture_vertices[texture_index].u;
            v = model->texture_vertices[texture_index].v;
			
			glTexCoord2f(u, 1 - v);

            vertex_index = model->triangles[i].points[k].vertex_index;
			
			
			
            x = model->vertices[vertex_index].x * scaleX;
            y = model->vertices[vertex_index].y * scaleY;
            z = model->vertices[vertex_index].z * scaleZ;
            glVertex3d(offsetX + x, offsetY + y, offsetZ + z);
        }
		
		glEnd();
    }
}

void draw_quads(const struct Model* model)
{
    int i, k;
    int vertex_index, texture_index, normal_index;
    double x, y, z, u, v;

    glBegin(GL_QUADS);

    for (i = 0; i < model->n_quads; ++i) {
        for (k = 0; k < 4; ++k) {

            texture_index = model->quads[i].points[k].texture_index;
            u = model->texture_vertices[texture_index].u;
            v = model->texture_vertices[texture_index].v;
            // NOTE: The 1-v is model file specific!
            glTexCoord2f(u, 1-v);

            vertex_index = model->quads[i].points[k].vertex_index;
            x = model->vertices[vertex_index].x;
            y = model->vertices[vertex_index].y;
            z = model->vertices[vertex_index].z;
            glVertex3d(x, y, z);
        }
    }

    glEnd();
}

void draw_normals(const struct Model* model, double length)
{
    int i;
    double x1, y1, z1, x2, y2, z2;

    glColor3f(0, 0, 1);

    glBegin(GL_LINES);

    for (i = 0; i < model->n_vertices; ++i) {
        x1 = model->vertices[i].x;
        y1 = model->vertices[i].y;
        z1 = model->vertices[i].z;
        x2 = x1 + model->normals[i].x * length;
        y2 = y1 + model->normals[i].y * length;
        z2 = z1 + model->normals[i].z * length;
        glVertex3d(x1, y1, z1);
        glVertex3d(x2, y2, z2);
    }

    glEnd();
}

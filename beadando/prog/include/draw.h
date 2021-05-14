#ifndef DRAW_H
#define DRAW_H

#include "model.h"

/**
 * Draw the model.
 */
void draw_model(const struct Model* model, float offsetX, float offsetY, float offsetZ, float scaleX, float scaleY, float scaleZ, float rotX, float rotY, float rotZ);

/**
 * Draw the triangles of the model.
 */
void draw_triangles(const struct Model* model, float offsetX, float offsetY, float offsetZ, float scaleX, float scaleY, float scaleZ, float rotX, float rotY, float rotZ);

/**
 * Draw the quads of the model.
 */
void draw_quads(const struct Model* model);

/**
 * Draw the normal vectors of the model.
 *
 * NOTE: It assumes that all vertex has normal vector with the same index.
 */
void draw_normals(const struct Model* model, double length);

#endif // DRAW_H

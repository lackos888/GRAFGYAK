#include "model.h"
#include "texture.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_BUFFER_SIZE 1024

// TODO: Add line info to the token array for easier debugging!

int count_tokens(const char* text)
{
    int i = 0;
    int is_token = FALSE;
    int count = 0;

    while (text[i] != 0) {
        if (is_token == FALSE && text[i] != ' ') {
            ++count;
            is_token = TRUE;
        }
        else if (is_token == TRUE && text[i] == ' ') {
            is_token = FALSE;
        }
        ++i;
    }

    return count;
}

void extract_tokens(const char* text, struct TokenArray* token_array)
{
    int n_tokens, token_length;
    char* token;
    int i;

    n_tokens = count_tokens(text);

    token_array->tokens = (char**)malloc(n_tokens * sizeof(char*));
    token_array->n_tokens = 0;

    i = 0;
    while (text[i] != 0) {
        if (text[i] != ' ') {
            token_length = calc_token_length(text, i);
            token = copy_token(text, i, token_length);
            insert_token(token, token_array);
            i += token_length;
        }
        else {
            ++i;
        }
    }
}

char* copy_token(const char* text, int offset, int length)
{
    char* token;
    int i;

    token = (char*)malloc((length + 1) * sizeof(char));
    for (i = 0; i < length; ++i) {
        token[i] = text[offset + i];
    }
    token[i] = 0;

    return token;
}

void insert_token(const char* token, struct TokenArray* token_array)
{
    token_array->tokens[token_array->n_tokens] = (char*)token;
    ++token_array->n_tokens;
}

int calc_token_length(const char* text, int start_index)
{
    int end_index, length;

    end_index = start_index;
    while (text[end_index] != 0 && text[end_index] != ' ') {
        ++end_index;
    }
    length = end_index - start_index;

    return length;
}

void free_tokens(struct TokenArray* token_array)
{
    int i;

    for (i = 0; i < token_array->n_tokens; ++i) {
        free(token_array->tokens[i]);
    }
    free(token_array->tokens);
}

void cleanup_material_data(struct MTLMaterialDatas *currentDatas)
{
	if(currentDatas->materialName)
	{
		free(currentDatas->materialName);
		
		currentDatas->materialName = NULL;
	}
	
	int i = 0;
	
	for(i = 0; i < 6; i++)
	{
		char *currPtr = currentDatas->texturesMapped[i];
		
		if(currPtr != NULL)
		{
			free(currPtr);
			
			currentDatas->texturesMapped[i] = NULL;
		}
	}
}

int load_mtl_datas(FILE *mtl_file, struct MTLDatas *model_mtl_datas)
{
    char line[LINE_BUFFER_SIZE];
	
	struct MTLMaterialDatas currentMaterialData = { 0 };

	struct MTLDatas currentDatas = { 0 };
	
	int materialCounter = 0;
	
    while (fgets(line, LINE_BUFFER_SIZE, mtl_file) != NULL) 
	{
		struct TokenArray token_array = {0};
		
		extract_tokens(line, &token_array);
		
		if(token_array.n_tokens < 1)
		{
			free_tokens(&token_array);
			
			continue;
		}
		
		if(!strcmp(token_array.tokens[0], "newmtl"))
		{
			memset(&currentMaterialData, 0, sizeof(struct MTLMaterialDatas));
			
			const char *nameOfMaterial = token_array.tokens[1];
			
			int lenOfName = strlen(nameOfMaterial);
			
			currentMaterialData.materialName = malloc(lenOfName + 1);
			
			memset(currentMaterialData.materialName, 0, lenOfName + 1);
			
			memcpy(currentMaterialData.materialName, nameOfMaterial, lenOfName - 1);
		} else if(!strcmp(token_array.tokens[0], "Ka"))
		{
			memset(&currentMaterialData.ambientColor, 0, sizeof(float) * 3);
			
			sscanf(line, "Ka %f %f %f", &currentMaterialData.ambientColor[0], &currentMaterialData.ambientColor[1], &currentMaterialData.ambientColor[2]);
		} else if(!strcmp(token_array.tokens[0], "Kd"))
		{
			memset(&currentMaterialData.diffuseColor, 0, sizeof(float) * 3);
			
			sscanf(line, "Kd %f %f %f", &currentMaterialData.diffuseColor[0], &currentMaterialData.diffuseColor[1], &currentMaterialData.diffuseColor[2]);
		} else if(!strcmp(token_array.tokens[0], "Ks"))
		{
			memset(&currentMaterialData.specularColor, 0, sizeof(float) * 3);
			
			sscanf(line, "Ks %f %f %f", &currentMaterialData.specularColor[0], &currentMaterialData.specularColor[1], &currentMaterialData.specularColor[2]);
		} else if(!strcmp(token_array.tokens[0], "Ns"))
		{
			currentMaterialData.specularWeight = 0.0;
			
			sscanf(line, "Ns %f", &currentMaterialData.specularWeight);
		} else if(!strcmp(token_array.tokens[0], "d"))
		{
			currentMaterialData.opacity = 0.0;
			
			sscanf(line, "d %f", &currentMaterialData.opacity);
		} else if(!strcmp(token_array.tokens[0], "Tr"))
		{
			currentMaterialData.opacity = 0.0;
			
			sscanf(line, "Tr %f", &currentMaterialData.opacity);
			
			currentMaterialData.opacity = 1.0f - currentMaterialData.opacity;
		} else if(!strcmp(token_array.tokens[0], "Ni"))
		{
			currentMaterialData.opticalDensity = 0.0;
			
			sscanf(line, "Ni %f", &currentMaterialData.opticalDensity);
		} else if(!strcmp(token_array.tokens[0], "map_Kd"))
		{
			const char *currentTextureName = token_array.tokens[1];
			
			int lenOfTextureName = strlen(currentTextureName);
			
			currentMaterialData.texturesMapped[1] = malloc(lenOfTextureName + 1);
			
			memset(currentMaterialData.texturesMapped[1], 0, lenOfTextureName + 1);
			memcpy(currentMaterialData.texturesMapped[1], currentTextureName, lenOfTextureName - 1);
			
			struct TextureReturnValue currRet = load_texture(currentMaterialData.texturesMapped[1]);
			
			currentMaterialData.texturesMappedToOpenGLIds[1] = currRet.textureId;
			
			printf("texture id: %d | name: %s\n", currentMaterialData.texturesMappedToOpenGLIds[1], currentMaterialData.texturesMapped[1]);
			
			memcpy(&model_mtl_datas->currentMaterials[materialCounter], &currentMaterialData, sizeof(struct MTLMaterialDatas));
			
			/*
			int i2 = 0;
			for(i2 = 0; i2 < materialCounter; i2++)
			{
				printf("DEBUG %s i: %d\n", model_mtl_datas->currentMaterials[i2].materialName, i2);
			}
			
			printf("loaded material %s | materialCounter: %d\n", currentMaterialData.materialName, materialCounter);
			*/
			
			materialCounter++;
		}
    }
	
	model_mtl_datas->numOfLoadedMaterials = materialCounter;
	
	return materialCounter;
}

int load_model(const char* filename, struct Model* model)
{
    FILE* obj_file = fopen(filename, "r");
    printf("Load model '%s' ...\n", filename);
    if (obj_file == NULL) {
        printf("ERROR: Unable to open '%s' file!\n", filename);
        return FALSE;
    }
	
	model->lastMTLLoaded = "UNKNOWN";
	
	int lenOfFullPath = strlen(filename);
	
	char mtlFileName[MAX_PATH + 1] = {0};
	memcpy(mtlFileName, filename, lenOfFullPath - 4);
	memcpy(mtlFileName + (lenOfFullPath - 4), ".mtl", 4);
	
	printf("newFileName: %s\n", mtlFileName);
	
	FILE *mtl_file = fopen(mtlFileName, "r");
	
	memset(&model->model_mtlDatas, 0, sizeof(struct MTLDatas));
	
	int numOfMaterials = load_mtl_datas(mtl_file, &model->model_mtlDatas);
	
	printf("numOfMaterials loaded: %d\n", numOfMaterials);
	
	fclose(mtl_file);
	
    count_elements(obj_file, model);
    //if (model->n_normals != 0 && model->n_normals != model->n_vertices) {
    //    printf("ERROR: Invalid number of normal vertices!\n");
    //    return FALSE;
    //}
    //if (model->n_normals == 0) {
    //    model->n_normals = model->n_vertices;
    //}
    create_arrays(model);
    read_elements(obj_file, model);
    //if (model->n_normals != model->n_vertices) {
    //    calc_normals(model);
    //}
	
	fclose(obj_file); //we don't forget cleanup!

    return TRUE;
}

void print_model_info(const struct Model* model)
{
    printf("Vertices: %d\n", model->n_vertices);
    printf("Texture vertices: %d\n", model->n_texture_vertices);
    printf("Normals: %d\n", model->n_normals);
    printf("Triangles: %d\n", model->n_triangles);
    printf("Quads: %d\n", model->n_quads);
}

void free_model(struct Model* model)
{
    free(model->vertices);
    free(model->texture_vertices);
    free(model->normals);
    free(model->triangles);
    free(model->quads);
}

void count_elements(FILE* file, struct Model* model)
{
    char line[LINE_BUFFER_SIZE];

    init_model_counters(model);
    while (fgets(line, LINE_BUFFER_SIZE, file) != NULL) {
        clear_comment(line);
        count_element_in_line(line, model);
    }
}

void read_elements(FILE* file, struct Model* model)
{
    char line[LINE_BUFFER_SIZE];

    init_model_counters(model);
    model->n_vertices = 1;
    model->n_texture_vertices = 1;
    model->n_normals = 1;

    fseek(file, 0, SEEK_SET);
    while (fgets(line, LINE_BUFFER_SIZE, file) != NULL) {
        clear_comment(line);
        read_element_from_line(line, model);
    }
}

void init_model_counters(struct Model* model)
{
    model->n_vertices = 0;
    model->n_texture_vertices = 0;
    model->n_normals = 0;
    model->n_triangles = 0;
    model->n_quads = 0;
}

void clear_comment(char* line)
{
    int i = 0;
    while (line[i] != 0 && line[i] != '#' && line[i] != 0x0D && line[i] != 0x0A) {
        ++i;
    }
    while (line[i] != 0) {
        line[i] = ' ';
        ++i;
    }
}

void count_element_in_line(const char* line, struct Model* model)
{
    struct TokenArray token_array;
    char* first_token;

    extract_tokens(line, &token_array);

    if (token_array.n_tokens > 0) {
        first_token = token_array.tokens[0];
        if (strcmp(first_token, "v") == 0) {
            ++model->n_vertices;
        }
        else if (strcmp(first_token, "vt") == 0) {
            ++model->n_texture_vertices;
        }
        else if (strcmp(first_token, "vn") == 0) {
            ++model->n_normals;
        }
        else if (strcmp(first_token, "f") == 0) {
            if (token_array.n_tokens == 1 + 3) {
                ++model->n_triangles;
            }
            else if (token_array.n_tokens == 1 + 4) {
                ++model->n_quads;
            }
            else {
                printf("WARN: Invalid number of face elements! %d\n", token_array.n_tokens);
            }
        }
    }

    free_tokens(&token_array);
}

void read_element_from_line(const char* line, struct Model* model)
{
    struct TokenArray token_array;
    char* first_token;
    struct Triangle* triangle;
    struct Quad* quad;

    extract_tokens(line, &token_array);

    if (token_array.n_tokens > 0) {
        first_token = token_array.tokens[0];
		
        if (strcmp(first_token, "v") == 0) {
            read_vertex(&token_array, &(model->vertices[model->n_vertices]));
            ++model->n_vertices;
        }
        else if (strcmp(first_token, "vt") == 0) {
            read_texture_vertex(&token_array, &(model->texture_vertices[model->n_texture_vertices]));
            ++model->n_texture_vertices;
        }
        else if (strcmp(first_token, "vn") == 0) {
            read_normal(&token_array, &(model->normals[model->n_normals]));
            ++model->n_normals;
        }
		else if(strcmp(first_token, "usemtl") == 0) {
			char *currToken = token_array.tokens[1];
			
			//printf("currToken: %s\n", currToken);
			
			int len = strlen(currToken);
			
			model->lastMTLLoaded = malloc(len + 1);
			
			memset(model->lastMTLLoaded, 0, len + 1);
			memcpy(model->lastMTLLoaded, currToken, len);
		}
        else if (strcmp(first_token, "f") == 0) {
            if (token_array.n_tokens == 1 + 3) {
                triangle = &(model->triangles[model->n_triangles]);
                read_triangle(&token_array, triangle, model);
                if (is_valid_triangle(triangle, model) == FALSE) {
                    printf("line: '%s'\n", line);
                }
				
				if(model->lastMTLLoaded)
				{
					//printf("loading line2 mtl: %s\n", model->lastMTLLoaded);
				}
				
                ++model->n_triangles;
            }
            else if (token_array.n_tokens == 1 + 4) {
                quad = &(model->quads[model->n_quads]);
                read_quad(&token_array, quad, model);
                if (is_valid_quad(quad, model) == FALSE) {
                    printf("line: '%s'\n", line);
                }
				
				if(model->lastMTLLoaded)
				{
					//printf("loading line mtl: %s\n", model->lastMTLLoaded);
				}
				
                ++model->n_quads;
            }
        }
    }

    free_tokens(&token_array);
}

void create_arrays(struct Model* model)
{
    model->vertices = (struct Vertex*)malloc((model->n_vertices + 1) * sizeof(struct Vertex));
    model->texture_vertices = (struct TextureVertex*)malloc((model->n_texture_vertices + 1) * sizeof(struct TextureVertex));
    model->normals = (struct Vertex*)malloc((model->n_normals + 1) * sizeof(struct Vertex));
    model->triangles = (struct Triangle*)malloc(model->n_triangles * sizeof(struct Triangle));
    model->quads = (struct Quad*)malloc(model->n_quads * sizeof(struct Quad));
}

void read_vertex(const struct TokenArray* token_array, struct Vertex* vertex)
{
    vertex->x = atof(token_array->tokens[1]);
    vertex->y = atof(token_array->tokens[2]);
    vertex->z = atof(token_array->tokens[3]);
}

void read_texture_vertex(const struct TokenArray* token_array, struct TextureVertex* texture_vertex)
{
    texture_vertex->u = atof(token_array->tokens[1]);
    texture_vertex->v = atof(token_array->tokens[2]);
}

void read_normal(const struct TokenArray* token_array, struct Vertex* normal)
{
    normal->x = atof(token_array->tokens[1]);
    normal->y = atof(token_array->tokens[2]);
    normal->z = atof(token_array->tokens[3]);
}

void read_triangle(const struct TokenArray* token_array, struct Triangle* triangle, struct Model *model)
{
    int i;

    for (i = 0; i < 3; ++i) {
        read_face_point(token_array->tokens[i + 1], &triangle->points[i], model);
    }
	
	int currentMaterialIndex = -1;
	
	for(i = 0; i < model->model_mtlDatas.numOfLoadedMaterials; i++)
	{
		struct MTLMaterialDatas *currentMaterialData = &model->model_mtlDatas.currentMaterials[i];

		if(!strcmp(currentMaterialData->materialName, model->lastMTLLoaded))
		{
			currentMaterialIndex = currentMaterialData->texturesMappedToOpenGLIds[1];
			
			break;
		}
	}
	
	triangle->points[0].material_index = currentMaterialIndex;
}

void read_quad(const struct TokenArray* token_array, struct Quad* quad, struct Model *model)
{
    int i;

    for (i = 0; i < 4; ++i) {
        read_face_point(token_array->tokens[i + 1], &quad->points[i], model);
    }
	
	int currentMaterialIndex = -1;
	
	for(i = 0; i < model->model_mtlDatas.numOfLoadedMaterials; i++)
	{
		struct MTLMaterialDatas *currentMaterialData = &model->model_mtlDatas.currentMaterials[i];

		if(!strcmp(currentMaterialData->materialName, model->lastMTLLoaded))
		{
			currentMaterialIndex = currentMaterialData->texturesMappedToOpenGLIds[1];
			
			break;
		}
	}
	
	quad->points[0].material_index = currentMaterialIndex;
}

void read_face_point(const char* text, struct FacePoint* face_point, struct Model *model)
{
    int delimiter_count;
    const char* token;
    int length;

    token = text;
    delimiter_count = count_face_delimiters(text);
	
    if (delimiter_count == 0) {
        face_point->vertex_index = read_next_index(token, &length);
        face_point->texture_index = INVALID_VERTEX_INDEX;
        face_point->normal_index = INVALID_VERTEX_INDEX;
    }
    else if (delimiter_count == 1) {
        face_point->vertex_index = read_next_index(token, &length);
        token += length;
        face_point->texture_index = read_next_index(token, &length);
        face_point->normal_index = INVALID_VERTEX_INDEX;
    }
    else if (delimiter_count == 2) {
        // TODO: Handle the v//n special case!
        face_point->vertex_index = read_next_index(token, &length);
        token += length;
        face_point->texture_index = read_next_index(token, &length);
        token += length;
        face_point->normal_index = read_next_index(token, &length);
    }
    else {
        printf("ERROR: Invalid face token! '%s'", text);
    }
}

int count_face_delimiters(const char* text)
{
    int count, i;

    count = 0;
    i = 0;
    while (text[i] != 0) {
        if (text[i] == '/') {
            ++count;
        }
        ++i;
    }

    return count;
}

int read_next_index(const char* text, int* length)
{
    int i, j, index;
    char buffer[32];

    i = 0;
    while (text[i] != 0 && is_digit(text[i]) == FALSE) {
        ++i;
    }

    if (text[i] == 0) {
        return INVALID_VERTEX_INDEX;
    }

    j = 0;
    while (text[i] != 0 && is_digit(text[i]) == TRUE) {
        buffer[j] = text[i];
        ++i;
        ++j;
    }
    buffer[j] = 0;

    index = atoi(buffer);
    *length = i;

    return index;
}

int is_digit(char c)
{
    if (c >= '0' && c <= '9') {
        return TRUE;
    }
    return FALSE;
}

int is_valid_triangle(const struct Triangle* triangle, const struct Model* model)
{
    int k;

    for (k = 0; k < 3; ++k) {
        if (triangle->points[k].vertex_index >= model->n_vertices) {
            printf("ERROR: Invalid vertex index in a triangle!\n");
            return FALSE;
        }
        if (triangle->points[k].texture_index >= model->n_texture_vertices) {
            printf("ERROR: Invalid texture vertex index in a triangle!\n");
            return FALSE;
        }
        if (triangle->points[k].normal_index >= model->n_normals) {
            printf("ERROR: Invalid normal index in a triangle!\n");
            return FALSE;
        }
    }
    return TRUE;
}

int is_valid_quad(const struct Quad* quad, const struct Model* model)
{
    int k;
    int vertex_index, texture_index, normal_index;

    for (k = 0; k < 4; ++k) {
        vertex_index = quad->points[k].vertex_index;
        texture_index = quad->points[k].texture_index;
        normal_index = quad->points[k].normal_index;
        if (vertex_index < 0 || vertex_index >= model->n_vertices) {
            printf("ERROR: Invalid vertex index in a quad!\n");
            return FALSE;
        }
        if (texture_index < 0 || texture_index >= model->n_texture_vertices) {
            printf("ERROR: Invalid texture vertex index in a quad!\n");
            return FALSE;
        }
        if (normal_index < 0 || normal_index >= model->n_normals) {
            printf("ERROR: Invalid normal index in a quad!");
            return FALSE;
        }
    }
    return TRUE;
}

void calc_normals(struct Model* model)
{
    apply_triangle_normals(model);
    apply_quad_normals(model);
    normalize_vertex_normals(model);
}

void apply_triangle_normals(struct Model* model)
{
    struct Vertex* v[3];
    struct Vertex n;
    int i, k;
    int vertex_index;

    for (i = 0; i < model->n_triangles; ++i) {
        for (k = 0; k < 3; ++k) {
            vertex_index = model->triangles[i].points[k].vertex_index;
            v[k] = &model->vertices[vertex_index];
        }
        calc_triangle_normal(v[0], v[1], v[2], &n);
        for (k = 0; k < 3; ++k) {
            vertex_index = model->triangles[i].points[k].vertex_index;
            model->normals[vertex_index].x += n.x;
            model->normals[vertex_index].y += n.y;
            model->normals[vertex_index].z += n.z;
        }
    }
}

void apply_quad_normals(struct Model* model)
{

}

void normalize_vertex_normals(struct Model* model)
{
    int i;
    double length;

    for (i = 0; i < model->n_normals; ++i) {
        length = calc_length(&model->normals[i]);
        model->normals[i].x /= length;
        model->normals[i].y /= length;
        model->normals[i].z /= length;
    }
}

void calc_triangle_normal(
    const struct Vertex* a,
    const struct Vertex* b,
    const struct Vertex* c,
    struct Vertex* n)
{
    struct Vertex p;
    struct Vertex q;

    p.x = b->x - a->x;
    p.y = b->y - a->y;
    p.z = b->z - a->z;

    q.x = c->x - a->x;
    q.y = c->y - a->y;
    q.z = c->z - a->z;

    n->x = p.y * q.z + q.y * p.z;
    n->y = p.z * q.x + q.z * p.x;
    n->z = p.x * q.y + q.x * p.y;
}

double calc_length(const struct Vertex* v)
{
    double s;
    double length;

    s = v->x * v->x + v->y * v->y + v->z * v->z;
    length = sqrt(s);

    return length;
}

void print_bounding_box(const struct Model* model)
{
    int i;
    double x, y, z;
    double min_x, max_x, min_y, max_y, min_z, max_z;

    if (model->n_vertices == 0) {
        return;
    }

    min_x = model->vertices[0].x;
    max_x = model->vertices[0].x;
    min_y = model->vertices[0].y;
    max_y = model->vertices[0].y;
    min_z = model->vertices[0].z;
    max_z = model->vertices[0].z;

    for (i = 0; i < model->n_vertices; ++i) {
        x = model->vertices[i].x;
        y = model->vertices[i].y;
        z = model->vertices[i].z;
        if (x < min_x) {
            min_x = x;
        }
        else if (x > max_x) {
            max_x = x;
        }
        if (y < min_y) {
            min_y = y;
        }
        else if (y > max_y) {
            max_y = y;
        }
        if (z < min_z) {
            min_z = z;
        }
        else if (z > max_z) {
            max_z = z;
        }
    }

    printf("Bounding box:\n");
    printf("x in [%lf, %lf]\n", min_x, max_x);
    printf("y in [%lf, %lf]\n", min_y, max_y);
    printf("z in [%lf, %lf]\n", min_z, max_z);
}

void scale_model(struct Model* model, double sx, double sy, double sz)
{
    int i;

    for (i = 0; i < model->n_vertices; ++i) {
        model->vertices[i].x *= sx;
        model->vertices[i].y *= sy;
        model->vertices[i].z *= sz;
    }
}

#ifndef TERRAIN_H
#define TERRAIN_H

#include "camera.h"
#include "texture.h"

#include "model.h"

#define LINE_BUFFER_SIZE 1024

struct ModelCache 
{
	char *objectName;
	
	struct Model currentModel;
};

struct ObjectsWithPositionAndRotation
{
	int currentModelCacheEntryIndex;
	
	float posX;
	float posY;
	float posZ;
	
	float rotX;
	float rotY;
	float rotZ;
	
	float scaleX;
	float scaleY;
	float scaleZ;
};

struct Terrain
{
	struct ObjectsWithPositionAndRotation *currentObjects;
	
	struct ModelCache *currentModelCache;
	
	int numberOfModelsCached;
	
	int numberOfObjects;
	
	float defaultScalingX;
	
	float defaultScalingY;
	
	float defaultScalingZ;
};

/**
 * Initialize the terrain, parsing terrain.txt
 */
void init_terrain(struct Terrain* terrain);
/**
 * Draw the terrain objects.
 */
void draw_terrain(const struct Terrain* terrain);

/* Loads model into cache, if it already exists then it returns the correct pointer */
int load_model_into_cache(struct Terrain *currentTerrain, const char *modelName);

/* Gets object bounding box depending on its scale aswell */
struct ModelBoundingBoxStruct get_sized_object_bounding_box(struct Terrain *currentTerrain, struct ObjectsWithPositionAndRotation *currentObject);

/* Parse current line from terrain.txt */

void parse_current_terrain_line(struct Terrain *currentTerrain, char **overallTokens, int numberOfTokens);

#endif /* TERRAIN_H */

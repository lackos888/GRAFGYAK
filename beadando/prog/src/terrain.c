#include "terrain.h"

#include <GL/glut.h>

#include "model.h"
#include "draw.h"

void init_terrain(struct Terrain* terrain)
{
	FILE* terrain_file = fopen("terrain.txt", "r");
	
    if (terrain_file == NULL)
	{
        printf("ERROR: Unable to open terrain file!\n");
        return FALSE;
    }
	
	terrain->defaultScalingX = 1.0f;
	terrain->defaultScalingY = 1.0f;
	terrain->defaultScalingZ = 1.0f;
	
    fseek(terrain_file, 0, SEEK_SET);
	
	terrain->currentModelCache = calloc(20, sizeof(struct ModelCache));
	
	terrain->currentObjects = calloc(1, sizeof(struct ObjectsWithPositionAndRotation));
	
	char line[LINE_BUFFER_SIZE + 1] = { 0 };
	
    while (fgets(line, LINE_BUFFER_SIZE, terrain_file) != NULL)
	{
		char *currentToken = strtok(line, " ");
		
		char **overallTokens = malloc(sizeof(int));
		
		int tokenCounter = 0;
		
		while(currentToken != NULL)
		{
			int currentTokenLen = strlen(currentToken);
			
			overallTokens = realloc(overallTokens, sizeof(int) * (tokenCounter + 1));
			
			overallTokens[tokenCounter] = malloc(currentTokenLen + 1);
			
			memset(overallTokens[tokenCounter], 0, currentTokenLen + 1);
			
			memcpy(overallTokens[tokenCounter], currentToken, currentTokenLen);
			
			currentToken = strtok(NULL, " ");
		
			tokenCounter++;
		}
		
		parse_current_terrain_line(terrain, overallTokens, tokenCounter);
	}
	
	fclose(terrain_file);
}

int load_model_into_cache(struct Terrain *currentTerrain, const char *modelName)
{
	int i = 0;
	
	int lenOfModelName = strlen(modelName);
	
	for(i = 0; i < currentTerrain->numberOfModelsCached; i++)
	{
		if(!strcmp(currentTerrain->currentModelCache[i].objectName, modelName))
		{
			return &currentTerrain->currentModelCache[i];
		}
	}
	
	int nextNum = currentTerrain->numberOfModelsCached++;
	
	currentTerrain->currentModelCache = realloc(currentTerrain->currentModelCache, currentTerrain->numberOfModelsCached * sizeof(struct ModelCache));
	
	currentTerrain->currentModelCache[nextNum].objectName = malloc(lenOfModelName + 1);
	
	memset(currentTerrain->currentModelCache[nextNum].objectName, 0, lenOfModelName + 1);
	
	memcpy(currentTerrain->currentModelCache[nextNum].objectName, modelName, lenOfModelName);

	char newModelName[MAX_PATH + 1] = {0};
	sprintf(newModelName, "%s.obj", modelName);
	
	load_model(newModelName, &currentTerrain->currentModelCache[nextNum].currentModel);

	return nextNum;
}

void parse_current_terrain_line(struct Terrain *currentTerrain, char **overallTokens, int numberOfTokens)
{
	if(!strcmp(overallTokens[0], "setdefaultscale"))
	{
		if(numberOfTokens != 4)
		{
			return;
		}
		
		sscanf(overallTokens[1], "%f", &currentTerrain->defaultScalingX);
		sscanf(overallTokens[2], "%f", &currentTerrain->defaultScalingY);
		sscanf(overallTokens[3], "%f", &currentTerrain->defaultScalingZ);
		
		printf("DefaultScaling set to %f %f %f\n", currentTerrain->defaultScalingX, currentTerrain->defaultScalingY, currentTerrain->defaultScalingZ);
	} else if(strcmp(overallTokens[0], "#") && strlen(overallTokens[0]) > 1)
	{
		char *modelNeedsToBePlaced = overallTokens[0];
		
		float scaleX = 0.0f;
		
		float scaleY = 0.0f;
		
		float scaleZ = 0.0f;
		
		float posX = 0.0f;
		
		float posY = 0.0f;
		
		float posZ = 0.0f;
		
		float rotX = 0.0f;
		
		float rotY = 0.0f;
		
		float rotZ = 0.0f;
	
		if(numberOfTokens == 7) //not including individual scaling
		{
			sscanf(overallTokens[1], "%f", &posX);
			sscanf(overallTokens[2], "%f", &posY);
			sscanf(overallTokens[3], "%f", &posZ);
			sscanf(overallTokens[4], "%f", &rotX);
			sscanf(overallTokens[5], "%f", &rotY);
			sscanf(overallTokens[6], "%f", &rotZ);
			
			scaleX = currentTerrain->defaultScalingX;
			scaleY = currentTerrain->defaultScalingY;
			scaleZ = currentTerrain->defaultScalingZ;
		} else if(numberOfTokens == 10) //including individiual scaling
		{
			sscanf(overallTokens[1], "%f", &scaleX);
			sscanf(overallTokens[2], "%f", &scaleY);
			sscanf(overallTokens[3], "%f", &scaleZ);
			sscanf(overallTokens[4], "%f", &posX);
			sscanf(overallTokens[5], "%f", &posY);
			sscanf(overallTokens[6], "%f", &posZ);
			sscanf(overallTokens[7], "%f", &rotX);
			sscanf(overallTokens[8], "%f", &rotY);
			sscanf(overallTokens[9], "%f", &rotZ);
		}
		
		int nextNum = currentTerrain->numberOfObjects++;

		currentTerrain->currentObjects = realloc(currentTerrain->currentObjects, currentTerrain->numberOfObjects * sizeof(struct ObjectsWithPositionAndRotation));
		
		currentTerrain->currentObjects[nextNum].posX = posX;
		currentTerrain->currentObjects[nextNum].posY = posY;
		currentTerrain->currentObjects[nextNum].posZ = posZ;
		
		currentTerrain->currentObjects[nextNum].rotX = rotX;
		currentTerrain->currentObjects[nextNum].rotY = rotY;
		currentTerrain->currentObjects[nextNum].rotZ = rotZ;
		
		currentTerrain->currentObjects[nextNum].scaleX = scaleX;
		currentTerrain->currentObjects[nextNum].scaleY = scaleY;
		currentTerrain->currentObjects[nextNum].scaleZ = scaleZ;
		
		currentTerrain->currentObjects[nextNum].currentModelCacheEntryIndex = load_model_into_cache(currentTerrain, modelNeedsToBePlaced);
	}
}

struct ModelBoundingBoxStruct get_sized_object_bounding_box(struct Terrain *currentTerrain, struct ObjectsWithPositionAndRotation *currentObject)
{
	const struct ModelCache *cacheEntry = &currentTerrain->currentModelCache[currentObject->currentModelCacheEntryIndex];
	
	const struct Model *currentModel = &cacheEntry->currentModel;
	
	struct ModelBoundingBoxStruct currStruct = { 0 };
	
	memcpy(&currStruct, &currentModel->boundingBox, sizeof(struct ModelBoundingBoxStruct));
	
	currStruct.min_x *= currentObject->scaleX;
	currStruct.max_x *= currentObject->scaleX;
	
	currStruct.min_y *= currentObject->scaleY;
	currStruct.max_y *= currentObject->scaleY;
	
	currStruct.min_z *= currentObject->scaleZ;
	currStruct.max_z *= currentObject->scaleZ;
	
	return currStruct;
}

void draw_terrain(const struct Terrain* terrain)
{
	int i = 0;
	
	for(i = 0; i < terrain->numberOfObjects; i++)
	{
		struct ObjectsWithPositionAndRotation *currentObject = &terrain->currentObjects[i];

		struct ModelCache *cacheEntry = &terrain->currentModelCache[currentObject->currentModelCacheEntryIndex];
		
		draw_model(&cacheEntry->currentModel, currentObject->posX, currentObject->posY, currentObject->posZ, currentObject->scaleX, currentObject->scaleY, currentObject->scaleZ);
	}
}

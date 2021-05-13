#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glut.h>

typedef GLubyte Pixel[3];

struct TextureReturnValue 
{
	GLuint textureId;
	
	int widthOfTexture;
	
	int heightOfTexture;
};

/**
 * Load texture from file and returns with the texture name.
 */
struct TextureReturnValue load_texture(char* filename);

#endif /* TEXTURE_H */


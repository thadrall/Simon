#ifndef TEXTURES_H
#define TEXTURES_H

#include <iostream>
#include <GL/eglew.h>

class Texture {

public:

	unsigned int TextureID;

	Texture(const char* texturePath);

	void use();
};

#endif
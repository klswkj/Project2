#pragma once
#include<SFML/Graphics.hpp>
#include <GL/glew.h>


#include <string>
#include <iostream>


class TextureAtlas
{
public:
	//TODO: add load method and make bind only glBindTexture
	void load(std::string dir) {
		sf::Image img;

		if (!img.loadFromFile(dir)) {
			std::cout << "Error: could not load texture from file" << std::endl;
		}

		glGenTextures(1, &ta_texture);
		glBindTexture(GL_TEXTURE_2D, ta_texture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getSize().x, img.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());

		glBindTexture(GL_TEXTURE_2D, ta_texture);
	}

	void bind() {
		glBindTexture(GL_TEXTURE_2D, ta_texture);
	}

private:
	unsigned int ta_texture;


};
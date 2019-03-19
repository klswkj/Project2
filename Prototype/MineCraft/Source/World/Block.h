#pragma once

#include "BoundingBox.h"
typedef enum {
	BLOCKTYPE_AIR,
	BLOCKTYPE_GRASS,
	BLOCKTYPE_DIRT,
	BLOCKTYPE_STONE,
	BLOCKTYPE_WATER
} BlockType;

typedef struct {
	int frontFace,
		backFace,
		leftFace,
		rightFace,
		topFace,
		bottomFace
		= 0;
} TextureOffsets;

class Block
{
public:

	bool opaque = true;

	BlockType type;
	TextureOffsets textureOffsets;

	Block();
	Block(BlockType type);
	~Block();

	void setType(BlockType type);

	BoundingBox box;
private:
	void setAllFaces(int textureOffset);
};


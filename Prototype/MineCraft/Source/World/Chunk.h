#pragma once

#include <vector>

#include "Block.h"
#include "Camera.h"

const int CHUNK_SIZE = 16;
const int CHUNK_HEIGHT = 64;

class Chunk
{
public:
	Chunk();
	~Chunk();

	bool loaded = false;

	Block blocks[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];

	void initializeBlocks();
	//void addBlock(BlockType type, )
	void generateMesh();
	void renderTerrain(Camera &cam);
	void renderWater(Camera &cam, float elapsedTime);
	void setPosition(glm::vec3 position);

	void bindMeshesToVAO();
	bool meshesBoundToVAO = false;


	void cleanup();

	glm::vec3 position;

	static int getChunkSize();

private:

	void tryAddFace(const float face[18], int i, int j, int k, int adj_i, int adj_j, int adj_k, int textureOffset);

	std::vector<float> terrainMesh;
	std::vector<float> waterMesh;

	glm::mat4 model = glm::mat4(1);

	void addToMesh(std::vector<float> & mesh, const float vertices[18], float xOffset, float yOffset, float zOffset, int textureOffset);

	void addMeshToVAO(unsigned int * vao, unsigned int * vbo, std::vector<float> mesh);

	unsigned int terrainVAO;
	unsigned int waterVAO;

	unsigned int terrainVBO;
	unsigned int waterVBO;

	int terrainMeshFaces;
	int waterMeshFaces;

};


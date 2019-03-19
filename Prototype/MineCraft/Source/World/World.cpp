#include "World.h"

#include "PerlinNoise.hpp"

double freq = 2;
int octaves = 2;
int waterLevel = 13;

const float ROOT_TWO = 1.4142135623730950488016887242097;

World::World(Camera * cam)
{
	startUpdatingChunks(cam);
}


World::~World()
{
	running = false;
	update_chunks_thread.join();
}

void World::render(Camera & cam, float elapsedTime)
{
	glm::vec3 chunkPos = getChunkPosAt(cam.getPosition());

	//Unload chunks if we have exceeded the maximum number of chunks to be rendered
	while (chunks.size() > MAX_CHUNKS) {
		//remove furthest chunk
		std::map<float, int> sorted = sortChunksByDistanceToCamera(cam);

		unloadChunk(sorted.rbegin()->second);
	}

	mutex.lock();
	//Render terrain first
	for (int i = 0; i < chunks.size(); i++) {
		if (isChunkLoaded(i)) {

			if (!chunks[i].meshesBoundToVAO) {
				chunks[i].bindMeshesToVAO();
			}

			chunks[i].renderTerrain(cam);
		}
	}

	//Sort the transparent meshes and render them last to first.
	std::map<float, int> sorted = sortChunksByDistanceToCamera(cam);

	for (std::map<float, int>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
	{
		if (isChunkLoaded(it->second)) {
			chunks[it->second].renderWater(cam, elapsedTime);
		}
	}

	mutex.unlock();
}

Block * World::getBlockAt(int x, int y, int z)
{
	glm::vec3 chunkPosition = glm::vec3(x / CHUNK_SIZE, 0, z / CHUNK_SIZE);

	int index = getChunkAt(chunkPosition);

	if (!isChunkLoaded(index)) {
		return NULL;
	}
	return &chunks[index].blocks[x % CHUNK_SIZE][y][z % CHUNK_SIZE];
}

void World::updateChunks(Camera * cam)
{
	while (running) {
		glm::vec3 chunkPos = getChunkPosAt(cam->getPosition());

		if(chunks.size() <= MAX_CHUNKS) {

			//This for loop allows chunks to be loaded radially around the player.
			int x, y, dx, dy;
			x = y = dx = 0;
			dy = -1;
			int t = SQRT_MAX_CHUNKS;
			int maxI = t * t;
			for (int i = 0; i < maxI; i++) {
				if ((-SQRT_MAX_CHUNKS / 2 <= x) && (x <= SQRT_MAX_CHUNKS / 2) && (-SQRT_MAX_CHUNKS / 2 <= y) && (y <= SQRT_MAX_CHUNKS / 2)) {
					glm::vec3 pos(chunkPos.x + x, 0, chunkPos.z + y);

					if (!isChunkLoaded(pos)) {
						loadChunk(pos);
						break;
					}
				}
				if ((x == y) || ((x < 0) && (x == -y)) || ((x > 0) && (x == 1 - y))) {
					t = dx;
					dx = -dy;
					dy = t;
				}
				x += dx;
				y += dy;
			}
		}
	}

}

void World::startUpdatingChunks(Camera * cam) {

	update_chunks_thread = std::thread(&World::updateChunks, this, cam);
}

int World::getHeightAtXZ(glm::vec2 position)
{

	siv::PerlinNoise noise(1234);

	double fx = 80 / freq, fz = 80 / freq;

	double val = noise.octaveNoise0_1(position.x / fx, position.y / fz, octaves) * (CHUNK_HEIGHT / 2);

	return (int) val;
}

int World::getChunkAt(glm::vec3 position)
{

	for (int i = 0; i < chunks.size(); i++) {
		if (!isChunkLoaded(i))
			continue;

		if (position == chunks[i].position) {
			return i;
		}
	}

	return -1;
}

glm::vec3 World::getChunkPosAt(glm::vec3 position)
{
	int c_x;
	int c_z;

	if (position.x < 0) {
		c_x = (position.x - CHUNK_SIZE) / CHUNK_SIZE;
	}
	else {
		c_x = position.x / CHUNK_SIZE;
	}

	if (position.z < 0) {
		c_z = (position.z - CHUNK_SIZE) / CHUNK_SIZE;
	}
	else {
		c_z = position.z / CHUNK_SIZE;
	}

	return glm::vec3(c_x, 0, c_z);
}

void World::loadChunk(glm::vec3 position)
{
	int x_start = position.x * CHUNK_SIZE;
	int z_start = position.z * CHUNK_SIZE;

	mutex.lock();
	Chunk c;
	mutex.unlock();
	c.setPosition(position);

	//This is the time consuming part of loading a chunk, so we do not want to lock the mutex here.
	//The loaded boolean in Chunk will allow us to check in render if we are finished, so calling render
	//in the middle of loading isn't an issue. But to check that, we must lock the mutex around the initialization
	//of all chunk objects, and ensure it is locked when added to our list of chunks. Otherwise the loaded variable
	//may not be initialized or able to be referenced, and the render function will fail.
	for (int x = x_start; x < x_start + CHUNK_SIZE; x++) {
		for (int z = z_start; z < z_start + CHUNK_SIZE; z++) {
			int h = getHeightAtXZ(glm::vec2(x, z));

			int abs_x = x - x_start;
			int abs_z = z - z_start;

			for (int y = 0; y < CHUNK_HEIGHT; y++) {
				if (y < h) {
					if (y == h - 1 && y >= waterLevel) {
						c.blocks[abs_x][y][abs_z].setType(BLOCKTYPE_GRASS);
					}
					else {
						c.blocks[abs_x][y][abs_z].setType(BLOCKTYPE_DIRT);
					}
				}
				else if (y <= waterLevel) {
					c.blocks[abs_x][y][abs_z].setType(BLOCKTYPE_WATER);
				}
			}


		}
	}

	c.generateMesh();


	mutex.lock();
	c.loaded = true;
	chunks.push_back(c);
	mutex.unlock();
}

bool World::isChunkLoaded(glm::vec3 position)
{
	int index = getChunkAt(position);
	
	return isChunkLoaded(index);
}

bool World::isChunkLoaded(int index)
{
	if (index != -1 && index < chunks.size() && chunks[index].loaded) {
		return true;
	}
	return false;
}

void World::unloadChunk(glm::vec3 position)
{
	int index = getChunkAt(position);
	unloadChunk(index);
}

void World::unloadChunk(int index)
{
	if (isChunkLoaded(index)) {
		mutex.lock();
		chunks[index].cleanup();
		chunks.erase(chunks.begin() + index);
		mutex.unlock();
	}

}

std::map<float, int> World::sortChunksByDistanceToCamera(Camera & cam)
{
	std::map<float, int> sorted;
	for (unsigned int i = 0; i < chunks.size(); i++)
	{
		if (!isChunkLoaded(i))
			continue;
		glm::vec3 b_position = glm::vec3(chunks[i].position.x * CHUNK_SIZE + CHUNK_SIZE / 2,
										chunks[i].position.y * CHUNK_HEIGHT + CHUNK_SIZE / 2,
										chunks[i].position.z * CHUNK_SIZE + CHUNK_SIZE / 2);
		float distance = glm::length(cam.getPosition() - b_position);
		sorted[distance] = i;
	}

	return sorted;
}

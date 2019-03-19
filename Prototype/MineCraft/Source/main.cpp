#include <SFML/Window.hpp>
#include <GL/glew.h>

#include <fstream>
#include <string>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL

#include "Camera.h"
#include "World.h"
#include "TextureAtlas.h"
#include "Skybox.h"
#include "BoundingBox.h"


#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

void processInput(sf::Window &window, float dt);

Camera * camera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT);

bool gameFocused = true;
float elapsedTime = 0.0f;

int main()
{
	//set version of OpenGL
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.majorVersion = 3;
	settings.minorVersion = 3;

	sf::Window window;


	window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "BenCraft", sf::Style::Default, settings);
	window.setVerticalSyncEnabled(true);
	window.setMouseCursorVisible(false);

	//make the SFML window the current OpenGL context
	window.setActive(true);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	//initialize glew (after setting the OpenGL context)
	glewExperimental = GL_TRUE;
	glewInit();

	World world(camera);
	Skybox skybox;
	TextureAtlas textureAtlas;

	//texture loading
	textureAtlas.load("res/textures/TextureAtlas.png");
	skybox.bindCubeMapTexture();


	//Enable z-buffer testing
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClearDepth(1.f);

	//Enable alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(0.0f, 0.3f, 0.8f, 1.0f);

	bool running = true;
	bool wireframe = false;
	sf::Clock clock;
	while (running)
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				running = false;
			}
			else if (event.type == sf::Event::Resized)
			{
				// adjust the viewport when the window is resized
				glViewport(0, 0, event.size.width, event.size.height);
			}
			else if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::M) {
					if (wireframe) {
						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					}
					else {
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
					}

					wireframe = !wireframe;
					std::cout << "Toggled wireframe... wireframe enabled: " << wireframe << std::endl;
				}
				else if (event.key.code == sf::Keyboard::Escape) {
					gameFocused = !gameFocused;
					window.setMouseCursorVisible(!gameFocused);

				}
			}
		}

		float dt = clock.getElapsedTime().asSeconds();
		elapsedTime += dt;
		clock.restart();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (gameFocused) {
			processInput(window, dt);
		}

		skybox.bindCubeMapTexture();
		skybox.render(*camera);
		textureAtlas.bind();
		world.render(*camera, elapsedTime);

		printf("%f\n", camera->getPosition().y);

		window.display();
	}
	world.running = false;
	window.setActive(false);

	return 0;
}


void processInput(sf::Window &window, float dt) {

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
		camera->fpKeyboardMove(FORWARD, dt);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		camera->fpKeyboardMove(LEFT, dt);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		camera->fpKeyboardMove(BACKWARD, dt);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		camera->fpKeyboardMove(RIGHT, dt);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		camera->fpKeyboardMove(UP, dt);
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
		camera->fpKeyboardMove(DOWN, dt);
	}

	//to fix beginning offset, set mouse position to centerScreen on init
	sf::Vector2i windowPos = window.getPosition();
	sf::Vector2i centerScreen = sf::Vector2i(windowPos.x + WINDOW_WIDTH / 2, windowPos.y + WINDOW_HEIGHT / 2);

	sf::Vector2i currentPos = sf::Mouse::getPosition() - centerScreen;
	camera->fpMouseMove(currentPos.x, -currentPos.y);
	sf::Mouse::setPosition(centerScreen);
}

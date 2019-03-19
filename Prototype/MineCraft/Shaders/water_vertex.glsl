#version 330 core
layout (location = 0) in vec3 aPos;
//layout (location = 1) in vec3 aCol;
layout (location = 1) in vec2 aTexCoord;

//out vec3 color;
out vec2 texCoord;

uniform mat4 transform;
uniform float t;

#define PI 3.1415926535897932384626433832795


void main()
{
	float x = t; //+ aPos.x + aPos.z;
	vec3 newPos = vec3(aPos.x, aPos.y - (sin(x*PI/8)+1.4)/6, aPos.z);
    gl_Position = transform * vec4(newPos, 1.0);
	//color = aCol;
	texCoord = aTexCoord;
}
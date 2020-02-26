#version 420

layout(location = 0) in vec3 _position;

uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(_position, 1.0f);
}
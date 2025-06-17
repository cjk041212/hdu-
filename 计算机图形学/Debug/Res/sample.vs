#version 440
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texcoord;
layout (location = 2) in vec3 normal;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform mat4 NM;

out vec4 V_WorldPos;

void main()
{
	V_WorldPos=M*vec4(pos,1.0);
    gl_Position=P*V*V_WorldPos;
}
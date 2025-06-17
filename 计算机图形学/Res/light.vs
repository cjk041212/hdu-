#version 440

layout (location = 0) in vec3 pos; //顶点坐标
layout (location = 1) in vec2 texcoord; //纹理坐标
layout (location = 2) in vec3 normal; //法线

uniform mat4 M;
uniform mat4 V; //观察矩阵
uniform mat4 P; //投影矩阵
uniform mat4 NM;
uniform mat4 LightViewMatrix;
uniform mat4 LightProjectionMatrix;

out vec3 V_Normal; //法线
out vec4 V_WorldPos; //世界坐标，即摆放好了场景
out vec2 V_Texcoord; //纹理坐标
out vec4 V_LightSpaceFragPos;

void main()
{
	V_Normal=mat3(NM)*normal;
	V_WorldPos=M*vec4(pos,1.0); //世界坐标系
	V_Texcoord=texcoord;
	V_LightSpaceFragPos = LightProjectionMatrix * LightViewMatrix * V_WorldPos; 
    gl_Position = P * V * V_WorldPos;  //输出物体场景在标准化坐标系中的位置
}
#version 440

//binding的作用：直接在着色器glsl中绑定纹理单元！！！
layout (binding = 0) uniform sampler2D U_MainTexture; //纹理采样器 通过CPU中roomTexture传入
layout (binding = 1) uniform sampler2D U_ShadowMap; //通过CPU中shadowMap传入



in vec3 V_Normal; //法线位置
in vec4 V_WorldPos; //世界坐标，即摆放好了场景
in vec2 V_Texcoord;  //纹理坐标
in vec4 V_LightSpaceFragPos;

uniform vec3 ViewPos;
uniform int lightOn; 

uniform float sunlight;

layout (location = 0) out vec4 color; //片段着色器要输出的颜色即最终渲染效果

float CalculateShadow()
{
	vec3 fragPos=V_LightSpaceFragPos.xyz/V_LightSpaceFragPos.w;
	fragPos=fragPos*0.5+vec3(0.5);
	float depthInShadowMap=texture2D(U_ShadowMap,fragPos.xy).r;
	float currentDepth=fragPos.z;
	vec2 texelSize=1.0/textureSize(U_ShadowMap,0);
	float shadow=0.0;
	for(int y=-1;y<=1;++y)
	{
		for(int x=-1;x<=1;++x)
		{
			float pcfDepth=texture2D(U_ShadowMap,fragPos.xy+texelSize*vec2(x,y)).r;
			shadow+=(currentDepth-0.001)>pcfDepth?1.0:0.0;
		}
	}
	shadow/=9.0;
	return shadow;
}

void main()
{
	vec3 lightPos=vec3(sunlight,sunlight,sunlight);
	//vec3 lightPos=vec3(1.0,1.0,1.0);
	vec3 L=lightPos;
	L=normalize(L);
	vec3 n=normalize(V_Normal);

	//ambient
	vec4 AmbientLightColor=vec4(0.3,0.3,0.3,1.0);
	vec4 AmbientMaterial=vec4(0.3,0.3,0.3,1.0);
	vec4 ambientColor=AmbientLightColor*AmbientMaterial;

	//diffuse
	vec4 DiffuseLightColor=vec4(2.0,2.0,2.0,1.0);
	vec4 DiffuseMaterial=vec4(0.8,0.8,0.8,1.0);
	vec4 diffuseColor=DiffuseLightColor*DiffuseMaterial*max(0.0,dot(L,n));

	//specular
	vec4 SpecularLightColor=vec4(1.0,1.0,1.0,1.0);
	vec4 SpecularMaterial=vec4(0.4,0.4,0.4,1.0);
	vec3 reflectDir=normalize(reflect(-L,n));
	//inverse view direction
	vec3 viewDir=normalize(ViewPos-V_WorldPos.xyz);
	vec4 specularColor=SpecularLightColor*SpecularMaterial*pow(max(0.0,dot(viewDir,reflectDir)),128.0);
	

	if(lightOn == 1) //灯打开
	{
		color = ambientColor+(diffuseColor+specularColor)*texture2D(U_MainTexture, V_Texcoord);
		color = color*vec4(vec3(1.0-CalculateShadow()),1.0);
	}
	
	if(lightOn == 0) //灯关闭
	{
		color = ambientColor*texture2D(U_MainTexture, V_Texcoord) * 1.5;
	}
}
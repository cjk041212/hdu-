#pragma once 
#define NDEBUG 
#include<GL/freeglut.h>
#include"model.h"
#include<Glm/glm.hpp> //OpenGL Mathematics 基于OpenGL着色语言（GLSL）规范的图形软件的头文件C ++数学库。
#include<Glm/ext.hpp>
#include<vector>

//   定义全局变量

// matrix
glm::mat4 modelMatrix;
glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;
glm::mat4 normalMatrix;
glm::mat4 lightProjectionMatrix;
glm::mat4 lightViewMatrix;

//  texture and program

GLuint livingRoomWallPosLoc, livingRoomWallTexcoordLoc, livingRoomWallNormalLoc;
GLuint livingRoomWallMLoc, livingRoomWallVLoc, livingRoomWallPLoc, livingRoomWallNMLoc, livingRoomWallViewPosLocation;
GLuint livingRoomWallLightVMatrixLoc, livingRoomWallLightPMatrixLoc;
GLuint livingRoomWallLightOnLoc;
GLuint livingRoomWallTextureLoc, livingRoomShadowMapLoc;

std::vector<GLuint> paintings;//客厅挂画纹理
std::vector<GLuint> walls;//客厅墙壁纹理
std::vector<GLuint> floors;//客厅地板纹理
std::vector<GLuint> otherPaintings;//客厅两边挂画纹理
int floorsSize;//vector的长度
int wallsSize;
int paintingsSize;
int OtherPaintingsSize;
GLuint floorTexture = 0;//vector的第几个纹理
GLuint wallTexture = 0;
GLuint paintTexture = 0;
GLuint otherPaintTexture = 0;


GLuint livingRoomWallTexture; //客厅墙壁纹理更换
GLuint roomTexture, shadowMap;  //房间纹理和阴影
GLuint renderProgram, depthProgram, livingRoomWallProgram;
GLuint flowerpotTexture, parterreTexture;

// 渲染所用参数
GLuint renderPosLoc, renderTexcoordLoc, renderNormalLoc;
GLuint renderMLoc, renderVLoc, renderPLoc, renderNMLoc, renderViewPosLocation;
GLuint renderLightVMatrixLoc, renderLightPMatrixLoc;
GLuint renderLightOnLoc;
GLuint renderTextureLoc, shadowMapLoc;

GLfloat rendersunlight;

GLuint depthPosLoc, depthTexcoordLoc, depthNormalLoc;
GLuint depthMLoc, depthVLoc, depthPLoc;


//----------------------------------------------------
//定义模型相关变量
//----------------------------------------------------
//桌子
VertexData* tableVertexData;
int tableVertexCount, tableIndexCount;
unsigned int* tableIndexes;
GLuint tableVbo, tableIbo;

//抽屉
VertexData* drawerVertexData;
int drawerVertexCount, drawerIndexCount;
unsigned int* drawerIndexes;
GLuint drawerVbo, drawerIbo;

//抽屉2
VertexData* drawerVertexData2;
int drawerVertexCount2, drawerIndexCount2;
unsigned int* drawerIndexes2;
GLuint drawerVbo2, drawerIbo2;

//墙壁
VertexData* wallVertexData;
int wallVertexCount, wallIndexCount;
unsigned int* wallIndexes;
GLuint wallVbo, wallIbo;

//地板
VertexData* floorVertexData;
int floorVertexCount, floorIndexCount;
unsigned int* floorIndexes;
GLuint floorVbo, floorIbo;

//厨房物品
VertexData* kitchenVertexData;
int kitchenVertexCount, kitchenIndexCount;
unsigned int* kitchenIndexes;
GLuint kitchenVbo, kitchenIbo;

//书房物品
VertexData* studyRoomVertexData;
int studyRoomVertexCount, studyRoomIndexCount;
unsigned int* studyRoomIndexes;
GLuint studyRoomVbo, studyRoomIbo;

//客厅
VertexData* roomVertexData;
int roomVertexCount, roomIndexCount;
unsigned int* roomIndexes;
GLuint roomVbo, roomIbo;

//客厅  墙壁--画1
VertexData* pVertexData;
int pVertexCount, pIndexCount;
unsigned int* pIndexes;
GLuint pVbo, pIbo;

//客厅  墙壁--画2
VertexData* p2VertexData;
int p2VertexCount, p2IndexCount;
unsigned int* p2Indexes;
GLuint p2Vbo, p2Ibo;

//卧室 床--双人床
VertexData* bedVertexData;
int bedVertexCount, bedIndexCount;
unsigned int* bedIndexes;
GLuint bedVbo, bedIbo;

//卧室 床--单人床
VertexData* bed2VertexData;
int bed2VertexCount, bed2IndexCount;
unsigned int* bed2Indexes;
GLuint bed2Vbo, bed2Ibo;

//卧室 左墙--窗户样式1
VertexData* window1VertexData;
int window1VertexCount, window1IndexCount;
unsigned int* window1Indexes;
GLuint window1Vbo, window1Ibo;

//卧室 左墙--窗户样式2
VertexData* window2VertexData;
int window2VertexCount, window2IndexCount;
unsigned int* window2Indexes;
GLuint window2Vbo, window2Ibo;

//卧室 左墙--窗户样式3
VertexData* window3VertexData;
int window3VertexCount, window3IndexCount;
unsigned int* window3Indexes;
GLuint window3Vbo, window3Ibo;

//卧室 左墙--窗户样式4
VertexData* window4VertexData;
int window4VertexCount, window4IndexCount;
unsigned int* window4Indexes;
GLuint window4Vbo, window4Ibo;

//卧室 正墙--装饰1 空调、画
VertexData* decoVertexData;
int decoVertexCount, decoIndexCount;
unsigned int* decoIndexes;
GLuint decoVbo, decoIbo;

//卧室 正墙--装饰2 书籍
VertexData* deco2VertexData;
int deco2VertexCount, deco2IndexCount;
unsigned int* deco2Indexes;
GLuint deco2Vbo, deco2Ibo;

//卧室 其余物品
VertexData* bedroomVertexData;
int bedroomVertexCount, bedroomIndexCount;
unsigned int* bedroomIndexes;
GLuint bedroomVbo, bedroomIbo;

//----------------------------------------
//模型相关变量定义结束
//----------------------------------------

//----------------------------------------
//定义摄像机相关变量
//----------------------------------------
glm::vec3 cameraPos;//摄像机位置
glm::vec3 cameraCenter;
glm::vec3 cameraForward;

glm::vec3 cameraTarget;//原点位置
glm::vec3 cameraDirection;//摄像机朝向
glm::vec3 up; //上向量
glm::vec3 cameraRight;//右向量
glm::vec3 cameraUp;//上向量
float cameraSpeed = 1.25f; //移动速度
float fov = 60.0f; //摄像机视角，通过监听鼠标滚轮，从而触发增加减小视角，产生放大缩小的效果。
//----------------------------------------
//摄像机相关变量定义结束
//----------------------------------------
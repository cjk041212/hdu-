#include"misc.h" 
#include"FBO.h"
#include"Param.h"
#include<vector>
#pragma comment(lib, "soil.lib")
#pragma comment(lib, "glew32.lib")
#pragma warning(disable:4996) 

FrameBufferObject* mFbo;

bool bCameraRotate = false;   //  摄像机是否允许旋转
float cameraXZRotateAngle = 0;   // 摄像机xz平面内的旋转角度 即左右旋转
float cameraYRotateAngle = 0;   //  摄像机垂直平面内的旋转角度   上下旋转

int lastMousePosX, lastMousePosY;  //  上次鼠标的位置  

void DrawRoom();//绘制房间
void DrawRoomSample();//绘制阴影
//------------------------------------
// 物品方案参数
//------------------------------------
int bed = 0;        //卧室床样式
int wallDeco = 0;   //卧室墙壁装饰
int window = 0;     //卧室窗户样式
int drawer = 0;     //抽屉样式

int lightOn = 1;   // 是否开启光照

float sunlight = 50.0;//光源的位置

/**
* @方法:读取模型
* @说明: VertexData: obj模型的全部顶点数据
*        Vbo:顶点缓冲对象,存储顶点的各类属性信息
*        IBO:索引缓冲对象,存储顶点索引,减少传输顶点数据,提高效率
*
*  注:方法中的变量于Param.h中声明
*/
void readModel() {
	//整体墙壁
	wallVertexData = LoadObjModel("Res/group1/wall.obj", &wallIndexes, wallVertexCount, wallIndexCount);
	wallVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * wallVertexCount, GL_STATIC_DRAW, wallVertexData);
	wallIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * wallIndexCount, GL_STATIC_DRAW, wallIndexes);

	//整体地板
	floorVertexData = LoadObjModel("Res/group1/floor.obj", &floorIndexes, floorVertexCount, floorIndexCount);
	floorVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * floorVertexCount, GL_STATIC_DRAW, floorVertexData);
	floorIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * floorIndexCount, GL_STATIC_DRAW, floorIndexes);

	//客厅内部(不含墙壁)
	roomVertexData = LoadObjModel("Res/group1/livingRoom.obj", &roomIndexes, roomVertexCount, roomIndexCount);
	roomVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * roomVertexCount, GL_STATIC_DRAW, roomVertexData);
	roomIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * roomIndexCount, GL_STATIC_DRAW, roomIndexes);

	//桌子
	tableVertexData = LoadObjModel("Res/group1/table.obj", &tableIndexes, tableVertexCount, tableIndexCount);
	tableVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * tableVertexCount, GL_STATIC_DRAW, tableVertexData);
	tableIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * tableIndexCount, GL_STATIC_DRAW, tableIndexes);

	//客厅抽屉
	drawerVertexData = LoadObjModel("Res/group1/drawerIn.obj", &drawerIndexes, drawerVertexCount, drawerIndexCount);
	drawerVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * drawerVertexCount, GL_STATIC_DRAW, drawerVertexData);
	drawerIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * drawerIndexCount, GL_STATIC_DRAW, drawerIndexes);

	//客厅抽屉2
	drawerVertexData2 = LoadObjModel("Res/group1/drawer.obj", &drawerIndexes2, drawerVertexCount2, drawerIndexCount2);
	drawerVbo2 = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * drawerVertexCount2, GL_STATIC_DRAW, drawerVertexData2);
	drawerIbo2 = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * drawerIndexCount2, GL_STATIC_DRAW, drawerIndexes2);

	//客厅 墙上相框--中间单独
	pVertexData = LoadObjModel("Res/group1/painting1.obj", &pIndexes, pVertexCount, pIndexCount);
	pVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * pVertexCount, GL_STATIC_DRAW, pVertexData);
	pIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * pIndexCount, GL_STATIC_DRAW, pIndexes);

	//客厅 墙上相框--其余相框
	p2VertexData = LoadObjModel("Res/group1/painting2.obj", &p2Indexes, p2VertexCount, p2IndexCount);
	p2Vbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * p2VertexCount, GL_STATIC_DRAW, p2VertexData);
	p2Ibo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * p2IndexCount, GL_STATIC_DRAW, p2Indexes);

	//卧室 床--1 双人床+前方地毯
	bedVertexData = LoadObjModel("Res/group2/bed1.obj", &bedIndexes, bedVertexCount, bedIndexCount);
	bedVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * bedVertexCount, GL_STATIC_DRAW, bedVertexData);
	bedIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * bedIndexCount, GL_STATIC_DRAW, bedIndexes);

	//卧室 床--2 单人床+右侧地毯
	bed2VertexData = LoadObjModel("Res/group2/bed2.obj", &bed2Indexes, bed2VertexCount, bed2IndexCount);
	bed2Vbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * bed2VertexCount, GL_STATIC_DRAW, bed2VertexData);
	bed2Ibo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * bed2IndexCount, GL_STATIC_DRAW, bed2Indexes);

	//卧室 窗户位置--1 窗帘开启
	window1VertexData = LoadObjModel("Res/group2/bedRoomCurtainOn.obj", &window1Indexes, window1VertexCount, window1IndexCount);
	window1Vbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * window1VertexCount, GL_STATIC_DRAW, window1VertexData);
	window1Ibo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * window1IndexCount, GL_STATIC_DRAW, window1Indexes);

	//卧室 窗户位置--2 窗帘关闭
	window2VertexData = LoadObjModel("Res/group2/bedRoomCurtainOff.obj", &window2Indexes, window2VertexCount, window2IndexCount);
	window2Vbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * window2VertexCount, GL_STATIC_DRAW, window2VertexData);
	window2Ibo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * window2IndexCount, GL_STATIC_DRAW, window2Indexes);

	//卧室 窗户位置--3 百叶窗开启
	window3VertexData = LoadObjModel("Res/group2/bedRoomWindowOn.obj", &window3Indexes, window3VertexCount, window3IndexCount);
	window3Vbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * window3VertexCount, GL_STATIC_DRAW, window3VertexData);
	window3Ibo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * window3IndexCount, GL_STATIC_DRAW, window3Indexes);

	//卧室 窗户位置--4 百叶窗拉上
	window4VertexData = LoadObjModel("Res/group2/bedRoomWindowOff.obj", &window4Indexes, window4VertexCount, window4IndexCount);
	window4Vbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * window4VertexCount, GL_STATIC_DRAW, window4VertexData);
	window4Ibo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * window4IndexCount, GL_STATIC_DRAW, window4Indexes);

	//卧室 墙上装饰--1
	decoVertexData = LoadObjModel("Res/group2/bedroomonWall.obj", &decoIndexes, decoVertexCount, decoIndexCount);
	decoVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * decoVertexCount, GL_STATIC_DRAW, decoVertexData);
	decoIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * decoIndexCount, GL_STATIC_DRAW, decoIndexes);

	//卧室 墙上装饰2
	deco2VertexData = LoadObjModel("Res/group2/bedroomonWall2.obj", &deco2Indexes, deco2VertexCount, deco2IndexCount);
	deco2Vbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * deco2VertexCount, GL_STATIC_DRAW, deco2VertexData);
	deco2Ibo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * deco2IndexCount, GL_STATIC_DRAW, deco2Indexes);

	//卧室 其他物品
	bedroomVertexData = LoadObjModel("Res/group2/bedroom.obj", &bedroomIndexes, bedroomVertexCount, bedroomIndexCount);
	bedroomVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * bedroomVertexCount, GL_STATIC_DRAW, bedroomVertexData);
	bedroomIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * bedroomIndexCount, GL_STATIC_DRAW, bedroomIndexes);

	//书房物品
	studyRoomVertexData = LoadObjModel("Res/group3/studyRoom.obj", &studyRoomIndexes, studyRoomVertexCount, studyRoomIndexCount);
	studyRoomVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * studyRoomVertexCount, GL_STATIC_DRAW, studyRoomVertexData);
	studyRoomIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * studyRoomIndexCount, GL_STATIC_DRAW, studyRoomIndexes);

	//厨房物品
	kitchenVertexData = LoadObjModel("Res/group3/kitchen.obj", &kitchenIndexes, kitchenVertexCount, kitchenIndexCount);
	kitchenVbo = CreateBufferObject(GL_ARRAY_BUFFER, sizeof(VertexData) * kitchenVertexCount, GL_STATIC_DRAW, kitchenVertexData);
	kitchenIbo = CreateBufferObject(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * kitchenIndexCount, GL_STATIC_DRAW, kitchenIndexes);
}

void loadTexture() {
	//paintings.push_back(CreateTexture("Res/paintings/paint1.png"));
	paintings.push_back(CreateTexture("Res/paintings/paint2.png"));
	paintings.push_back(CreateTexture("Res/paintings/paint3.png"));
	//paintings.push_back(CreateTexture("Res/paintings/paint4.png"));
	paintingsSize = paintings.size();

	//otherPaintings.push_back(CreateTexture("Res/paintings/paint1.png"));
	otherPaintings.push_back(CreateTexture("Res/paintings/paint2.png"));
	otherPaintings.push_back(CreateTexture("Res/paintings/paint3.png"));
	//otherPaintings.push_back(CreateTexture("Res/paintings/paint4.png"));
	OtherPaintingsSize = otherPaintings.size();

	walls.push_back(CreateTexture("Res/walls/wall1.png"));
	walls.push_back(CreateTexture("Res/walls/wall2.png"));
	walls.push_back(CreateTexture("Res/walls/wall3.png"));
	walls.push_back(CreateTexture("Res/walls/wall4.png"));
	walls.push_back(CreateTexture("Res/colors/red.png"));
	walls.push_back(CreateTexture("Res/colors/white.png"));
	walls.push_back(CreateTexture("Res/colors/yellow.png"));
	walls.push_back(CreateTexture("Res/colors/green.png"));
	walls.push_back(CreateTexture("Res/colors/orange.png"));
	walls.push_back(CreateTexture("Res/colors/gray.png"));
	walls.push_back(CreateTexture("Res/colors/brown.png"));
	walls.push_back(CreateTexture("Res/colors/blue.png"));
	walls.push_back(CreateTexture("Res/colors/pink.png"));
	wallsSize = walls.size();

	floors.push_back(CreateTexture("Res/floors/Floor1.png"));
	floors.push_back(CreateTexture("Res/floors/Floor2.png"));
	floors.push_back(CreateTexture("Res/floors/Floor3.png"));
	floors.push_back(CreateTexture("Res/floors/Floor4.png"));
	floors.push_back(CreateTexture("Res/floors/Floor5.png"));
	floors.push_back(CreateTexture("Res/colors/red.png"));
	floors.push_back(CreateTexture("Res/colors/white.png"));
	floors.push_back(CreateTexture("Res/colors/yellow.png"));
	floors.push_back(CreateTexture("Res/colors/green.png"));
	floors.push_back(CreateTexture("Res/colors/orange.png"));
	floors.push_back(CreateTexture("Res/colors/gray.png"));
	floors.push_back(CreateTexture("Res/colors/brown.png"));
	floors.push_back(CreateTexture("Res/colors/blue.png"));
	floors.push_back(CreateTexture("Res/colors/pink.png"));
	floorsSize = floors.size();
}
void init()
{
	//读取模型=============================================================================
	readModel();
	loadTexture();
	//初始化相机===========================================================================
	cameraPos = glm::vec3(0.0f, 10.0f, 40.0f); //初始化摄像机的位置(x,y,z)
	cameraTarget = glm::vec3(0.0f, 10.0f, 0.0f);//摄像机朝向
	up = glm::vec3(0.0f, 1.0f, 0.0f);//定义上向量 用于叉乘

	cameraDirection = glm::normalize(cameraTarget - cameraPos);//摄像机朝向
	cameraRight = glm::normalize(glm::cross(up, cameraDirection));//右轴
	cameraUp = glm::cross(cameraDirection, cameraRight);//上轴
	cameraSpeed = 2.5f;//设置摄像机速度

	viewMatrix = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
	//=======================================================================================

	//前两个参数指定了平截头体的左右坐标，第三和第四参数指定了平截头体的底部和顶部。
	//通过这四个参数我们定义了近平面和远平面的大小，
	//然后第五和第六个参数则定义了近平面和远平面的距离。
	//lightProjectionMatrix = glm::ortho(-320.0f, 320.0f, -320.0f, 320.0f, 0.1f, 1000.0f);
	lightProjectionMatrix = glm::ortho(-520.0f, 520.0f, -520.0f, 520.0f, 0.1f, 1000.0f);//扩大光源视角，防止重复渲染
	//第一个参数为视锥上下面之间的夹角，第二个参数为宽高比，即视窗的宽/高，第三第四个参数分别为近截面和远界面的深度
	lightViewMatrix = glm::lookAt(glm::vec3(150.0f, 150.0f, 100.0f), glm::vec3(0.0f, 0.0f, -50.0f), glm::vec3(0.0f, 0.0f, -1.0f));

	renderProgram = CreateGPUProgram("Res/light.vs", "Res/light.fs");           // 创建gpu program
	depthProgram = CreateGPUProgram("Res/sample.vs", "Res/sample.fs");
	livingRoomWallProgram = CreateGPUProgram("Res/livingRoomWallTexture.vs", "Res/livingRoomWallTexture.fs"); //创建客厅墙壁着色器程序

	//此段代码统统对应于light.vs 和 light.fs中的内容
	renderPosLoc = 0;  //对应于着色器程序中light顶点着色器中的layout = 0 的pos变量
	renderTexcoordLoc = 1;//对应于着色器程序中light顶点着色器中的layout = 1 的texcoord变量
	renderNormalLoc = 2;//对应于着色器程序中light顶点着色器中的layout = 2 的normal变量
	renderMLoc = glGetUniformLocation(renderProgram, "M"); // 获取gpu program中参数的位置
	renderVLoc = glGetUniformLocation(renderProgram, "V");
	renderPLoc = glGetUniformLocation(renderProgram, "P");
	renderNMLoc = glGetUniformLocation(renderProgram, "NM");
	renderViewPosLocation = glGetUniformLocation(renderProgram, "ViewPos");
	renderTextureLoc = glGetUniformLocation(renderProgram, "U_MainTexture");
	shadowMapLoc = glGetUniformLocation(renderProgram, "U_ShadowMap");
	renderLightVMatrixLoc = glGetUniformLocation(renderProgram, "LightViewMatrix"); //观察矩阵
	renderLightPMatrixLoc = glGetUniformLocation(renderProgram, "LightProjectionMatrix"); //投影矩阵
	renderLightOnLoc = glGetUniformLocation(renderProgram, "lightOn");

	rendersunlight = glGetUniformLocation(renderProgram, "sunlight");

	roomTexture = CreateTexture("Res/Colors.png"); //在misc.cpp中 获取所有的颜色纹理


	depthPosLoc = 0;
	depthTexcoordLoc = 1;
	depthNormalLoc = 2;
	depthMLoc = glGetUniformLocation(depthProgram, "M");
	depthVLoc = glGetUniformLocation(depthProgram, "V");
	depthPLoc = glGetUniformLocation(depthProgram, "P");

	glEnable(GL_DEPTH_TEST);//深度测试

	//以下是FBO技术，用到FBO.h 和 FBO.cpp
	mFbo = new FrameBufferObject;												//FBO.h
	mFbo->AttachColorBuffer("color", GL_COLOR_ATTACHMENT0, 800, 600);			//绑定一个colorbuffer，800,600是屏幕宽高
	mFbo->AttachDepthBuffer("depth", 800, 600);									//绑定一个depthbuffer，800,600是屏幕宽高
	mFbo->Finish();

	mFbo->Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	DrawRoomSample();
	mFbo->Unbind();

	shadowMap = mFbo->GetBuffer("depth");

	glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
}

void display()
{
	mFbo->Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	DrawRoomSample();               //  简单绘制一帧，得到阴影图

	mFbo->Unbind();
	shadowMap = mFbo->GetBuffer("depth");

	glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	DrawRoom();         //  绘制

	glutSwapBuffers();
	glutPostRedisplay();
}


void keyFunc(GLubyte key, int x, int y)          // 键盘交互函数，   ws移动摄像机   c切换方案，  l开关灯
{
	cameraRight = glm::normalize(glm::cross(up, cameraDirection));//根据朝向改变右轴

	switch (key)
	{
		//摄像机移动------------------------------------------------------
	case 'w': case 'W'://前移
		cameraPos += cameraSpeed * cameraDirection;
		cameraTarget += cameraSpeed * cameraDirection;
		break;
	case 's': case 'S'://后移
		cameraPos -= cameraSpeed * cameraDirection;
		cameraTarget -= cameraSpeed * cameraDirection;
		break;
	case 'a': case 'A'://左移
		cameraPos += cameraSpeed * cameraRight;
		cameraTarget += cameraSpeed * cameraRight;
		break;
	case 'd': case 'D'://右移
		cameraPos -= cameraSpeed * cameraRight;
		cameraTarget -= cameraSpeed * cameraRight;
		break;
	case ' ': //飞天
		cameraPos += cameraSpeed * up;
		cameraTarget += cameraSpeed * up;
		break;
	case 'x'://遁地
		cameraPos -= cameraSpeed * up;
		cameraTarget -= cameraSpeed * up;
		break;
	case 'k': case 'K':
		sunlight -= 0.5;
		//printf("%f", sunlight);
		if (sunlight <= 0) {
			sunlight = 0.5;
		}
		break;
	case 'j': case 'J':
		sunlight += 0.5;
		//if (sunlight >= 50.0) sunlight = 50.0;
		break;
	case 'h':case 'H':
		sunlight = 50.0;
		break;
		//物品样式切换------------------------------------------------------
	case '1'://切换卧室床样式
		bed = (bed + 1) % 2;
		break;
	case '2'://切换卧室正墙装饰
		wallDeco = (wallDeco + 1) % 2;
		break;
	case '3'://切换卧室窗户样式
		window = (window + 1) % 4;
		break;
	case '4'://切换抽屉样式
		drawer = (drawer + 1) % 2;
		break;
	case '0'://切换客厅墙壁纹理
		wallTexture++;
		wallTexture %= wallsSize;
		break;
	case '9'://切换客厅地板纹理
		floorTexture++;
		floorTexture %= floorsSize;
		break;
	case '8'://切换客厅中间挂画纹理
		paintTexture++;
		paintTexture %= paintingsSize;
		break;
	case '7'://切换客厅两边纹理
		otherPaintTexture++;
		otherPaintTexture %= OtherPaintingsSize;
		break;
		//开关灯------------------------------------------------------------
	case 'l': case 'L':
		lightOn = lightOn == 1 ? 0 : 1;
		break;
	}
	//viewMatrix = glm::lookAt(cameraPos, cameraCenter, glm::vec3(0.0f, 1.0f, 0.0f));
	viewMatrix = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
	lightViewMatrix = glm::lookAt(glm::vec3(150.0f, 150.0f, 100.0f), glm::vec3(0.0f, 0.0f, -50.0f) + glm::vec3(sunlight, sunlight, 0), glm::vec3(0.0f, 0.0f, -1.0f));

}

void MouseFunc(int button, int state, int x, int y)      // 鼠标函数，  单击右键允许移动摄像头， 松开右键即不允许移动
{
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		bCameraRotate = true;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		bCameraRotate = false;
	}

	//滚轮控制视野的放大和缩小
	if (button == 3)
	{
		//printf("Scroll %s At %d %d\n", "Up" , x, y);
		fov += 1.5f;
		//printf("%f\n", fov);
	}
	if (button == 4) {
		//printf("Scroll %s At %d %d\n", "DOWN", x, y);
		fov -= 1.5f;
		//printf("%f\n", fov);
	}
}

void MotionFunc(int x, int y)       // 鼠标移动函数，  右键摁下移动即摆动摄像头
{
	y = 600 - y;
	if (bCameraRotate)
	{
		if (x > lastMousePosX)
		{
			cameraXZRotateAngle += 0.02f;
			lastMousePosX = x;
		}
		else if (x < lastMousePosX)
		{
			cameraXZRotateAngle -= 0.02f;
			lastMousePosX = x;
		}
		if (y > lastMousePosY)
		{
			cameraYRotateAngle += 0.02f;
			lastMousePosY = y;
		}
		else if (y < lastMousePosY)
		{
			cameraYRotateAngle -= 0.02f;
			lastMousePosY = y;
		}
	}
	cameraDirection.x = sin(cameraXZRotateAngle);
	cameraDirection.z = -cos(cameraXZRotateAngle);
	cameraDirection.y = sin(cameraYRotateAngle);

	cameraDirection = glm::normalize(cameraDirection);

	cameraTarget = cameraPos + cameraDirection;

	viewMatrix = glm::lookAt(cameraPos, cameraTarget, glm::vec3(0.0f, 1.0f, 0.0f));
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("opengl");

	glewInit();

	init();
	glutDisplayFunc(display);  // 设置显示回调函数 -- 每帧执行
	glutKeyboardFunc(keyFunc);
	glutMouseFunc(MouseFunc);
	glutMotionFunc(MotionFunc);
	printf("===============漫游功能================\n");
	printf("输入w、s、d、a前后左右移动...\n");
	printf("输入空格向上移动...\n");
	printf("输入x向下移动...\n");
	printf("通过滚轮控制放大缩小\n");
	printf("===============切换形状================\n");
	printf("输入1切换床...\n");
	printf("输入2切换卧室墙装饰...\n");
	printf("输入3切换卧室窗户样式...\n");
	printf("输入4切换抽屉样式...\n");
	printf("===============切换纹理================\n");
	printf("输入0切换客厅墙壁样式...\n");
	printf("输入9切换客厅地板样式...\n");
	printf("输入8切换客厅中间壁画样式...\n");
	printf("输入7切换客厅两边壁画样式...\n");
	printf("================阳光控制===============\n");
	printf("输入L开关灯...\n");
	printf("输入k太阳落山...\n");
	printf("输入j太阳升起...\n");
	printf("输入h重置阳光...\n");

	glutMainLoop();

	return 0;
}

/*
* @方法：绘制一个物体
* @说明：传入初始方法中生成的vbo、ibo、indexcount来绘制单个物体
*/
void drawOne(GLuint vbo, GLuint ibo, int indexCount) {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);//绑定vbo
	//告诉opengl该如何解析顶点数据！！！ 第一个参数是vs中location的值
	glVertexAttribPointer(renderPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
	glVertexAttribPointer(renderTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
	glVertexAttribPointer(renderNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);        // 利用ibo中的index绘制图形
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void drawOneSample(GLuint vbo, GLuint ibo, int indexCount) {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//告诉opengl该如何解析顶点数据！！！ 第一个参数是vs中location的值
	glVertexAttribPointer(depthPosLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
	glVertexAttribPointer(depthTexcoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 3));
	glVertexAttribPointer(depthNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(float) * 5));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);        // 利用ibo中的index绘制图形
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DrawRoom()
{
	//--------------------------------------------------------------------------------------------------------------
	//准备绘制
	//--------------------------------------------------------------------------------------------------------------
	modelMatrix = glm::scale(2.0f, 2.0f, 2.0f);
	normalMatrix = glm::inverseTranspose(modelMatrix); //模型矩阵，控制平移、旋转、缩放
	/**
	*void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
	*通过一致变量（uniform修饰的变量）引用将一致变量值传入渲染管线。
	*location : uniform的位置。
	*count : 需要加载数据的数组元素的数量或者需要修改的矩阵的数量。
	*transpose : 指明矩阵是列优先(column major)矩阵（GL_FALSE）还是行优先(row major)矩阵（GL_TRUE）。
	*value : 指向由count个元素的数组的指针
	*/

	//更新uniform之前你必须先使用程序（调用glUseProgram)，因为它是在当前激活的着色器程序中设置uniform的。
	glUseProgram(renderProgram);
	//glUniformMatrix4fv想着色器程序传值，第一个参数是位置，2、3不变，第四个就是你要传的矩阵
	glUniformMatrix4fv(renderMLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));             // 向gpu program中传值
	glUniformMatrix4fv(renderVLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));			//这些参数都在47行赋值
	projectionMatrix = glm::perspective(fov, 800.0f / 600.0f, 0.1f, 800.0f);//投影矩阵，因滚轮修改了fov参数，所以需要实时传递矩阵给着色器程序
	glUniformMatrix4fv(renderPLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(renderNMLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	glUniform3fv(renderViewPosLocation, 1, glm::value_ptr(cameraPos));
	//glUniform1i设置每个采样器的方式单纯告诉OpenGL每个着色器采样器属于哪个纹理单元。纹理单元总共有16个
	glUniformMatrix4fv(renderLightPMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightProjectionMatrix));
	glUniformMatrix4fv(renderLightVMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightViewMatrix));
	glUniform1i(renderLightOnLoc, lightOn);

	//光源位置传值
	glUniform1f(rendersunlight, sunlight);

	//绑定纹理单元
	glBindTextureUnit(0, roomTexture);      // 绑定纹理单元0
	glBindTextureUnit(1, shadowMap);

	glEnableVertexAttribArray(renderPosLoc);
	glEnableVertexAttribArray(renderTexcoordLoc);
	glEnableVertexAttribArray(renderNormalLoc);
	//--------------------------------------------------------------------------------------------------------------
	//开始绘制
	//--------------------------------------------------------------------------------------------------------------
	//地板
	glBindTextureUnit(0, floors[floorTexture]);      // 绑定纹理单元0
	drawOne(floorVbo, floorIbo, floorIndexCount);
	glBindTextureUnit(0, roomTexture);
	//墙壁
	glBindTextureUnit(0, walls[wallTexture]);      // 绑定纹理单元0
	drawOne(wallVbo, wallIbo, wallIndexCount);
	glBindTextureUnit(0, roomTexture);

	//桌子
	drawOne(tableVbo, tableIbo, tableIndexCount);
	//厨房物品
	drawOne(kitchenVbo, kitchenIbo, kitchenIndexCount);
	//书房物品
	drawOne(studyRoomVbo, studyRoomIbo, studyRoomIndexCount);
	//客厅物品
	drawOne(roomVbo, roomIbo, roomIndexCount);
	//客厅 墙壁--画1
	glBindTextureUnit(0, paintings[paintTexture]);
	drawOne(pVbo, pIbo, pIndexCount);
	glBindTextureUnit(0, roomTexture);
	//客厅 墙壁--画2
	glBindTextureUnit(0, otherPaintings[otherPaintTexture]);
	drawOne(p2Vbo, p2Ibo, p2IndexCount);
	glBindTextureUnit(0, roomTexture);

	//卧室 床
	if (bed == 0)
		drawOne(bedVbo, bedIbo, bedIndexCount);//床1
	else
		drawOne(bed2Vbo, bed2Ibo, bed2IndexCount);//床2
	//抽屉
	if (drawer == 0)
		drawOne(drawerVbo, drawerIbo, drawerIndexCount);//抽屉1
	else
		drawOne(drawerVbo2, drawerIbo2, drawerIndexCount2);//抽屉2

	//卧室 左墙--窗户
	switch (window) {
	case(0)://样式1 窗帘关闭
		drawOne(window1Vbo, window1Ibo, window1IndexCount); break;
	case(1)://样式2 窗帘打开
		drawOne(window2Vbo, window2Ibo, window2IndexCount); break;
	case(2)://样式3 百叶窗拉下
		drawOne(window3Vbo, window3Ibo, window3IndexCount); break;
	case(3)://样式4 百叶窗拉上
		drawOne(window4Vbo, window4Ibo, window4IndexCount); break;
	}

	//卧室 正墙
	if (wallDeco == 0)
		drawOne(decoVbo, decoIbo, decoIndexCount);// 装饰1 空调、画框
	else
		drawOne(deco2Vbo, deco2Ibo, deco2IndexCount); //装饰2 书籍

	//卧室 其他物品 望远镜 台灯 床头柜 镜子
	drawOne(bedroomVbo, bedroomIbo, bedroomIndexCount);
	glUseProgram(0);
	//--------------------------------------------------------------------------------------------------------------
	//绘制结束
	//--------------------------------------------------------------------------------------------------------------
}

void DrawRoomSample()
{
	modelMatrix = glm::scale(2.0f, 2.0f, 2.0f);
	glUseProgram(depthProgram);
	glUniformMatrix4fv(depthMLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	glUniformMatrix4fv(depthVLoc, 1, GL_FALSE, glm::value_ptr(lightViewMatrix));
	glUniformMatrix4fv(depthPLoc, 1, GL_FALSE, glm::value_ptr(lightProjectionMatrix));

	glEnableVertexAttribArray(depthPosLoc);
	glEnableVertexAttribArray(depthTexcoordLoc);
	glEnableVertexAttribArray(depthNormalLoc);

	//--------------------------------------------------------------------------------------------------------------
	//开始绘制
	//--------------------------------------------------------------------------------------------------------------
	//桌子
	drawOneSample(tableVbo, tableIbo, tableIndexCount);

	//地板
	drawOneSample(floorVbo, floorIbo, floorIndexCount);
	//墙壁
	drawOneSample(wallVbo, wallIbo, wallIndexCount);
	//厨房物品
	drawOneSample(kitchenVbo, kitchenIbo, kitchenIndexCount);
	//书房物品
	drawOneSample(studyRoomVbo, studyRoomIbo, studyRoomIndexCount);
	//客厅物品
	drawOne(roomVbo, roomIbo, roomIndexCount);
	//客厅 墙壁--画1
	drawOneSample(pVbo, pIbo, pIndexCount);
	//客厅 墙壁--画2
	drawOneSample(p2Vbo, p2Ibo, p2IndexCount);

	//卧室 床
	if (bed == 0)
		drawOneSample(bedVbo, bedIbo, bedIndexCount);//床1
	else
		drawOneSample(bed2Vbo, bed2Ibo, bed2IndexCount);//床2
	//抽屉
	if (drawer == 0)
		drawOneSample(drawerVbo, drawerIbo, drawerIndexCount);//抽屉1
	else
		drawOneSample(drawerVbo2, drawerIbo2, drawerIndexCount2);//抽屉2
	//卧室 左墙--窗户
	switch (window) {
	case(0)://样式1 窗帘关闭
		drawOneSample(window1Vbo, window1Ibo, window1IndexCount); break;
	case(1)://样式2 窗帘打开
		drawOneSample(window2Vbo, window2Ibo, window2IndexCount); break;
	case(2)://样式3 百叶窗拉下
		drawOneSample(window3Vbo, window3Ibo, window3IndexCount); break;
	case(3)://样式4 百叶窗拉上
		drawOneSample(window4Vbo, window4Ibo, window4IndexCount); break;
	}

	//卧室 正墙
	if (wallDeco == 0)
		drawOneSample(decoVbo, decoIbo, decoIndexCount);// 装饰1 空调、画框
	else
		drawOneSample(deco2Vbo, deco2Ibo, deco2IndexCount); //装饰2 书籍

	//卧室 其他物品 望远镜 台灯 床头柜 镜子
	drawOneSample(bedroomVbo, bedroomIbo, bedroomIndexCount);
	glUseProgram(0);
}
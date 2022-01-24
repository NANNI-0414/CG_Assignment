
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include <GL/glew.h>
#include <GL/glut.h>


void myKeyboard(unsigned char key, int x, int y);

using namespace std;

GLuint programID;
GLuint matLoc;
GLint pColor;
GLint left_signal = 0;
GLint right_signal = 0;
float transL[16] = { 1.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 1.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 1.0f };
float transR[16] = { 1.0f, 0.0f, 0.0f, 0.0f,
						0.0f, 1.0f, 0.0f, 0.0f,
						0.0f, 0.0f, 1.0f, 0.0f,
						0.0f, 0.0f, 0.0f, 1.0f };


GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path)
{
	//create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	//Read the vertex shader code from the file
	string VertexShaderCode;
	ifstream VertexShaderStream(vertex_file_path, ios::in);
	if (VertexShaderStream.is_open())
	{
		string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}

	//Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const* VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	//Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	vector<char> VertexShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, VertexShaderErrorMessage.data());
	fprintf(stdout, "%s\n", VertexShaderErrorMessage.data());

	//Read the fragment shader code from the file
	string FragmentShaderCode;
	ifstream FragmentShaderStream(fragment_file_path, ios::in);
	if (FragmentShaderStream.is_open())
	{
		string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	//Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const* FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	//Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	vector<char> FragmentShaderErrorMessage(InfoLogLength);
	glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, FragmentShaderErrorMessage.data());
	fprintf(stdout, "%s\n", FragmentShaderErrorMessage.data());

	//Link the program
	fprintf(stdout, "Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	vector<char> ProgramErrorMessage(max(InfoLogLength, int(1)));
	glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, ProgramErrorMessage.data());
	fprintf(stdout, "%s\n", ProgramErrorMessage.data());

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void renderScene(void)
{

	//Clear all pixels
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Let's draw something here
	matLoc = glGetUniformLocation(programID, "trans");
	pColor = glGetAttribLocation(programID, "aColor");

	//왼쪽 정육면체 색 지정(파란색)
	glVertexAttribPointer(pColor, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(sizeof(GLfloat)));
	glEnableVertexAttribArray(pColor);

	//왼쪽 정육면체를 이동시키는 키보드가 눌리면 
	if (left_signal == 1) {
		glDisableVertexAttribArray(pColor);
		glVertexAttrib4f(pColor, 0.0, 1.0, 0.0, 1.0);
	}
	else
		glEnableVertexAttribArray(pColor);


	//왼쪽 정육면체 그리기
	glUniformMatrix4fv(matLoc, 1, GL_FALSE, transL);
	glDrawArrays(GL_TRIANGLES, 0, 36);



	//오른쪽 정육면체 색 지정(빨간색)
	glVertexAttribPointer(pColor, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));
	glEnableVertexAttribArray(pColor);

	//왼쪽 정육면체를 이동시키는 키보드가 눌리면 
	if (right_signal == 1) {
		glDisableVertexAttribArray(pColor);
		glVertexAttrib4f(pColor, 0.0, 1.0, 0.0, 1.0);
	}
	else
		glEnableVertexAttribArray(pColor);


	//오른쪽 정육면체 그리기
	glUniformMatrix4fv(matLoc, 1, GL_FALSE, transR);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	//Double buffer
	glutSwapBuffers();
}

void init()
{
	//initilize the glew and check the errors.
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s' \n", glewGetErrorString(res));
	}

	//select the background color
	glClearColor(0.0, 0.0, 0.0, 1.0);

	glEnable(GL_POINT_SPRITE);	//추가
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);



}

void myKeyboard(unsigned char key, int x, int y) {

	//'a', 'w' 키로 왼쪽 6면체의 y좌표를 변경할수 있습니다.
	//a: 왼쪽 큐브가 y좌표를 0.1만큼 이동
	//w: 왼쪽 큐브가 y좌표를 - 0.1만큼 이동
	if (key == 'a') {
		left_signal = 1;
		right_signal = 0;
		transL[13] += -0.1f;
		printf("a: 왼쪽 큐브 y좌표: %f\n", transL[13]);
	}
	else if (key == 'w') {
		left_signal = 1;
		right_signal = 0;
		transL[13] += 0.1f;
		printf("w: 왼쪽 큐브 y좌표: %f\n", transL[13]);
	}

	//'s', 'd' 키로 왼쪽 6면체의 x좌표를 변경할수 있습니다.  문제.
	//s: 왼쪽 큐브 x좌표를 0.1만큼 이동
	//d: 왼쪽 큐브 x좌표를 -0.1만큼 이동
	if (key == 's') {
		left_signal = 1;
		right_signal = 0;
		transL[12] += -0.1f;
		printf("s: 왼쪽 큐브 x좌표: %f\n", transL[12]);
	}
	else if (key == 'd') {
		left_signal = 1;
		right_signal = 0;
		transL[12] += 0.1f;
		printf("d: 왼쪽 큐브 x좌표: %f\n", transL[12]);
	}

	//'z', 'x' 키로 왼쪽 6면체의 z좌표를 변경할수 있습니다.
	if (key == 'z') {
		left_signal = 1;
		right_signal = 0;
		transL[14] += -0.1f;
		printf("z: 왼쪽 큐브 z좌표: %f\n", transL[14]);
	}
	else if (key == 'x') {
		left_signal = 1;
		right_signal = 0;
		transL[14] += 0.1f;
		printf("x: 왼쪽 큐브 z좌표: %f\n", transL[14]);
	}


	//'i', 'j' 키로 오른쪽 6면체의 y좌표를 변경할수 있습니다.
	if (key == 'i') {
		left_signal = 0;
		right_signal = 1;
		transR[13] += 0.1f;
		printf("i: 오른쪽 큐브 y좌표: %f\n", transR[13]);
	}
	else if (key == 'j') {
		left_signal = 0;
		right_signal = 1;
		transR[13] += -0.1f;
		printf("j: 오른쪽 큐브 y좌표: %f\n", transR[13]);
	}


	//'k', 'l' 키로 오른쪽 6면체의 x좌표를 변경할수 있습니다.: 문제
	if (key == 'k') {
		left_signal = 0;
		right_signal = 1;
		transR[12] += -0.1f;
		printf("k: 오른쪽 큐브 x좌표: %f\n", transR[12]);
	}
	else if (key == 'l') {
		left_signal = 0;
		right_signal = 1;
		transR[12] += 0.1f;
		printf("l: 오른쪽 큐브 x좌표: %f\n", transR[12]);
	}

	//'n', 'm' 키로 오른쪽 6면체의 z좌표를 변경할수 있습니다.
	if (key == 'n') {
		left_signal = 0;
		right_signal = 1;
		transR[14] += -0.1f;
		printf("n: 오른쪽 큐브 z좌표: %f\n", transR[14]);
	}
	else if (key == 'm') {
		left_signal = 0;
		right_signal = 1;
		transR[14] += 0.1f;
		printf("m: 오른쪽 큐브 z좌표: %f\n", transR[14]);
	}

	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	GLfloat cubeVertices[] = {
	0.1f, 0.1f,-0.1f, // triangle 2 : begin
	-0.1f,-0.1f,-0.1f,
	-0.1f, 0.1f,-0.1f, // triangle 2 : end
	0.1f, 0.1f,-0.1f,
	0.1f,-0.1f,-0.1f,
	-0.1f,-0.1f,-0.1f,

	-0.1f,-0.1f,-0.1f, // triangle 1 : begin
	-0.1f,-0.1f, 0.1f,
	-0.1f, 0.1f, 0.1f, // triangle 1 : end
	-0.1f,-0.1f,-0.1f,
	-0.1f, 0.1f, 0.1f,
	-0.1f, 0.1f,-0.1f,

	-0.1f, 0.1f, 0.1f,
	-0.1f,-0.1f, 0.1f,
	0.1f,-0.1f, 0.1f,
	0.1f, 0.1f, 0.1f,
	-0.1f, 0.1f, 0.1f,
	0.1f,-0.1f, 0.1f,

	0.1f, 0.1f, 0.1f,
	0.1f,-0.1f,-0.1f,
	0.1f, 0.1f,-0.1f,
	0.1f,-0.1f,-0.1f,
	0.1f, 0.1f, 0.1f,
	0.1f,-0.1f, 0.1f,

	0.1f,-0.1f, 0.1f,
	-0.1f,-0.1f,-0.1f,
	0.1f,-0.1f,-0.1f,
	0.1f,-0.1f, 0.1f,
	-0.1f,-0.1f, 0.1f,
	-0.1f,-0.1f,-0.1f,

	0.1f, 0.1f, 0.1f,
	0.1f, 0.1f,-0.1f,
	-0.1f, 0.1f,-0.1f,
	0.1f, 0.1f, 0.1f,
	-0.1f, 0.1f,-0.1f,
	-0.1f, 0.1f, 0.1f,
	};
	GLfloat cubeColors[] = {
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,		//삼각형 2개로 이루어진 큐브 한 면의 점 색깔

		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,		//2

		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,		//3

		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,		//4

		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,		//5

		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0,
		1.0, 0.0, 0.0, 1.0, 1.0		//6
	};

	transL[12] = -0.5f;
	transR[12] = 0.5f;

	//init GLUT and create Window
	//initialize the GLUT
	glutInit(&argc, argv);
	//GLUT_DOUBLE enables double buffering (drawing to a background buffer while the other buffer is displayed)
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	//These two functions are used to define the position and size of the window. 
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(480, 480);
	//This is used to define the name of the window.
	glutCreateWindow("작품명: 이동하는 정육면체");


	//call initization function
	init();

	//1.
	//Generate VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	//3. 
	programID = LoadShaders("VertexShader8.txt", "FragmentShader8.txt");
	glUseProgram(programID);


	//create two arrays
	GLuint VAO[1];
	glGenVertexArrays(1, VAO);
	//Bind the ﬁrst array
	glBindVertexArray(VAO[0]);

	//Create two Buffers
	GLuint VBO[1];
	glGenBuffers(1, VBO);	//vertex buffer
	//Bind the first buffer	
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	//Put data
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	//Set attribute pointer
	GLint posAttribLoc = glGetAttribLocation(programID, "vtxPosition");
	glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));
	//Enable attribute arrays
	glEnableVertexAttribArray(posAttribLoc);



	//Color Buffer
	GLuint color_VBO[1];
	glGenBuffers(1, color_VBO);	//color buffer
	glBindBuffer(GL_ARRAY_BUFFER, color_VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColors), cubeColors, GL_STATIC_DRAW);


	glutDisplayFunc(renderScene);
	glutKeyboardFunc(myKeyboard);

	//enter GLUT event processing cycle
	glutMainLoop();

	glDeleteVertexArrays(1, &VertexArrayID);

	return 1;
}


#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



//void myKeyboard(unsigned char key, int x, int y);

using namespace std;

GLuint programID;
GLuint matLoc;
GLint pColor;
//glm::mat4 Model = glm::mat4(1.0f);
//GLint left_signal = 0;
//GLint right_signal = 0;
GLint screenX, screenY;
GLfloat ndcX, ndcY;


glm::mat4 Model = glm::mat4(1.0f);	//4차원 항등행렬을 만듦


float trans[16] = { 1.0f, 0.0f, 0.0f, 0.0f,
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
	matLoc = glGetUniformLocation(programID, "transform");
	pColor = glGetAttribLocation(programID, "aColor");


	glUniformMatrix4fv(matLoc, 1, GL_FALSE, &Model[0][0]); //행렬을 shader에 줌


	//정육면체 색 지정
	glVertexAttribPointer(pColor, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));
	glEnableVertexAttribArray(pColor);


	//정육면체 그리기
	//glUniformMatrix4fv(matLoc, 1, GL_FALSE, trans);
	glDrawArrays(GL_TRIANGLES, 0, 36);


	//Double buffer
	glutSwapBuffers();
	glutPostRedisplay();
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


/*
//마우스 함수
void myMouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		screenX = x;
		screenY = y;
		printf("Screen Coordinate: (%d, %d)\n", x, y);

		ndcX = (screenX / 0.5) / 479 - 1.0;
		ndcY = -((screenY / 0.5) / 479 - 1.0);
		printf("Normalized Coordinate: (%f, %f)\n\n", ndcX, ndcY);


		glm::vec3 z(-ndcX, -ndcY, 0);	//z축 회전 벡터
		glm::vec3 y(0, 1, 0);	//y축 회전 벡터(임의)

		z = glm::normalize(z);	//z축 회전벡터의 크기를 1로 만듦
		y = glm::normalize(y);	//y축 회전벡터의 크기를 1로 만듦

		glm::vec3 x = glm::cross(y, z);	//y와 z축 회전벡터를 내적해, x축 회전 벡터를 구함
		x = glm::normalize(x);	//x축 회전벡터의 크기를 1로 만듦

		y = glm::cross(z, x);	//임의로 설정되었던 y축 회전벡터를 구함
		y = glm::normalize(y);	//y축 회전벡터의 크기를 1로 만듦

		glm::mat4 Model = glm::mat4(1.0f);	//4차원 항등행렬을 만듦
		Model[0] = glm::vec4(x, 1.0f);	//4차원 행렬의 0번 행에 x축 회전벡터를 넣음
		Model[1] = glm::vec4(y, 1.0f);	//4차원 행렬의 1번 행에 y축 회전벡터를 넣음
		Model[2] = glm::vec4(z, 1.0f);	//4차원 행렬의 2번 행에 z축 회전벡터를 넣음

		glUniformMatrix4fv(matLoc, 1, GL_FALSE, &Model[0][0]);	//행렬을 shader에 줌



		glutPostRedisplay();
	}

}

void  myKeyboard(unsigned char key, int x, int y)
{
	GLfloat curX = 0, curY = 0;

	switch (key) {
	case 27:
		exit(0);
		break;
	case 'w':
		curX = float(10.0f);
		Model = glm::rotate(
			Model,
			glm::radians(curX * 10) * 3.14f / 180.0f,
			glm::vec3(-1.0f, 0.0f, 0.0f)
		);
		break;
	case 'W':
		curX = float(10.0f);
		Model = glm::rotate(
			Model,
			glm::radians(curX * 10) * 3.14f / 180.0f,
			glm::vec3(-1.0f, 0.0f, 0.0f)
		);
		break;
	case 's':
		curX = float(10.0f);
		Model = glm::rotate(
			Model,
			glm::radians(curX * 10) * 3.14f / 180.0f,
			glm::vec3(1.0f, 0.0f, 0.0f)
		);
		break;
	case 'S':
		curX = float(10.0f);
		Model = glm::rotate(
			Model,
			glm::radians(curX * 10) * 3.14f / 180.0f,
			glm::vec3(1.0f, 0.0f, 0.0f)
		);
		break;
	case 'd':
		curY = float(10.0f);
		Model = glm::rotate(
			Model,
			glm::radians(curY * 10) * 3.14f / 180.0f,
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
		break;
	case 'D':
		curY = float(10.0f);
		Model = glm::rotate(
			Model,
			glm::radians(curY * 10) * 3.14f / 180.0f,
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
		break;
	case 'a':
		curY = float(10.0f);
		Model = glm::rotate(
			Model,
			glm::radians(curY * 10) * 3.14f / 180.0f,
			glm::vec3(0.0f, -1.0f, 0.0f)
		);
		break;
	case 'A':
		curY = float(10.0f);
		Model = glm::rotate(
			Model,
			glm::radians(curY * 10) * 3.14f / 180.0f,
			glm::vec3(0.0f, -1.0f, 0.0f)
		);
		break;
	}
}
*/
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
		1.0, 0.0, 0.0, 1.0,

		0.0, 1.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,
		0.0, 1.0, 0.0, 1.0,

		0.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 1.0,
		0.0, 0.0, 1.0, 1.0,

		1.0, 1.0, 0.0, 1.0,
		1.0, 1.0, 0.0, 1.0,
		1.0, 1.0, 0.0, 1.0,
		1.0, 1.0, 0.0, 1.0,
		1.0, 1.0, 0.0, 1.0,
		1.0, 1.0, 0.0, 1.0,

		1.0, 0.0, 1.0, 1.0,
		1.0, 0.0, 1.0, 1.0,
		1.0, 0.0, 1.0, 1.0,
		1.0, 0.0, 1.0, 1.0,
		1.0, 0.0, 1.0, 1.0,
		1.0, 0.0, 1.0, 1.0,

		0.0, 1.0, 1.0, 1.0,
		0.0, 1.0, 1.0, 1.0,
		0.0, 1.0, 1.0, 1.0,
		0.0, 1.0, 1.0, 1.0,
		0.0, 1.0, 1.0, 1.0,
		0.0, 1.0, 1.0, 1.0,
	};

	//init GLUT and create Window
	//initialize the GLUT
	glutInit(&argc, argv);
	//GLUT_DOUBLE enables double buffering (drawing to a background buffer while the other buffer is displayed)
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	//These two functions are used to define the position and size of the window. 
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(480, 480);
	//This is used to define the name of the window.
	glutCreateWindow("작품명: 주사위 굴리기");


	//call initization function
	init();

	//1.
	//Generate VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	//3. 
	programID = LoadShaders("VertexShader9.txt", "FragmentShader9.txt");
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


	//회전을 위해 추가한 부분
	glm::vec3 z(1, 2, -3);	//z축 회전 벡터
	glm::vec3 y(0, 1, 0);	//y축 회전 벡터(임의)

	z = glm::normalize(z);	//z축 회전벡터의 크기를 1로 만듦
	y = glm::normalize(y);	//y축 회전벡터의 크기를 1로 만듦

	glm::vec3 x = glm::cross(y, z);	//y와 z축 회전벡터를 내적해, x축 회전 벡터를 구함
	x = glm::normalize(x);	//x축 회전벡터의 크기를 1로 만듦

	y = glm::cross(x, z);		//임의로 설정되었던 y축 회전벡터를 구함
	y = glm::normalize(y);	//y축 회전벡터의 크기를 1로 만듦

	//glm::mat4 Model = glm::mat4(1.0f);	//4차원 항등행렬을 만듦
	Model[0] = glm::vec4(x, 0.0f);	//4차원 행렬의 0번 행에 x축 회전벡터를 넣음
	Model[1] = glm::vec4(y, 0.0f);	//4차원 행렬의 1번 행에 y축 회전벡터를 넣음
	Model[2] = glm::vec4(z, 0.0f);	//4차원 행렬의 2번 행에 z축 회전벡터를 넣음
	Model[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);


	glutDisplayFunc(renderScene);
	//glutMouseFunc(myMouse);
	//glutKeyboardFunc(myKeyboard);


	//enter GLUT event processing cycle
	glutMainLoop();

	glDeleteVertexArrays(1, &VertexArrayID);

	return 1;
}

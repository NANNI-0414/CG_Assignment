
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include <GL/glew.h>
#include <GL/glut.h>

void myMouse(int button, int state, int x, int y);
void myKeyboard(unsigned char key, int x, int y);

using namespace std;

GLuint programID;
GLint screenX, screenY;
GLfloat ndcX, ndcY;
GLuint cnt = 0;
GLfloat vertices[16] = { 0.5, 0.5, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0, 5.0 };

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
	glutMouseFunc(myMouse);
	cnt++;
	glutKeyboardFunc(myKeyboard);


	//추가한 부분1
	GLuint m_VBO[1];

	glGenBuffers(1, m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);	//sizeof(GLfloat) * 16

	GLint posAttribLoc = glGetAttribLocation(programID, "inPos");
	GLint pColor = glGetAttribLocation(programID, "aColor");

	//VBO[0]의 bind가 들어가야 하는 위치
	glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 8, (GLvoid*)(0));
	glVertexAttribPointer(pColor, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 7, (GLvoid*)(sizeof(GLfloat) * 3));

	glEnableVertexAttribArray(posAttribLoc);
	glEnableVertexAttribArray(pColor);
	//추가한 부분 1 끝


	//Clear all pixels
	glClear(GL_COLOR_BUFFER_BIT);
	//Let's draw something here

	//define the size of point and draw a point.
	glDrawArrays(GL_POINTS, 0, 2);
	glDrawArrays(GL_LINES, 0, 2);


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
	glClearColor(0.373, 0.0, 1.0, 1.0);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

}


void myMouse(int button, int state, int x, int y) {

	//마우스 오른쪽 버튼이 눌렸을 경우
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		//색깔 바꿔주는 거 추가하기
		if (cnt % 3 == 0) {
			vertices[3] = 1.0;	vertices[4] = 0.0; vertices[5] = 0.0;
			vertices[10] = 1.0;	vertices[11] = 0.0;	vertices[12] = 0.0;
		}

		if (cnt % 3 == 1) {
			vertices[3] = 0.0;	vertices[4] = 1.0; vertices[5] = 0.0;
			vertices[10] = 0.0;	vertices[11] = 1.0;	vertices[12] = 0.0;
		}

		if (cnt % 3 == 2) {
			vertices[3] = 0.0;	vertices[4] = 0.0; vertices[5] = 1.0;
			vertices[10] = 0.0;	vertices[11] = 0.0;	vertices[12] = 1.0;
		}
	}
}

//키보드 함수 추가 
void myKeyboard(unsigned char key, int x, int y) {
	//s키를 눌렀을 때
	if (key == 's') {
		//시작점 색깔 바꾸기: 시작점을 검정색으로 바꾼다. 
		vertices[3] = 0.0;	vertices[4] = 0.0;	vertices[5] = 0.0;
	}

	//e키를 눌렀을 때
	else if (key == 'e') {
		//끝점 색깔 바꾸기: 끝점을 흰색으로 바꾼다. 
		vertices[10] = 1.0;	vertices[11] = 1.0;	vertices[12] = 1.0;
	}

	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	//init GLUT and create Window
	//initialize the GLUT
	glutInit(&argc, argv);
	//GLUT_DOUBLE enables double buffering (drawing to a background buffer while the other buffer is displayed)
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	//These two functions are used to define the position and size of the window. 
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(480, 480);
	//This is used to define the name of the window.
	glutCreateWindow("작품명: 별똥별");

	//call initization function
	init();

	//1.
	//Generate VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	//3. 
	programID = LoadShaders("VertexShader6_2.txt", "FragmentShader6_2.txt");


	glUseProgram(programID);


	glutDisplayFunc(renderScene);

	//enter GLUT event processing cycle
	glutMainLoop();

	glDeleteVertexArrays(1, &VertexArrayID);

	return 1;
}

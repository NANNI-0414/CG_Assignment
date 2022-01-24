
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
vector<GLfloat> vertices = {};
GLfloat pSize = 5.0;

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
	glutKeyboardFunc(myKeyboard);

	GLuint Buffers[1];	//버퍼 아이디를 저장할 배열


	glGenBuffers(1, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);


	GLint posAttrib = glGetAttribLocation(programID, "vtxPosition");
	glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 4, (void*)(0));	//여기를 바꿔야할 거 같음. 
	glEnableVertexAttribArray(posAttrib);


	//size pointer처리
	GLint sizeAttrib = glGetAttribLocation(programID, "size");
	glVertexAttribPointer(sizeAttrib, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(sizeAttrib);


	//Clear all pixels
	glClear(GL_COLOR_BUFFER_BIT);
	//Let's draw something here

	//define the size of point and draw a point.
	glDrawArrays(GL_POINTS, 0, 1000);

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

	glEnable(GL_POINT_SPRITE);	//추가
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

}


void myMouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		screenX = x;
		screenY = y;
		printf("Screen Coordinate: (%d, %d)\n", x, y);

		ndcX = (screenX / 0.5) / 479 - 1.0;
		ndcY = -((screenY / 0.5) / 479 - 1.0);
		printf("Normalized Coordinate: (%f, %f)\n\n", ndcX, ndcY);

		vertices.push_back(ndcX);
		vertices.push_back(ndcY);
		vertices.push_back(1.0);
		vertices.push_back(pSize);

		glutPostRedisplay();
	}

}


void myKeyboard(unsigned char key, int x, int y) {

	//n키를 눌렀을 때
	if (key == 'n') {
		//점의 크기가 작아진다. 
		pSize = 5.0;
		printf("point size: 5.0\n");
	}

	else if (key == 'b') {
		//점의 크기가 커진다.  
		pSize = 9.0;
		printf("point size: 9.0\n");

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
	glutCreateWindow("작품명: 별자리");

	//call initization function
	init();

	//1.
	//Generate VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	//3. 
	programID = LoadShaders("VertexShader7.txt", "FragmentShader7.txt");

	glUseProgram(programID);


	glutDisplayFunc(renderScene);

	//enter GLUT event processing cycle
	glutMainLoop();

	glDeleteVertexArrays(1, &VertexArrayID);

	return 1;
}

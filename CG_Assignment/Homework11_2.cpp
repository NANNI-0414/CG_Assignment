// Phong Shading

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <time.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define Stacks 36
#define Slices 18

using namespace std;
GLuint programID;

GLuint modelLoc;
GLint projectionLoc;
GLuint viewLoc;
GLint pColor;

glm::mat4 projection;	//projection matrix
glm::mat4 view;			//view matrix
glm::mat4 model = glm::mat4(1.0f);

vector<GLfloat> vertices = {};
vector<GLuint> indices = {};
GLfloat Radius = 1;

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

	modelLoc = glGetUniformLocation(programID, "model");
	pColor = glGetAttribLocation(programID, "aColor");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]); //행렬을 shader에 줌

	//구 그리기
	glDrawElements(GL_TRIANGLES, 10000, GL_UNSIGNED_INT, (GLvoid*)(0));

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
	glutCreateWindow("작품명: 해왕성");


	//call initization function
	init();

	//1.
	//Generate VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	//3. 
	programID = LoadShaders("VertexShader11_2.txt", "FragmentShader11_2.txt");
	glUseProgram(programID);



	//create two arrays
	GLuint VAO[1];
	glGenVertexArrays(1, VAO);
	//Bind the ﬁrst array
	glBindVertexArray(VAO[0]);



	//제공해주신 코드
	// Calc The Vertices
	for (int i = 0; i <= Stacks; ++i) {

		float V = i / (float)Stacks;
		float phi = V * glm::pi <float>();

		// Loop Through Slices
		for (int j = 0; j <= Slices; ++j) {

			float U = j / (float)Slices;
			float theta = U * (glm::pi <float>() * 2);

			// Calc The Vertex Positions
			float x = cosf(theta) * sinf(phi);
			float y = cosf(phi);
			float z = sinf(theta) * sinf(phi);

			// Push Back Vertex Data
			//vertices.push_back(glm::vec3(x, y, z) * Radius);
			vertices.push_back(x * Radius);
			vertices.push_back(y * Radius);
			vertices.push_back(z * Radius);

		}
	}

	// Calc The Index Positions
	for (int i = 0; i < Slices * Stacks + Slices; ++i) {

		indices.push_back(i);
		indices.push_back(i + Slices + 1);
		indices.push_back(i + Slices);

		indices.push_back(i + Slices + 1);
		indices.push_back(i);
		indices.push_back(i + 1);
	}


	//Create two Buffers
	GLuint VBO[1];
	glGenBuffers(1, VBO);	//Vertex Buffer 

	//Bind the first buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);

	//Put data
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	//Set attribute pointer
	GLint posAttribLoc = glGetAttribLocation(programID, "aPos");
	glVertexAttribPointer(posAttribLoc, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*)(0));

	//Enable attribute arrays
	glEnableVertexAttribArray(posAttribLoc);


	//수직벡터 만들기
	GLint aNormalLoc = glGetAttribLocation(programID, "aNormal");
	glVertexAttribPointer(aNormalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (void*)(0));
	glEnableVertexAttribArray(aNormalLoc);


	//Index Buffer 만들기
	GLuint IBO[1];
	glGenBuffers(1, IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);


	//구의 색깔 지정하기
	GLint objectColorLoc = glGetUniformLocation(programID, "objectColor");
	glUniform3f(objectColorLoc, 0.373, 0.0, 1.0);


	//projection 행렬 초기화
	projectionLoc = glGetUniformLocation(programID, "projection");
	projection = glm::mat4(1.0);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);


	//view 행렬 초기화
	viewLoc = glGetUniformLocation(programID, "view");
	glm::mat4 view = glm::mat4(1.0);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);


	//광원의 위치, 시점의 위치, 광원의 색 초기화
	GLint lightPosLoc = glGetUniformLocation(programID, "lightPos");
	glUniform3f(lightPosLoc, 0.5, 0.5, -1.2);
	GLint viewPosLoc = glGetUniformLocation(programID, "viewPos");
	glUniform3f(viewPosLoc, 0.0, 0.0, 0.0);
	GLint lightColorLoc = glGetUniformLocation(programID, "lightColor");
	glUniform3f(lightColorLoc, 1.0, 1.0, 1.0);


	glutDisplayFunc(renderScene);

	//enter GLUT event processing cycle
	glutMainLoop();

	glDeleteVertexArrays(1, &VertexArrayID);

	return 1;
}



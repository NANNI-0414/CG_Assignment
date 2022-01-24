
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

void myKeyboard(unsigned char key, int x, int y);		//orthogonal, persective 
void myKeySpecialOrtho(int key, int x, int y);			//orthogonal의 화살표키
void myKeySpecialPerspective(int key, int x, int y);	//perspective의 화살표키
void myKeyBoardView(unsigned char key, int x, int y);	//view matrix
double randomDouble();

using namespace std;
GLuint programID;
GLuint transLoc;
GLint projectionLoc;
GLuint viewLoc;
GLint pColor;
//glm::mat4 Model = glm::mat4(1.0f);
//GLint left_signal = 0;
//GLint right_signal = 0;
//GLint screenX, screenY;
//GLfloat ndcX, ndcY;
GLfloat minX = -10.0f, maxX = 10.0f, minY = -10.0f, maxY = 10.0f;	//orthogonal projection의 min/max X, Y
GLfloat fovy;	//perspective projection의 y축 FOV

glm::mat4 projection;	//projection matrix
glm::mat4 view;			//view matrix
glm::mat4 Model = glm::mat4(1.0f);	//4차원 항등행렬을 만듦


glm::mat4 T_inverse = glm::mat4(1.0f);
GLfloat transX = 0, transY = 0, transZ = 0;

glm::mat4 R = glm::mat4(1.0f);
glm::mat4 R_inverse = glm::mat4(1.0f);
GLfloat rotateX = 0.0f, rotateY = 0.0f;
float eX = 0, eY = 0, eZ = 0;

GLfloat red, green, blue;


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

	transLoc = glGetUniformLocation(programID, "transform");
	pColor = glGetAttribLocation(programID, "aColor");
	//GLfloat red = glGetUniformLocation(programID, "r");
	//GLfloat green = glGetUniformLocation(programID, "g");
	//GLfloat blue = glGetUniformLocation(programID, "b");


	GLint idxX, idxY;
	for (idxX = -10.0; idxX <= 10.0; idxX += 1.0)
	{
		for (idxY = -10.0; idxY <= 10.0; idxY += 1.0)
		{

			glm::mat4 transform = glm::translate(Model, glm::vec3(idxX, idxY, 0.0f));

			glUniformMatrix4fv(transLoc, 1, GL_FALSE, &transform[0][0]);

			//정육면체 색 지정
			red = randomDouble();
			green = randomDouble();
			blue = randomDouble();

			//glVertexAttribPointer(pColor, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)(0));
			//glDisableVertexAttribArray(pColor);
			glVertexAttrib4f(pColor, red, green, blue, 1.0);

			//glEnableVertexAttribArray(pColor);

			//define the size of point and draw a point.
			//정육면체 그리기
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}


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

void  myKeyboard(unsigned char key, int x, int y) {
	//orthogonal projection
	if (key == 'o') {
		//Make orthogonal projection
		printf("o: orthogonal projection\n");
		projection = glm::ortho(minX, maxX, minY, maxY, 1.0f, -1.0f);

		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

		glutSpecialFunc(myKeySpecialOrtho);
		glutKeyboardFunc(myKeyBoardView);
		glutPostRedisplay();
	}

	//perspective projection
	if (key == 'p') {
		//Make perspective projections
		//glm::mat4 perspective(float foxy /* in radian */, float aspect /* width/height */, float near, float far);
		printf("p: prospective projection\n");
		fovy = 45.0f;	// radian
		int weight = 4, height = 3;
		//float aspect = (float)weight / (float)height;	// width/height
		projection = glm::perspective(glm::radians(fovy), 1.0f / 1.0f, 0.1f, 100.0f);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

		glutSpecialFunc(myKeySpecialPerspective);
		glutKeyboardFunc(myKeyBoardView);
		glutPostRedisplay();
	}

}

void myKeySpecialOrtho(int key, int x, int y) {
	//With arrow keys, change min / max X, Y
	if (key == GLUT_KEY_LEFT) {
		printf("orthogonal minX~maxX를 줄임 \n");
		minX += 2;
		maxX -= 2;
		projection = glm::ortho(minX, maxX, minY, maxY, 1.0f, -1.0f);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

		glutPostRedisplay();
	}

	if (key == GLUT_KEY_RIGHT) {
		printf("orthogonal minX~maxX를 늘림 \n");
		minX -= 2;
		maxX += 2;
		projection = glm::ortho(minX, maxX, minY, maxY, 1.0f, -1.0f);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

		glutPostRedisplay();
	}

	if (key == GLUT_KEY_UP) {
		printf("orthogonal minY~maxY를 줄임 \n");
		minY += 2;
		maxY -= 2;
		projection = glm::ortho(minX, maxX, minY, maxY, 1.0f, -1.0f);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

		glutPostRedisplay();
	}

	if (key == GLUT_KEY_DOWN) {
		printf("orthogonal minY~maxY를 늘림 \n");
		minY -= 2;
		maxY += 2;
		projection = glm::ortho(minX, maxX, minY, maxY, 1.0f, -1.0f);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

		glutPostRedisplay();
	}
}

void myKeySpecialPerspective(int key, int x, int y) {
	//with arrow keys, change its FOV

	//위쪽 화살표를 누르면 y축 FOV 10도 증가
	if (key == GLUT_KEY_UP) {
		printf("y축 FOV 10도 증가 -> FOV: %f\n", fovy);
		fovy += 10.0f;
		projection = glm::perspective(glm::radians(fovy), 1.0f / 1.0f, 0.1f, 100.0f);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
		glutPostRedisplay();

	}

	//아래쪽 화살표를 누르면 y축 FOV 10도 감소
	if (key == GLUT_KEY_DOWN) {
		printf("y축 FOV 10도 감소 -> FOV: %f\n", fovy);
		fovy -= 10.0f;
		projection = glm::perspective(glm::radians(fovy), 1.0f / 1.0f, 0.1f, 100.0f);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
		glutPostRedisplay();

	}
}

void myKeyBoardView(unsigned char key, int x, int y) {
	/*
	//이동행렬 초기화
	glm::mat4 T = glm::mat4(1.0f);
	glm::mat4 T_inverse = glm::mat4(1.0f);
	GLfloat transX = 0.0f, transY = 0.0f, transZ = 0.0f;
	T_inverse[0] = glm::vec4(1.0f, 0.0f, 0.0f, -transX);
	T_inverse[1] = glm::vec4(0.0f, 1.0f, 0.0f, -transY);
	T_inverse[2] = glm::vec4(0.0f, 0.0f, 1.0f, -transZ);
	T_inverse[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);


	//회전행렬 초기화
	glm::mat4 R = glm::mat4(1.0f);
	glm::mat4 R_inverse = glm::mat4(1.0f);
	*/


	glm::vec3 zAxis(rotateX, rotateY, 1.0f);	//수직벡터N
	glm::vec3 yAxis(0.0f, 1.0f, 0.0f);	//up vector

	//pan, tilt
	//pan: 카메라를 y축 중심으로 회전 -> 수정 필요
	if (key == 'q') {
		printf("pan: 카메라를 y축 중심으로 양의 방향 회전\n");
		rotateY += 0.1f;
		//이쪽에서 view matrix 재정의
		zAxis = glm::normalize(zAxis);
		yAxis = glm::normalize(yAxis);

		glm::vec3 xAxis = glm::cross(yAxis, zAxis);
		xAxis = glm::normalize(xAxis);

		yAxis = glm::cross(zAxis, xAxis);
		yAxis = glm::normalize(yAxis);

		R[0] = glm::vec4(xAxis, 0.0f);
		R[1] = glm::vec4(yAxis, 0.0f);
		R[2] = glm::vec4(zAxis, 0.0f);
		R[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		R_inverse = glm::transpose(R);

		view = R_inverse * T_inverse;

		/*
		//eY += 0.1;
		glm::vec3 e = { eX, eY, eZ }; // 카메라 위치
		//glm::vec3 e = { 0.0, 0.0, 1.0 }; // 카메라 위치
		glm::vec3 f = { 0.0, 0.0, 0.0 }; // 타겟
		glm::vec3 u = { 0.0, 1.0, 0.0 }; // up vector
		u = glm::normalize(u);
		view = glm::lookAt(e, f, u);
		*/
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		glutPostRedisplay();

	}
	if (key == 'i') {
		printf("pan: 카메라를 y축 중심으로 음의 방향 회전\n");
		rotateY -= 0.1f;
		//이쪽에서 view matrix 재정의
		zAxis = glm::normalize(zAxis);
		yAxis = glm::normalize(yAxis);

		glm::vec3 xAxis = glm::cross(yAxis, zAxis);
		xAxis = glm::normalize(xAxis);

		yAxis = glm::cross(zAxis, xAxis);
		yAxis = glm::normalize(yAxis);

		R[0] = glm::vec4(xAxis, 0.0f);
		R[1] = glm::vec4(yAxis, 0.0f);
		R[2] = glm::vec4(zAxis, 0.0f);
		R[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		R_inverse = glm::transpose(R);

		view = R_inverse * T_inverse;
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		glutPostRedisplay();

	}
	//tilt: 카메라를 x축 중심으로 회전 -> 수정 필요
	if (key == 'w') {
		printf("tilt: 카메라를 x축 중심으로 양의 방향 회전\n");	//양의 방향인지 확실하지 않음.
		//이쪽에서 view matrix 재정의
		rotateX += 0.1f;

		zAxis = glm::normalize(zAxis);
		yAxis = glm::normalize(yAxis);

		glm::vec3 xAxis = glm::cross(yAxis, zAxis);
		xAxis = glm::normalize(xAxis);

		yAxis = glm::cross(zAxis, xAxis);
		yAxis = glm::normalize(yAxis);

		R[0] = glm::vec4(xAxis, 0.0f);
		R[1] = glm::vec4(yAxis, 0.0f);
		R[2] = glm::vec4(zAxis, 0.0f);
		R[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		R_inverse = glm::transpose(R);

		view = R_inverse * T_inverse;
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		glutPostRedisplay();
	}
	if (key == 'u') {
		printf("tilt: 카메라를 x축 중심으로 음의 방향 회전\n");	//양의 방향인지 확실하지 않음.
		//이쪽에서 view matrix 재정의
		rotateX -= 0.1f;

		zAxis = glm::normalize(zAxis);
		yAxis = glm::normalize(yAxis);

		glm::vec3 xAxis = glm::cross(yAxis, zAxis);
		xAxis = glm::normalize(xAxis);

		yAxis = glm::cross(zAxis, xAxis);
		yAxis = glm::normalize(yAxis);

		R[0] = glm::vec4(xAxis, 0.0f);
		R[1] = glm::vec4(yAxis, 0.0f);
		R[2] = glm::vec4(zAxis, 0.0f);
		R[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		R_inverse = glm::transpose(R);

		view = R_inverse * T_inverse;
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		glutPostRedisplay();
	}

	//crab, ped, track
	//crab: 카메라를 x축 중심으로 이동 -> 수정 필요
	if (key == 'a') {
		printf("crab: 카메라를 x축 중심으로 양의 방향 이동\n");
		//이쪽에서 view matrix 재정의

		transX -= 0.1;
		T_inverse[0] = glm::vec4(1.0f, 0.0f, 0.0f, -transX);
		T_inverse[1] = glm::vec4(0.0f, 1.0f, 0.0f, -transY);
		T_inverse[2] = glm::vec4(0.0f, 0.0f, 1.0f, -transZ);
		T_inverse[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		view = R_inverse * T_inverse;

		/*
		transX += 0.05;
		glm::vec3 e = { 0.0, 0.0, 1.0 }; // 카메라 위치
		glm::vec3 f = { 0.0, 0.0, 0.0 }; // 타겟
		glm::vec3 u = { 0.0, 1.0, 0.0 }; // up vector
		u = glm::normalize(u);
		view = glm::lookAt(e, f, u);
		*/
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		glutPostRedisplay();
	}
	if (key == 'l') {
		printf("crab: 카메라를 x축 중심으로 음의 방향 이동\n");
		//이쪽에서 view matrix 재정의
		transX += 0.1;
		T_inverse[0] = glm::vec4(1.0f, 0.0f, 0.0f, -transX);
		T_inverse[1] = glm::vec4(0.0f, 1.0f, 0.0f, -transY);
		T_inverse[2] = glm::vec4(0.0f, 0.0f, 1.0f, -transZ);
		T_inverse[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		view = R_inverse * T_inverse;
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		glutPostRedisplay();
	}

	//ped: 카메라를 y축 중심으로 이동 -> 수정 필요
	if (key == 's') {
		printf("ped: 카메라를 y축 중심으로 양의 방향 이동\n");
		//이쪽에서 view matrix 재정의
		transY -= 0.1;
		T_inverse[0] = glm::vec4(1.0f, 0.0f, 0.0f, -transX);
		T_inverse[1] = glm::vec4(0.0f, 1.0f, 0.0f, -transY);
		T_inverse[2] = glm::vec4(0.0f, 0.0f, 1.0f, -transZ);
		T_inverse[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		view = R_inverse * T_inverse;
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		glutPostRedisplay();
	}
	if (key == 'k') {
		printf("ped: 카메라를 y축 중심으로 음의 방향 이동\n");
		//이쪽에서 view matrix 재정의
		transY += 0.1;
		T_inverse[0] = glm::vec4(1.0f, 0.0f, 0.0f, -transX);
		T_inverse[1] = glm::vec4(0.0f, 1.0f, 0.0f, -transY);
		T_inverse[2] = glm::vec4(0.0f, 0.0f, 1.0f, -transZ);
		T_inverse[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		view = R_inverse * T_inverse;
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		glutPostRedisplay();
	}

	//track: 카메라를 z축 중심으로 이동 -> 수정 필요
	if (key == 'd') {
		printf("track: 카메라를 z축 중심으로 양의 방향 이동\n");
		//이쪽에서 view matrix 재정의
		transZ -= 0.1f;
		T_inverse[0] = glm::vec4(1.0f, 0.0f, 0.0f, -transX);
		T_inverse[1] = glm::vec4(0.0f, 1.0f, 0.0f, -transY);
		T_inverse[2] = glm::vec4(0.0f, 0.0f, 1.0f, -transZ);
		T_inverse[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		view = R_inverse * T_inverse;
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		glutPostRedisplay();
	}
	if (key == 'j') {
		printf("track: 카메라를 z축 중심으로 음의 방향 이동\n");
		//이쪽에서 view matrix 재정의
		transZ += 0.1f;
		T_inverse[0] = glm::vec4(1.0f, 0.0f, 0.0f, -transX);
		T_inverse[1] = glm::vec4(0.0f, 1.0f, 0.0f, -transY);
		T_inverse[2] = glm::vec4(0.0f, 0.0f, 1.0f, -transZ);
		T_inverse[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		view = R_inverse * T_inverse;
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		glutPostRedisplay();
	}

	//zoom: orthogonal
	if (key == 'z') {
		printf("zoom in\n");
		//이쪽에서 view matrix 재정의
		minX += 0.1f; maxX -= 0.1f;
		minY += 0.1f; maxY -= 0.1f;
		projection = glm::ortho(minX, maxX, minY, maxY, 1.0f, -1.0f);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
		glutPostRedisplay();
	}
	if (key == 'm') {
		printf("zoom out\n");
		//이쪽에서 view matrix 재정의
		minX -= 0.1f; maxX += 0.1f;
		minY -= 0.1f; maxY += 0.1f;
		projection = glm::ortho(minX, maxX, minY, maxY, 1.0f, -1.0f);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
		glutPostRedisplay();
	}

	//zoom: perspective
	if (key == 'x') {
		//이쪽에서 view matrix 재정의
		//FOV를 줄이면 Zoom in
		fovy -= 10;
		printf("Zoom in: , FOV: %f\n", fovy);
		projection = glm::perspective(glm::radians(fovy), 1.0f / 1.0f, 0.1f, 100.0f);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
		glutPostRedisplay();
	}
	if (key == 'n') {
		//이쪽에서 view matrix 재정의
		//FOV를 키우면 zoom out
		fovy += 10;
		printf("Zoom out, FOV: %f\n", fovy);
		projection = glm::perspective(glm::radians(fovy), 1.0f / 1.0f, 0.1f, 100.0f);
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);
		glutPostRedisplay();
	}

}

double randomDouble() {
	double result = (double)rand() / RAND_MAX;
	return result;
}

int main(int argc, char** argv)
{
	//srand(time(NULL));
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
	programID = LoadShaders("VertexShader10.txt", "FragmentShader10.txt");
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


	//projection 행렬 초기화
	projectionLoc = glGetUniformLocation(programID, "projection");
	projection = glm::mat4(1.0);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);

	//view 행렬 초기화
	viewLoc = glGetUniformLocation(programID, "view");
	glm::mat4 view = glm::mat4(1.0);
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

	//이동행렬 초기화
	//glm::mat4 T = glm::mat4(1.0f);
	//glm::mat4 T_inverse = glm::mat4(1.0f);
	//GLfloat transX = 0.0f, transY = 0.0f, transZ = 0.0f;
	//T_inverse[0] = glm::vec4(1.0f, 0.0f, 0.0f, -transX);
	//T_inverse[1] = glm::vec4(0.0f, 1.0f, 0.0f, -transY);
	//T_inverse[2] = glm::vec4(0.0f, 0.0f, 1.0f, -transZ);
	//T_inverse[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);


	//회전행렬 초기화
	//glm::mat4 R = glm::mat4(1.0f);
	//glm::mat4 R_inverse = glm::mat4(1.0f);


	glutDisplayFunc(renderScene);
	glutKeyboardFunc(myKeyboard);


	//enter GLUT event processing cycle
	glutMainLoop();

	glDeleteVertexArrays(1, &VertexArrayID);

	return 1;
}



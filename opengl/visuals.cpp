#include <math.h>
#include <iostream>
#include <ctime>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "visuals.h"
#include <time.h>
#include "shader.hpp"
#include "RgbImage.h"



#include <string>
#include <fstream>
#include <sstream>
#include <istream>
#include <vector>
#include <algorithm>
#include <iterator>


#define PI 3.14159265359

typedef struct
{
	GLfloat x;
	GLfloat y;
	GLfloat z;
} Vertex;

// OpenGL IDs
GLuint ShaderMainProgramId, ShaderModelProgramId, TextureId, VertexBufferId, IndexBufferId, MatricesBufferId;
glm::mat4 Projection, View, Model;


// Main shader IDs
GLuint MainShaderMvpId, MainShaderTextureId;
GLuint ModelShaderMvpId, ModelShaderColorId;

// Final Homogeneous Matrix
glm::mat4 MVP;
glm::mat4 Rotation;

std::vector<Group>* Data;

RgbImage theTexMap("sweetsA01.bmp");




std::vector<Group>* ParseFile(char *filename)
{
	std::string line;
	std::ifstream file(filename);

	// First line - number of groups
	std::getline(file, line);
	auto numGroupsStr = line.substr(8);
	int numGroups = ::atoi(numGroupsStr.c_str());

	std::vector<Group>* result = new std::vector<Group>(numGroups);

	// Iterate groups
	for (int i = 0; i < numGroups; i++)
	{
		Group group;

		// Get color
		std::getline(file, line);
		auto colorsStr = line.substr(7);

		int separator1, separator2;
		separator1 = colorsStr.find(',');
		separator2 = colorsStr.find(',', separator1 + 1);

		group.color.R = ::atof(colorsStr.substr(0, separator1).c_str());
		group.color.G = ::atof(colorsStr.substr(separator1 + 1, separator2 - separator1).c_str());
		group.color.B = ::atof(colorsStr.substr(separator2 + 1).c_str());


		// Get number of coordinates
		std::getline(file, line);
		auto numCoordinatesStr = line.substr(7);
		int numCoordinates = ::atoi(numCoordinatesStr.c_str());

		group.items.resize(numCoordinates);

		// Get coordinates
		for (int i = 0; i < numCoordinates; i++)
		{
			std::getline(file, line);
			int separator = line.find(',');

			Coordinate coord;
			coord.x = ::atof(line.substr(0, separator).c_str());
			coord.y = ::atof(line.substr(separator + 1).c_str());

			group.items[i] = coord;
		}

		(*result)[i] = group;
	}

	file.close();

	return result;
}




void Idle()
{
	glutPostRedisplay();
}

void ReshapeGL(int w, int h)
{
	std::cout << "ReshapGL( " << w << ", " << h << " );" << std::endl;

	// Prevent a divide-by-zero error
	// Making Height Equal One
	if (h == 0)
	{
		h = 1;
	}

	g_iWindowWidth = w;
	g_iWindowHeight = h;

	glViewport(0, 0, 800, 600);
}


void KeyboardGL(unsigned char c, int x, int y)
{
	switch (c)
	{
	case 's':
	case 'S':
	{
				std::cout << "Shade Model: GL_SMOOTH" << std::endl;
				// Switch to smooth shading model
				glShadeModel(GL_SMOOTH);
	}
		break;
	case 'f':
	case 'F':
	{
				std::cout << "Shade Model: GL_FLAT" << std::endl;
				// Switch to flat shading model
				glShadeModel(GL_FLAT);
	}
		break;
	case '\033': // escape quits
	case '\015': // Enter quits
	case 'Q':    // Q quits
	case 'q':    // q (or escape) quits
	{
					 // Cleanup up and quit
					 Cleanup(0);
	}
		break;

	case '\72': //arrow up
	{

	}
		break;
	}

	glutPostRedisplay();
}


void Cleanup(int errorCode, bool bExit)
{
	if (g_iGLUTWindowHandle != 0)
	{
		glutDestroyWindow(g_iGLUTWindowHandle);
		g_iGLUTWindowHandle = 0;
	}

	if (bExit)
	{
		exit(errorCode);
	}
}


void DisplayGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clean up the colour of the window
	// and the depth buffer

	RenderScene();

	glutSwapBuffers();
	// All drawing commands applied to the
	// hidden buffer, so now, bring forward
	// the hidden buffer and hide the visible one
}

void RenderScene()
{
	// Model matrix : being updated by idle
	// Our ModelViewProjection : multiplication of our 3 matrices
	MVP = Projection * View * Model; // Remember, matrix multiplication is the other way around

	// Use our shader
	glUseProgram(ShaderMainProgramId);

	// Send our transformation to the currently bound shader, in the "MVP" uniform

	glUniformMatrix4fv(MainShaderMvpId, 1, GL_FALSE, &MVP[0][0]);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Bind texture to texture unit
	glActiveTexture(GL_TEXTURE0); // change active texture unit to number 0
	glBindTexture(GL_TEXTURE_2D, TextureId); // bind the texture to the active texture unit (which is now 0)

	// Bind shader variable "Texture" to texture unit
	glUniform1iARB(MainShaderTextureId, 0); // Notice the 0 to indicate you want to sample from the texture bound to Texture Unit GL_TEXTURE0.

	float imageRatio = (float)theTexMap.GetNumCols() / theTexMap.GetNumRows();
	if (imageRatio > 1)
	{
		imageRatio = 1.0 / imageRatio;
	}

	glBegin(GL_QUADS);

	glTexCoord2f(0.0, 1.0);
	glVertex3f(-1.0, 0.0, -1.0*imageRatio);

	glTexCoord2f(0.0, 0.0);
	glVertex3f(-1.0, 0.0, 1.0*imageRatio);

	glTexCoord2f(1.0, 0.0);
	glVertex3f(1.0, 0.0, 1.0*imageRatio);

	glTexCoord2f(1.0, 1.0);
	glVertex3f(1.0, 0.0, -1.0*imageRatio);

	glEnd();


	// Draw groups
	glUseProgram(ShaderModelProgramId);
	glUniformMatrix4fv(ModelShaderMvpId, 1, GL_FALSE, &MVP[0][0]);

	int modelNum = 0;
	for (std::vector<Group>::iterator it = (*Data).begin(); it != (*Data).end(); ++it)
	{
		RenderGroup(it->items, it->color, modelNum++);
	}
}

void CreateInstancedTransformMatrices(std::vector<Coordinate> coordinates)
{
	// Create VBO with matrices from coordinates
	std::vector<glm::mat4> matrices;
	matrices.resize(coordinates.size());

	for (int i = 0; i < coordinates.size(); i++)
	{
		matrices[i] = glm::translate(glm::mat4(1.0f), glm::vec3(coordinates[i].x, 0.0f, coordinates[i].y));
	}


	// Vertex buffer
	glGenBuffers(1, &MatricesBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, MatricesBufferId);
	glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(glm::mat4), &matrices[0], GL_STATIC_DRAW);


	// Bind VBO
	int pos = glGetAttribLocation(ShaderModelProgramId, "TransformMatrix");
	int pos1 = pos + 0;
	int pos2 = pos + 1;
	int pos3 = pos + 2;
	int pos4 = pos + 3;
	glEnableVertexAttribArray(pos1);
	glEnableVertexAttribArray(pos2);
	glEnableVertexAttribArray(pos3);
	glEnableVertexAttribArray(pos4);
	glBindBuffer(GL_ARRAY_BUFFER, MatricesBufferId);
	glVertexAttribPointer(pos1, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 4 * 4, (void*)(0));
	glVertexAttribPointer(pos2, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 4 * 4, (void*)(sizeof(float)* 4));
	glVertexAttribPointer(pos3, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 4 * 4, (void*)(sizeof(float)* 8));
	glVertexAttribPointer(pos4, 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)* 4 * 4, (void*)(sizeof(float)* 12));
	glVertexAttribDivisor(pos1, 1);
	glVertexAttribDivisor(pos2, 1);
	glVertexAttribDivisor(pos3, 1);
	glVertexAttribDivisor(pos4, 1);
}

// http://www.packtpub.com/article/tips-tricks-getting-started-with-opengl-glsl-4'
// http://www.packtpub.com/article/opengl-glsl-4-shaders-basics
// http://sol.gfxile.net/instancing.html
void RenderGroup(std::vector<Coordinate> coordinates, Color color, int modelNum)
{
	CreateInstancedTransformMatrices(coordinates);

	// Upload uniform data
	glUniform3f(ModelShaderColorId, color.R, color.G, color.B);

	// Draw
	glEnableVertexAttribArray(0);

	// 1rst attribute buffer : vertices
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);

	// Draw the triangle !
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
		);

	glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0, coordinates.size());
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (void*)0);

	glDisableVertexAttribArray(0);
}


void SetupGL()
{
	// Projection matrix
	Projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 100.0f);

	// Camera matrix
	View = glm::lookAt(
		glm::vec3(1, 2, 2), // Camera is at (4,3,3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);

	//Parameter handling
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);

	// polygon rendering mode
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	//Set up light source
	GLfloat light_position[] = { 0.0, 30.0, -50.0, 0.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// Black background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Register GLUT callbacks
	glutDisplayFunc(DisplayGL);
	glutKeyboardFunc(KeyboardGL);
	glutReshapeFunc(ReshapeGL);

	// Setup initial GL State
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearDepth(1.0f);

	// Init GLEW
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW." << std::endl;
		exit(-1);
	}

	// Setup initial GL State
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearDepth(1.0f);

	std::cout << "Initialise OpenGL: Success!" << std::endl;
}

void LoadResources()
{
	// Load file
	Data = ParseFile("A01.txt");




	// Load texture
	// Let OpenGL create us a new texture ID to use
	glGenTextures(1, &TextureId);

	// Bind against the texture ID
	glBindTexture(GL_TEXTURE_2D, TextureId);

	// Set properties on current bound texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);

	// Load texture data
	auto h = gluBuild2DMipmaps(GL_TEXTURE_2D, 3, theTexMap.GetNumCols(), theTexMap.GetNumRows(), GL_RGB, GL_UNSIGNED_BYTE, theTexMap.ImageData());
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, theTexMap.GetNumCols(), theTexMap.GetNumRows(), 0, GL_RGB, GL_UNSIGNED_BYTE, theTexMap.ImageData());





	// Create and compile our GLSL program from the shaders
	ShaderMainProgramId = LoadShaders("Main.vertexshader", "Main.fragmentshader");
	ShaderModelProgramId = LoadShaders("Model.vertexshader", "Model.fragmentshader");

	MainShaderMvpId = glGetUniformLocation(ShaderMainProgramId, "MVP");
	MainShaderTextureId = glGetUniformLocation(ShaderMainProgramId, "Texture");

	ModelShaderMvpId = glGetUniformLocation(ShaderModelProgramId, "MVP");
	ModelShaderColorId = glGetUniformLocation(ShaderModelProgramId, "Color");





	// Upload and bind vertexes/indices
	Vertex vertices[8] = {
		{ 0.0f*0.05, 1.0f*0.05, 0.0f*0.05 },
		{ 0.0f*0.05, 1.0f*0.05, 1.0f*0.05 },
		{ 1.0f*0.05, 1.0f*0.05, 1.0f*0.05 },
		{ 1.0f*0.05, 1.0f*0.05, 0.0f*0.05 },
		{ 0.0f*0.05, 0.0f*0.05, 0.0f*0.05 },
		{ 0.0f*0.05, 0.0f*0.05, 1.0f*0.05 },
		{ 1.0f*0.05, 0.0f*0.05, 1.0f*0.05 },
		{ 1.0f*0.05, 0.0f*0.05, 0.0f*0.05 }
	};

	GLubyte indices[36] = {
		0, 3, 7,
		5, 7, 0,
		4, 0, 7,
		0, 3, 7,
		7, 3, 6,
		3, 2, 6,
		6, 2, 5,
		2, 1, 5,
		5, 1, 4,
		1, 0, 4,
		5, 4, 6,
		4, 7, 6
	};

	// Vertex buffer
	glGenBuffers(1, &VertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Index buffer
	glGenBuffers(1, &IndexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}
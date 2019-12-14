#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <iostream>
#include "Shaders.h"
#include "Textures.h"
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "cameraInput.h"


//Variables
const int WINDOW_WIDTH = 900, WINDOW_HEIGHT = 900;
unsigned int VBO, VAO, lightVAO;

//objects positions in space
glm::vec3 cubePositions[] = {
	   glm::vec3(0.0f,  0.0f,  0.0f),
	   glm::vec3(2.0f,  5.0f, -15.0f),
	   glm::vec3(-1.5f, -2.2f, -2.5f),
	   glm::vec3(-3.8f, -2.0f, -12.3f),
	   glm::vec3(2.4f, -0.4f, -3.5f),
	   glm::vec3(-1.7f,  3.0f, -7.5f),
	   glm::vec3(1.3f, -2.0f, -2.5f),
	   glm::vec3(1.5f,  2.0f, -2.5f),
	   glm::vec3(1.5f,  0.2f, -1.5f),
	   glm::vec3(-1.3f,  1.0f, -1.5f)
	};


// lighting position
glm::vec3 lightPosition(0.0f, 2.0f, 5.0f);


//crate objects function declaration
void CreateLamp();

void CreateCube();

int main()
{
	//GFLW Initialise
	if (!glfwInit())
	{
		std::cout << "GLFW initialisation failed!" << std::endl;
		glfwTerminate();

		return -1;
	}

	// Setup OpenGL version and window
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	glewExperimental = GL_TRUE;

	GLFWwindow* mainWindow = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "HelloGL!", NULL, NULL);
	if (!mainWindow)
	{
		std::cout << "GLFW Window creation failed!" << std::endl;
		glfwTerminate();

		return -1;
	}
	
	glfwMakeContextCurrent(mainWindow);
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("GLEW initialisation failed!");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return -1;
	}

	glfwSetCursorPosCallback(mainWindow, mouse_callback);

	// Load Shader
	Shader myShader("src/shaders/shader.vr", "src/shaders/shader.fr");
	Shader lightShader("src/shaders/lightShader.vr", "src/shaders/lightShader.fr");

	// Load Texture
	Texture myTexture("src/textures/test3.jpg");

	// Create Object
	CreateCube();
	CreateLamp();


	// Z-buffer set true
	glEnable(GL_DEPTH_TEST);
	//Draw loop
	while (!glfwWindowShouldClose(mainWindow))
	{
		//handle Time
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//handle input
		processInput(mainWindow);

		//Clear z-buffer and screen buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//bind texture and shader
		myTexture.use();

		lightShader.use();
		processCameraTransformation(lightShader, mainWindow);

		glBindVertexArray(lightVAO);
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, lightPosition);
			model = glm::scale(model, glm::vec3(0.5f));
			lightShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		myShader.use();
		// light properties
		myShader.setVec3("cameraPos", cameraPos);
		myShader.setVec3("light.position", lightPosition);
		glm::vec3 lightColor = glm::vec3(1.0f);
		glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // decrease the influence
		glm::vec3 ambientColor = diffuseColor * glm::vec3(0.5f); // low influence
		myShader.setVec3("light.ambient", ambientColor);
		myShader.setVec3("light.diffuse", diffuseColor);
		myShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

		// material properties
		myShader.setVec3("material.ambient", 1.0f, 1.0f, 1.0f);
		myShader.setVec3("material.diffuse", 1.0f, 1.0f, 1.0f);
		myShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
		myShader.setFloat("material.shininess", 128.0f);

		processCameraTransformation(myShader, mainWindow);
		glBindVertexArray(VAO);

		for (unsigned int i = 0; i < 10; i++)
		{
			// calculate the model matrix for each object and pass it to shader before drawing
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, (cubePositions[i] * 0.5f));
			model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.01f, 0.0f));
			myShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			//glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
		}
		glfwSwapBuffers(mainWindow);

		glBindVertexArray(0);

		glUseProgram(0);

		// IO events and input
		glfwPollEvents();
	}

	return 0;
}


//create object function body
void CreateCube()
{
	float vertices[] = {
		//positions				//textures		//normals
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		 0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		 0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		 0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		 0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,		 0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,	0.0f, 0.0f,		 0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		 0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,	1.0f, 0.0f,		 0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 1.0f,		 0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 1.0f,		 0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,	0.0f, 1.0f,		 0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		 0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f,	0.0f, 1.0f,		-1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		-1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		-1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		-1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		-1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,	0.0f, 1.0f,		-1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,		 1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		 1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,	0.0f, 1.0f,		 1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,	0.0f, 1.0f,		 1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		 1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,		 1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		 0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,	1.0f, 1.0f,		 0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,	0.0f, 1.0f,		 0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,	0.0f, 1.0f,		 0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,	0.0f, 0.0f,		 0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,	1.0f, 0.0f,		 0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,		 0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,	1.0f, 1.0f,		 0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,		 0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,	1.0f, 0.0f,		 0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,	0.0f, 0.0f,		 0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,	0.0f, 1.0f,		 0.0f,  1.0f,  0.0f

	};


	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void CreateLamp()
{
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}


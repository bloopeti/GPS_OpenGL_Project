#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "model.h"
#include "wtypes.h"

#include <iostream>
#include <cstdlib>
#include <ctime>

// abstracts for functions defined at the end
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
void GetDesktopResolution(int& horizontal, int& vertical);
bool initWindow();
glm::vec3 vec3randomizer(float lo, float hi);
glm::vec3* vec3arrRandomizer(float lo, float hi);
int shaderLightSetup(Shader shader);
void renderScene(Shader &shader, unsigned int planeVAO, unsigned int floorDiffuseMap, unsigned int floorSpecularMap, Model knucklesModel, Model nanoModel, Model treeModel);
unsigned int loadCubemap(vector<std::string> faces);

// settings
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;
bool blinn = true;
bool flash = false;
bool wireframe = false;
bool blinnKeyPressed = false;
bool flashKeyPressed = false;
bool wireframeKeyPressed = false;
GLFWwindow* window;

// camera
Camera camera(glm::vec3(0.0f, 2.0f, 10.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

float boxVertices[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
	0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
};

// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
float planeVertices[] = {
	// positions            // normals         // texcoords
	10.0f, 0.0f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
	-10.0f, 0.0f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
	-10.0f, 0.0f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

	10.0f, 0.0f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
	-10.0f, 0.0f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
	10.0f, 0.0f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
};

// positions of the point lights
glm::vec3 pointLightPositions[] = {
	glm::vec3(0.0f, 0.5f, 3.0f),
	glm::vec3(-2.0f, 4.0f, -1.0f),
	glm::vec3(9.0f,  0.5f, -3.0f)
};

// positions of the trees
glm::vec3 treePositions[] = {
	glm::vec3(-4.04f, 0.0f, 8.0f),
	glm::vec3(6.9f,  0.0f,  7.0f),
	glm::vec3(1.14f,  0.0f, -6.66f),
	glm::vec3(-6.9f,  0.0f, -6.66f),
};

// positions of the knuckles'
glm::vec3 knucklesPositions[] = {
	glm::vec3(1.41f, 0.0f, -3.0f),
	glm::vec3(2.95f, 0.0f, 7.0f),
	glm::vec3(3.43f, 0.0f, 4.0f),
	glm::vec3(4.77f, 0.0f, 0.5f),
	glm::vec3(5.52f, 0.0f, 6.8f),
	glm::vec3(6.66f, 0.0f, 6.6f),
	glm::vec3(7.34f, 0.0f, -5.4f),
	glm::vec3(8.45f, 0.0f, 1.3f),
	glm::vec3(9.44f, 0.0f, -7.2f),
	glm::vec3(-1.1f, 0.0f, 5.0f),
	glm::vec3(-2.6f, 0.0f, 1.9f),
	glm::vec3(-3.2f, 0.0f, 1.0f),
	glm::vec3(-4.2f, 0.0f, 7.0f),
	glm::vec3(-5.6f, 0.0f, -9.6f),
	glm::vec3(-6.3f, 0.0f, 6.9f),
	glm::vec3(-7.9f, 0.0f, -6.9f),
	glm::vec3(-8.9f, 0.0f, 0.0f),
	glm::vec3(-9.4f, 0.0f, -3.8f),
	glm::vec3(1.71f, 0.0f, 9.5f),
	glm::vec3(2.93f, 0.0f, 2.1f),
	glm::vec3(3.41f, 0.0f, 6.5f),
	glm::vec3(4.78f, 0.0f, -7.1f),
	glm::vec3(5.50f, 0.0f, 4.1f),
	glm::vec3(6.00f, 0.0f, -7.3f),
	glm::vec3(7.33f, 0.0f, 5.5f),
	glm::vec3(8.45f, 0.0f, -7.9f),
	glm::vec3(9.42f, 0.0f, 5.8f),
	glm::vec3(-1.2f, 0.0f, -5.2f),
	glm::vec3(-2.6f, 0.0f, 5.3f),
	glm::vec3(-3.2f, 0.0f, 2.0f),
	glm::vec3(-4.2f, 0.0f, 8.5f),
	glm::vec3(-5.6f, 0.0f, -6.0f),
	glm::vec3(-6.3f, 0.0f, 9.3f),
	glm::vec3(-7.9f, 0.0f, 6.0f),
	glm::vec3(-8.9f, 0.0f, 7.0f),
	glm::vec3(-9.4f, 0.0f, -6.66f),
};

float skyboxVertices[] = {
	// positions          
	-40.0f,  40.0f, -40.0f,
	-40.0f, -40.0f, -40.0f,
	40.0f, -40.0f, -40.0f,
	40.0f, -40.0f, -40.0f,
	40.0f,  40.0f, -40.0f,
	-40.0f,  40.0f, -40.0f,

	-40.0f, -40.0f,  40.0f,
	-40.0f, -40.0f, -40.0f,
	-40.0f,  40.0f, -40.0f,
	-40.0f,  40.0f, -40.0f,
	-40.0f,  40.0f,  40.0f,
	-40.0f, -40.0f,  40.0f,

	40.0f, -40.0f, -40.0f,
	40.0f, -40.0f,  40.0f,
	40.0f,  40.0f,  40.0f,
	40.0f,  40.0f,  40.0f,
	40.0f,  40.0f, -40.0f,
	40.0f, -40.0f, -40.0f,

	-40.0f, -40.0f,  40.0f,
	-40.0f,  40.0f,  40.0f,
	40.0f,  40.0f,  40.0f,
	40.0f,  40.0f,  40.0f,
	40.0f, -40.0f,  40.0f,
	-40.0f, -40.0f,  40.0f,

	-40.0f,  40.0f, -40.0f,
	40.0f,  40.0f, -40.0f,
	40.0f,  40.0f,  40.0f,
	40.0f,  40.0f,  40.0f,
	-40.0f,  40.0f,  40.0f,
	-40.0f,  40.0f, -40.0f,

	-40.0f, -40.0f, -40.0f,
	-40.0f, -40.0f,  40.0f,
	40.0f, -40.0f, -40.0f,
	40.0f, -40.0f, -40.0f,
	-40.0f, -40.0f,  40.0f,
	40.0f, -40.0f,  40.0f
};

int main()
{
	if (!initWindow())
		return -1;

	// build and compile shaders
	// -------------------------
	Shader simpleDepthShader("shaders/shadow.vs", "shaders/shadow.fs");
	Shader lampShader("shaders/lightBox.vs", "shaders/lightBox.fs");
	//Shader planeShader("shaders/plane.vs", "shaders/plane.fs");
	Shader knucklesShader("shaders/knuckles.vs", "shaders/knuckles.fs");
	//Shader nanoShader("shaders/nano.vs", "shaders/nano.fs");
	//Shader treeShader("shaders/tree.vs", "shaders/tree.fs");
	Shader skyboxShader("shaders/skybox.vs", "shaders/skybox.fs");

	Model knucklesModel("resources/objects/knuckles/Knuckles.obj");
	Model nanoModel("resources/objects/nanosuit/nanosuit.obj");
	Model treeModel("resources/objects/TreeCartoon1_OBJ/TreeCartoon1_OBJ/TreeCartoon1_OBJ.obj");

	// plane VAO
	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);

	// second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int lightVAO, lightVBO;
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &lightVBO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);
	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);



	// load textures
	// -------------
	unsigned int floorDiffuseMap = loadTexture("resources/textures/dirt.jpg");
	unsigned int floorSpecularMap = loadTexture("resources/textures/dirt_specular.jpg");

	// shader configuration
	// --------------------
	knucklesShader.use();
	knucklesShader.setInt("floorDiffuseMap", 0);
	//knucklesShader.setInt("floorSpecularMap", 1);

	int width = 0;
	int height = 0;
	GetDesktopResolution(width, height);
	const unsigned int SHADOW_WIDTH = width, SHADOW_HEIGHT = height;
	//const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	// create depth texture
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return -1;

	// skybox VAO
	unsigned int skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	vector<std::string> faces
	{
		"resources/textures/ame_nebula/purplenebula_rt.tga",
		"resources/textures/ame_nebula/purplenebula_lf.tga",
		"resources/textures/ame_nebula/purplenebula_up.tga",
		"resources/textures/ame_nebula/purplenebula_dn.tga",
		"resources/textures/ame_nebula/purplenebula_bk.tga",
		"resources/textures/ame_nebula/purplenebula_ft.tga"
	};
	unsigned int cubemapTexture = loadCubemap(faces);


	// shader configuration
	// --------------------
	knucklesShader.use();
	knucklesShader.setInt("shadowMap", 3);

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);
	
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);
		if (wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		// render
		// ------
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		glm::mat4 model;
		// 1. render depth of scene to texture (from light's perspective)
		// --------------------------------------------------------------
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 0.1f, far_plane = 20.0f;
		lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		lightView = glm::lookAt(glm::vec3(0.0f, 10.0f, 10.0f), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		// render scene from light's point of view
		simpleDepthShader.use();
		simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		simpleDepthShader.setMat4("model", model);

		glCullFace(GL_FRONT);
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
			glClear(GL_DEPTH_BUFFER_BIT);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, depthMap);
			//knucklesShader.setInt("shadowMap", 3);
			renderScene(simpleDepthShader, planeVAO, floorDiffuseMap, floorSpecularMap, knucklesModel, nanoModel, treeModel);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glCullFace(GL_BACK); // don't forget to reset original culling face

		// reset viewport
		int width = 0;
		int height = 0;
		GetDesktopResolution(width, height);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		knucklesShader.use();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		knucklesShader.setMat4("projection", projection);
		knucklesShader.setMat4("view", view);

		glm::mat3 lightDirMatrix = glm::mat3(transpose(inverse(view)));
		knucklesShader.setMat3("lightDirMatrix", lightDirMatrix);
		// set light uniforms
		knucklesShader.setVec3("viewPos", camera.Position);
		shaderLightSetup(knucklesShader);
		knucklesShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorDiffuseMap);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		renderScene(knucklesShader, planeVAO, floorDiffuseMap, floorSpecularMap, knucklesModel, nanoModel, treeModel);

		//glm::mat4 model;
		//glm::mat4 projection;
		//glm::mat4 view;
		/////////////////////////////////////////////////////////
		//////////////////////////LAMP///////////////////////////
		/////////////////////////////////////////////////////////
		// also draw the lamp object(s)
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);

		// we now draw as many light bulbs as we have point lights.
		glBindVertexArray(lightVAO);
		for (unsigned int i = 0; i < sizeof(pointLightPositions)/sizeof(glm::vec3); i++)
		{
			model = glm::mat4();
			model = glm::translate(model, pointLightPositions[i]);
			model = glm::scale(model, glm::vec3(0.1f)); // Make it a smaller cube
			lampShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		//std::cout << (blinn ? "Blinn-Phong" : "Phong") << std::endl;

		// draw skybox as last
		glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
		skyboxShader.setMat4("view", view);
		skyboxShader.setMat4("projection", projection);
		// skybox cube
		glBindVertexArray(skyboxVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS); // set depth function back to default

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &planeVAO); 
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteBuffers(1, &planeVBO);
	glDeleteBuffers(1, &lightVBO);

	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
	{
		blinn = !blinn;
		blinnKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
	{
		blinnKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !flashKeyPressed)
	{
		flash = !flash;
		flashKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
	{
		flashKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !wireframeKeyPressed)
	{
		wireframe = !wireframe;
		wireframeKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE)
	{
		wireframeKeyPressed = false;
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

bool initWindow()
{
	// glfw: initialize and configure
	// ------------------------------
	if (!glfwInit())
	{
		std::cout << "ERROR: could not start GLFW3" << std::endl;
		return false;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/*
	#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
	#endif
	*/

	// glfw window creation
	// --------------------
	//windowed
/*
	int width = 0;
	int height = 0;
	GetDesktopResolution(width, height);
	width = width * 9 / 10;
	height = height * 9 / 10;
	window = glfwCreateWindow(width, height, "Project_OpenGL", NULL, NULL);
	glfwSetWindowPos(window, height/50, width/50);
*/

	//borderless fullscreen
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	window = glfwCreateWindow(mode->width, mode->height, "Project_OpenGL", monitor, NULL);

	//window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Project_OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return true;
}

glm::vec3 vec3randomizer(float lo, float hi)
{
	float r1 = lo + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (hi - lo)));
	float r2 = lo + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (hi - lo)));
	float r3 = lo + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (hi - lo)));
	glm::vec3 vec(r1, r2, r3);

	return vec;
}

glm::vec3* vec3arrRandomizer(float lo, float hi)
{
	srand(static_cast <unsigned> (time(0)));

	glm::vec3 arr[5];
	for (int i = 0; i < sizeof(arr) / sizeof(glm::vec3); i++)
	{
		arr[i] = vec3randomizer(lo, hi);
	}
	return arr;
}

int shaderLightSetup(Shader shader)
{
	// directional light
	shader.setVec3("dirLight.direction", 0.0f, 10.0f, 10.0f);
	shader.setVec3("dirLight.ambient", 0.0125f, 0.0125f, 0.0125f);
	shader.setVec3("dirLight.diffuse", 0.1f, 0.1f, 0.1f);
	shader.setVec3("dirLight.specular", 0.25f, 0.25f, 0.25f);

	// spotLight
	shader.setVec3("spotLight.position", camera.Position);
	shader.setVec3("spotLight.direction", camera.Front);
	shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("spotLight.constant", 1.0f);
	shader.setFloat("spotLight.linear", 0.09);
	shader.setFloat("spotLight.quadratic", 0.032);
	shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

	// pointLights
	// point light 1
	shader.setVec3("pointLights[0].position", pointLightPositions[0]);
	shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[0].constant", 1.0f);
	shader.setFloat("pointLights[0].linear", 0.09);
	shader.setFloat("pointLights[0].quadratic", 0.032);
	// point light 2
	shader.setVec3("pointLights[1].position", pointLightPositions[1]);
	shader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[1].constant", 1.0f);
	shader.setFloat("pointLights[1].linear", 0.09);
	shader.setFloat("pointLights[1].quadratic", 0.032);
	// point light 3
	shader.setVec3("pointLights[2].position", pointLightPositions[2]);
	shader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[2].constant", 1.0f);
	shader.setFloat("pointLights[2].linear", 0.09);
	shader.setFloat("pointLights[2].quadratic", 0.032);

	// material properties
	shader.setFloat("material.shininess", 64.0f);

	shader.setInt("blinn", blinn);

	shader.setInt("flash", flash);

	return 0;
}

void renderScene(Shader &shader, unsigned int planeVAO, unsigned int floorDiffuseMap, unsigned int floorSpecularMap, Model knucklesModel, Model nanoModel, Model treeModel)
{
	glm::mat4 model;
	// draw objects
	shader.use();
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera.GetViewMatrix();
	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setVec3("viewPos", camera.Position);

	shader.setMat4("model", model);

	shaderLightSetup(shader);
	// floor
	glBindVertexArray(planeVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorDiffuseMap);
	// bind specular map
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, floorSpecularMap);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// render the loaded model
	model = glm::mat4(1.0);
	/////////////////////////////////////////////////////////
	////////////////////////KNUCKLES/////////////////////////
	/////////////////////////////////////////////////////////
	model = glm::translate(model, glm::vec3(0.0f, (float)(abs(sin(glfwGetTime()))), -1.0f)); // translate it down so it's at the center of the scene
	model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
	shader.setMat4("model", model);

	glm::mat3 normMat;
	normMat = glm::mat3(transpose(inverse(model)));
	shader.setMat3("normMat", normMat);
	knucklesModel.Draw(shader);

	for (unsigned int i = 0; i < sizeof(knucklesPositions) / sizeof(glm::vec3); i++)
	{
		// calculate the model matrix for each object and pass it to shader before drawing
		model = glm::mat4(1.0);
		if (i % 4 == 0)
			model = glm::translate(model, glm::vec3(knucklesPositions[i].x, knucklesPositions[i].y + (float)(abs(sin(glfwGetTime()))), knucklesPositions[i].z));
		else
			model = glm::translate(model, knucklesPositions[i]);
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));	// it's a bit too big for our scene, so scale it down
		if (i % 3 == 0)
		{
			float angle = 0;
			if (i % 4 == 0)
				angle = glfwGetTime() * 180.0f;
			else
				angle = glfwGetTime() * -30.0f;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		else
		{
			float angle = 15.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		}
		shader.setMat4("model", model);

		glm::mat3 normMat;
		normMat = glm::mat3(transpose(inverse(model)));
		shader.setMat3("normMat", normMat);
		knucklesModel.Draw(shader);
	}


	if ((glfwGetTime() > 10.0) && (glfwGetTime() < 12.5))
	{
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(camera.Position.x, camera.Position.y - 1, camera.Position.z - 1));
		model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		shader.setMat4("model", model);

		glm::mat3 normMat;
		normMat = glm::mat3(transpose(inverse(model)));
		shader.setMat3("normMat", normMat);
		knucklesModel.Draw(shader);
	}
	/////////////////////////////////////////////////////////
	////////////////////////NANOSUIT/////////////////////////
	/////////////////////////////////////////////////////////
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(5.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setMat4("model", model);

	normMat = glm::mat3(transpose(inverse(model)));
	shader.setMat3("normMat", normMat);
	nanoModel.Draw(shader);

	/////////////////////////////////////////////////////////
	//////////////////////////TREE///////////////////////////
	/////////////////////////////////////////////////////////
	for (unsigned int i = 0; i < sizeof(treePositions) / sizeof(glm::vec3); i++)
	{
		model = glm::mat4(1.0);
		model = glm::translate(model, treePositions[i]);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		float angle = 33.3f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
		shader.setMat4("model", model);

		normMat = glm::mat3(transpose(inverse(model)));
		shader.setMat3("normMat", normMat);
		treeModel.Draw(shader);
	}
}

unsigned int loadCubemap(vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

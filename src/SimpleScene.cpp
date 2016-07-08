#include "SimpleScene.h"

#include <cstdio>
#include <cstdlib>

#include <iostream>

#include <vector>

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/constants.hpp"

SimpleScene::SimpleScene(int w, int h) : IScene(w, h) { }
SimpleScene::~SimpleScene() {
}

void SimpleScene::initScene() {
	compileAndLinkShader();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	glGenTextures(1, &tex_output);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_output);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, WINDOW_WIDTH, WINDOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, nullptr);
	glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	{ // query up the workgroups
		int work_grp_size[3], work_grp_inv;
		// maximum global work group (total work in a dispatch)
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_size[0]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_size[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_size[2]);
		printf("max global (total) work group size x:%i y:%i z:%i\n",
			work_grp_size[0], work_grp_size[1], work_grp_size[2]);
		// maximum local work group (one shader's slice)
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);
		printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
			work_grp_size[0], work_grp_size[1], work_grp_size[2]);
		// maximum compute shader invocations (x * y * z)
		glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
		printf("max computer shader invocations %i\n", work_grp_inv);
	}

	computeProgram.load("../shader/compute.comp", GL_COMPUTE_SHADER);
	computeProgram.compile_and_link();
	computeProgram.add_uniform("time");
	computeProgram.add_uniform("size");

	GLfloat verts[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f
	};
	GLuint bufHandle;
	glGenBuffers(1, &bufHandle);
	glBindBuffer(GL_ARRAY_BUFFER, bufHandle);
	glBufferData(GL_ARRAY_BUFFER, 4 * 3 * sizeof(GLfloat), verts, GL_STATIC_DRAW);

	// Set up the vertex array object
	glGenVertexArrays(1, &fsQuad);
	glBindVertexArray(fsQuad);

	glBindBuffer(GL_ARRAY_BUFFER, bufHandle);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);  // Vertex position

	glBindVertexArray(0);

	glEnable(GL_DEPTH_TEST);
}


void SimpleScene::update( float t ) {
	angle += 0.1 * t;
}

void SimpleScene::draw(Camera* camera) {
	prog.use();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	view = camera->GetViewMatrix();
	projection = camera->GetProjectionMatrix();

	// draw scene
	drawScene();

	prog.unuse();
}

void SimpleScene::updateMatrices() {
	glm::mat4 modelView = view * model;

	prog.send_uniform("normal", glm::mat3(glm::vec3(modelView[0]), glm::vec3(modelView[1]), glm::vec3(modelView[2])));
	prog.send_uniform("modelView", modelView);
	prog.send_uniform("modelViewProj", projection * modelView);
}

void SimpleScene::resize(int w, int h) {
	glViewport(0, 0, w, h);
	width = w;
	height = h;
}

void SimpleScene::compileAndLinkShader() {
	prog.load("../shader/shader.vert", GL_VERTEX_SHADER);
	prog.load("../shader/shader.frag", GL_FRAGMENT_SHADER);
	prog.compile_and_link();
	prog.add_uniform("normal");
	prog.add_uniform("modelView");
	prog.add_uniform("modelViewProj");
	prog.add_uniform("Kd");
}

void SimpleScene::drawScene() {
	{ // launch compute shaders!
		computeProgram.use();
		computeProgram.send_uniform("time", (float)glfwGetTime());
		computeProgram.send_uniform("size", glm::vec2((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT));
		glDispatchCompute((GLuint)WINDOW_WIDTH, (GLuint)WINDOW_HEIGHT, 1);
	}
	// prevent sampling befor all writes to image are done
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glClear(GL_COLOR_BUFFER_BIT);
	prog.use();
	glBindVertexArray(fsQuad);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindVertexArray(0);
}

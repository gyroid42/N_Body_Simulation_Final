
//#include <GL\glew.h>
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <stdlib.h>
#include <stdio.h>


static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

int main(void) {


	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {

		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glClearColor(0.39f, 0.58f, 93.0f, 1.0f);


	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(640, 480, "Hello World!", NULL, NULL);
	if (!window) {

		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSwapInterval(1);



	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	do {

		glClear(GL_COLOR_BUFFER_BIT);


		glfwSwapBuffers(window);
		glfwPollEvents();
	} while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);
	



	return 0;
}

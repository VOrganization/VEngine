#include "Engine.h"

double r = 20.0;

void window_scroll(GLFWwindow*, double s_x, double s_y) {
	r -= s_y * 0.5;
}

using namespace std;

int main(int argc, char** argv) {
	Engine* engine = new Engine(argc, argv);


	glfwHideWindow(engine->context->window);
	glfwSetScrollCallback(engine->context->window, window_scroll);

	engine->scene = new Scene("main.scene");
	engine->InitScene();

	ObjectCamera* cam = engine->scene->defaultCamera;
	cam->UseDirection = true;

	glfwShowWindow(engine->context->window);

	while (engine->IsOpen()) {
		glfwSetWindowTitle(engine->context->window, string("VKEngineV2 -- " + to_string(engine->window_fps)).c_str());

		cam->transform->position = vec3(sin(glfwGetTime())*r, 5.0, cos(glfwGetTime())*r);
		cam->Direction = -cam->transform->position;

		engine->Update();
	}

	engine->~Engine();

	return 0;
}

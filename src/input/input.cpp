#include "input.h"
#include <GLFW/glfw3.h>

namespace Input {
	Input::Input(GLFWwindow& glfw_window) : glfw_window(&glfw_window)
	{
	}
	bool Input::is_key_pressed(int key)
	{
		int result = glfwGetKey(glfw_window, key);
		return (result == GLFW_PRESS);
	}
	void Input::enable_cursor()
	{
		glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	void Input::disable_cursor()
	{
		glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}
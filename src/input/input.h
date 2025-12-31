#pragma once

#include <string>

struct GLFWwindow;

namespace Input {
	class Input {
	public:
		Input(GLFWwindow& glfw_window);
		bool is_key_pressed(int key);
		bool is_key_pressed_str(std::string key);
		void enable_cursor();
		void disable_cursor();
	private:
		GLFWwindow* glfw_window{};
	};
}
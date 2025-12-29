#pragma once

struct GLFWwindow;

namespace Input {
	class Input {
	public:
		Input(GLFWwindow& glfw_window);
		bool is_key_pressed(int key);
		void enable_cursor();
		void disable_cursor();
	private:
		GLFWwindow* glfw_window{};
	};
}
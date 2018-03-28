#include <iostream>
#include <vector>
#include <string>

#include <glfw3.h>
#include "glad.h"



namespace {
	void errorCallback(int errnum, const char *errmsg) {
		std::cerr << errnum << ": " << errmsg << std::endl;
	}

	void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
		// construct error message
		std::string sourceString;
		std::string typeString;
		std::string severityString;

		switch (source) {
		case GL_DEBUG_SOURCE_API:
			sourceString = "OpenGL API";
			break;
		case GL_DEBUG_SOURCE_APPLICATION:
			sourceString = "Application";
			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:
			sourceString = "Shader Compiler";
			break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:
			sourceString = "Third Party";
			break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
			sourceString = "Window System";
			break;
		default:
			sourceString = "Other";
			break;
		}

		switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			typeString = "Error";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			typeString = "Deprecated behavior";
			break;
		case GL_DEBUG_TYPE_MARKER:
			typeString = "Marker";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			typeString = "Performance";
			break;
		case GL_DEBUG_TYPE_POP_GROUP:
			typeString = "Pop group";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			typeString = "Portability";
			break;
		case GL_DEBUG_TYPE_PUSH_GROUP:
			typeString = "Push group";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			typeString = "Undefined behavior";
			break;
		default:
			typeString = "Other";
			break;
		}

		switch (severity) {
		case GL_DEBUG_SEVERITY_HIGH:
			severityString = "High";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			severityString = "Medium";
			break;
		case GL_DEBUG_SEVERITY_LOW:
			severityString = "Low";
			break;
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			severityString = "Notification";
			break;
		default:
			severityString = "Other";
		}

		// Print formatted and human readable message
		std::cerr << "Type:" << typeString << std::endl
			<< "Source: " << sourceString << std::endl
			<< "Severity: " << severityString << std::endl
			<< "ID: " << id << std::endl
			<< "Message: " << message << std::endl << std::endl;
	}
} // namespace

static const std::string vertexShader = R"glsl(
		#version 450
		precision mediump float;

		in vec2 position;
		in vec3 color;

		out vec4 vertColor;

		void main() {
			gl_Position = vec4(position, 0.0, 1.0);
			vertColor = vec4(color, 1.f);
		}
	)glsl";

static const std::string fragmentShader = R"glsl(
		#version 450
		precision mediump float;

		out vec4 outColor;

		in vec4 vertColor;

		void main() {
			outColor = vertColor;
		}
	)glsl";

GLuint newShader(const GLenum shaderType, const std::string &source) {
	GLuint id = glCreateShader(shaderType);

	const char *c_str = source.c_str();

	glShaderSource(id, 1, &c_str, nullptr);
	glCompileShader(id);

	GLint status = 0;
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE) {
		GLint logLength = 0;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);

		auto log = std::string{};
		log.resize(logLength + 1, '\x00'); // TODO: +1 needed here?
		glGetShaderInfoLog(id, logLength, nullptr, const_cast<char *>(log.data()));
		throw std::runtime_error(log);
	}

	return id;
}

GLuint newProgram(const std::string &vertexShaderSource, const std::string &fragmentShaderSource) {

	GLuint vertexShader = newShader(GL_VERTEX_SHADER, vertexShaderSource);
	GLuint fragmentShader = newShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	const auto program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	GLint status = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &status);

	if (status == GL_FALSE) {
		GLint logLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

		auto log = std::string();
		log.resize(logLength + 1, '\x00'); // TODO: +1 needed here?
		glGetProgramInfoLog(program, logLength, nullptr, const_cast<char *>(log.data()));

		throw std::runtime_error(log);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}

int main(int /*argc*/, char * /*argv[]*/) {
	glfwSetErrorCallback(errorCallback);


	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // will not work
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE); // works
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow *window = glfwCreateWindow(1280, 720, "OpenGL", nullptr, nullptr);
	if (!window) {
		glfwTerminate();
		return -2;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGL())
		return -3;
	{
		// Output information about Vendor, Version and Renderer
		std::cout << glGetString(GL_VENDOR) << std::endl;
		std::cout << glGetString(GL_VERSION) << std::endl;
		std::cout << glGetString(GL_RENDERER) << std::endl;

		// Set debug output
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback((GLDEBUGPROC)debugCallback, 0);


		GLuint program = newProgram(vertexShader, fragmentShader);

		glUseProgram(program);

		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);


		const auto colorAttrib = glGetAttribLocation(program, "color");
		const auto posAttrib = glGetAttribLocation(program, "position");

		// Create Vertex Buffer
		GLuint vertexBuffer = 0;
		glGenBuffers(1, &vertexBuffer);

		// Fill vertex buffer with demo data (16,777,216 vec2 float Values, all zero)
		const std::vector<float> vertices = std::vector<float>(134217728, 0.f);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);

		// Create Color Buffer
		GLuint colorBuffer = 0;
		glGenBuffers(1, &colorBuffer);

		// Fill color buffer with demo data (16,777,216 RGB Values, all 255)
		const std::vector<char> colors = std::vector<char>(50331648, (char)255);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glBufferData(GL_ARRAY_BUFFER, colors.size(), colors.data(), GL_DYNAMIC_DRAW);

		// Bind color buffer and set format
		glEnableVertexAttribArray(colorAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
		glVertexAttribPointer(colorAttrib, 3, GL_UNSIGNED_BYTE, GL_TRUE, 0, nullptr);

		// Bind vertex buffer and set format
		glEnableVertexAttribArray(posAttrib);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 0, nullptr);


		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
				glfwSetWindowShouldClose(window, GLFW_TRUE);

			glClearColor(0.1f, 0.1f, 0.1f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glPointSize(10.f);
			glDrawArrays(GL_POINTS, 2048, 4096);

			glfwSwapBuffers(window);
		}
	}

	glfwTerminate();

	return 0;
}

/*
	This is a demo program designed to teach you how to compile an application that uses
	multiple libraries:

	1) OpenGL is a graphics application programming interface (API) designed to interact
		directly with graphics cards to draw three-dimensional stuff on the screen.
		https://www.opengl.org/

	2) GLFW is a "window manager" specifically designed for OpenGL. It works with the
		operating system to create a window that acts as an OpenGL "context", which is
		a part of the screen that acts as a target for OpenGL commands.
		https://www.glfw.org/

	3) ImGui is a real-time graphical user interface library. This uses a graphics API
		like OpenGL to render standard GUI objects like buttons, menus, list boxes, etc.
		The GitHub page is worth looking at: https://github.com/ocornut/imgui
		Here is a web-based demo that shows you the code needed to create any UI element:
		https://pthom.github.io/imgui_manual_online/manual/imgui_manual.html

	4) GLM is a light-weight linear algebra library that defines C/C++ objects useful for
		2D and 3D graphics.
		https://github.com/g-truc/glm
*/

// This header files contains declarations for functions and variables that are used across multiple C files.
#include "kessel.h"

// I include this file to throw runtime errors, but you can also use it to output debugging information.
#include <iostream>
#include <fstream>

#include <glm/glm.hpp>
glm::vec3 a;

int resolution = 64;					// resolution of the output image (you can add a user interface element to change this)
float* output_image_ptr = nullptr;		// pointer to the output image data (if you change the resolution make sure to change this!)
float frame_seconds = 0.0f;		// time it takes to go through the main "game" loop (directly translates to frame rate or fps)

std::vector<float> gx;
std::vector<float> gy;
std::vector<float> m;

std::vector< std::vector<float> > px;
std::vector< std::vector<float> > py;

void load_gravity(const char* filename) {
	std::ifstream gravity_file(filename);
	if (!gravity_file)
		throw std::runtime_error("Failed to open gravity file");

	while (!gravity_file.eof()) {
		float x, y, mass;
		char c;
		gravity_file >> x >> y >> mass >> c;
		gx.push_back(x);
		gy.push_back(y);
		m.push_back(mass);
	}

}


void dummy_data(int G = 10, int P = 100) {

	for (int g = 0; g < G; g++) {
		double x = (double)rand() / (double)RAND_MAX * 20.0 - 10.0;
		double y = (double)rand() / (double)RAND_MAX * 20.0 - 10.0;
		gx.push_back(x);
		gy.push_back(y);
		m.push_back(1);
	}

	float dp = 20.0f / (float)P;
	std::vector<float> path_x;
	std::vector<float> path_y;
	for (int p = 0; p < P; p++) {
		float x = -10.0 + (float) p * dp;
		float y = 10 * sin(x);
		path_x.push_back(x);
		path_y.push_back(y);
	}
	px.push_back(path_x);
	py.push_back(path_y);

}

int main(int argc, const char* argv[]) {

	/*
	* GLFW is the window manager that we will be using. Its job is to create
	* an "OpenGL context", which is a region of the screen that is used as
	* a target for OpenGL commands. This function initializes the GLFW library
	* and throws an exception if there are any hardware or software issues that
	* prevent it from initializing.
	*/
	if (!glfwInit())
		throw std::runtime_error("Failed to initialize GLFW");

	/*
	* This function call uses GLFW to create a window, including an OpenGL context. The
	* GLFWwindow pointer will be used in any GLFW library calls that effect this particular
	* window. For example, if you want to assign a callback function that responds to keyboard
	* or mouse input, you'll pass the GLFWwindow pointer. This also allows you to generate
	* multiple windows - you just make another call to glfwCreateWindow() and store the window
	* in a different GLFWwindow pointer! The next line of code makes sure the window is
	* successfully created (and throws a runtime exception if something went wrong).
	*/
	GLFWwindow* window = glfwCreateWindow(1920, 1080, "Hello World", nullptr, nullptr);
	if (window == nullptr)
		throw std::runtime_error("Failed to create GLFW window");

	/*
	* This function makes the OpenGL context assigned to the window we created the "current"
	* context. That means that any OpenGL calls that we make will be sent to this context. If
	* we had multiple windows, we would have to set the contexts to the appropriate window
	* before we started sending commands to draw stuff.
	*/
	glfwMakeContextCurrent(window);

	/*
	* Now we're going to initialize the ImGui library. This is a really cool library that
	* uses OpenGL (or any graphics API) to render standard graphical user interface components.
	*/
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();

	// This sets the GUI style to one that I personally think looks cool
	ImGui::StyleColorsDark();

	/*
	* These functions tell ImGui to use OpenGL and GLFW to render the user interface
	* and also specifies the version of the OpenGL shading language (GLSL) to use
	* for rendering. We will discuss shading languages near the end of the class since this
	* is the foundation for GPU computing. For now, all you need to know is that it's a
	* sub-language that is part of OpenGL and used to render stuff to individual pixels in the window.
	*/	
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	const char* glsl_version = "#version 120";
	ImGui_ImplOpenGL3_Init(glsl_version);

	/*
	* This initializes a library called the OpenGL Extension Wrangler (GLEW) library. This is mostly
	* used for Windows and is a side effect of how the OpenGL library works with the operating system.
	* Basically, by default Windows only provides access to a very early version of OpenGL. This library
	* "mines" the OpenGL libraries to figure out what extensions are ACTUALLY supported by your graphics
	* driver. Don't worry too much about this - you'll probably never use anything beyond these two lines
	* of code.
	*/
	//if (glewInit() != GLEW_OK)
	//	throw std::runtime_error("Failed to initialize GLEW");


	/*
	 * Allocate space on the heap to store the image that will be displayed on the screen.
	 */
	output_image_ptr = new float[resolution * resolution * 4];


	/* Add dummy data so that some points appear in the plot when students first compile and run the demo
	 */
	if (argc > 1) load_gravity(argv[1]);
	else dummy_data();

	run_kessel_simulations();   // <-- ADD THIS LINE (runs ~400 Monte-Carlo simulations)
	/*
	* This is what we call the "main rendering loop" (sometimes in gaming you'll call it the "main game loop".
	* This is where everything in the program happens. Every iteration through the loop re-draws everything
	* onto the window (including the user interface). It also queries sources for user input (mouse,
	* keyboard, etc.) and updates variables based on callback functions that you define. This loop runs
	* continuously until you kill the program (ex. by pressing the X button to close the window).
	*/
	while (!glfwWindowShouldClose(window)) {
		auto start = std::chrono::high_resolution_clock::now();

		// This function checks potential input sources (keyboard, mouse, etc.) and executes callback functions
		glfwPollEvents();

		// This function tells OpenGL to clear the window (in this case it writes the color "black" to all pixels)
		glClear(GL_COLOR_BUFFER_BIT);

		// This function is defined in the helloworld_gui.cpp file and renders the user interface
		ImGuiRender();

		/*
		* This function takes advantage of a concept called "double buffering". There is a region
		* of memory on your graphics card that displays the contents of the screen. The monitor reads this region
		* of memory every time it refreshes. This introduces a problem: if you draw to this area of memory
		* the user may be able to see the individual objects that you draw, resulting in "flickering". Double
		* buffering prevents this by sending the draw calls to a "back buffer" that isn't displayed on the
		* computer monitor. This function switches the memory pointers so that the information on the monitor
		* is updated immediately.
		*/
		glfwSwapBuffers(window);                    // swap the double buffer

		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> duration = end - start;
		frame_seconds = duration.count();
	}

	/*
	* All of these functions just destroy the stuff that we've created to make sure that there aren't any
	* memory leaks.
	*/
	ImGui_ImplOpenGL3_Shutdown();					// Shut down ImGui's connection with OpenGL
	ImGui_ImplGlfw_Shutdown();						// Shut down ImGui's connection with GLFW
	ImPlot::DestroyContext();						// Clear the ImPlot user interface
	ImGui::DestroyContext();                        // Clear the ImGui user interface

	glfwDestroyWindow(window);                      // Destroy the GLFW rendering window
	glfwTerminate();                                // Terminate GLFW
}
#include "cg2application.h"
#include "gl_utils.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>


#define UNUSED_PARAM(x) do { (void)(x); } while (0)

/**
 * @brief resize_event Will be called every time the framebuffer is
 * resized!
 * @param w new width in pixels
 * @param h new height in pixels
 */
void CG2App::resize_event(int w, int h)
{
}

/**
 * @brief keyboard_event will be called every time a key on the key
 * board is pressed or released.
 * @param key The key (GLFW_KEY_*)
 * @param action The action that was performed.
 *	One of GLFW_RELEASE,GLFW_PRESS, GLFW_REPEAT
 * @param mods The modifyer keys, that were pressed.
 *	A bitmask composed of these bits:
 *      GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, GLFW_MOD_ALT, GLFW_MOD_SUPER
 */

void CG2App::keyboard_event(int key, int action, int mods)
{
	UNUSED_PARAM(action); UNUSED_PARAM(mods);

	if(key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)
		stop_rendering();
}
/**
 * @brief mouse_button_event will be called every time a mouse button
 * was pressed or released
 * @param button The button (GLFW_MOUSE_BUTTON_*)
 * @param action The action that was performed.
 *	One of GLFW_RELEASE,GLFW_PRESS, GLFW_REPEAT
 * @param mods The modifyer keys, that were pressed.
 *	A bitmask composed of these bits:
 *      GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, GLFW_MOD_ALT, GLFW_MOD_SUPER
 */

void CG2App::mouse_button_event(int button, int action, int mods)
{
	UNUSED_PARAM(button);UNUSED_PARAM(action);UNUSED_PARAM(mods);
}

/**
 * @brief mouse_wheel_event will be called, whenever the mouse wheele is
 * turned/shifted.
 * @param x The offset in the x direction
 * @param y The offset in the y direction
 */
void CG2App::mouse_wheel_event(float x, float y)
{
	UNUSED_PARAM(x);UNUSED_PARAM(y);
}



struct MyVertex
{
	float position[3];
	float color[3];
};

/*
	 *   3-------2
	 *  /|      /|
	 * 7-+-----6 |
	 * | |     | |
	 * | 0-----+-1
	 * |/      |/
	 * 4-------5
	 * */
static MyVertex cube_data[36] =
{
        {{ -1,-1,-1},{0,0,0}},  //0
		{{1,-1,-1},{1,0,0}},    //1
        {{ 1, 1,-1},{1,1,0}},   //2
        {{-1, 1,-1},{0,1,0}},   //3
        {{-1,-1, 1},{0,0,1}},   //4
        {{ 1,-1, 1},{1,0,1}},   //5
        {{ 1, 1, 1},{1,1,1}},   //6
        {{-1, 1, 1},{0,1,1}},   //7

};


/**
 * @brief init_gl_state will be called once when the renderloop is
 * entered.
 */
void CG2App::init_gl_state()
{
	GLfloat positions[9] = {-1,-1,0, 1,-1,0, 0,1,0};
	GLfloat colors[9] = { 1.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,1.0 };
	GLuint index_data[] = {
		0,1,2,  //1
		2,3,0,  //2
		0,1,5,  //3
		5,4,0,  //4
		0,3,7,  //5
		7,4,0,  //6
		6,5,4,  //7
		7,6,4,  //8
		7,6,2,  //9
		7,3,2,  //10
		6,5,1,  //11
		6,2,1   //12
	};
	// Set the background color
	glClearColor(0.1f,0.1f,0.2f,1.0f);
	// Enable Depth Test
	glEnable(GL_DEPTH_TEST);

	// Build our shader program
	prg = cg2_load_compile_link_program("data/shaders/example.vs.glsl",
	                                    "data/shaders/example.fs.glsl");
	// user our shader program
	glUseProgram(prg);

	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);
	// Erzeugung des Buffers
	glGenBuffers(1,&bn);
	glBindBuffer(GL_ARRAY_BUFFER,bn);
	glBufferData(GL_ARRAY_BUFFER,sizeof(cube_data), cube_data,GL_STATIC_DRAW);
	//Element buffer
	glGenBuffers(2, &ibn);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibn);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data), index_data, GL_STATIC_DRAW);
	// Erstellung des Vertex Attribute Pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// A1 (a): Erzeugen des Buffer Objects color_bn und uebertragen der
	// Farbwerte aus colors.
	// Hinweis: Orientieren Sie sich an der Uebertragung der Positionsdaten.

	// A1 (b): Erstellung des Vertex Attribute Pointers fuer das
	// Farbattribut (location 1).
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (GLvoid*)(offsetof(MyVertex,position)));
	glEnableVertexAttribArray(1);
	//glBindVertexArray(0);
}

/**
 * @brief render_one_frame called once per frame
 */
void CG2App::render_one_frame()
{
	// clear the color buffer
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glm::mat4 MVP(1.0);

	// A2 (b) & (c): Modifizieren Sie MVP so, dass die Vertexpositionen
	// entsprechend Transformiert werden.
	//matrix persp(fov,w/h,near,far)
	MVP = glm::perspective(glm::radians(60.0f), static_cast<float>(framebuffer_size.x) / framebuffer_size.y, 0.1f, 10.0f)* 
		glm::lookAt(glm::vec3(3.0f, 3.0f, 3.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f))*
		//glm::translate(glm::vec3(-0.5f, 0.0f, 0.0f))*
		glm::rotate(static_cast<float>(time_stamp), glm::vec3(0.0f, 1.0f, 0.0f));


	// Setzen des Matrix Uniforms
	glUniformMatrix4fv(0,1,GL_FALSE,&(MVP[0][0]));

	glBindVertexArray(vao);

	// Der Drawcall
	glDrawElements(GL_TRIANGLES,36,GL_UNSIGNED_INT,0);

}

///-----------------------------------------------------------------------------

int main (int argc, char **argv)
{
	UNUSED_PARAM(argc);
	UNUSED_PARAM(argv);
	CG2App app;
	app.enter_render_loop();
}


#include "util.h"
#include "cg2application.h"




/****************************************************************************
 * WINDOW-RELATED CALLBACKS                                                 *
 ****************************************************************************/

/* This function is registered as the framebuffer size callback for GLFW,
 * so GLFW will call this during glfwPollEvents() whenever the window
 * is resized. */
static void callback_Resize(GLFWwindow *win,
                            int w,
                            int h)
{
	CG2App *app=reinterpret_cast<CG2App*>(glfwGetWindowUserPointer(win));
	app->event_data.framebuffer_size = glm::ivec2(w,h);
	app->resize_event(w,h);
}

/*  This function is registered as the scroll callback for GLFW,
 *  so GLFW will call this furing glfwPollEvents whenever the mouse
 *  wheel was used */
static void callback_Scroll(GLFWwindow *win,
                            double xoffset,
                            double yoffset)
{
	CG2App *app=reinterpret_cast<CG2App*>(glfwGetWindowUserPointer(win));
	app->mouse_wheel_event(static_cast<float>(xoffset),
	                       static_cast<float>(yoffset));
}

/* This function is registered as the keyboard callback for GLFW, so GLFW
 * will call this during glfwPollEvents() whenever a key is pressed. */
static void callback_Keyboard(GLFWwindow *win,
                              int key,
                              int /*scancode*/,
                              int action,
                              int mods)
{
	CG2App *app=reinterpret_cast<CG2App*>(glfwGetWindowUserPointer(win));
	if (key < 0 || key > GLFW_KEY_LAST) {
		warn("invalid key code %d?!",key);
		return;
	}
	if(action == GLFW_RELEASE){
		app->event_data.key_pressed[key] = false;
		app->event_data.key_was_released[key] = true;
	}else {
		app->event_data.key_pressed[key] = true;
	}

	app->keyboard_event(key,action,mods);
}

/* This function is registered as the mouse button callback for GLFW, so GLFW
 * will call this during glfwPollEvents() whenever a button is pressed. */
static void callback_MouseButton(GLFWwindow *win,
                                 int button,
                                 int action,
                                 int mods)
{
	CG2App *app=reinterpret_cast<CG2App*>(glfwGetWindowUserPointer(win));

	if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) {
		warn("invalid button code %d?!",button);
		return;
	}

	if(action == GLFW_RELEASE){
		app->event_data.button_pressed[button] = false;
		app->event_data.button_was_released[button] = true;
	}else {
		app->event_data.button_pressed[button] = true;
	}
	app->mouse_button_event(button,action,mods);
}

/* This function is registered as the curso move callback for GLFW, so GLFW
 * will call this during glfwPollEvents() giving us the nur cursor position */
static void callback_Cursor(GLFWwindow *win,
                            double x,
                            double y)
{
	CG2App *app=reinterpret_cast<CG2App*>(glfwGetWindowUserPointer(win));
	int w = 0,h = 0;
	glfwGetFramebufferSize(win,&w,&h);
	glm::vec2 cp(static_cast<float>(x),
	             static_cast<float>(h-y));
	app->event_data.mouse_cursor_delta = cp-app->event_data.mouse_cursor;
	app->event_data.mouse_cursor = cp;
}


/****************************************************************************
 * GLOBAL INITIALIZATION AND CLEANUP                                        *
 ****************************************************************************/

/* Wrapper for glfwProcAddresss (which is a wrapper around the platform's
 * wgl/glX/whatever GetProcAddress) for use with the GLAD loader...
 */
static void *
wrap_getprocaddress(const char *name, void *user_ptr)
{
	(void)user_ptr; // unusued
	return reinterpret_cast<void*>(glfwGetProcAddress(name));
}


bool CG2App::init_glfw_backend()
{
	/* initialize GLFW library */
	if (!glfwInit()) {
		warn("Failed to initialze GLFW");
		return false;
	}

	GLFWmonitor *monitor=nullptr;

	/* request a OpenGL 4.3 core profile context */
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	/* create the window and the gl context */

	m_glfw_win=glfwCreateWindow( 800, 800,
	                             "CG2 Application",
	                             monitor,
	                             nullptr);
	if (!m_glfw_win) {
		warn("failed to get window with OpenGL 4.3 core context");
		/* retry without a debug context */
	}


	/* store a pointer to our application context in GLFW's window data.
	 * This allows us to access our data from within the callbacks */
	glfwSetWindowUserPointer(m_glfw_win, this);
	/* register our callbacks */
	glfwSetFramebufferSizeCallback(m_glfw_win, callback_Resize);
	glfwSetKeyCallback(m_glfw_win, callback_Keyboard);
	glfwSetMouseButtonCallback(m_glfw_win, callback_MouseButton);
	glfwSetScrollCallback(m_glfw_win, callback_Scroll);
	glfwSetCursorPosCallback(m_glfw_win,callback_Cursor);


	/* make the context the current context (of the current thread) */
	glfwMakeContextCurrent(m_glfw_win);

	/* initialize glad (our opengl loader) */
	if (!gladLoadGL(wrap_getprocaddress,nullptr)) {
		warn("failed to intialize glad");
		return false;
	}
	/* just make sure we got a >= 4.3 context */
	if (!GLAD_GL_VERSION_4_3) {
		warn("did not get a GL >= 4.3 context!");
		return false;
	}

	/* print some information about the gl context */
	info("GL_VERSION  : %s",glGetString(GL_VERSION));
	info("GL_VENDOR   : %s",glGetString(GL_VENDOR));
	info("GL_RENDERER : %s",glGetString(GL_RENDERER));
	info("GLSL_VERSION: %s",glGetString(GL_SHADING_LANGUAGE_VERSION));


	/* reset the cursor to the window center */
	int w,h;
	glfwGetFramebufferSize(m_glfw_win, &w, &h);
	resize_event(w,h);
	event_data.framebuffer_size = glm::ivec2(w,h);
	glfwSetCursorPos(m_glfw_win, w/2, h/2);
	event_data.mouse_cursor = glm::vec2(w/2,h/2);
	event_data.mouse_cursor_delta = glm::vec2(0,0);

	m_watch[0] = new CG2GLWatch<>();
	m_watch[1] = new CG2GLWatch<>();

	m_cpu_time = m_gpu_time =  m_num_timestamps = 0;

	m_time_passed = 1000;
	glfwSetInputMode(m_glfw_win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return true;
}
void CG2App::destroy_glfw_backend()
{
	delete m_watch[0];
	delete m_watch[1];
	glfwDestroyWindow(m_glfw_win);
	glfwTerminate();
	m_glfw_win = nullptr;

}

void CG2App::update_title()
{
	m_time_passed+=event_data.delta_time;

	m_gpu_time += m_watch[1]->get_gpu_time_in_ms()-
	                m_watch[0]->get_gpu_time_in_ms();

	m_cpu_time += m_watch[1]->get_cpu_time_in_ms()-
	                m_watch[0]->get_cpu_time_in_ms();

	m_num_timestamps++;

	if(m_time_passed >= 0.5) // update every 500ms
	{
		m_gpu_time/= static_cast<double>(m_num_timestamps);
		m_cpu_time/= static_cast<double>(m_num_timestamps);

		std::stringstream str;
		str.precision(3);
		str.setf(std::ios::fixed);
		str<<"CG2 Application\t  CPU:"<<m_cpu_time
		  <<"ms\t  GPU:"<<m_gpu_time<<"ms";
		glfwSetWindowTitle(m_glfw_win,str.str().c_str());

		m_cpu_time = m_gpu_time = m_time_passed = m_num_timestamps=0;
	}

}

void CG2App::enter_render_loop()
{
	// call the resize event after init_gl_state
	resize_event(event_data.framebuffer_size.x,event_data.framebuffer_size.y);
	// init/reset the time stamp
	event_data.time_stamp = glfwGetTime();
	while(!glfwWindowShouldClose(m_glfw_win)){
		// update time
		double new_ts = glfwGetTime();
		event_data.delta_time = new_ts-event_data.time_stamp;
		event_data.time_stamp = new_ts;
		// take time 0
		m_watch[0]->take();

		render_one_frame();

		// take time 1
		m_watch[1]->take();

		glfwSwapBuffers(m_glfw_win);
		// generate and set the window's title text
		update_title();
		event_data.reset_released();
		glfwPollEvents();

	}
}

void CG2App::catchCursor(bool c)
{
	if(c)
		glfwSetInputMode(m_glfw_win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		glfwSetInputMode(m_glfw_win, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}


void CG2App::stop_rendering(){glfwSetWindowShouldClose(m_glfw_win,1);}

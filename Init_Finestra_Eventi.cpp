#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "gestione_interazioni.h"

using namespace std;

extern GLFWwindow* window;
extern int height, width;
extern float h_update, w_update;

int inizializza_window_context_events() 
{

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    window = glfwCreateWindow(mode->width*0.95, mode->height*0.95, "Hello World", NULL, NULL);
    height = mode->height*0.95;
    width=mode->width*0.95;
    h_update = (float)(height);
    w_update = (float)(width);
    if (!window)
    {
        cout << "Failde to create the window !" << endl;
        glfwTerminate(); 
        return -1;
    }

    glfwMakeContextCurrent(window);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to load opengl function pointers !" << endl;
        glfwTerminate();
        return -1;
    }


        glfwSetKeyCallback(window, key_callback);

        
        int oX = 0;
        int oY = 0;

       
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPos(window, oX, oY);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(window, cursor_position_callback);
       

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback1);

}
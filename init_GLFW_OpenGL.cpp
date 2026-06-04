#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
int init_gl_GLFW();

int  init_gl_GLFW()
{

if (!glfwInit())   
return -1;

glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

}
#version 330 core
layout (location = 0) in vec3 aPos;   
layout (location = 1) in vec4 aColor; 
  
out vec4 ourColor; 
uniform mat4 Projection;  
uniform mat4 Model;
void main()
{
    gl_Position = Projection*Model*vec4(aPos, 1.0);
    ourColor = aColor; // set ourColor to the input color we got from the vertex data
}  


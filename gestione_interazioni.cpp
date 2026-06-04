#pragma once
#include "lib.h"
#include "Gui.h"
#include "strutture.h"
#include "inizializzazioni.h"
#include "init_geometrie.h"
#include "Utilities.h"
#include <ImGui\imgui.h>           
#include <ImGui\imgui_impl_glfw.h> 
#include <ImGui\imgui_impl_opengl3.h> 

#define STEP 10.0f
#define MAXIMUM_PLAYER 2
#define PAC_MAN_IN_SCENE 2
#define MAN_PAC_IN_SCENE 1
#define WALL_INDEX 3
#define MAXIMUM_BULLET 5

extern int TOT_WALLS;

extern int height, width;
extern float  w_update, h_update;
extern GLFWwindow* window;

extern unsigned int programId;
extern mat4 Projection;
extern vector<Figura> Scena;

extern double mousex,mousey;

extern Figura proiettile;
extern vector<Figura> proiettili;

extern bool showScore;
extern bool gameIsOver;
extern bool pacmanWon;
extern int manPacShotsFired;

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) 
{
    mousex = xpos;
    mousey =  height-ypos;
}

void attemptMove(Figura& player, vec3 moveDelta) 
{
    vec3 newPos = player.position + moveDelta;

    Figura testPlayer = player;
    testPlayer.position = newPos;

    testPlayer.Model = mat4(1.0f);
    testPlayer.Model = translate(testPlayer.Model, testPlayer.position);
    testPlayer.Model = scale(testPlayer.Model, testPlayer.scale);
    updateBB(&testPlayer);

    bool hasCollided = false;

    int wall_index = WALL_INDEX; 
    int num_wall = TOT_WALLS;   

    for (int i = wall_index; i < wall_index + num_wall && i < Scena.size(); i++) 
    {
        if (checkCollision(testPlayer, Scena[i])) {
            hasCollided = true;
            break;
        }
    }

    for (int i = 1; i <= MAXIMUM_PLAYER; i++) 
    {
        if (&Scena[i] != &player) {
            if (checkCollision(testPlayer, Scena[i])) 
            {
               
                gameIsOver = true;
                pacmanWon = false; 
                hasCollided = true; 
                break;
            }
        }
    }

    if (!hasCollided) 
    {
        player.position = newPos;
        player.Model = mat4(1.0f);
        player.Model = translate(player.Model, player.position);
        player.Model = scale(player.Model, player.scale);
        updateBB(&player);
    }
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) 
{

    const float step = STEP;
    float offset = 0;

    if (gameIsOver) {
        if (key == GLFW_KEY_F1 && action == GLFW_PRESS) 
        {
            showScore = !showScore;
        }
        return; 
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT) 
    {
        // Pac-Man (IJKL)
        if (Scena.size() > MAN_PAC_IN_SCENE)
        {
            vec3 delta(0.0f);
            switch (key) {
            case GLFW_KEY_I: delta = vec3(0.0f, step, 0.0f); break;
            case GLFW_KEY_K: delta = vec3(0.0f, -step, 0.0f); break;
            case GLFW_KEY_J: delta = vec3(-step, 0.0f, 0.0f); break;
            case GLFW_KEY_L: delta = vec3(step, 0.0f, 0.0f); break;
            }
            attemptMove(Scena[1], delta);
        }

        // Man-Pac (WASD)
        if (Scena.size() > PAC_MAN_IN_SCENE) 
        {
            vec3 delta(0.0f);
            switch (key) {
            case GLFW_KEY_W: delta = vec3(0.0f, step, 0.0f); break;
            case GLFW_KEY_S: delta = vec3(0.0f, -step, 0.0f); break;
            case GLFW_KEY_A: delta = vec3(-step, 0.0f, 0.0f); break;
            case GLFW_KEY_D: delta = vec3(step, 0.0f, 0.0f); break;
            }
            attemptMove(Scena[2], delta); 
        }

    }

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) 
    {
        if (!gameIsOver) 
        {
            if (manPacShotsFired < MAXIMUM_BULLET) 
            {
                Figura newP;
                newP.shader_programId = programId;
                newP.scale = vec3(15.0f, 15.0f, 1.0f);
                newP.isalive = true;
                newP.nTriangles = 20;

                INIT_CIRCLE(0.0f, 0.0f, 0.60f, 0.60f, &newP, vec4(0.0, 1.0, 1.0, 1.0), vec4(0.0, 0.6, 0.8, 1.0)); // colore ciano
                INIT_VAO(&newP);

                offset = Scena[2].scale.x * 0.5f + newP.scale.x * 0.5f;
                newP.position = Scena[2].position + vec3(offset, 0.0f, 0.0f);

                newP.Model = mat4(1.0f);
                newP.Model = translate(newP.Model, newP.position);
                newP.Model = scale(newP.Model, newP.scale);
                updateBB(&newP);

                proiettili.push_back(newP);

                manPacShotsFired++;
            }
            
        }
    }

    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) 
    {
        showScore = !showScore;
        printf("punteggio: %d", showScore);
    }

}

void framebuffer_size_callback1(GLFWwindow* window, int w, int h) 
{
    if (h == 0) h = 1;
    w_update = (float)w;
    h_update = (float)h;
    glViewport(0, 0, w, h);
}
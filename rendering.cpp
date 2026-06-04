#pragma once
#include <glad/glad.h>
#include "ShaderMaker.h"
#include "strutture.h"
#include "init_geometrie.h"
#include "inizializzazioni.h"
#include "Utilities.h"
#include "Gui.h"
#include <ImGui\imgui.h>
#include <ImGui\imgui_impl_glfw.h>
#include <ImGui\imgui_impl_opengl3.h>
#include "lib.h"

#define PI 3.14159265358979323
#define WALL_INDEX 3

extern int TOT_WALLS;
extern int POWERUPS_INDEX;

extern GLFWwindow* window;
extern int  width, height;
extern ImVec4 clear_color;
extern mat4 Projection;
extern unsigned int programId;

extern vector<Figura> Scena;
extern vector<Figura> proiettili;

extern double deltaTime;
extern int  powerUpCounter;
extern int  totalPowerUps;
extern bool showScore;

extern bool gameIsOver;
extern bool pacmanWon;

// CVD — impostati una volta sola all'avvio dal risultato MLP
extern int   cvdType;      // 0=Protan  1=Deutan  2=Tritan
extern float cvdSeverity;  // 0.0 (normale) ... 1.0 (dicromatismo pieno)


void disegnaFormaConBB(Figura& forma)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBindVertexArray(forma.VAO);
    glDrawArrays(forma.render, 0, forma.nv - 4);
}

// Passa al fragment shader i due uniform che controllano la correzione
static void setCVDUniforms(unsigned int shaderId)
{
    GLint locType = glGetUniformLocation(shaderId, "cvdType");
    if (locType != -1)
        glUniform1i(locType, cvdType);

    GLint locSev = glGetUniformLocation(shaderId, "cvdSeverity");
    if (locSev != -1)
        glUniform1f(locSev, cvdSeverity);
}


void renderizza(float currentTime)
{
    glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Sfondo (usa shader BK separato, non ha bisogno di CVD)
    Scena[0].Model = mat4(1.0);
    Scena[0].Model = scale(Scena[0].Model, vec3((float)(width), (float)(height), 1.0));
    updateBB(&Scena[0]);
    usaShaderConUniforms(Scena[0].shader_programId, Projection, Scena[0].Model, currentTime, vec2(width, height));
    disegnaFormaConBB(Scena[0]);

    if (!gameIsOver)
    {
        for (int i = 1; i < (int)Scena.size(); i++)
        {
            if (!Scena[i].isalive) continue;
            Figura& f = Scena[i];

            f.Model = mat4(1.0f);
            f.Model = translate(f.Model, f.position);
            f.Model = scale(f.Model, f.scale);
            updateBB(&f);

            glUseProgram(f.shader_programId);
            setCVDUniforms(f.shader_programId);
            usaShaderConUniforms(f.shader_programId, Projection, f.Model, currentTime, vec2(width, height));
            disegnaFormaConBB(f);
        }

        for (int i = POWERUPS_INDEX; i < (int)Scena.size(); i++)
        {
            Figura& powerUp = Scena[i];
            if (powerUp.isalive && checkCollision(Scena[1], powerUp))
            {
                powerUp.isalive = false;
                powerUpCounter++;
            }
            if (powerUpCounter >= totalPowerUps)
            {
                gameIsOver = true;
                pacmanWon = true;
            }
        }

        aggiornaProiettili((float)deltaTime);

        for (auto& p : proiettili)
        {
            if (!p.isalive) continue;

            if (checkCollision(p, Scena[1]))
            {
                p.isalive = false;
                gameIsOver = true;
                pacmanWon = false;
                return;
            }

            for (int i = WALL_INDEX; i < WALL_INDEX + TOT_WALLS && i < (int)Scena.size(); i++)
            {
                if (checkCollision(p, Scena[i]))
                {
                    p.isalive = false;
                    break;
                }
            }

            if (p.isalive)
            {
                glUseProgram(p.shader_programId);
                setCVDUniforms(p.shader_programId);
                usaShaderConUniforms(p.shader_programId, Projection, p.Model, currentTime, vec2(width, height));
                disegnaFormaConBB(p);
            }
        }
    }
    else
    {
        for (int i = 1; i < (int)Scena.size(); i++)
        {
            if (!Scena[i].isalive) continue;
            Figura& f = Scena[i];

            glUseProgram(f.shader_programId);
            setCVDUniforms(f.shader_programId);
            usaShaderConUniforms(f.shader_programId, Projection, f.Model, currentTime, vec2(width, height));
            disegnaFormaConBB(f);
        }

        for (auto& p : proiettili)
        {
            if (p.isalive)
            {
                glUseProgram(p.shader_programId);
                setCVDUniforms(p.shader_programId);
                usaShaderConUniforms(p.shader_programId, Projection, p.Model, currentTime, vec2(width, height));
                disegnaFormaConBB(p);
            }
        }
    }
}
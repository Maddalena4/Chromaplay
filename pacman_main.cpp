#include "lib.h"
#include "ShaderMaker.h"
#include "strutture.h"
#include "inizializzazioni.h"
#include "init_geometrie.h"
#include "gestione_interazioni.h"
#include "Init_Finestra_Eventi.h"
#include "init_GLFW_OpenGL.h"
#include <ImGui\imgui.h>
#include <ImGui\imgui_impl_glfw.h>
#include <ImGui\imgui_impl_opengl3.h>
#include "Scena.h"
#include "rendering.h"
#include "Gui.h"
#include "pacman_game.h"   
#include <iostream>

#define PI 3.14159265358979323
#define TOTAL_POWERUPS 5

int TOT_WALLS = 0;
int POWERUPS_INDEX = 0;

using namespace std;

unsigned int programId, programId_BK;
int   height, width;
GLFWwindow* window;
ImVec4 clear_color = ImVec4(0.45f, 0.0f, 0.0f, 1.00f);
mat4  Projection;
GLuint IdProj, IdModel;

float w_update = 0.0f;
float h_update = 0.0f;

vector<Figura> Scena;
vector<Figura> proiettili;
Figura pacman;
Figura manpac;

double deltaTime = 0.0;
double lastFrame = 0.0;
double mousex = 0.0;
double mousey = 0.0;
int    powerUpCounter = 0;
int    totalPowerUps = TOTAL_POWERUPS;
int    manPacShotsFired = 0;
bool   showScore = false;
bool   gameIsOver = false;
bool   pacmanWon = false;

int   cvdType = 0;     
float cvdSeverity = 0.0f; 



void runPacMan(int cvdTypeIn, int cvdPercentIn)
{
    cvdType = cvdTypeIn;
    cvdSeverity = (cvdPercentIn < 5) ? 0.0f
        : static_cast<float>(cvdPercentIn) / 100.0f;

    const char* cvdName = "Normale";
    if (cvdType == 0) cvdName = "Protan";
    else if (cvdType == 1) cvdName = "Deutan";
    else if (cvdType == 2) cvdName = "Tritan";

    std::cout << "\n===== AVVIO PACMAN =====" << std::endl;
    std::cout << "Tipo anomalia: " << cvdName
        << " (" << cvdType << ")" << std::endl;
    std::cout << "Percentuale: " << cvdPercentIn << "%" << std::endl;
    std::cout << "Severity shader: " << cvdSeverity << std::endl;
    std::cout << "========================\n" << std::endl;

    gameIsOver = false;
    pacmanWon = false;
    powerUpCounter = 0;
    manPacShotsFired = 0;
    showScore = false;
    Scena.clear();
    proiettili.clear();

    init_gl_GLFW();
    inizializza_window_context_events();

    Initialize_IMGUI(window);

    INIT_SHADER();
    costruisci_Scena(&Scena);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Projection = ortho(0.0f, float(width), 0.0f, float(height));
    IdProj = glGetUniformLocation(programId, "Projection");
    IdModel = glGetUniformLocation(programId, "Model");

    lastFrame = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        float currentTime = (float)glfwGetTime();
        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;

        renderizza(currentTime);

        my_interface();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    close_GUI();
    glfwDestroyWindow(window);
    window = nullptr;

    glDeleteProgram(programId);

    for (int i = 0; i < (int)Scena.size(); i++)
    {
        glDeleteBuffers(1, &Scena[i].VBO_vertices);
        glDeleteBuffers(1, &Scena[i].VBO_colors);
        glDeleteVertexArrays(1, &Scena[i].VAO);
    }
    for (auto& p : proiettili)
    {
        glDeleteBuffers(1, &p.VBO_vertices);
        glDeleteBuffers(1, &p.VBO_colors);
        glDeleteVertexArrays(1, &p.VAO);
    }

    glfwTerminate();
}
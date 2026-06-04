#pragma once
#include <ImGui\imgui.h>
#include <ImGui\imgui_impl_glfw.h>
#include <ImGui\imgui_impl_opengl3.h>
#include "lib.h"
#include <math.h>

extern int width, height;
extern unsigned int programId, programId_BK;
extern ImVec4 clear_color;

extern double mousex, mousey;

extern int  powerUpCounter;
extern int  manPacShotsFired;
extern bool showScore;
extern bool gameIsOver;
extern bool pacmanWon;

void Initialize_IMGUI(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.FontGlobalScale = 2.0f;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
}

void my_interface()
{
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    // Pannello punteggio (F1 per mostrare/nascondere)
    if (showScore && !gameIsOver)
    {
        ImGui::SetNextWindowPos(ImVec2(10, 10));
        ImGui::Begin("Punteggio", &showScore, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Potenziamenti raccolti: %d/5", powerUpCounter);
        ImGui::Text("Proiettili sparati: %d/5", manPacShotsFired);
        ImGui::End();
    }

    // Schermata fine partita
    if (gameIsOver)
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(450, 150));

        ImGui::Begin("Fine Partita", NULL,
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoCollapse);

        ImGui::SetWindowFontScale(1.5f);

        const char* message = pacmanWon ? "Pac-Man ha vinto!" : "Man-Pac ha vinto!";

        ImVec2 textSize = ImGui::CalcTextSize(message);
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImGui::SetCursorPos(ImVec2(
            (windowSize.x - textSize.x) * 0.5f,
            (windowSize.y - textSize.y) * 0.5f
        ));
        ImGui::Text("%s", message);

        ImGui::End();
    }

    ImGui::Render();
}

void close_GUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
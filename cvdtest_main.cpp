#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <chrono>

#include "AppState.h"
#include "ImageLoader.h"
#include "UI.h"
#include "launcher.h"      
#include "pacman_game.h"   

bool g_launchPacman = false;
int  g_launchCvdType = 0;
int  g_launchCvdPct = 0;

/**
 * Punto di ingresso dell'applicazione.
 *
 * Inizializza tutte le librerie necessarie (GLFW, GLAD e ImGui),
 * carica il modello di classificazione se disponibile,
 * esegue il ciclo principale dell'interfaccia grafica per il test CVD
 * e, al termine, avvia il videogioco Pac-Man adattato al profilo
 * cromatico rilevato dall'utente.
 *
 * Flusso principale:
 * 1. Inizializzazione finestra OpenGL.
 * 2. Inizializzazione ImGui.
 * 3. Creazione dello stato applicativo.
 * 4. Caricamento del modello MLP salvato.
 * 5. Esecuzione del loop grafico.
 * 6. Rendering delle schermate dell'applicazione.
 * 7. Rilascio delle risorse.
 * 8. Eventuale avvio di Pac-Man.
 *
 * RETURN:
 * - 0 se il programma termina correttamente.
 */
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        1100, 720, "CVD Test — Rilevamento Daltonismo", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.WindowBorderSize = 0.0f;
    style.WindowPadding = { 0, 0 };
    style.FrameRounding = 6.0f;
    style.ItemSpacing = { 8, 6 };

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = { 0.043f, 0.043f, 0.086f, 1.0f };
    colors[ImGuiCol_ChildBg] = { 0.055f, 0.055f, 0.110f, 1.0f };
    colors[ImGuiCol_Text] = { 0.88f,  0.88f,  0.94f,  1.0f };
    colors[ImGuiCol_Border] = { 0.12f,  0.12f,  0.24f,  1.0f };

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    AppState    state;
    ImageLoader loader;
    loader.setImageDir("images");

    if (state.mlp.load("model.bin"))
        state.mlpReady = true;

    auto lastTime = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(window) && !g_launchPacman)
    {
        glfwPollEvents();

        auto now = std::chrono::high_resolution_clock::now();
        double dt = std::chrono::duration<double>(now - lastTime).count();
        lastTime = now;
        if (dt > 0.1) dt = 0.1;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        switch (state.screen) {
        case AppScreen::WELCOME:  UI::drawWelcome(state);        break;
        case AppScreen::TRAINING: UI::drawTraining(state);       break;
        case AppScreen::TESTING:  UI::drawTesting(state, dt);    break;
        case AppScreen::RESULTS:  UI::drawResults(state, loader); break;
        }

        ImGui::Render();

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        glViewport(0, 0, w, h);
        glClearColor(0.043f, 0.043f, 0.086f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    loader.release();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();   

    if (g_launchPacman)
    {
        runPacMan(g_launchCvdType, g_launchCvdPct);
    }

    return 0;
}
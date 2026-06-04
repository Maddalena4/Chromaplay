#pragma once

/**
 * @brief Inizializza il contesto ImGui e lo collega a GLFW e OpenGL.
 *
 * Imposta il tema scuro, la scala del font e i binding con le librerie grafiche.
 * Deve essere chiamata una sola volta dopo la creazione della finestra OpenGL.
 *
 * @param window Puntatore alla finestra GLFW da associare.
 * @return void
 */
void Initialize_IMGUI(GLFWwindow* window);

/**
 * @brief Gestisce e disegna tutti gli elementi dell'interfaccia grafica.
 *
 * Crea e aggiorna le finestre ImGui che mostrano:
 * - il punteggio e i power-up raccolti;
 * - il messaggio di fine partita.
 *
 * Deve essere chiamata a ogni frame, prima del rendering finale.
 *
 * @return void
 */
void my_interface(void);

/**
 * @brief Chiude ImGui e libera tutte le risorse allocate.
 *
 * Arresta i backend di OpenGL e GLFW e distrugge il contesto ImGui.
 *
 * @return void
 */
void close_GUI(void);
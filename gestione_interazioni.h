#pragma once
#include "lib.h"
#include "Utilities.h"

/**
 * @brief Callback per la gestione della posizione del cursore.
 *
 * Aggiorna le coordinate globali del mouse (`mousex`, `mousey`)
 * in base alla posizione corrente del cursore nella finestra.
 *
 * @param window Puntatore alla finestra GLFW.
 * @param xpos Coordinata X del cursore.
 * @param ypos Coordinata Y del cursore.
 * @return void
 */
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

/**
  * @brief Tenta di muovere un giocatore applicando un vettore di spostamento.
  *
  * Calcola la nuova posizione potenziale, aggiorna la bounding box e verifica collisioni
  * con muri e altri giocatori.
  * Se viene rilevata una collisione, il movimento viene annullato.
  *
  * @param player Riferimento al giocatore da muovere.
  * @param moveDelta Vettore di spostamento proposto.
  * @return void
  */
void attemptMove(Figura& player, vec3 moveDelta);

/**
 * @brief Callback per la gestione della tastiera.
 *
 * Gestisce i comandi di movimento dei due giocatori (WASD e IJKL),
 * il fuoco dei proiettili, e la visualizzazione del punteggio con F1.
 * Inoltre, verifica lo stato di fine partita e impedisce ulteriori azioni quando `gameIsOver` è attivo.
 *
 * @param window Puntatore alla finestra GLFW.
 * @param key Codice del tasto premuto.
 * @param scancode Codice fisico della tastiera.
 * @param action Tipo di evento (pressione, rilascio, ripetizione).
 * @param mods Modificatori (Shift, Ctrl, Alt...).
 * @return void
 */
 void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

 /**
 * @brief Callback per l’aggiornamento della viewport durante il ridimensionamento della finestra.
 *
 * Aggiorna i parametri globali `w_update` e `h_update` e ridefinisce
 * la viewport OpenGL in modo che l’immagine venga scalata correttamente.
 *
 * @param window Puntatore alla finestra GLFW.
 * @param width Larghezza della nuova finestra.
 * @param height Altezza della nuova finestra.
 * @return void
 */
 void framebuffer_size_callback1(GLFWwindow* window, int width, int height);

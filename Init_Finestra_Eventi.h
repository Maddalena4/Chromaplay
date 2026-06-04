#pragma once

/**
 * @brief Inizializza la finestra di gioco e imposta il contesto OpenGL.
 *
 * Crea una finestra GLFW a schermo ridotto rispetto alla risoluzione del monitor principale.
 * Inizializza GLAD per il caricamento delle funzioni OpenGL e registra le callback di input:
 * - @ref key_callback per la gestione della tastiera;
 * - @ref cursor_position_callback per il tracciamento del mouse;
 * - @ref framebuffer_size_callback1 per l’adattamento della viewport al resize.
 *
 * In caso di errore di inizializzazione, termina GLFW e restituisce -1.
 *
 * @return int 0 in caso di successo, -1 in caso di errore.
 */
int inizializza_window_context_events();

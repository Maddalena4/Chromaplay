#pragma once

/**
 * @file gl.h
 * @brief Dichiarazione delle funzioni di inizializzazione di OpenGL e GLFW.
 *
 * Questo header contiene la dichiarazione della funzione `init_gl_GLFW()`,
 * utilizzata per inizializzare la libreria GLFW e impostare i parametri
 * principali del contesto OpenGL.
 *
 * Viene utilizzata nella fase iniziale del programma per preparare l'ambiente
 * grafico prima della creazione della finestra e del caricamento delle geometrie.
 */

 /**
  * @brief Inizializza la libreria GLFW e imposta i parametri del contesto OpenGL.
  *
  * Questa funzione esegue l’inizializzazione di base di GLFW, necessaria per
  * creare e gestire finestre OpenGL. Inoltre, definisce alcune impostazioni del
  * contesto, come la versione di OpenGL da utilizzare e il profilo core.
  *
  * @return Restituisce `0` se l’inizializzazione è avvenuta correttamente,
  *         oppure `-1` in caso di errore nell'inizializzazione di GLFW.
  *
  * @note Deve essere chiamata prima della creazione della finestra con `glfwCreateWindow()`.
  */
int init_gl_GLFW();
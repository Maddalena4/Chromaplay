#pragma once

/**
 * @brief Inizializza e compila i programmi shader principali del gioco.
 *
 * Crea due programmi shader:
 * - uno per gli oggetti della scena (vertexshaderM.glsl / fragmentshaderM.glsl);
 * - uno per il background (vertexshader_BK.glsl / fragmentshader_BK.glsl).
 *
 * Gli ID dei programmi vengono salvati in variabili globali esterne (`programId`, `programId_BK`).
 *
 * @return void
 */
void INIT_SHADER(void);

/**
 * @brief Inizializza il Vertex Array Object (VAO) e i buffer di vertici e colori di una figura.
 *
 * Genera e configura i buffer OpenGL necessari per disegnare una figura:
 * - Crea un VAO e due VBO (uno per i vertici e uno per i colori);
 * - Associa i dati alla GPU;
 * - Imposta i layout degli attributi (posizione e colore).
 *
 * @param fig Puntatore alla figura da inizializzare.
 * @return void
 */
void INIT_VAO(Figura* fig);
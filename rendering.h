#pragma once
#include "strutture.h"

/**
 * @brief Disegna una singola figura sullo schermo.
 *
 * Imposta la modalità di riempimento, associa il VAO e disegna l’oggetto.
 *
 * @param forma Riferimento alla figura da disegnare.
 */
void disegnaFormaConBB(Figura& forma);

/**
 * @brief Funzione principale di rendering del gioco.
 *
 * Viene chiamata ogni frame per:
 * - Pulire lo schermo e disegnare il background.
 * - Disegnare tutte le figure attive nella scena.
 * - Gestire le collisioni (Pac-Man, potenziamenti, proiettili).
 * - Aggiornare e disegnare i proiettili.
 * - Gestire lo stato di fine gioco (vittoria/sconfitta).
 * - Mostrare l’interfaccia grafica del punteggio tramite ImGui.
 *
 * @param currentTime Tempo corrente (usato per uniform iTime negli shader).
 * @return void
 */
void renderizza(float currentTime);
 
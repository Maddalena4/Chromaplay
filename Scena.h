#pragma once
#include "strutture.h"

/**
 * @brief Funzione che costruisce e popola il vettore della scena di gioco.
 * * Questa funzione assembla tutti gli elementi fissi e dinamici della scena
 * nell'ordine corretto:
 * 1. Background (Scena[0])
 * 2. Personaggi (Scena[1] = Pacman, Scena[2] = ManPac)
 * 3. Muri (Scena[3] in poi)
 * 4. Power-Up (Successivi ai Muri)
 *
 * Utilizza funzioni statiche interne (creaBackGround, inizializzaPersonaggi,
 * creaMuri, creaPowerUps) per mantenere pulita la logica.
 *
 * @param Scena Puntatore al vettore di tipo Figura che rappresenta tutti gli oggetti della scena.
 */
void costruisci_Scena(vector<Figura>* Scena);
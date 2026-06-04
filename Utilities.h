#pragma once
#include "strutture.h"
#include "lib.h"

/**
 * @brief Attiva un programma shader e imposta le sue variabili uniform principali.
 *
 * Invia allo shader le matrici di proiezione e modello, oltre a variabili uniform
 * per la risoluzione e il tempo corrente.
 *
 * @param shader_programId ID del programma shader da usare.
 * @param projection Matrice di proiezione.
 * @param model Matrice modello.
 * @param currentTime Tempo corrente.
 * @param resolution Risoluzione corrente della finestra.
 */
void usaShaderConUniforms(unsigned int shader_programId, glm::mat4& projection, mat4& model, float currentTime, vec2 resolution);

/**
 * @brief Calcola il bounding box locale (oggetto) di una figura.
 *
 * Scorre tutti i vertici della figura per determinare i valori minimi e massimi
 * lungo gli assi X e Y, impostando i campi `min_BB_obj` e `max_BB_obj`.
 *
 * @param fig Puntatore alla figura di cui calcolare il bounding box locale.
 */
void findBB(Figura* fig);

/**
 * @brief Aggiorna il bounding box globale (mondo) di una figura.
 *
 * Applica la matrice di modellazione `Model` ai punti minimi e massimi locali
 * e aggiorna le coordinate del bounding box nel mondo.
 *
 * @param fig Puntatore alla figura di cui aggiornare il bounding box globale.
 */
void updateBB(Figura* fig);

/**
 * @brief Controlla la collisione tra due figure tramite AABB.
 *
 * Verifica se i bounding box globali (allineati agli assi) dei due oggetti si
 * sovrappongono sugli assi X e Y.
 *
 * @param obj1 Prima figura.
 * @param obj2 Seconda figura.
 * @return true se le figure collidono, false altrimenti.
 */
bool checkCollision(Figura obj1, Figura obj2);

/**
 * @brief Verifica se una posizione nello spazio di gioco è libera da collisioni.
 *
 * Crea temporaneamente una figura centrata nella posizione indicata e controlla
 * se collide con muri, Pac-Man o Man-Pac.
 *
 * @param pos Posizione da testare.
 * @param muri Vettore dei muri presenti nella scena.
 * @param pacman Figura del giocatore Pac-Man.
 * @param manpac Figura dell’avversario Man-Pac.
 * @param radius Raggio di esclusione per il controllo (default: 18.0f).
 * @return true se la posizione è libera, false altrimenti.
 */
bool isFree(vec3 pos, const std::vector<Figura>& muri, const Figura& pacman, const Figura& manpac, float radius = 18.0f);

/**
 * @brief Genera posizioni casuali per i potenziamenti (power-up).
 *
 * Crea n posizioni casuali all’interno dell’area di gioco evitando collisioni
 * e una zona centrale “vietata”. Usa la funzione isFree() per verificare
 * la validità delle posizioni.
 *
 * @param n Numero di posizioni da generare.
 * @param muri Riferimento ai muri.
 * @param pacman Figura del giocatore Pac-Man.
 * @param manpac Figura dell’avversario Man-Pac.
 * @param width Larghezza della finestra di gioco.
 * @param height Altezza della finestra di gioco.
 * @param radius Raggio del potenziamento (default: 18.0f).
 * @return Vettore di posizioni valide.
 */
vector<vec3> generatePowerUpPositions(int n, const std::vector<Figura>& muri, const Figura& pacman, const Figura& manpac, int width, int height, float radius = 18.0f);

/**
 * @brief Aggiorna la logica e la posizione di tutti i proiettili attivi.
 *
 * Muove i proiettili verso destra, li disattiva se escono dallo schermo,
 * aggiorna le matrici `Model` e i relativi bounding box, e rimuove quelli inattivi.
 *
 * @param deltaTime Tempo trascorso dall'ultimo frame.
 */
void aggiornaProiettili(float deltaTime);
#pragma once

#include "strutture.h"

/**
 * @brief Calcola un punto sulla curva di Hermite cubica.
 *
 * Dati due punti di controllo e due tangenti, restituisce il punto interpolato
 * corrispondente al parametro `u`.
 *
 * @param p0 Punto iniziale.
 * @param p1 Punto finale.
 * @param t0 Tangente nel punto iniziale.
 * @param t1 Tangente nel punto finale.
 * @param u Parametro dell’interpolazione (parametro compreso tra 0 e 1)
 * @return Il punto calcolato sulla curva.
 */
vec3 hermite(vec3 p0, vec3 p1, vec3 t0, vec3 t1, float u);

/**
 * @brief Inizializza una figura a forma di cerchio ellittico.
 *
 * Genera una geometria circolare centrata in `(cx, cy)` con raggi `raggiox` e `raggioy`.
 * I vertici vengono disposti a ventaglio e colorati con un gradiente tra `color_top` e `color_bottom`.
 *
 * @param cx Coordinata X del centro.
 * @param cy Coordinata Y del centro.
 * @param raggiox Raggio lungo l’asse X.
 * @param raggioy Raggio lungo l’asse Y.
 * @param fig Puntatore alla figura da inizializzare.
 * @param color_top Colore del centro del cerchio.
 * @param color_bottom Colore del bordo.
 * @return void
 */
void INIT_CIRCLE(float cx, float cy, float raggiox, float raggioy, Figura* fig, vec4 color_top, vec4 color_bottom);

/**
 * @brief Inizializza una figura di tipo piano (quadrato unitario).
 *
 * Crea un piano con vertici in coordinate normalizzate (0–1) e lo colora con due tonalità.
 * Calcola il bounding box e definisce la modalità di rendering.
 *
 * @param fig Puntatore alla figura piano da inizializzare.
 * @param color_top Colore per i vertici superiori.
 * @param color_bottom Colore per i vertici inferiori.
 * @return void
 */
void INIT_PIANO(Figura* fig, vec4 color_top, vec4 color_bottom);

/**
 * @brief Inizializza una figura a forma di muro (rettangolo centrato).
 *
 * Genera un quadrato centrato nell’origine con lati di lunghezza 1,
 * utile per rappresentare i muri del labirinto.
 * Colora la metà superiore e inferiore con tonalità differenti.
 *
 * @param muro Puntatore alla figura muro da inizializzare.
 * @param color_top Colore della parte superiore.
 * @param color_bottom Colore della parte inferiore.
 * @return void
 */
void INIT_WALL(Figura* muro, vec4 color_top, vec4 color_bottom);

/**
 * @brief Inizializza una figura seguendo una forma chiusa basata su curve di Hermite.
 *
 * Crea una figura complessa interpolando un insieme di punti e tangenti
 * mediante curve di Hermite cubiche.
 * I vertici vengono organizzati a ventaglio (GL_TRIANGLE_FAN) e colorati
 * con una sfumatura verticale.
 *
 * @param fig Puntatore alla figura da inizializzare.
 * @param punti Vettore dei punti di controllo.
 * @param tangenti Vettore delle tangenti corrispondenti.
 * @param color_top Colore del centro.
 * @param color_bottom Colore del bordo.
 * @return void
 */
void INIT_HERMITE_CURVE_SHAPE(Figura* fig, std::vector<vec3> punti, std::vector<vec3> tangenti, vec4 color_top, vec4 color_bottom);
#pragma once
#include <glad/glad.h>
#include "ShaderMaker.h"
#include "strutture.h"
#include "init_geometrie.h"
#include "inizializzazioni.h"
#include "Utilities.h"

#define N_TRIANGLES_POWERUPS 40
#define N_TRIANGLES_PACMAN 60

using namespace std;

extern int width, height;
extern unsigned int programId, programId_BK;
extern Figura pacman;
extern Figura manpac;

/**
 * @brief Crea e inizializza l'oggetto Figura per lo sfondo (Background).
 *
 * @return Figura L'oggetto Background pronto per il rendering.
 */
static Figura creaBackGround()
{

    Figura bkg;
    bkg.nTriangles = 2;
    bkg.render = GL_TRIANGLE_FAN;
    bkg.shader_programId = programId_BK;
    INIT_PIANO(&bkg, vec4(0.0, 0.0, 0.1, 1.0), vec4(0.05, 0.05, 0.2, 1.0));
    INIT_VAO(&bkg);
    bkg.Model = mat4(1.0);
    bkg.Model = scale(bkg.Model, vec3((float)(width), (float)(height), 1.0));
    return bkg;
}

/**
 * @brief Inizializza le figure globali Pac-Man e Man-Pac.
 *
 * Questa funzione popola le struct 'pacman' e 'manpac' con geometria,
 * scala e posizione iniziale.
 */
static void inizializzaPersonaggi()
{

    vector<vec3> punti = { {0.5,0.0,0}, {0.0,0.5,0}, {-0.5,0.0,0}, {0.0,-0.5,0} };
    vector<vec3> tang = { {0.0,0.4,0}, {-0.4,0.0,0}, {0.0,-0.4,0}, {0.4,0.0,0} };

    // Pacman
    pacman.render = GL_TRIANGLE_FAN;
    pacman.isalive = true;
    pacman.nTriangles = N_TRIANGLES_PACMAN;
    pacman.shader_programId = programId;
    INIT_HERMITE_CURVE_SHAPE(&pacman, punti, tang, vec4(1.0, 1.0, 0.0, 1.0), vec4(1.0, 1.0, 0.0, 1.0));

    pacman.position = vec3(width * 0.85f, height * 0.2f, 0.0f);
    pacman.scale = vec3(45.0f, 45.0f, 1.0f);
    INIT_VAO(&pacman);
    pacman.Model = translate(mat4(1.0f), pacman.position);
    pacman.Model = scale(pacman.Model, pacman.scale);
    updateBB(&pacman);

    // ManPac
    manpac.render = GL_TRIANGLE_FAN;
    manpac.isalive = true;
    manpac.nTriangles = N_TRIANGLES_PACMAN;
    manpac.shader_programId = programId;
    INIT_HERMITE_CURVE_SHAPE(&manpac, punti, tang, vec4(0.0, 1.0, 1.0, 1.0), vec4(0.0, 1.0, 1.0, 1.0));

    manpac.position = vec3(width * 0.15f, height * 0.8f, 0.0f);
    manpac.scale = vec3(45.0f, 45.0f, 1.0f);
    INIT_VAO(&manpac);
    manpac.Model = translate(mat4(1.0f), manpac.position);
    manpac.Model = scale(manpac.Model, manpac.scale);
    updateBB(&manpac);
}

/**
 * @brief Crea tutti gli oggetti muro del labirinto.
 *
 * @return vector<Figura> Un vettore contenente tutti gli oggetti muro.
 */
static vector<Figura> creaMuri()
{
    vector<Figura> muriVec;
    float W = width;
    float H = height;

    // --- 1. MURI ESTERNI (Pallini pi? piccoli e numerosi) ---
    float dotRadius = 10.0f; // Ridotto da 16 a 10
    int numDotsX = 40;      // Aumentato da 18 a 40 per renderli vicini
    int numDotsY = 25;      // Aumentato da 12 a 25

    auto addDot = [&](vec3 pos, vec4 col) {
        Figura dot;
        dot.isalive = true;
        dot.nTriangles = 15; // Meno triangoli perch? sono piccoli, risparmiamo memoria
        dot.render = GL_TRIANGLE_FAN;
        dot.shader_programId = programId;
        dot.position = pos;
        dot.scale = vec3(dotRadius * 2.0f);
        INIT_CIRCLE(0.0f, 0.0f, 0.5f, 0.5f, &dot, col, col);
        INIT_VAO(&dot);
        dot.Model = translate(mat4(1.0f), dot.position);
        dot.Model = scale(dot.Model, dot.scale);
        updateBB(&dot); // Fondamentale: aggiorna la hitbox del singolo pallino
        muriVec.push_back(dot);
        };

    vec4 colors[] = {
        vec4(1.0, 0.2, 0.2, 1.0), vec4(0.2, 1.0, 0.2, 1.0),
        vec4(0.2, 0.5, 1.0, 1.0), vec4(1.0, 1.0, 0.2, 1.0),
        vec4(1.0, 0.6, 0.0, 1.0), vec4(0.8, 0.2, 0.8, 1.0)
    };

    // Bordo Superiore e Inferiore
    for (int i = 0; i < numDotsX; i++) {
        float x = 15.0f + i * ((W - 30.0f) / (numDotsX - 1));
        addDot({ x, 15.0f, 0 }, colors[i % 6]);
        addDot({ x, H - 15.0f, 0 }, colors[(i + 3) % 6]);
    }
    // Bordo Sinistro e Destro
    for (int i = 1; i < numDotsY - 1; i++) {
        float y = 15.0f + i * ((H - 30.0f) / (numDotsY - 1));
        addDot({ 15.0f, y, 0 }, colors[(i + 1) % 6]);
        addDot({ W - 15.0f, y, 0 }, colors[(i + 4) % 6]);
    }

    // --- 2. MURI INTERNI (Capsule arrotondate) ---
    vector<vec3> posizioni;
    vector<vec3> walls_scale;

    auto addInner = [&](vec3 pos, vec3 scale) {
        posizioni.push_back(pos);
        walls_scale.push_back(scale);
        };

    // Manteniamo le esatte posizioni e scale originali per il labirinto
    addInner({ W / 2.0f, H / 2.0f, 0 }, { 200.0f, 20.0f, 1 });
    addInner({ W / 2.0f, H / 2.0f + 60.0f, 0 }, { 200.0f, 20.0f, 1 });
    addInner({ W / 2.0f - 100.0f, H / 2.0f + 30.0f, 0 }, { 20.0f, 60.0f, 1 });
    addInner({ W / 2.0f + 100.0f, H / 2.0f + 30.0f, 0 }, { 20.0f, 60.0f, 1 });
    addInner({ W / 4.0f, H * 0.8f, 0 }, { 180.0f, 20.0f, 1 });
    addInner({ W * 3.0f / 4.0f, H * 0.8f, 0 }, { 180.0f, 20.0f, 1 });
    addInner({ W / 4.0f, H * 0.65f, 0 }, { 20.0f, 120.0f, 1 });
    addInner({ W * 3.0f / 4.0f, H * 0.65f, 0 }, { 20.0f, 120.0f, 1 });
    addInner({ W / 4.0f, H * 0.25f, 0 }, { 180.0f, 20.0f, 1 });
    addInner({ W * 3.0f / 4.0f, H * 0.25f, 0 }, { 180.0f, 20.0f, 1 });
    addInner({ W / 4.0f, H * 0.35f, 0 }, { 20.0f, 100.0f, 1 });
    addInner({ W * 3.0f / 4.0f, H * 0.35f, 0 }, { 20.0f, 100.0f, 1 });
    addInner({ W / 2.0f, H * 0.75f, 0 }, { 250.0f, 20.0f, 1 });
    addInner({ W / 2.0f, H * 0.25f, 0 }, { 250.0f, 20.0f, 1 });
    addInner({ W / 2.0f - 150.0f, H * 0.5f, 0 }, { 20.0f, 200.0f, 1 });
    addInner({ W / 2.0f + 150.0f, H * 0.5f, 0 }, { 20.0f, 200.0f, 1 });
    addInner({ width * 0.25f, height * 0.9f, 0 }, { 150.0f, 20.0f, 1 });
    addInner({ width * 0.75f, height * 0.15f, 0 }, { 150.0f, 20.0f, 1 });

    for (size_t i = 0; i < posizioni.size(); i++) {
        Figura muro;
        muro.isalive = true;
        muro.nTriangles = 30;
        muro.render = GL_TRIANGLE_FAN;
        muro.shader_programId = programId;
        muro.position = posizioni[i];
        muro.scale = walls_scale[i];

        // Alterniamo i colori per rompere la monotonia visiva
        vec4 cTop = (i % 2 == 0) ? vec4(0.2, 0.8, 0.8, 1.0) : vec4(0.8, 0.4, 0.0, 1.0);
        vec4 cBot = (i % 2 == 0) ? vec4(0.1, 0.5, 0.5, 1.0) : vec4(0.5, 0.2, 0.0, 1.0);

        // Usando INIT_CIRCLE associato a scale non uniformi, creiamo ellissi/capsule!
        INIT_CIRCLE(0.0f, 0.0f, 0.5f, 0.5f, &muro, cTop, cBot);
        INIT_VAO(&muro);
        muro.Model = translate(mat4(1.0f), muro.position);
        muro.Model = scale(muro.Model, muro.scale);
        updateBB(&muro);
        muriVec.push_back(muro);
    }

    return muriVec;
}

/**
 * @brief Crea tutti gli oggetti Power-Up in posizioni libere.
 *
 * @param muriVec Il vettore dei muri per evitare collisioni di spawn.
 * @return vector<Figura> Un vettore contenente tutti gli oggetti Power-Up.
 */
static vector<Figura> creaPowerUps(const vector<Figura>& muriVec)
{
    vector<Figura> powerUps;
    auto pot_pos = generatePowerUpPositions(5, muriVec, pacman, manpac, width, height);

    for (int i = 0; i < pot_pos.size(); i++)
    {
        Figura pot;
        pot.isalive = true;
        pot.render = GL_TRIANGLE_FAN;
        pot.shader_programId = programId;
        pot.scale = vec3(22, 22, 1);
        pot.position = pot_pos[i];

        vector<vec3> punti_p;
        vector<vec3> tang_p;
        vec4 c1, c2;

        // Switch per creare 5 forme uniche
        // Switch per creare 5 forme uniche
        switch (i) {
        case 0: // Cuore (Rosso) - INVARIATO
            pot.nTriangles = 40;
            punti_p = { {0.0, 0.2, 0}, {0.3, 0.5, 0}, {0.5, 0.2, 0}, {0.0, -0.5, 0}, {-0.5, 0.2, 0}, {-0.3, 0.5, 0} };
            tang_p = { {0.2, 0.3, 0}, {0.1, 0.2, 0}, {0.0, -0.3, 0}, {-0.2, -0.2, 0}, {-0.1, 0.3, 0}, {0.1, 0.3, 0} };
            c1 = vec4(1.0, 0.0, 0.0, 1.0); c2 = vec4(0.8, 0.0, 0.0, 1.0);
            INIT_HERMITE_CURVE_SHAPE(&pot, punti_p, tang_p, c1, c2);
            break;

        case 1: // Fiore (Petali curvi con pallino giallo al centro)
            pot.nTriangles = 60;
            punti_p = {
                {0.0, 0.5, 0}, {0.15, 0.15, 0}, {0.5, 0.0, 0}, {0.15, -0.15, 0},
                {0.0, -0.5, 0}, {-0.15, -0.15, 0}, {-0.5, 0.0, 0}, {-0.15, 0.15, 0},
                {0.0, 0.5, 0} // <-- AGGIUNTO: Ripetizione del primo punto per chiudere il petalo
            };
            tang_p = {
                {1.0, 0.0, 0}, {0.2, -0.2, 0}, {0.0, -1.0, 0}, {-0.2, -0.2, 0},
                {-1.0, 0.0, 0}, {-0.2, 0.2, 0}, {0.0, 1.0, 0}, {0.2, 0.2, 0},
                {1.0, 0.0, 0} // <-- AGGIUNTO: Ripetizione della prima tangente
            };
            c1 = vec4(1.0, 1.0, 0.0, 1.0);
            c2 = vec4(1.0, 0.2, 0.8, 1.0);
            INIT_HERMITE_CURVE_SHAPE(&pot, punti_p, tang_p, c1, c2);
            break;

        case 2: // Sole (Giallo/Arancio) - INVARIATO
            pot.nTriangles = 30;
            c1 = vec4(1.0, 1.0, 0.0, 1.0); c2 = vec4(1.0, 0.5, 0.0, 1.0);
            INIT_CIRCLE(0.0f, 0.0f, 0.5f, 0.5f, &pot, c1, c2);
            break;

        case 3: // Farfalla (curve realistiche e morbide)
            pot.nTriangles = 80; // più dettaglio

            punti_p = {
                {0.0, 0.1, 0},     // centro alto
                {0.5, 0.45, 0},    // ala superiore dx (più piena)
                {0.15, -0.05, 0},  // rientro morbido
                {0.35, -0.45, 0},  // ala inferiore dx
                {0.0, -0.25, 0},   // centro basso
                {-0.35, -0.45, 0}, // ala inferiore sx
                {-0.15, -0.05, 0}, // rientro morbido
                {-0.5, 0.45, 0},   // ala superiore sx
                {0.0, 0.1, 0}      // chiusura
            };

            tang_p = {
                {0.8, 0.4, 0},     // salita morbida
                {0.3, -0.9, 0},    // curva ampia ala sup
                {0.2, -0.6, 0},    // raccordo dolce
                {-0.3, -0.6, 0},   // curva ala inf
                {-0.8, 0.0, 0},    // passaggio centrale
                {-0.3, 0.6, 0},    // simmetria sotto
                {0.2, 0.6, 0},     // raccordo dolce
                {0.3, 0.9, 0},     // curva ampia ala sup
                {0.8, 0.4, 0}      // chiusura fluida
            };

            c1 = vec4(0.0, 1.0, 1.0, 1.0);
            c2 = vec4(0.0, 0.4, 1.0, 1.0);

            INIT_HERMITE_CURVE_SHAPE(&pot, punti_p, tang_p, c1, c2);
            break;

        case 4: // Stella a 5 punte
            pot.nTriangles = 60;
            punti_p = {
                {0.0, 0.5, 0}, {0.12, 0.15, 0}, {0.48, 0.15, 0}, {0.19, -0.06, 0},
                {0.29, -0.4, 0}, {0.0, -0.2, 0}, {-0.29, -0.4, 0}, {-0.19, -0.06, 0},
                {-0.48, 0.15, 0}, {-0.12, 0.15, 0},
                {0.0, 0.5, 0} // <-- Chiusura
            };
            tang_p = {
                {0.1, 0.0, 0}, {0.1, -0.1, 0}, {0.0, -0.1, 0}, {-0.1, -0.1, 0},
                {-0.1, 0.0, 0}, {-0.1, 0.0, 0}, {-0.1, 0.1, 0}, {0.1, 0.1, 0},
                {0.0, 0.1, 0}, {0.1, 0.1, 0},
                {0.1, 0.0, 0} // <-- Chiusura
            };
            c1 = vec4(0.0, 1.0, 0.0, 1.0); c2 = vec4(0.0, 0.5, 0.0, 1.0);
            INIT_HERMITE_CURVE_SHAPE(&pot, punti_p, tang_p, c1, c2);
            break;
        }

        INIT_VAO(&pot);
        pot.Model = translate(mat4(1.0f), pot.position);
        pot.Model = scale(pot.Model, pot.scale);
        updateBB(&pot);

        powerUps.push_back(pot);
    }
    return powerUps;
}

void costruisci_Scena(vector<Figura>* Scena)
{
    Scena->push_back(creaBackGround());
    inizializzaPersonaggi();
    Scena->push_back(pacman);
    Scena->push_back(manpac);

    auto muri = creaMuri();
    for (auto& m : muri) Scena->push_back(m);

    auto powerUps = creaPowerUps(muri);
    for (auto& p : powerUps) Scena->push_back(p);

    // AGGIUNGI QUESTE 4 RIGHE ALLA FINE:
    extern int TOT_WALLS;
    extern int POWERUPS_INDEX;
    TOT_WALLS = muri.size();
    POWERUPS_INDEX = 3 + TOT_WALLS; // 1 Sfondo + 2 Player + N Muri
}
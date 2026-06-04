#pragma once

#include "lib.h"

/**
 * Identifica le differenti tipologie di figure
 * grafiche utilizzate nell'applicazione.
 */
enum Forma {
    BACKGROUND=0,
    FARFALLA = 1,
    CUORE=2
};

/**
 * @struct Figura
 *
 * Struttura che rappresenta un oggetto grafico renderizzato
 * mediante OpenGL.
 *
 * La struttura contiene:
 * - geometria (vertici e colori);
 * - buffer OpenGL (VAO e VBO);
 * - trasformazioni geometriche;
 * - informazioni sul bounding box;
 * - shader associato;
 * - stato logico dell'oggetto.
 *
 * Ogni istanza rappresenta un'entità renderizzabile
 * presente nella scena.
 */
typedef struct {
    GLuint VAO;
    GLuint  VBO_vertices; 
    GLuint  VBO_colors;

    int nTriangles;  

    vector<vec3> vertices; 
    vector<vec4> colors; 

    int nv;  
    int render; 
    mat4 Model; 
    vec3 position;  
    vec3 scale;  
    unsigned int shader_programId; 
    vec4 min_BB_obj;
    vec4 max_BB_obj;
    vec4 min_BB_w;
    vec4 max_BB_w;
    float timerFig;
    bool isalive;
} Figura;  


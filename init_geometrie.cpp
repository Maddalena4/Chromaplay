#pragma once
#include "strutture.h"
#include "Utilities.h"

#define PI 3.14159265358979323
#define STEP 20

using namespace std;

vec3 hermite(vec3 p0, vec3 p1, vec3 t0, vec3 t1, float u) 
{
    float h1 = 2 * u * u * u - 3 * u * u + 1;
    float h2 = -2 * u * u * u + 3 * u * u;
    float h3 = u * u * u - 2 * u * u + u;
    float h4 = u * u * u - u * u;
    return h1 * p0 + h2 * p1 + h3 * t0 + h4 * t1;
}

void INIT_HERMITE_CURVE_SHAPE(Figura* fig, vector<vec3> punti, vector<vec3> tangenti, vec4 color_top, vec4 color_bottom) 
{
    int n = punti.size();
    fig->vertices.clear();
    fig->colors.clear();

    fig->vertices.push_back(vec3(0.0, 0.0, 0.0)); 
    fig->colors.push_back(color_top);

    for (int i = 0; i < n; i++) 
    {
        vec3 p0 = punti[i];
        vec3 p1 = punti[(i + 1) % n];
        vec3 t0 = tangenti[i];
        vec3 t1 = tangenti[(i + 1) % n];
        for (int j = 0; j <= STEP; j++) 
        {
            float u = (float)j / 20.0f;
            vec3 p = hermite(p0, p1, t0, t1, u);
            fig->vertices.push_back(p);
            fig->colors.push_back(color_bottom);
        }
    }

    findBB(fig);
    fig->nv = fig->vertices.size();
    fig->render = GL_TRIANGLE_FAN;
}

void INIT_CIRCLE(float cx, float cy, float raggiox, float raggioy, Figura* fig, vec4 color_top, vec4 color_bottom) 
{

    int i;
    float stepA = (2 * PI) / fig->nTriangles;
    float t, xx, yy;


    fig->vertices.push_back(vec3(cx, cy, 0.0));

    fig->colors.push_back(color_top);

    for (i = 0; i <= fig->nTriangles; i++)
    {
        t = (float)i * stepA;
        xx = cx + raggiox * cos(t);
        yy = cy + raggioy * sin(t);
        fig->vertices.push_back(vec3(xx, yy, 0.0));
        fig->colors.push_back(color_bottom); 
    }

    findBB(fig);
    fig->vertices.push_back(vec3(fig->min_BB_obj.x, fig->min_BB_obj.y, 0.0));
    fig->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
    fig->vertices.push_back(vec3(fig->max_BB_obj.x, fig->min_BB_obj.y, 0.0));
    fig->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
    fig->vertices.push_back(vec3(fig->max_BB_obj.x, fig->max_BB_obj.y, 0.0));
    fig->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
    fig->vertices.push_back(vec3(fig->min_BB_obj.x, fig->max_BB_obj.y, 0.0));
    fig->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));


    fig->nv = fig->vertices.size();
    fig->render = GL_TRIANGLE_FAN;
}


void INIT_PIANO(Figura* fig, vec4 color_top, vec4 color_bottom)
{
    fig->vertices.clear();

    fig->vertices.push_back(vec3(0.0, 0.0, 0.0));
    fig->colors.push_back(color_top);
    fig->vertices.push_back(vec3(1.0,0.0, 0.0));
    fig->colors.push_back(color_bottom);
    fig->vertices.push_back(vec3(1.0, 1.0, 0.0));
    fig->colors.push_back(color_top);
    fig->vertices.push_back(vec3(0.0, 1.0, 0.0));
    fig->colors.push_back(color_bottom);

    findBB(fig);

    fig->vertices.push_back(vec3(fig->min_BB_obj.x, fig->min_BB_obj.y, 0.0));
    fig->colors.push_back(vec4(1.0, 1.0, 1.0, 1.0));
    fig->vertices.push_back(vec3(fig->max_BB_obj.x, fig->min_BB_obj.y, 0.0));
    fig->colors.push_back(vec4(1.0, 1.0, 1.0, 1.0));
    fig->vertices.push_back(vec3(fig->max_BB_obj.x, fig->max_BB_obj.y, 0.0));
    fig->colors.push_back(vec4(1.0, 1.0, 1.0, 1.0));
    fig->vertices.push_back(vec3(fig->min_BB_obj.x, fig->max_BB_obj.y, 0.0));
    fig->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
    fig->nv = fig->vertices.size();
    fig->render = GL_TRIANGLE_FAN;
    
    
 }

void INIT_WALL(Figura* muro, vec4 color_top, vec4 color_bottom) 
{
    muro->vertices.clear();

    muro->vertices.push_back(vec3(-0.5f, -0.5f, 0.0f));
    muro->vertices.push_back(vec3(0.5f, -0.5f, 0.0f));
    muro->vertices.push_back(vec3(0.5f, 0.5f, 0.0f));
    muro->vertices.push_back(vec3(-0.5f, 0.5f, 0.0f));

    muro->colors.push_back(color_top);
    muro->colors.push_back(color_top);
    muro->colors.push_back(color_bottom);
    muro->colors.push_back(color_bottom);

    findBB(muro);

    muro->vertices.push_back(vec3(muro->min_BB_obj.x, muro->min_BB_obj.y, 0.0));
    muro->colors.push_back(vec4(1.0, 1.0, 1.0, 1.0));
    muro->vertices.push_back(vec3(muro->max_BB_obj.x, muro->min_BB_obj.y, 0.0));
    muro->colors.push_back(vec4(1.0, 1.0, 1.0, 1.0));
    muro->vertices.push_back(vec3(muro->max_BB_obj.x, muro->max_BB_obj.y, 0.0));
    muro->colors.push_back(vec4(1.0, 1.0, 1.0, 1.0));
    muro->vertices.push_back(vec3(muro->min_BB_obj.x, muro->max_BB_obj.y, 0.0));
    muro->colors.push_back(vec4(1.0, 1.0, 1.0, 1.0));

    muro->nv = muro->vertices.size();
    muro->render = GL_TRIANGLE_FAN;
}



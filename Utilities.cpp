#pragma once
#include "lib.h"
#include "strutture.h"
#include <random>

#define BULLET_SPEED 300.0f
using namespace std;

extern int width, height;
extern GLuint IdProj, IdModel;
extern vector<Figura> proiettili;

void usaShaderConUniforms(unsigned int shader_programId, glm::mat4& projection, mat4& model, float currentTime, vec2 resolution) 
{
    
    glUseProgram(shader_programId);

    glUniformMatrix4fv(IdProj, 1, GL_FALSE, glm::value_ptr(projection));

    glUniformMatrix4fv(IdModel, 1, GL_FALSE, glm::value_ptr(model));

    if (resolution.x > 0.0f && resolution.y > 0.0f) 
    {
        glUniform2f(glGetUniformLocation(shader_programId, "iResolution"), resolution.x, resolution.y);
    }

    if (currentTime > 0.0f) 
    {
        glUniform1f(glGetUniformLocation(shader_programId, "iTime"), currentTime);
    }
}

void findBB(Figura* fig) 
{
    int n;
    n = fig->vertices.size();
    float minx = fig->vertices[0].x; // Primo elemento-> minimo iniziale
    float miny = fig->vertices[0].y; 

    float maxx = fig->vertices[0].x; //primo elemento-> sia il massimo iniziale
    float maxy = fig->vertices[0].y; 

    for (int i = 1; i < n; i++) 
    {
        if (fig->vertices[i].x < minx) 
        {
            minx = fig->vertices[i].x;
        }
        if (fig->vertices[i].x > maxx) 
        {
            maxx = fig->vertices[i].x;
        }

        if (fig->vertices[i].y < miny) 
        {
            miny = fig->vertices[i].y;
        }

        if (fig->vertices[i].y > maxy) 
        {
            maxy = fig->vertices[i].y;
        }
    }

    fig->min_BB_obj=vec4(minx,miny,0.0,1.0);
    fig->max_BB_obj = vec4(maxx, maxy, 0.0,1.0);

}

void updateBB(Figura* fig) 
{

    vec4 p1_w = fig->Model * fig->min_BB_obj;
    vec4 p2_w = fig->Model * fig->max_BB_obj;

    fig->min_BB_w.x = std::min(p1_w.x, p2_w.x);
    fig->min_BB_w.y = std::min(p1_w.y, p2_w.y);
    fig->min_BB_w.z = 0.0f;
    fig->min_BB_w.w = 1.0f;

    fig->max_BB_w.x = std::max(p1_w.x, p2_w.x);
    fig->max_BB_w.y = std::max(p1_w.y, p2_w.y);
    fig->max_BB_w.z = 0.0f;
    fig->max_BB_w.w = 1.0f;
}

bool checkCollision(Figura obj1, Figura obj2) 
{
    
    bool collisionX = obj1.max_BB_w.x > obj2.min_BB_w.x &&
        obj1.min_BB_w.x < obj2.max_BB_w.x;

    bool collisionY = obj1.max_BB_w.y > obj2.min_BB_w.y &&
        obj1.min_BB_w.y < obj2.max_BB_w.y;

    return collisionX && collisionY;
}

bool isFree(vec3 pos, const vector<Figura>& muri, const Figura& pacman, const Figura& manpac, float radius = 18.0f) 
{
    Figura temp;
    temp.isalive = true;
    temp.position = pos;
    temp.scale = vec3(radius * 2.0f); 

    temp.min_BB_obj = vec4(-0.5f, -0.5f, 0.0f, 1.0f);
    temp.max_BB_obj = vec4(0.5f, 0.5f, 0.0f, 1.0f);

    temp.Model = mat4(1.0f);
    temp.Model = translate(temp.Model, temp.position);
    temp.Model = scale(temp.Model, temp.scale);

    updateBB(&temp);

    if (checkCollision(temp, pacman)) return false;

    if (checkCollision(temp, manpac)) return false;

    for (auto& muro : muri) 
    {
        if (checkCollision(temp, muro)) return false;
    }

    return true;
}

vector<vec3> generatePowerUpPositions(int n, const vector<Figura>& muri, const Figura& pacman, const Figura& manpac, int width, int height, float radius = 18.0f) 
{ 
    vector<vec3> positions;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> disX(radius, width - radius);
    uniform_real_distribution<float> disY(radius, height - radius);

    float centerX = width / 2.0f;
    float centerY = height / 2.0f;

    float safeZoneWidth = width * 0.25f;   
    float safeZoneHeight = height * 0.25f; 

    while (positions.size() < n) 
    {
        vec3 pos = { disX(gen), disY(gen), 0 };

        bool insideCenter =
            fabs(pos.x - centerX) < safeZoneWidth / 2 &&
            fabs(pos.y - centerY) < safeZoneHeight / 2;

        if (!insideCenter && isFree(pos, muri, pacman, manpac, radius))
        {
            positions.push_back(pos);
        }
    }

    return positions;
}

void aggiornaProiettili(float deltaTime) 
{
    float vel_x_proiettile = BULLET_SPEED;


    for (auto& p : proiettili) 
    {
        if (p.isalive) 
        {

            p.position.x += vel_x_proiettile * deltaTime;

            if (p.position.x > width) 
            {
                p.isalive = false;
            }

            p.Model = mat4(1.0f);
            p.Model = translate(p.Model, vec3(p.position.x, p.position.y, 0.0));
            p.Model = scale(p.Model, p.scale);

            updateBB(&p);
        }
    }

    proiettili.erase(
        remove_if(
            proiettili.begin(), proiettili.end(),
            [](const Figura& p) {
                return !p.isalive; }
        ),
        proiettili.end());
}
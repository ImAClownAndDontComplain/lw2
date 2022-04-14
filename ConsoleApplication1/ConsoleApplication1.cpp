#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glm/glm.hpp"
#include <algorithm>
#include "glm/mat4x4.hpp"
#include <iostream>

using namespace glm;
float scalex = 0.001;
float scaley = 0.001;
float scaler = 0.005;
float scale = 0.01;

mat4 World; //translation
mat4 Worldr; //rotation
mat4 Worlds; //scaling

vec2 len(vec3 v1, vec3 v2, vec3 v3) {
    vec2 v;
    float l12 = sqrtf((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
    float l23 = sqrtf((v3.x - v2.x) * (v3.x - v2.x) + (v3.y - v2.y) * (v3.y - v2.y));
    float l13 = sqrtf((v1.x - v3.x) * (v1.x - v3.x) + (v1.y - v3.y) * (v1.y - v3.y));
    float m1 = max(l12, l23);
    v.x = max(m1, l13);
    float m2 = min(l12, l23);
    v.y = min(m2, l13);
    return v;
}

vec3 Triangle[3] = {
{ -0.1, -0.1, 0.0f },
{ 0.1, -0.1, 0.0f },
 { 0, 0.5, 0.0f },
};

/*vec4 mult(mat4 M, vec4 V) {
    vec4 res;
    res.x = M[0][0] * V.x + M[0][1] * V.y + M[0][2] * V.z + M[0][3] * V.w;
    res.y = M[1][0] * V.x + M[1][1] * V.y + M[1][2] * V.z + M[1][3] * V.w;
    res.z = M[2][0] * V.x + M[2][1] * V.y + M[2][2] * V.z + M[2][3] * V.w;
    res.w = M[3][0] * V.x + M[3][1] * V.y + M[3][2] * V.z + M[3][3] * V.w;
    return res;
}*/

void trans(vec3& V) {
    World[0][0] = 1.0f; World[0][1] = 0.0f; World[0][2] = 0.0f; World[0][3] = sinf(scalex);
    World[1][0] = 0.0f; World[1][1] = 1.0f; World[1][2] = 0.0f; World[1][3] = sinf(scaley);
    World[2][0] = 0.0f; World[2][1] = 0.0f; World[2][2] = 1.0f; World[2][3] = 0.0f;
    World[3][0] = 0.0f; World[3][1] = 0.0f; World[3][2] = 0.0f; World[3][3] = 1.0f;
    vec4 v1 = vec4(V, 1.0) * World;
    //std::cout << v1.x << "\n";
    V = vec3(v1);
}

void rot(vec3& V) {
    Worldr[0][0] = cosf(scaler); Worldr[0][1] = -sinf(scaler);Worldr[0][2] = 0.0f; Worldr[0][3] = 0.0;
    Worldr[1][0] = sinf(scaler); Worldr[1][1] = cosf(scaler); Worldr[1][2] = 0.0f; Worldr[1][3] = 0.0;
    Worldr[2][0] = 0.0f; Worldr[2][1] = 0.0f; Worldr[2][2] = 1.0f; Worldr[2][3] = 0.0f;
    Worldr[3][0] = 0.0f; Worldr[3][1] = 0.0f; Worldr[3][2] = 0.0f; Worldr[3][3] = 1.0f;
    vec4 v1 = vec4(V, 1.0) * Worldr;
    //std::cout << v1.x << "\n";
    V = vec3(v1);
}

void scal(vec3& V) {
    float sc = 1 - sinf(scale);
    Worlds[0][0] = sc; Worlds[0][1] = 0.0f; Worlds[0][2] = 0.0f; Worlds[0][3] = 0.0;
    Worlds[1][0] = 0.0f; Worlds[1][1] = sc; Worlds[1][2] = 0.0f; Worlds[1][3] = 0.0;
    Worlds[2][0] = 0.0f; Worlds[2][1] = 0.0f; Worlds[2][2] = sc; Worlds[2][3] = 0.0f;
    Worlds[3][0] = 0.0f; Worlds[3][1] = 0.0f; Worlds[3][2] = 0.0f; Worlds[3][3] = 1.0f;
    vec4 v1 = vec4(V, 1.0) * Worlds;
    //std::cout << scalex << "\n";
    V = vec3(v1);
}

GLuint VBO;

void RenderSceneGB() {
    vec2 v = len(Triangle[0], Triangle[1], Triangle[2]);
    if (v.y < 0.05) scale *= -1;
    if (v.x > 3) scale *= -1;

    if (Triangle[1].x > 1 || Triangle[0].x < -1) scalex *= -1, scale *= -1;
    if (Triangle[2].y > 1 || Triangle[0].y < -1) scaley *= -1, scale *= -1;

    for (int i = 0; i < 3; i++) {
        // trans(Triangle[i]);
        rot(Triangle[i]);
        // scal(Triangle[i]);
    }


    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Triangle), Triangle);

    glEnableVertexAttribArray(0);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);
    glutPostRedisplay();
    glutSwapBuffers();
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

    glutInitWindowSize(1024, 768);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("IDKWTD");

    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle), Triangle, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glutDisplayFunc(RenderSceneGB);
    glutMainLoop();
}
﻿#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glm/glm.hpp"
//#include "glm/gtc/matrix_transform.hpp"
#include "glm/mat4x4.hpp"
#include <iostream>

using namespace glm;
float scalex = 0.001;
float scaley = 0.001;
float scaler = 0.005;
mat4 World;                   //translation
mat4 Worldr;
vec3 Triangle[3] = {
{ -0.1, -0.1, 0.0f },
{ 0.1, -0.1, 0.0f },
 { 0, 0.5, 0.0f },
};

vec4 mult(mat4 M, vec4 V) {
    vec4 res;
    res.x = M[0][0] * V.x + M[0][1] * V.y + M[0][2] * V.z + M[0][3] * V.w;
    res.y = M[1][0] * V.x + M[1][1] * V.y + M[1][2] * V.z + M[1][3] * V.w;
    res.z = M[2][0] * V.x + M[2][1] * V.y + M[2][2] * V.z + M[2][3] * V.w;
    res.w = M[3][0] * V.x + M[3][1] * V.y + M[3][2] * V.z + M[3][3] * V.w;
    return res;
}

void trans(vec3 &V) {
    /*if (V.x > 1)scalex *= -1;
    if (V.y > 1)scaley *= -1;*/
    World[0][0] = 1.0f; World[0][1] = 0.0f; World[0][2] = 0.0f; World[0][3] = 10*sinf(scalex);
    World[1][0] = 0.0f; World[1][1] = 1.0f; World[1][2] = 0.0f; World[1][3] = 10 * sinf(scaley);
    World[2][0] = 0.0f; World[2][1] = 0.0f; World[2][2] = 1.0f; World[2][3] = 0.0f;
    World[3][0] = 0.0f; World[3][1] = 0.0f; World[3][2] = 0.0f; World[3][3] = 1.0f;
    vec4 v1 = mult(World, vec4(V, 0.1));
    std::cout << v1.x << "\n";
    V = vec3(v1);
}

void inc(float& n) {
    if (n < 0) n -= 0.0001;
    if (n < -3.14) n *=-1;
    if (n >= 0) n += 0.0001;
    if (n > 3.14)n = -0.0001;
}

GLuint VBO;
/*struct Matrix4f {
    float m[4][4];
};*/
void RenderSceneGB() { 
    for (int i = 0; i < 3; i++) {
        if (i == 0 && Triangle[i].x < -1)scalex *= -1;
        if (i == 0 && Triangle[i].x > 1)scalex *= -1;
        if (i == 0 && Triangle[i].y > 1)scaley *= -1;
        if (i == 0 && Triangle[i].y < -1)scaley *= -1;
        trans(Triangle[i]);
        
    }
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle), Triangle, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
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

    glutDisplayFunc(RenderSceneGB);
    glutMainLoop();
}
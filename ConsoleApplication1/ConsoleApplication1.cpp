#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glm/glm.hpp"
//#include "glm/gtc/matrix_transform.hpp"
#include "glm/mat4x4.hpp"
#include <iostream>

using namespace glm;
float scalex = 0.0001;
float scaley = 0.0001;
float scaler = 0.005;
float scale = 3.14;

mat4 World{ //translation
    World[0][0] = 1.0f, World[0][1] = 0.0f, World[0][2] = 0.0f, World[0][3] = 10*sinf(scalex),
    World[1][0] = 0.0f, World[1][1] = 1.0f, World[1][2] = 0.0f, World[1][3] = 10*sinf(scaley),
    World[2][0] = 0.0f, World[2][1] = 0.0f, World[2][2] = 1.0f, World[2][3] = 0.0f,
    World[3][0] = 0.0f, World[3][1] = 0.0f, World[3][2] = 0.0f, World[3][3] = 1.0f
};

mat4 Worldr{ //rotation
    Worldr[0][0] = cosf(scaler), Worldr[0][1] = -sinf(scaler), Worldr[0][2] = 0.0f, Worldr[0][3] = 0.0,
    Worldr[1][0] = sinf(scaler), Worldr[1][1] = cosf(scaler), Worldr[1][2] = 0.0f, Worldr[1][3] = 0.0,
    Worldr[2][0] = 0.0f, Worldr[2][1] = 0.0f, Worldr[2][2] = 1.0f, Worldr[2][3] = 0.0f,
    Worldr[3][0] = 0.0f, Worldr[3][1] = 0.0f, Worldr[3][2] = 0.0f, Worldr[3][3] = 1.0f
};

mat4 Worlds{ //scaling
    Worlds[0][0] = 1+sinf(scale), Worlds[0][1] = 0.0f, Worlds[0][2] = 0.0f, Worlds[0][3] = 0.0,
    Worlds[1][0] = 0.0f, Worlds[1][1] = 1+sinf(scale), Worlds[1][2] = 0.0f, Worlds[1][3] = 0.0,
    Worlds[2][0] = 0.0f, Worlds[2][1] = 0.0f, Worlds[2][2] = 1+sinf(scale), Worlds[2][3] = 0.0f,
    Worlds[3][0] = 0.0f, Worlds[3][1] = 0.0f, Worlds[3][2] = 0.0f, Worlds[3][3] = 1.0f
};

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
    World[0][0] = 1.0f; World[0][1] = 0.0f; World[0][2] = 0.0f; World[0][3] = 10 * sinf(scalex);
    World[1][0] = 0.0f; World[1][1] = 1.0f; World[1][2] = 0.0f; World[1][3] = 10 * sinf(scaley);
    World[2][0] = 0.0f; World[2][1] = 0.0f; World[2][2] = 1.0f; World[2][3] = 0.0f;
    World[3][0] = 0.0f; World[3][1] = 0.0f; World[3][2] = 0.0f; World[3][3] = 1.0f;
    vec4 v1 = mult(World, vec4(V, 1.0));
    //std::cout << v1.x << "\n";
    V = vec3(v1);
}

void rot(vec3& V) {
    Worldr[0][0] = cosf(scaler); Worldr[0][1] = -sinf(scaler); Worldr[0][2] = 0.0f; Worldr[0][3] = 0.0;
    Worldr[1][0] = sinf(scaler); Worldr[1][1] = cosf(scaler); Worldr[1][2] = 0.0f; Worldr[1][3] = 0.0;
    Worldr[2][0] = 0.0f; Worldr[2][1] = 0.0f; Worldr[2][2] = 1.0f; Worldr[2][3] = 0.0f;
    Worldr[3][0] = 0.0f; Worldr[3][1] = 0.0f; Worldr[3][2] = 0.0f; Worldr[3][3] = 1.0f;
    vec4 v1 = mult(Worldr, vec4(V, 1.0));
    //std::cout << v1.x << "\n";
    V = vec3(v1);
}

void scal(vec3& V) {
    Worldr[0][0] = 1+sinf(scaler); Worldr[0][1] = 0.0f; Worldr[0][2] = 0.0f; Worldr[0][3] = 0.0;
    Worldr[1][0] = 0.0f; Worldr[1][1] = 1+sinf(scaler); Worldr[1][2] = 0.0f; Worldr[1][3] = 0.0;
    Worldr[2][0] = 0.0f; Worldr[2][1] = 0.0f; Worldr[2][2] = 1+sinf(scaler); Worldr[2][3] = 0.0f;
    Worldr[3][0] = 0.0f; Worldr[3][1] = 0.0f; Worldr[3][2] = 0.0f; Worldr[3][3] = 1.0f;
    vec4 v1 = mult(Worlds, vec4(V, 1.0));
    //std::cout << v1.x << "\n";
    V = vec3(v1);
}

GLuint VBO;

void RenderSceneGB() { 
    for (int i = 0; i < 3; i++) {
        if (i == 0 && Triangle[i].x < -1) { scalex *= -1; scale *= -1; }
        if (i == 1 && Triangle[i].x > 1) { scalex *= -1; }
        if (Triangle[i].y > 1)scaley *= -1;
        if (i == 0 && Triangle[i].y < -1)scaley *= -1;
        scal(Triangle[i]);

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
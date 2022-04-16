#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "glm/glm.hpp"
#include "glm/mat4x4.hpp"
#include <iostream>

float scale = 0.01f;

using namespace glm;

GLuint VBO;
GLuint IBO;
GLuint gWorldLocation;

static const char* vertex = "                                                      \n\
    #version 330                                                                   \n\
    in vec3 pos;                                                                   \n\
    uniform mat4 gWorld;                                                           \n\
    out vec4 vertexcolor;                                                          \n\
    void main()                                                                    \n\
    {                                                                              \n\
        gl_Position = vec4(pos, 1.0) * gWorld ;                                    \n\
        vertexcolor = vec4(clamp(pos, 0.0, 1.0), 0.5);                             \n\
    }";

static const char* frag = "                                                         \n\
    #version 330                                                                    \n\
    in vec4 vertexcolor;                                                            \n\
    out vec4 fragcolor;                                                             \n\
    void main()                                                                     \n\
    {                                                                               \n\
        fragcolor = vertexcolor;                                                    \n\
    }";

mat4 m = {
        m[0][0] = 1.0f, m[0][1] = 0.0f, m[0][2] = 0.0f, m[0][3] = 0.0f,
        m[1][0] = 0.0f, m[1][1] = 1.0f, m[1][2] = 0.0f, m[1][3] = 0.0f,
        m[2][0] = 0.0f, m[2][1] = 0.0f, m[2][2] = 1.0f, m[2][3] = 0.0f,
        m[3][0] = 0.0f, m[3][1] = 0.0f, m[3][2] = 0.0f, m[3][3] = 1.0f,
};

struct projection {
    float FOV;
    float Width;
    float Height;
    float zNear;
    float zFar;
};
vec3 cross(vec3 v1, vec3 v2) {
    float x = v1.y * v2.z - v1.z * v2.y;
    float y = v1.z * v2.x - v1.x * v2.z;
    float z = v1.x * v2.y - v1.y * v2.x;
    return vec3(x, y, z);
}
void norm(vec3& v) {
    float len = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    v.x /= len;
    v.y /= len;
    v.z /= len;
}
struct camera {
    vec3 pos;
    vec3 target;
    vec3 up;
};


class Pipeline
{
private:
    mat4 ScaleTrans, RotateTrans, TransTrans, Proj, Cam, CamTrans;
    vec3 m_scale, m_trans, m_rot;
    mat4 m_transform;
    projection myproj;
    camera mycam;
    
    void InitScaleTransform() {
        ScaleTrans = m;
        ScaleTrans[0][0] = m_scale.x;
        ScaleTrans[1][1] = m_scale.y;
        ScaleTrans[2][2] = m_scale.z;
    };
    void InitRotateTransform() {
        mat4 rx, ry, rz;
        rx = m;
        ry = m;
        rz = m;
        const float x = radians(10*m_rot.x);
        const float y = radians(m_rot.y);
        const float z = radians(m_rot.z);
        
        rx[1][1] = cosf(x); rx[1][2] = -sinf(x); 
        rx[2][1] = sinf(x); rx[2][2] = cosf(x);

        ry[0][0] = cosf(y); ry[0][2] = -sinf(y); 
        ry[2][0] = sinf(y); ry[2][2] = cosf(y);

        rz[0][0] = cosf(z); rz[0][1] = -sinf(z); 
        rz[1][0] = sinf(z); rz[1][1] = cosf(z);

        RotateTrans = rz * ry * rx;
    };
    void InitTranslationTransform() {
        TransTrans = m;
        TransTrans[0][3] = 5*m_trans.x;
        TransTrans[1][3] = 10*m_trans.y;
        TransTrans[2][3] = m_trans.z;
    };
    void InitPerspective() {
        float ar = myproj.Width / myproj.Height;
        float zNear = myproj.zNear;
        float zFar = myproj.zFar;
        float zRange = zNear - zFar;
        float tanHalfFOV = tanf(radians(myproj.FOV / 2.0));

        Proj = m;
        Proj[0][0] = 1 / (tanHalfFOV * ar);
        Proj[1][1] = 1 / tanHalfFOV;
        Proj[2][2] = (-zNear - zFar) / zRange;
        Proj[2][3] = 2. * zFar * zNear / zRange;
        Proj[3][2] = 1.0f;
        Proj[3][3] = 0.0f;
    };
    void InitCamera() {
        vec3 t = mycam.target;
        vec3 u = mycam.up;
        norm(t);
        norm(u);
        u = cross(u, mycam.target);
        vec3 v = cross(t, u);
        Cam = m;
        Cam[0][0] = u.x; Cam[0][1] = u.y; Cam[0][2] = u.z;
        Cam[1][0] = v.x; Cam[1][1] = v.y; Cam[1][2] = v.z;
        Cam[2][0] = t.x; Cam[2][1] = t.y; Cam[2][2] = t.z;
    }
    void InitCamTrans() {
        CamTrans = m;
        CamTrans[0][3] = -mycam.pos.x;
        CamTrans[1][3] = -mycam.pos.y;
        CamTrans[2][3] = -mycam.pos.z;
    }

public:
    Pipeline() {
        m_scale = { 1.0f, 1.0f, 1.0f };
        m_trans = { 0.0f, 0.0f, 0.0f };
        m_rot = { 0.0f, 0.0f, 0.0f };
        m_transform = m;
    }

    void scale(float x, float y, float z){
        m_scale = { x, y,z };        
    }

    void trans(float x, float y, float z){ 
        m_trans = { x, y,z };
    }

    void rotate(float x, float y, float z){
        m_rot = { x, y,z};
    }

    void proj(float a, float b, float c, float d, float e) {
        myproj.FOV = a;
        myproj.Height = b;
        myproj.Width = c;
        myproj.zFar = d;
        myproj.zNear = e;
    }

    void cam(vec3 pos, vec3 target, vec3 up) {
        mycam.pos = pos;
        mycam.target = target;
        mycam.up = up;
    }

    mat4* GetTrans();
};

mat4* Pipeline::GetTrans()
{
    InitScaleTransform();
    InitRotateTransform();
    InitTranslationTransform();
    InitPerspective();
    InitCamera();
    InitCamTrans();

    //m_transform = ScaleTrans * RotateTrans * TransTrans;
    m_transform = ScaleTrans * RotateTrans * TransTrans *CamTrans*Cam * Proj;
    return &m_transform;
}

static void RenderSceneCB()
{
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f); 
    glClear(GL_COLOR_BUFFER_BIT);

    scale += 0.01f;

    Pipeline p;
    //p.scale(sinf(scale), sinf(scale), sinf(scale));
    p.trans(sinf(scale), 0.0f, 0.0f);
    p.rotate(scale, 0, scale);
    p.proj(30.0f, 800, 800, 1.0f, 1000.0f);
    vec3 pos(1.0, 1.0, 3.0);
    vec3 target(0.45, 0.0, 1.0);
    vec3 up(0.0, 1.0, 0.0);
    p.cam(pos, target, up);

    glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, (const GLfloat*)p.GetTrans());

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(0);
    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);
    glutPostRedisplay();

    glutSwapBuffers();
}

void genbuffers() {
    vec3 Pyramid[4]{
       { -1, -1, 10 },
       { 0, -1, 10 },
       { 0, -1, 10},
        { 0, 1, 20 },
    };

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Pyramid), Pyramid, GL_STATIC_DRAW);

    unsigned int Indices[] = { 0, 3, 1,
                               1, 3, 2,
                               2, 3, 0,
                               0, 2, 1 };

    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
}

GLuint genshader(const char* shadertext, GLenum shaderType) { 
    GLuint shader = glCreateShader(shaderType);

    GLint success; 	GLchar InfoLog[1024];

    const GLchar* vertexShaderSource[1];
    vertexShaderSource[0] = shadertext;
    glShaderSource(shader, 1, vertexShaderSource, NULL);

    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, sizeof(InfoLog), NULL, InfoLog);
        printf("Error compiling shader: '%s'\n", InfoLog);
    }
    return shader;
}

void bindshader(GLuint program, GLuint shader) {
    GLint success; 	GLchar InfoLog[1024];

    glAttachShader(program, shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == 0)
    {
        glGetProgramInfoLog(program, sizeof(InfoLog), NULL, InfoLog);
        printf("Error linking shader program: '%s'\n", InfoLog);
    }
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("IDKWTD");

    glutDisplayFunc(RenderSceneCB);
    glutIdleFunc(RenderSceneCB);

    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }
    
    genbuffers();
    GLuint vshader = genshader(vertex, GL_VERTEX_SHADER);
    GLuint fshader = genshader(frag, GL_FRAGMENT_SHADER);
    GLuint program = glCreateProgram();
    bindshader(program, vshader);
    bindshader(program, fshader);
    glUseProgram(program);
    //genshaders(vertex, frag);

    glutMainLoop();
}

/*void genshaders(const char* shadertext_v, const char* shadertext_f)
{
    GLint success; 	GLchar InfoLog[1024];
    GLuint ShaderProgram = glCreateProgram();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar* vertexShaderSource[1];
    vertexShaderSource[0] = shadertext_v;
    glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    const GLchar* fragmentShaderSource[1];
    fragmentShaderSource[0] = shadertext_f;
    glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Checking for vertex shader compilation errors
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, sizeof(InfoLog), NULL, InfoLog);
        printf("Error compiling shader type %d: '%s'\n", GL_VERTEX_SHADER, InfoLog);
    }
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, sizeof(InfoLog), NULL, InfoLog);
        printf("Error compiling shader type %d: '%s'\n", GL_FRAGMENT_SHADER, InfoLog);
    }

    glAttachShader(ShaderProgram, vertexShader);
    glAttachShader(ShaderProgram, fragmentShader);
    glLinkProgram(ShaderProgram);

    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &success);
    if (success == 0)
    {
        glGetProgramInfoLog(ShaderProgram, sizeof(InfoLog), NULL, InfoLog);
        printf("Error linking shader program: '%s'\n", InfoLog);
    }

    glUseProgram(ShaderProgram);

    gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
    //assert(gWorldLocation != 0xFFFFFFFF);
}*/
////#include <GL/glew.h>
////#include <GL/freeglut.h>
////#include "glm/glm.hpp"
////#include <algorithm>
////#include "glm/mat4x4.hpp"
////#include <iostream>
////
////using namespace glm;
////float scalex = 0.001;
////float scaley = 0.001;
////float scaler = 0.005;
////float scale = 0.01;
////
////float s1 = 0.001;
////float s2 = 0.001;
////
////vec2 len(vec3 v1, vec3 v2, vec3 v3) {
////    vec2 v;
////    float l12 = sqrtf((v1.x - v2.x) * (v1.x - v2.x) + (v1.y - v2.y) * (v1.y - v2.y));
////    float l23 = sqrtf((v3.x - v2.x) * (v3.x - v2.x) + (v3.y - v2.y) * (v3.y - v2.y));
////    float l13 = sqrtf((v1.x - v3.x) * (v1.x - v3.x) + (v1.y - v3.y) * (v1.y - v3.y));
////    float m1 = max(l12, l23);
////    v.x = max(m1, l13);
////    float m2 = min(l12, l23);
////    v.y = min(m2, l13);
////    return v;
////}
////class Pipeline
////{
////private:
////    mat4 ScaleTrans, RotateTrans, TransTrans;
////    vec3 m_scale;  vec3 m_trans;   vec3 m_rot;
////    mat4 m_transform;
////    mat4 m{
////    m[0][0] = 1.0f, m[0][1] = 0.0f, m[0][2] = 0.0f, m[0][3] = 0.0f,
////    m[1][0] = 0.0f, m[1][1] = 1.0f, m[1][2] = 0.0f, m[1][3] = 0.0f,
////    m[2][0] = 0.0f, m[2][1] = 0.0f, m[2][2] = 1.0f, m[2][3] = 0.0f,
////    m[3][0] = 0.0f, m[3][1] = 0.0f, m[3][2] = 0.0f, m[3][3] = 1.0f,
////    };
////public:
////    Pipeline() {}
////
////    void Scale(float ScaleX, float ScaleY, float ScaleZ){
////        m_scale = { ScaleX, ScaleY,ScaleZ };
////        ScaleTrans = m;
////        ScaleTrans[0][0] = m_scale.x;
////        ScaleTrans[1][1] = m_scale.y;
////        ScaleTrans[2][2] = m_scale.z;
////        //std::cout << ScaleTrans[0][0];
////    }
////
////    void Trans(float x, float y, float z){ 
////        m_trans = { x, y,z };
////        TransTrans = m;
////        TransTrans[0][3] = m_trans.x;
////        TransTrans[1][3] = m_trans.y;
////        TransTrans[2][3] = m_trans.z;
////    }
////
////    void Rotate(float RotateX, float RotateY){
////        m_rot = { RotateX, RotateY,1.0};
////        RotateTrans = m;
////        RotateTrans[0][0] = m_rot.x;
////        RotateTrans[1][1] = m_rot.x;
////        RotateTrans[1][0] = -m_rot.y;
////        RotateTrans[0][1] = m_rot.y;
////    }
////
////    mat4 GetTrans();
////};
////mat4 Pipeline::GetTrans()
////{
////    m_transform = ScaleTrans * RotateTrans * TransTrans;
////    return m_transform;
////}
////
////
////mat4 World; //translation
////mat4 Worldr; //rotation
////mat4 Worlds; //scaling
////
////vec3 Triangle[3] = {
////{ -0.1, -0.1, 0.0f },
////{ 0.1, -0.1, 0.0f },
//// { 0, 0.5, 0.0f },
////};
////
////vec3 Pyramid[4]{
////{ -0.1, -0.1, 0.0f },
////{ 0.0f, 0.15f, 0.1f },
////{ 0.1f,-0.1f, 0.0f},
//// { 0, 0.5, 0.0f },
////};
////
////unsigned int Indices[] = { 0, 3, 1,
////                           1, 3, 2,
////                           2, 3, 0,
////                           0, 2, 1 };
//////vec4 mult(mat4 M, vec4 V) {
//////    vec4 res;
//////    res.x = M[0][0] * V.x + M[0][1] * V.y + M[0][2] * V.z + M[0][3] * V.w;
//////    res.y = M[1][0] * V.x + M[1][1] * V.y + M[1][2] * V.z + M[1][3] * V.w;
//////    res.z = M[2][0] * V.x + M[2][1] * V.y + M[2][2] * V.z + M[2][3] * V.w;
//////    res.w = M[3][0] * V.x + M[3][1] * V.y + M[3][2] * V.z + M[3][3] * V.w;
//////    return res;
//////}
////
////void trans(vec3& V) {
////    World[0][0] = 1.0f; World[0][1] = 0.0f; World[0][2] = 0.0f; World[0][3] = sinf(scalex);
////    World[1][0] = 0.0f; World[1][1] = 1.0f; World[1][2] = 0.0f; World[1][3] = sinf(scaley);
////    World[2][0] = 0.0f; World[2][1] = 0.0f; World[2][2] = 1.0f; World[2][3] = 0.0f;
////    World[3][0] = 0.0f; World[3][1] = 0.0f; World[3][2] = 0.0f; World[3][3] = 1.0f;
////    vec4 v1 = vec4(V, 1.0) * World;
////    //std::cout << v1.x << "\n";
////    V = vec3(v1);
////}
////
////void rot(vec3& V) {
////    Worldr[0][0] = cosf(scaler); Worldr[0][1] = -sinf(scaler);Worldr[0][2] = 0.0f; Worldr[0][3] = 0.0;
////    Worldr[1][0] = sinf(scaler); Worldr[1][1] = cosf(scaler); Worldr[1][2] = 0.0f; Worldr[1][3] = 0.0;
////    Worldr[2][0] = 0.0f; Worldr[2][1] = 0.0f; Worldr[2][2] = 1.0f; Worldr[2][3] = 0.0f;
////    Worldr[3][0] = 0.0f; Worldr[3][1] = 0.0f; Worldr[3][2] = 0.0f; Worldr[3][3] = 1.0f;
////    vec4 v1 = vec4(V, 1.0) * Worldr;
////    //std::cout << v1.x << "\n";
////    V = vec3(v1);
////}
////
////void scal(vec3& V) {
////    float sc = 1 - sinf(scale);
////    Worlds[0][0] = sc; Worlds[0][1] = 0.0f; Worlds[0][2] = 0.0f; Worlds[0][3] = 0.0;
////    Worlds[1][0] = 0.0f; Worlds[1][1] = sc; Worlds[1][2] = 0.0f; Worlds[1][3] = 0.0;
////    Worlds[2][0] = 0.0f; Worlds[2][1] = 0.0f; Worlds[2][2] = sc; Worlds[2][3] = 0.0f;
////    Worlds[3][0] = 0.0f; Worlds[3][1] = 0.0f; Worlds[3][2] = 0.0f; Worlds[3][3] = 1.0f;
////    vec4 v1 = vec4(V, 1.0) * Worlds;
////    //std::cout << scalex << "\n";
////    V = vec3(v1);
////}
////
////GLuint VBO;
////GLuint IBO;
////
////void RenderSceneGB() {
////    /*vec2 v = len(Triangle[0], Triangle[1], Triangle[2]);
////    if (v.y < 0.05) scale *= -1;
////    if (v.x > 3) scale *= -1;
////
////    if (Triangle[1].x > 1 || Triangle[0].x < -1) scalex *= -1, scale *= -1;
////    if (Triangle[2].y > 1 || Triangle[0].y < -1) scaley *= -1, scale *= -1;
////
////    for (int i = 0; i < 3; i++) {
////        // trans(Triangle[i]);
////        rot(Triangle[i]);
////        // scal(Triangle[i]);
////    }*/
////    /*vec2 v = len(Triangle[0], Triangle[1], Triangle[2]);
////    if (v.y < 0.05) s1 *= -1;
////    if (v.x > 3) s1 *= -1;
////
////    if (Triangle[1].x > 1 || Triangle[0].x < -1) s2 *= -1, s1 *= -1;
////    if (Triangle[2].y > 1 || Triangle[0].y < -1) s2 *= -1, s1 *= -1;
////
////    Pipeline p;
////    p.Scale(1-sinf(s1), 1-sinf(s1), 1-sinf(s1));
////    p.Trans(sinf(s2), sinf(s2), sinf(s2));
////    p.Rotate(cosf(s2), sinf(s2));
////    mat4 mat = p.GetTrans(); */
////
////    /*std::cout << mat[0][0] << " " << mat[0][1] << " " << mat[0][2] << " " << mat[0][3] << "\n";
////    std::cout << mat[1][0] << " " << mat[1][1] << " " << mat[1][2] << " " << mat[1][3] << "\n";
////    std::cout << mat[2][0] << " " << mat[2][1] << " " << mat[2][2] << " " << mat[2][3] << "\n";
////    std::cout << mat[3][0] << " " << mat[3][1] << " " << mat[3][2] << " " << mat[3][3] << "\n";*/
////    /*
////    for (int i = 0; i < 3; i++) {
////        vec4 v = vec4(Triangle[i], 1.0);
////        Triangle[i] = vec3(v * mat);
////        //std::cout << Triangle[i].x << " " << Triangle[i].y << " " << Triangle[i].z << "\n";
////    }*/
////
////   /if (Pyramid[1].x > 1 || Pyramid[0].x < -1) s2 *= -1, s1 *= -1;
////    if (Pyramid[2].y > 1 || Pyramid[0].y < -1) s2 *= -1, s1 *= -1;
////
////    Pipeline p;
////    p.Scale(1 - sinf(s1), 1 - sinf(s1), 1 - sinf(s1));
////    p.Trans(sinf(s2), sinf(s2), sinf(s2));
////    p.Rotate(cosf(s2), sinf(s2));
////    mat4 mat = p.GetTrans();
////    for (int i = 0; i < 4; i++) {
////        vec4 v = vec4(Pyramid[i], 1.0);
////        Pyramid[i] = vec3(v * mat);
////        //std::cout << Triangle[i].x << " " << Triangle[i].y << " " << Triangle[i].z << "\n";
////    }
////
////    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
////    glClear(GL_COLOR_BUFFER_BIT);
////    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
////    glBindBuffer(GL_ARRAY_BUFFER, VBO);
////    //glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(Indices), Indices);
////
////    glEnableVertexAttribArray(0);
////    glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
////
////    //glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Triangle), Triangle);
////    
////    //glDrawArrays(GL_TRIANGLES, 0, 3);
////    glDisableVertexAttribArray(0);
////    
////    glutPostRedisplay();
////    glutSwapBuffers();
////}
////
////
////int main(int argc, char** argv)
////{
////    glutInit(&argc, argv);
////    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
////
////    glutInitWindowSize(1024, 768);
////    glutInitWindowPosition(100, 100);
////    glutCreateWindow("IDKWTD");
////
////    GLenum res = glewInit();
////    if (res != GLEW_OK) {
////        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
////        return 1;
////    }
////    /*glGenBuffers(1, &VBO);
////    glBindBuffer(GL_ARRAY_BUFFER, VBO);
////    glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle), Triangle, GL_STATIC_DRAW);
////    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);*/
////
////    glGenBuffers(1, &VBO);
////    glBindBuffer(GL_ARRAY_BUFFER, VBO);
////    glBufferData(GL_ARRAY_BUFFER, sizeof(Pyramid), Pyramid, GL_STATIC_DRAW);
////    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
////
////    glGenBuffers(1, &IBO);
////    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
////    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
////
////    glutDisplayFunc(RenderSceneGB);
////    glutMainLoop();
////}*/
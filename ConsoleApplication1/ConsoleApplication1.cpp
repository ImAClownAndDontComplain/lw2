#include <GL/glew.h>
#include <GL/freeglut.h>
#include "glm/glm.hpp"
#include <iostream>

float scale = 0.01f;
const float winH = 800;
const float winW = 800;
GLint success; 	
GLchar InfoLog[1024];

using namespace glm;

GLuint VBO;
GLuint IBO;
GLint gWorldLocation;

static const char* vertex = "                                                      \n\
    #version 330                                                                   \n\
    layout (location = 0) in vec3 pos;                                             \n\
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

GLuint genshader(const char* shadertext, GLenum shaderType) { 
    GLuint shader = glCreateShader(shaderType);

    const GLchar* ShaderSource[1];
    ShaderSource[0] = shadertext;
    glShaderSource(shader, 1, ShaderSource, nullptr);

    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shader, sizeof(InfoLog), nullptr, InfoLog);
        std::cerr << "Error compiling shader: " << InfoLog << std::endl;
    }
    return shader;
}
void bindshader(GLuint program, GLuint shader) {
    glAttachShader(program, shader);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (success == 0)
    {
        glGetProgramInfoLog(program, sizeof(InfoLog), nullptr, InfoLog);
        std::cerr << "Error linking shader program: " << InfoLog << std::endl;
    }
}
void genbuffers() {
    vec3 Pyramid[4]{
       { -1, -1, 20 },
       { 1, -1, 20 },
       { 0, -1, 10},
        { 0, 5, 15 },
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
    mat4 ScaleTrans = m, RotateTrans = m, TransTrans = m, Proj = m, Cam = m, CamTrans = m;
    vec3 m_scale, m_trans, m_rot;
    projection myproj;
    camera mycam;
    mat4 m_transform = m;
    
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
        const float x = radians(m_rot.x);
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
        vec3 n = mycam.target;
        vec3 u = mycam.up;
        norm(n);
        norm(u);
        u = cross(u, mycam.target);
        vec3 v = cross(n, u);
        Cam = m;
        Cam[0][0] = u.x; Cam[0][1] = u.y; Cam[0][2] = u.z;
        Cam[1][0] = v.x; Cam[1][1] = v.y; Cam[1][2] = v.z;
        Cam[2][0] = n.x; Cam[2][1] = n.y; Cam[2][2] = n.z;
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
    //m_transform = ScaleTrans * RotateTrans * TransTrans * Proj;
    m_transform = ScaleTrans * RotateTrans * TransTrans * CamTrans * Cam * Proj;
    return &m_transform;
}

void RenderSceneCB()
{
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f); 
    glClear(GL_COLOR_BUFFER_BIT);

    scale += 0.01f;

    Pipeline p;
    p.scale(1, 1, 1);
    p.trans(sinf(scale), 0.0f, 0.0f);
    p.rotate( 0, scale, 0);
    p.proj(30.0f, winH, winW, 1.0f, 1000.0f);
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


int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(winH, winW);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("IDKWTD");

    GLenum res = glewInit();
    if (res != GLEW_OK) {
        std::cerr << "Error: " << glewGetErrorString(res) << std::endl;
        return 1;
    }
    
    genbuffers();
    GLuint vshader = genshader(vertex, GL_VERTEX_SHADER);
    GLuint fshader = genshader(frag, GL_FRAGMENT_SHADER);
    GLuint program = glCreateProgram();
    bindshader(program, vshader);
    bindshader(program, fshader);

    glLinkProgram(program);
    glUseProgram(program);

    gWorldLocation = glGetUniformLocation(program, "gWorld");
    assert(gWorldLocation != 0xFFFFFFFF);


    glutDisplayFunc(RenderSceneCB);
    glutIdleFunc(RenderSceneCB);

    glutMainLoop();
}
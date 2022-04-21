# lw2 done by loginova polina of pro-227

//All of the included libraries have been covered in the readme for the previous lab;

float scale  =  0.01f;	   //a global variable whose values are used when setting values for transformation matrices;

const float winH = 800;    // window height;
const float winW = 800;    // window width;
Glint success;	           // value of successful compilation/binding of shader objects;
GLchar InfoLog[1024];      // array for storing the error message;

using namespace glm;	     //not to write every time glm::…;

Glint VBO;		             //vertex buffer;
Glint IBO;			           //index buffer;
Glint gWorldLocation;      //uniform variable;



//Next is the source code for the vertex and fragment shaders:

static const char* vertex = "                     \n\
#version 330 				              	        	    \n\
layout (location  =  0) in vec3 pos;        			\n\
uniform mat4 gWorld; 				                    	\n\
out vec4 vertexcolor; 			                  		\n\
void main()							                          \n\
{						                                  		\n\
	gl_Position  =  vec4(pos, 1.0) * gWorld ;     	 \n\
	vertexcolor  =  vec4(clamp(pos, 0.0, 1.0), 0.5); \n\
}";

// vertex shader:

// vertex shader inputs can specify the index of an attribute that a particular input uses. This is done with this syntax:
// layout(location  =  attribute index) in vec3 pos;
// with this syntax, you can completely eliminate the use of glBindAttribLocation.

// input - 3D vector(vertex position), uniform matrix(transformation matrix);
// output is a 4D vector (vertex color in RGBA);
// in the main function
//	    the position of the vertex is calculated - first, the input vector is converted into a four-dimensional one then multiplied by the input matrix
//	    set vertex color:
//	    	  clamp limits the values of the input vector (0 and 1 are the upper and lower limits);
//	    	  bounded values of the input vector are converted to a four-dimensional vector and passed as color in RGBA;

static const char* frag = "      \n\
#version 330 			               \n\
in vec4 vertexcolor;        		 \n\
out vec4 fragcolor;	         		 \n\
void main() 	              		 \n\
{					                       \n\
	fragcolor  =  vertexcolor; 		 \n\
}";
// fragment shader:

// input data - a four-dimensional vector (vertex color in RGBA);
// output data is a four-dimensional vector (the color of the fragment in RGBA);
// in the main function, the color of the fragment is set equal to the color of the vertex;



// universal function for creating shaders by source code (shadertext) and type(shaderType):

Glint genshader(const char* shadertext, glenum shaderType) {
        
 	Glint shader  =  glCreateShader(shaderType);            //creating a shader object by type;

 	const GLchar* ShaderSource[1];                          //array of pointers to strings (data source for the shader);

	ShaderSource[0] = shadertext;                           //assigning the address of the data source to the first element;

	glShaderSource(shader, 1, ShaderSource, NULLPTR);

//installing the source code in the shader:
//		shader - shader object;
//		1(count) - number of elements in the array;
//		ShaderSource - an array of pointers to strings containing code to load into the shader;
//		NULLPTR is an array of string lengths.

	glCompileShader(shader);				                        //compiling the shader object.

 	glGetShaderiv(shader,GL_COMPILE_STATUS, &success);

//shader parameter consideration:
//		shader - the shader object to which the request is sent;
//		GL_COMPILE_STATUSis a required parameter of the object (in this case, the compilation status)
//		the shader was compiled successfully or not);
//		&success - the necessary parameter is passed here - the result of compilation;

	if(!success)                       //checking the result (if the compilation was not successful);

	{
 		glGetShaderInfoLog(shader, sizeof(Infolog), nullptr, Infolog);

//return shader object log:
//	shader - the shader in question;
//	sizeof(InfoLog) – character buffer size for storing the returned log;
//	nullptr is the length of the string returned to the infolog;
//	InfoLog is a character array used to return the log.

		std::cerr <<"Error compiling shader: "<< InfoLog << std::endl;              //compile error output;
	}

	 return shader;
// if the shader object was compiled successfully, the program returns it.
}



// universal function to bind an arbitrary type shader to a program object:

void bindshader(GLint program,GLint shader) {

	 glAttachShader(program,shader);                // attaching a shader to a program object;

	 glGetProgramiv(program,GL_LINK_STATUS, &success);
// returns program parameter of the object, in this case, the success of the binding. if not successful, an error output;

 	if(success  =  =  0) {
 		glGetProgramInfoLog(program, sizeof(Infolog), nullptr, Infolog);
		std::cerr <<"Error linking shader program: "<< InfoLog << std::endl;
	}
}



// function for generating and binding the vertex and index buffer:

void genbuffers() {
 	vec3 Pyramid[4]{                       // pyramid vertex coordinates;
		{-1, -1, 20},
		{ 1, -1, 20 },
		{0, -1, 10},
		{ 0, 5, 15 },
};

// creating, binding and setting data for the vertex buffer (detailed in the previous readme);
 glGenBuffers(1, &VBO);
 glBindBuffer(GL_ARRAY_BUFFER, VBO);
 glBufferData(GL_ARRAY_BUFFER,sizeof(Pyramid), Pyramid,GL_STATIC_DRAW);

	 unsigned int Indices[]  = 
		{ 0, 3, 1, 		                        // coordinates of pyramid fragments indices (by vertices);
		1, 3, 2,
		2, 3, 0,
		0, 2, 1};

// creating, binding and setting data for the index buffer;
 glGenBuffers(1, &IBO);
 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
 glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(Indices), Indices,GL_STATIC_DRAW);
}



// identity matrix:
mat4 m  =  {
	m[0][0] =  1.0f, m[0][1] =  0.0f, m[0][2] =  0.0f, m[0][3] =  0.0f,
	m[1][0] =  0.0f, m[1][1] =  1.0f, m[1][2] =  0.0f, m[1][3] =  0.0f,
	m[2][0] =  0.0f, m[2][1] =  0.0f, m[2][2] =  1.0f, m[2][3] =  0.0f,
	m[3][0] =  0.0f, m[3][1] =  0.0f, m[3][2] =  0.0f, m[3][3] =  1.0f,
};

// structure used when projecting:
struct projection{
	 float FOV;                         // camera angle;
	 float Width;                       // width of the screen on which the projection is being made;
	 float Height;                      // screen height;
	 float zNear;                       // position of the near plane Z;
	 float zFar;                        // far Z plane position;
};

// method for vector multiplication of two vectors - returns a vector perpendicular to plane defined by the original vectors:
vec3 cross(vec3 v1,vec3 v2) {
	 float x = v1.y * v2.z - v1.z * v2.y;
	 float y = v1.z * v2.x - v1.x * v2.z
	 float z = v1.x * v2.y - v1.y * v2.x;
	 return vec3(x, y, z);
}

// vector normalization – creation of a vector of unit length from the original one;
void norm(vec3& v) {
 	float len  =  sqrt(v.x*v.x+v.y*v.y +v.z *v.z);      // calculation of the length of the vector;
 	v.x / =  len;
 	v.y / =  len;
 	v.z / =  len;
}

// structure used when working with camera transformations:
struct camera{
	 vec3 pos;                 // camera position;
	 vec3 target;              // direction vector from the camera to its target;
	 vec3 up;                  // vector perpendicular to the top;
};



// pipeline class that combines transformations:
class Pipeline
	{
	private:
// transformation matrices (scaling, rotation, translation, projection, camera rotation and translation):
	 mat4 ScaleTrans  =  m, RotateTrans  =  m, TransTrans  =  m, Proj  =  m, Cam  =  m, CamTrans  =  m;

// scaling, translation and rotation parameters:
	 vec3 m_scale, m_trans, m_rot;

// used projection and camera parameters:
	 projection myproj;
	 camera mycam;

// final transformation matrix:
 	mat4 m_transform  =  m;
 
 	void InitScaleTransform() {                  // initialization of scaling matrix values;
		Scale Trans = m;
		Scale Trans[0][0] =  m_scale.x;
		Scale Trans[1][1] =  m_scale.y;
		Scale Trans[2][2] =  m_scale.z;
	};

 	void InitRotateTransform() {	             // initialization of rotation matrix values;
		Scale Trans = m;

		 mat4	rx, ry, rz;
		 rx = m;                                 // first, the rotation matrices around all axes are set as initial ones;
	 	 ry = m;
		 rz = m;
 		 const float x  =  radians(m_rot.x);     //convert values to radians;
		 const float y  =  radians(m_rot.y);
		 const float z  =  radians(m_rot.z);
 
	rx[1][1] =  cos(x); rx[1][2] =  -sinf(x);   // rotation matrices around the axes OX, OY, OZ;
	rx[2][1] =  sinf(x); rx[2][2] =  cos(x);
	
	ry[0][0] =  cos(y); ry[0][2] =  -sinf(y);
	ry[2][0] =  sinf(y); ry[2][2] =  cos(y);

	rz[0][0] =  cos(z); rz[0][1] =  -sinf(z);
	rz[1][0] =  sinf(z); rz[1][1] =  cos(z);

	RotateTrans = rz * ry * rx;                // getting the rotation matrix;
	};

	 void InitTranslationTransform() {	       // initialization of translation matrix values;
		TransTrans = m;
		TransTrans[0][3] =  5 * m_trans.x;
		TransTrans[1][3] =  10 * m_trans.y;
		TransTrans[2][3] =  m_trans.z;
	};

 	void InitPerspective() {                   // initialization of perspective matrix values;

		 float ar  =  myproj.width / myproj.height;              // aspect ratio of the projection screen;
		 float zNear  =  myproj.zNear;
		 float zFar  =  myproj.zFar;
		 float zRange  =  zNear - zFar;                          // distance between near and far planes;
		 float tanHalfFOV  =  tanf(radians(myproj.FOV / 2.0));   // tangent of half view angle;

		Proj = m;
		Proj[0][0] =  1 / (tanHalfFOV  *  ar);       //transformation of projected coordinates;
		Proj[1][1] =  1 / tanHalfFOV;                //distance from the camera to the projection plane;
		Proj[2][2] =  (-zNear - zFar) / zRange;      // normalization of Z values;
		Proj[2][3] =  2. * zFar * zNear / zRange;
		Proj[3][2] =  1.0f;                          //saving Z value for further depth test (transfer to W);
		Proj[3][3] =  0.0f;
	};

	 void InitCamera() { // initialization of camera rotation matrix values:
// UVN camera:
//	N - The vector from the camera to its target (axis Z).
//	V - If you stand straight, then this vector will come from the head to the sky (axis Y).
// 	U - This vector exits the camera to the right (axis X).
// to translate coordinates from the world system to the camera system defined by UVN vectors, we need to find the inner product between the position vector and the UVN vectors.
		vec3 n  =  mycam.target;
		vec3 u = mycam.up;
		norm(n);
		norm(u);         
		u = cross(u, mycam.target);              // calculation of the vector to the right;
		vec3 v  =  cross(n, u);                 // recalculation of the vector up;

		Cam = m;
		Cam[0][0] = u.x; Cam[0][1] = u.y; Cam[0][2] = u.z;
		Cam[1][0] = v.x; Cam[1][1] = v.y; Cam[1][2] = v.z;
		Cam[2][0] = n.x; Cam[2][1] = n.y; Cam[2][2] = n.z;
// vectors u,v,n are calculated and entered into the rows of the matrix. Since the position vector will be multiplied from the right (in the form of a column), we get the dot product between this vector and vectors u,v,n. This will calculate the values of 3 scalar projections which will become XYZ position values in screen space.
	};

 	void InitCamTrans() {                      // initialization of camera transfer matrix values:
	CamTrans = m;
	CamTrans[0][3] =  - mycam.pos.x;
	CamTrans[1][3] =  - mycam.pos.y;
	CamTrans[2][3] =  - mycam.pos.z;
	};

public:
	Pipeline() {...} // default initialization;

	void scale(float x,float y,float z){...}     // initialization of all used vectors;
	void trans(float x,float y,float z){...}
 	void rotate(float x,float y,float z){...}
 	void proj(float a,float b,float c,float d,float e) {...}
 	void cam(vec3 pos,vec3 target,vec3 up) {...}

 	mat4* GetTrans();                           // declaration of a function to obtain a final transformation matrix;
};

mat4* Pipeline::GetTrans()
{
	InitScaleTransform();                       // getting all transformation matrices;
	InitRotateTransform();
	InitTranslationTransform();
	InitPerspective();
	InitCamera();
	InitCamTrans();

	m_transform = ScaleTrans * RotateTrans * TransTrans * CamTrans * Cam * Proj;
 	return &m_transform;
// getting a final transformation matrix by multiplying all matrices and return its address;
}

void RenderSceneCB()
{
	glClearColor(0.5f, 0.5f, 0.5f, 0.0f);        // pure color declaration and screen fill;
	glClear(GL_COLOR_BUFFER_BIT);

	scale + =  0.01f;                            // increment of the global variable for transformation;

 	Pipeline p;                                  // declaration of the pipeline;
	p.scale(1, 1, 1);                            // initialization of all used pipeline parameters;
	p.trans(sinf(scale), 0.0f, 0.0f);
	p.rotate( 0, scale, 0);
	p.proj(30.0f, winW, winH, 1.0f, 1000.0f);
	 vec3 pos(1.0, 1.0, 3.0);
	 vec3 target(0.45, 0.0, 1.0);
	 vec3 up(0.0, 1.0, 0.0);
	p.cam(pos, target, up);

	 glUniformMatrix4fv(gWorldLocation, 1,GL_TRUE, (const GLfloat*) p.GetTrans());

//	gWorldLocation - Specifies the location of the uniform variable to be changed;
//	1 - the number of matrices that need to be changed;
//	GL_TRUE - indicates whether to transpose the matrix when loading values into the uniform variable (transposing is performed);
//	(const GLfloat*)p.GetTrans()); - matrix for updating gWorldLocation;

	 glBindBuffer(GL_ARRAY_BUFFER, VBO);
	 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
// vertex buffer is bound first,
// then a buffer of indexes defined by vertices is attached;
 
	 glVertexAttribPointer(0, 3,GL_FLOAT,GL_FALSE, 0, 0); 
// This call tells the pipeline how to accept the data inside the buffer (see previous readme for details);
 
	 glEnableVertexAttribArray(0);
// Access to a generic array of vertex attributes;

	  glDrawElements(GL_TRIANGLES, 12,GL_UNSIGNED_INT, 0);
// drawing twelve indices from the bound buffer - 3 for each of the 4 faces of the pyramid;

	glDisableVertexAttribArray(0);			
//Disable each vertex attribute as soon as it is no longer needed;

	glutPostRedisplay();
// marks the current window as needing replay;

	glutSwapBuffers();
// replacing the current buffer with a new one with fresh rendering;

}

int main(int argc,char**argv)
{
 	glutInit (&argc,argv);                       // initialize GLEW, create window (previous readme);
	glutInitDisplayMode (GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowSize (winW, winH);
	glutInitWindowPosition (100, 100);
 	glutCreateWindow ("IDKWTD");

 	GLenum res  =  glewInit();                   // check for an error during initialization GLEW;
	if (res ! = GLEW_OK) {
		std::cerr <<"Error: "<< glewGetErrorString(res) << std::endl;
		return 1;
	}
 
	genbuffers(); // creating buffers;

// creating a vertex and fragment shader;
	 Glint vshader  =  genshader(vertex, GL_VERTEX_SHADER);
	 Glint fshader  =  genshader(frag, GL_FRAGMENT_SHADER);

// creating a program object, binding shaders to it;
 	Glint program  = glCreateProgram();
	bindshader(program, vshader);
	bindshader(program, fshader);


 	glLinkProgram(program); // binding the program object to the executable code;

// set the program object as part of the current render state (executables
// software project files);
	 glUseProgram(program);

	gWorldLocation  =  glGetUniformLocation(program, "gWorld");
// returns the location of the uniform variable (in this case 0 - the value is set in the vertex shader source code);

	assert(gWorldLocation ! =  0xFFFFFFFF);                // checking value;

	glutDisplayFunc(RenderSceneCB);
	glutIdleFunc(RenderSceneCB); 
// glutIdleFunc sets the global idle callback to be func so a GLUT program can perform background processing tasks or continuous animation when window system events are not being received.

	glutMainLoop();
}

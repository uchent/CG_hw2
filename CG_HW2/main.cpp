#include <stdio.h>
#include <stdlib.h>
#include <stddef.h> /*for function: offsetof */
#include <math.h>
#include <string.h>
#include "../GL/glew.h"
#include "../GL/glut.h""
#include "../shader_lib/shader.h"
#include "glm/glm.h"
extern "C"
{
#include "glm_helper.h"
}

struct Vertex
{
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texcoord[2];
};
typedef struct Vertex Vertex;

void init(void);
void display(void);
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void keyboardup(unsigned char key, int x, int y);
void motion(int x, int y);
void mouse(int button, int state, int x, int y);
void idle(void);
void draw_light_bulb(void);
void camera_light_ball_move();
GLuint loadTexture(char* name, GLfloat width, GLfloat height);

namespace
{
	char *obj_file_dir = "../Resources/bunny.obj";
	char *main_tex_dir = "../Resources/Stone.ppm";
	char *noise_tex_dir = "../Resources/Noise.ppm";
	char *ramp_tex_dir = "../Resources/Ramp.ppm";

	GLfloat light_rad = 0.05;//radius of the light bulb
	float eyet = 0.0;//theta in degree
	float eyep = 90.0;//phi in degree
	bool mleft = false;
	bool mright = false;
	bool mmiddle = false;
	bool forward = false;
	bool backward = false;
	bool left = false;
	bool right = false;
	bool up = false;
	bool down = false;
	bool lforward = false;
	bool lbackward = false;
	bool lleft = false;
	bool lright = false;
	bool lup = false;
	bool ldown = false;
	bool bforward = false;
	bool bbackward = false;
	bool bleft = false;
	bool bright = false;
	bool bup = false;
	bool bdown = false;
	bool bx = false;
	bool by = false;
	bool bz = false;
	bool brx = false;
	bool bry = false;
	bool brz = false;
	int mousex = 0;
	int mousey = 0;
}

const float speed = 0.01; // camera / light / ball moving speed
const float rotation_speed = 0.05; // ball rotating speed


GLuint mainTextureID;
GLuint noiseTextureID;
GLuint rampTextureID;

GLMmodel *model;

float eyex = 0.0;
float eyey = 0.0;
float eyez = 5.6;

GLfloat light_pos[] = { 1.1, 1.0, 1.3 };
GLfloat ball_pos[] = { 0.0, 0.0, 0.0 };
GLfloat ball_rot[] = { 0.0, 0.0, 0.0 };

// shader IDs and program ID for shader code usage
GLuint vert0, frag0, program0, vert1, frag1, program1, vert2, frag2, program2;
GLuint program;

// parameters for phong shading
GLfloat light_specular[] = { 0.5, 0.5, 0.5, 1.0 };
GLfloat light_diffuse[] = { 0.8, 0.8, 0.8, 1.0 };
GLfloat light_ambient[] = { 0.2, 0.2, 0.2, 1.0 };

GLfloat Ka[] = { 1.0 , 1.0 , 1.0 };
GLfloat Kd[] = { 1.0 , 1.0 , 1.0 };
GLfloat Ks[] = { 1.0 , 1.0 , 1.0 };
GLfloat Ns = 100;

GLfloat t = 0.0; //threshold
bool tf = false;//flag
GLint mode = 0;

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("Shading");
	glutReshapeWindow(512, 512);

	glewInit();

	init();

	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardup);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);

	glutMainLoop();

	glmDelete(model);
	return 0;
}

void changemode(int mode) {
	switch (mode) {
	case 0:
		program = program0;
		break;
	case 1:
		program = program1;
		t = 0.0;
		tf = false;
		break;
	case 2:
		program = program2;
		break;
	}
}

void Light()
{
	glShadeModel(GL_SMOOTH);

	// z buffer enable
	// glEnable(GL_DEPTH_TEST);

	// enable lighting
	glEnable(GL_LIGHTING);
	// set light property
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);

	glMaterialfv(GL_FRONT, GL_AMBIENT, Ka);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, Kd);
	glMaterialfv(GL_FRONT, GL_SPECULAR, Ks);
	glMaterialf(GL_FRONT, GL_SHININESS, Ns);
}

void init(void)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glEnable(GL_CULL_FACE);
	model = glmReadOBJ(obj_file_dir);

	mainTextureID = loadTexture(main_tex_dir, 512, 256);
	noiseTextureID = loadTexture(noise_tex_dir, 360, 360);
	rampTextureID = loadTexture(ramp_tex_dir, 256, 256);

	glmUnitize(model);
	glmFacetNormals(model);
	glmVertexNormals(model, 90.0, GL_FALSE);
	glEnable(GL_DEPTH_TEST);
	print_model_info(model);
	
	GLuint vbo_id;
	glGenBuffers(1, &vbo_id);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
	Vertex *vertices = (Vertex *)malloc(sizeof(Vertex) * model->numtriangles * 3);
	for (GLuint i = 0; i < model->numtriangles; i++) {
		for (GLuint j = 0; j < 3; j++) {
			// vertex position
			vertices[i * 3 + j].position[0] = model->vertices[3 * (model->triangles[i].vindices[j]) + 0];
			vertices[i * 3 + j].position[1] = model->vertices[3 * (model->triangles[i].vindices[j]) + 1];
			vertices[i * 3 + j].position[2] = model->vertices[3 * (model->triangles[i].vindices[j]) + 2];
			// vertex normal
			vertices[i * 3 + j].normal[0] = model->normals[3 * (model->triangles[i].nindices[j]) + 0];
			vertices[i * 3 + j].normal[1] = model->normals[3 * (model->triangles[i].nindices[j]) + 1];
			vertices[i * 3 + j].normal[2] = model->normals[3 * (model->triangles[i].nindices[j]) + 2];
			// vertex texcoord
			vertices[i * 3 + j].texcoord[0] = model->texcoords[2 * (model->triangles[i].tindices[j]) + 0];
			vertices[i * 3 + j].texcoord[1] = model->texcoords[2 * (model->triangles[i].tindices[j]) + 1];
		}
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * model->numtriangles * 3, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texcoord)));
	glBindVertexArray(0);

	vert0 = createShader("Shaders/Phong.vert", "vertex");
	frag0 = createShader("Shaders/Phong.frag", "fragment");
	program0 = createProgram(vert0, frag0);
	vert1 = createShader("Shaders/Dissolving.vert", "vertex");
	frag1 = createShader("Shaders/Dissolving.frag", "fragment");
	program1 = createProgram(vert1, frag1);
	vert2 = createShader("Shaders/Ramp.vert", "vertex");
	frag2 = createShader("Shaders/Ramp.frag", "fragment");
	program2 = createProgram(vert2, frag2);
	program = program0;
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_LIGHTING);

	GLfloat mtx[16];
	GLint loc;
	glGetFloatv(GL_MODELVIEW_MATRIX, mtx);
	GLfloat mtx2[16];
	GLint loc2;
	glGetFloatv(GL_PROJECTION_MATRIX, mtx2);
	loc2 = glGetUniformLocation(program, "P");

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		eyex,
		eyey,
		eyez,
		eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180),
		eyey + sin(eyet*M_PI / 180),
		eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180),
		0.0,
		1.0,
		0.0);
	draw_light_bulb();
	glPushMatrix();
	glTranslatef(ball_pos[0], ball_pos[1], ball_pos[2]);
	glRotatef(ball_rot[0], 1, 0, 0);
	glRotatef(ball_rot[1], 0, 1, 0);
	glRotatef(ball_rot[2], 0, 0, 1);

	glEnable(GL_TEXTURE_2D);
	// glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);// combine the color from lighting with the color read from the texture
	Light();

	// start to use shaders
	glUseProgram(program);
	
	// parse modelview matrix
	loc = glGetUniformLocation(program, "MV");
	glUniformMatrix4fv(loc, 1, GL_FALSE, mtx);
	// parse projection matrix
	glUniformMatrix4fv(loc2, 1, GL_FALSE, mtx2);

	// parse light position
	GLint loc3;
	loc3 = glGetUniformLocation(program, "Light_position");
	glUniform3fv(loc3, 1, light_pos);
	// parse light specular
	GLint loc4;
	loc4 = glGetUniformLocation(program, "Light_specular");
	glUniform4fv(loc4, 1, light_specular);
	// parse light diffuse
	GLint loc5;
	loc5 = glGetUniformLocation(program, "Light_diffuse");
	glUniform4fv(loc5, 1, light_diffuse);
	// parse light ambient
	GLint loc6;
	loc6 = glGetUniformLocation(program, "Light_ambient");
	glUniform4fv(loc6, 1, light_ambient);

	// parse Ka
	GLint loc7;
	loc7 = glGetUniformLocation(program, "Ka");
	glUniform3fv(loc7, 1, Ka);
	// parse Kd
	GLint loc8;
	loc8 = glGetUniformLocation(program, "Kd");
	glUniform3fv(loc8, 1, Kd);
	// parse Ks
	GLint loc9;
	loc9 = glGetUniformLocation(program, "Ks");
	glUniform3fv(loc9, 1, Ks);
	// parse Ns
	GLint loc10;
	loc10 = glGetUniformLocation(program, "Ns");
	glUniform1f(loc10, Ns);

	GLint loc11 = glGetUniformLocation(program, "mainTexture");
	glActiveTexture(GL_TEXTURE0 + 0); //GL_TEXTUREi = GL_TEXTURE0 + i
	glBindTexture(GL_TEXTURE_2D, mainTextureID);
	glUniform1i(loc11, 0);

	if (mode == 1)
	{
		GLint loc12 = glGetUniformLocation(program, "noiseTexture");
		glActiveTexture(GL_TEXTURE0 + 1); //GL_TEXTUREi = GL_TEXTURE0 + i
		glBindTexture(GL_TEXTURE_2D, noiseTextureID);
		glUniform1i(loc12, 0);

		if (tf == false)
		{
			t += 0.0001;
			if (t > 1)
				tf = true;
		}
		if (tf == true)
		{
			t -= 0.0001;
			if (t < 0)
				tf = false;
		}

		GLint loc13 = glGetUniformLocation(program, "threshold");
		glUniform1f(loc13, t);
	}

	// glmDraw(model, GLM_TEXTURE);
	glDrawArrays(GL_TRIANGLES, 0, model->numtriangles * 3);

	glBindTexture(GL_TEXTURE_2D, NULL);
	// stop to use shaders
	glUseProgram(NULL);

	glPopMatrix();

	glutSwapBuffers();
	camera_light_ball_move();
}


void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 27:
	{	//ESC
		break;
	}
	case 'b'://toggle mode
	{
		//you may need to do somting here
		mode = (mode + 1) % 3;
		changemode(mode);
		break;
	}
	case 'd':
	{
		right = true;
		break;
	}
	case 'a':
	{
		left = true;
		break;
	}
	case 'w':
	{
		forward = true;
		break;
	}
	case 's':
	{
		backward = true;
		break;
	}
	case 'q':
	{
		up = true;
		break;
	}
	case 'e':
	{
		down = true;
		break;
	}
	case 't':
	{
		lforward = true;
		break;
	}
	case 'g':
	{
		lbackward = true;
		break;
	}
	case 'h':
	{
		lright = true;
		break;
	}
	case 'f':
	{
		lleft = true;
		break;
	}
	case 'r':
	{
		lup = true;
		break;
	}
	case 'y':
	{
		ldown = true;
		break;
	}
	case 'i':
	{
		bforward = true;
		break;
	}
	case 'k':
	{
		bbackward = true;
		break;
	}
	case 'l':
	{
		bright = true;
		break;
	}
	case 'j':
	{
		bleft = true;
		break;
	}
	case 'u':
	{
		bup = true;
		break;
	}
	case 'o':
	{
		bdown = true;
		break;
	}
	case '7':
	{
		bx = true;
		break;
	}
	case '8':
	{
		by = true;
		break;
	}
	case '9':
	{
		bz = true;
		break;
	}
	case '4':
	{
		brx = true;
		break;
	}
	case '5':
	{
		bry = true;
		break;
	}
	case '6':
	{
		brz = true;
		break;
	}

	//special function key
	case 'z'://move light source to front of camera
	{
		light_pos[0] = eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180);
		light_pos[1] = eyey + sin(eyet*M_PI / 180);
		light_pos[2] = eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180);
		break;
	}
	case 'x'://move ball to front of camera
	{
		ball_pos[0] = eyex + cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) * 3;
		ball_pos[1] = eyey + sin(eyet*M_PI / 180) * 5;
		ball_pos[2] = eyez - cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180) * 3;
		break;
	}
	case 'c'://reset all pose
	{
		light_pos[0] = 1.1;
		light_pos[1] = 1.0;
		light_pos[2] = 1.3;
		ball_pos[0] = 0;
		ball_pos[1] = 0;
		ball_pos[2] = 0;
		ball_rot[0] = 0;
		ball_rot[1] = 0;
		ball_rot[2] = 0;
		eyex = 0.0;
		eyey = 0.0;
		eyez = 5.6;
		eyet = 0;
		eyep = 90;
		break;
	}
	default:
	{
		break;
	}
	}
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.001f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void motion(int x, int y)
{
	if (mleft)
	{
		eyep -= (x - mousex)*0.1;
		eyet -= (y - mousey)*0.12;
		if (eyet > 89.9)
			eyet = 89.9;
		else if (eyet < -89.9)
			eyet = -89.9;
		if (eyep > 360)
			eyep -= 360;
		else if (eyep < 0)
			eyep += 360;
	}
	mousex = x;
	mousey = y;
}

void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN && !mright && !mmiddle)
		{
			mleft = true;
			mousex = x;
			mousey = y;
		}
		else
			mleft = false;
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN && !mleft && !mmiddle)
		{
			mright = true;
			mousex = x;
			mousey = y;
		}
		else
			mright = false;
	}
	else if (button == GLUT_MIDDLE_BUTTON)
	{
		if (state == GLUT_DOWN && !mleft && !mright)
		{
			mmiddle = true;
			mousex = x;
			mousey = y;
		}
		else
			mmiddle = false;
	}
}

void camera_light_ball_move()
{
	GLfloat dx = 0, dy = 0, dz = 0;
	if (left || right || forward || backward || up || down)
	{
		if (left)
			dx = -speed;
		else if (right)
			dx = speed;
		if (forward)
			dy = speed;
		else if (backward)
			dy = -speed;
		eyex += dy * cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx * sin(eyep*M_PI / 180);
		eyey += dy * sin(eyet*M_PI / 180);
		eyez += dy * (-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx * cos(eyep*M_PI / 180);
		if (up)
			eyey += speed;
		else if (down)
			eyey -= speed;
	}
	if (lleft || lright || lforward || lbackward || lup || ldown)
	{
		dx = 0;
		dy = 0;
		if (lleft)
			dx = -speed;
		else if (lright)
			dx = speed;
		if (lforward)
			dy = speed;
		else if (lbackward)
			dy = -speed;
		light_pos[0] += dy * cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx * sin(eyep*M_PI / 180);
		light_pos[1] += dy * sin(eyet*M_PI / 180);
		light_pos[2] += dy * (-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx * cos(eyep*M_PI / 180);
		if (lup)
			light_pos[1] += speed;
		else if (ldown)
			light_pos[1] -= speed;
	}
	if (bleft || bright || bforward || bbackward || bup || bdown)
	{
		dx = 0;
		dy = 0;
		if (bleft)
			dx = -speed;
		else if (bright)
			dx = speed;
		if (bforward)
			dy = speed;
		else if (bbackward)
			dy = -speed;
		ball_pos[0] += dy * cos(eyet*M_PI / 180)*cos(eyep*M_PI / 180) + dx * sin(eyep*M_PI / 180);
		ball_pos[1] += dy * sin(eyet*M_PI / 180);
		ball_pos[2] += dy * (-cos(eyet*M_PI / 180)*sin(eyep*M_PI / 180)) + dx * cos(eyep*M_PI / 180);
		if (bup)
			ball_pos[1] += speed;
		else if (bdown)
			ball_pos[1] -= speed;
	}
	if (bx || by || bz || brx || bry || brz)
	{
		dx = 0;
		dy = 0;
		dz = 0;
		if (bx)
			dx = -rotation_speed;
		else if (brx)
			dx = rotation_speed;
		if (by)
			dy = rotation_speed;
		else if (bry)
			dy = -rotation_speed;
		if (bz)
			dz = rotation_speed;
		else if (brz)
			dz = -rotation_speed;
		ball_rot[0] += dx;
		ball_rot[1] += dy;
		ball_rot[2] += dz;
	}
}

void draw_light_bulb()
{
	GLUquadric *quad;
	quad = gluNewQuadric();
	glPushMatrix();
	glColor3f(0.4, 0.5, 0);
	glTranslatef(light_pos[0], light_pos[1], light_pos[2]);
	gluSphere(quad, light_rad, 40, 20);
	glPopMatrix();
}

void keyboardup(unsigned char key, int x, int y)
{
	switch (key) {
	case 'd':
	{
		right = false;
		break;
	}
	case 'a':
	{
		left = false;
		break;
	}
	case 'w':
	{
		forward = false;
		break;
	}
	case 's':
	{
		backward = false;
		break;
	}
	case 'q':
	{
		up = false;
		break;
	}
	case 'e':
	{
		down = false;
		break;
	}
	case 't':
	{
		lforward = false;
		break;
	}
	case 'g':
	{
		lbackward = false;
		break;
	}
	case 'h':
	{
		lright = false;
		break;
	}
	case 'f':
	{
		lleft = false;
		break;
	}
	case 'r':
	{
		lup = false;
		break;
	}
	case 'y':
	{
		ldown = false;
		break;
	}
	case 'i':
	{
		bforward = false;
		break;
	}
	case 'k':
	{
		bbackward = false;
		break;
	}
	case 'l':
	{
		bright = false;
		break;
	}
	case 'j':
	{
		bleft = false;
		break;
	}
	case 'u':
	{
		bup = false;
		break;
	}
	case 'o':
	{
		bdown = false;
		break;
	}
	case '7':
	{
		bx = false;
		break;
	}
	case '8':
	{
		by = false;
		break;
	}
	case '9':
	{
		bz = false;
		break;
	}
	case '4':
	{
		brx = false;
		break;
	}
	case '5':
	{
		bry = false;
		break;
	}
	case '6':
	{
		brz = false;
		break;
	}

	default:
	{
		break;
	}
	}
}

void idle(void)
{
	glutPostRedisplay();
}

GLuint loadTexture(char* name, GLfloat width, GLfloat height)
{
	return glmLoadTexture(name, false, true, true, true, &width, &height);
}
#include"SolidProcess.h"
#include<cstdio>
#include<cstdlib>
#include<iostream>
#include<sstream>
#include<GL\freeglut.h>
#include<Windows.h>
#include<ctime>
#include<random>
#include<functional>

Solid *s;
GLfloat step = 2.5f;
GLfloat xMove = 0.0f;
GLfloat yMove = 0.0f;

// mouse
bool mouseLeftDown;
bool mouseRightDown;
float mouseX;
float mouseY;
GLfloat cameraDistance = 30.0f;
GLfloat cameraAngleX;
GLfloat cameraAngleY;

// scale parameter
GLfloat scalex = 1;
GLfloat scaley = 1;
GLfloat scalez = 1;

int facecolor[100][3];
int idx = 0;

// random generator
std::random_device rd;
std::mt19937 eng(rd());
std::uniform_int_distribution<int> dis(1, INT_MAX);

// opengl callback function
void CALLBACK beginCallback(GLenum which)
{
	glBegin(which);
}

void CALLBACK errorCallback(GLenum errorCode)
{
	const GLubyte *errStr;
	errStr = gluErrorString(errorCode);
	cout << "Tessellation Error: " << errStr << endl;
	exit(0);
}

void CALLBACK endCallback(void)
{
	glEnd();
}

void CALLBACK vertexCallback(GLvoid *vertex)
{
	const GLdouble* p;
	p = (GLdouble *)vertex;
	glVertex3dv(p);
}
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ);

GLUtesselator* obj = gluNewTess();

// opengl Initialize
bool InitGL()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClearDepth(1.0);
	glShadeModel(GL_SMOOTH);
	glDepthFunc(GL_DEPTH_TEST);
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	gluTessCallback(obj, GLU_TESS_VERTEX, (void (CALLBACK *)())vertexCallback);
	gluTessCallback(obj, GLU_TESS_BEGIN, (void (CALLBACK *)())beginCallback);
	gluTessCallback(obj, GLU_TESS_END, (void (CALLBACK *)())endCallback);
	gluTessCallback(obj, GLU_TESS_ERROR, (void (CALLBACK *)())errorCallback);

	setCamera(0, 0, 5, 0, 0, 0);
	return true;
}



// Random Face Color Matrix
void showInit()
{
	int numface = s->numface;
	for (int i = 0; i < numface; i++)
	{
		unsigned char firstchannel = dis(eng) % 255;
		unsigned char secondchannel = dis(eng) % 255;
		unsigned char thirdchannel = dis(eng) % 255;

		facecolor[idx][0] = firstchannel;
		facecolor[idx][1] = secondchannel;
		facecolor[idx][2] = thirdchannel;

		idx++;
	}
}


void ChangeSize(int w, int h)
{
	if (h == 0) h = 1;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLfloat nRange = 150.0f;

	if (w <= h)
		glOrtho(-nRange, nRange, -nRange*h / w, nRange*h / w, -nRange, nRange);
	else
		glOrtho(-nRange*w / h, nRange*w / h, -nRange, nRange, -nRange, nRange);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// Drawing Faces
void RenderFace(Solid* s)
{
	Solid* tmps = s;
	while (tmps)
	{
		Face* tmpf = tmps->sfaces;
		int find = 0;
		while (tmpf)
		{
			gluTessBeginPolygon(obj, NULL);
			Loop* tmplp = tmpf->floops;

			unsigned char fch = facecolor[find][0];
			unsigned char sch = facecolor[find][1];
			unsigned char tch = facecolor[find][2];

			while (tmplp)
			{
				glColor3b(fch, sch, tch);
				HalfEdge* he = tmplp->ledge;
				Vertex* startv = he->hev;
				gluTessBeginContour(obj);
				gluTessVertex(obj, startv->coord, startv->coord);
				he = he->nxt;
				Vertex* tmpv = he->hev;
				while (tmpv != startv)
				{
					gluTessVertex(obj, tmpv->coord, tmpv->coord);
					he = he->nxt;
					tmpv = he->hev;
				}
				gluTessEndContour(obj);
				tmplp = tmplp->nextl;

			}
			gluTessEndPolygon(obj);
			tmpf = tmpf->nextf;
			find++;
		}
		tmps = tmps->nexts;
	}
}

// Set Camera Position and lookat direction
void setCamera(float posX, float posY, float posZ, float targetX, float targetY, float targetZ)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(posX, posY, posZ, targetX, targetY, targetZ, 0, 1, 0); // eye(x,y,z), focal(x,y,z), up(x,y,z)
}

// Rendering Function
void displayCB()
{
	// clear buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// save the initial ModelView matrix before modifying ModelView matrix
	glPushMatrix();

	glTranslatef(xMove, yMove, 0.0); // Move up down left right
	// tramsform camera
	glTranslatef(0, -30.0f, cameraDistance);
	glRotatef(cameraAngleX, 1, 0, 0);   // pitch
	glRotatef(cameraAngleY, 0, 1, 0);   // heading

	glScalef(scalex, scaley, scalez);
	glFrontFace(GL_CCW);	// counter clock wise face --> front face

	// Render Solid
	RenderFace(s);
	glPopMatrix();
	glFlush();
	
	glutSwapBuffers();

}

void SpecialKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)  yMove += step;
	if (key == GLUT_KEY_DOWN) yMove -= step;
	if (key == GLUT_KEY_LEFT) xMove -= step;
	if (key == GLUT_KEY_RIGHT) xMove += step;

	glutPostRedisplay();
}

void mouseCB(int button, int state, int x, int y)
{
	mouseX = x;
	mouseY = y;

	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseLeftDown = true;
		}
		else if (state == GLUT_UP)
			mouseLeftDown = false;
	}

	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mouseRightDown = true;
		}
		else if (state == GLUT_UP)
			mouseRightDown = false;
	}
}


void mouseMotionCB(int x, int y)
{
	if (mouseLeftDown)
	{
		cameraAngleY += (x - mouseX);
		cameraAngleX += (y - mouseY);
		mouseX = x;
		mouseY = y;
	}
	if (mouseRightDown)
	{
		cameraDistance += (y - mouseY) * 0.1f;
		mouseY = y;
	}

	//glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{   
	if (key == '+') {
		scalex += 0.1;
		scaley += 0.1;
		scalez += 0.1;

	}
	else if (key == '-') {
		scalex -= 0.1;
		scaley -= 0.1;
		scalez -= 0.1;

		if (scalex == 0 || scaley == 0 || scalez == 0)
		{
			scalex = 1.0;
			scaley = 1.0;
			scalez = 1.0;
		}
	}
	else if (key == 27)
	{
		exit(0);
	}
	else if (key == 'q')
	{
		exit(0);
	}
	else if (key == 'Q')
	{
		exit(0);
	}

	glutPostRedisplay();
}

int main(int argc, char* argv[])
{
	SolidProcess* sweepObj = new SolidProcess();
	string filename;

	std::cout << "***************************************************************************************" << endl;
	std::cout << "                    " << "Solid Modeling Using Euler Operations" << "                        " << endl;
	std::cout << "***************************************************************************************" << endl;
	std::cout << endl;
	std::cout << "Please Input the Config File Of your Model: (Press Number 0 or 1 For Test Case)" << endl;
	std::cin >> filename;

	if (filename == "0")
		s = sweepObj->SolidSingleRing();
	else if (filename == "1")
		s = sweepObj->SolidMultiRing();
	else
		s = sweepObj->SolidConstruction(filename);


	showInit();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1000, 800);
	glutCreateWindow("HXZ EulerOperation: Sweeping Object");
	InitGL();
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutMouseFunc(mouseCB);
	glutMotionFunc(mouseMotionCB);
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(displayCB);

	glutMainLoop();
	system("pause");
	return 0;
}
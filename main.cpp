#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "glm/glm.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef OSX
#include <GLUT/glut.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#endif

#pragma once
#include "BezierPatch.h"

#pragma once
#include "Scene.h"

using namespace std;

class Viewport;

class Viewport {
public:
	int w, h; // width and height
};

//****************************************************
// Global Variables
//****************************************************
Viewport	viewport;
Scene scene;
unsigned const int SPACE = 0;
unsigned const int S = 1;
unsigned const int W = 2;
unsigned const int LEFT = 3;
unsigned const int RIGHT = 4;
unsigned const int UP = 5;
unsigned const int DOWN = 6;
unsigned const int SHIFT = 7;
unsigned const int ZOOM_IN = 8;
unsigned const int ZOOM_OUT = 9;

const float CAMERA_STEP = .05;
const float OBJECT_STEP = .05;
const float OBJECT_ROT = 5.0f; //taking it to mean five degrees

//glm::vec3 CAMERA_POS, CAMERA_LOOK, CAMERA_UP;

glm::vec3 CAMERA_POS(0,0,2);
glm::vec3 CAMERA_LOOK(0,0,0);
glm::vec3 CAMERA_UP(0,1,0);

//Values which will affect the object or camera orientation
glm::vec3 TRANSLATE(0.0,0.0,0.0);
glm::vec3 ZOOM(0,0,0);
float HORIZONTAL_ROT = 0;
float VERTICAL_ROT = 0;

bool WIREFRAME_ON;
bool SMOOTH_SHADING_ON;

void initScene(){
	// Nothing to do here for this simple example.
	SMOOTH_SHADING_ON = true;
	WIREFRAME_ON = false;

	//need to generalize these values later. (max(x,y,z) and min(x,y,z));
	/*CAMERA_POS = glm::vec3(0.0,1.0,.2);
	CAMERA_LOOK = glm::vec3(0.0,0.0,0.2);
	CAMERA_UP = glm::vec3(0.0,0.0,1.0);*/
    
    GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
    GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_diffuse[] = { 0.5, 0.0, 0.7, 1.0 };
	GLfloat mat_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    glShadeModel(GL_SMOOTH);
    
    glClearColor (0.0, 0.0, 0.0, 0.0);
    
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
}

void keyPressed(unsigned char key, int x, int y) {
	float norm;
	glm::vec3 direction;

	switch(key){
	case ' ':
		exit(0);
		break;
	case 's':
		//toggle between flat and smooth
		SMOOTH_SHADING_ON = !SMOOTH_SHADING_ON;

		glutPostRedisplay();
		break;
	case 'w':
		//toggle between filled and wireframe
		WIREFRAME_ON = !WIREFRAME_ON;

		glutPostRedisplay();
		break;
	case '+':
		//zoom in by amount CAMERA_STEP
		direction = CAMERA_LOOK-CAMERA_POS;

		norm = glm::dot(direction,direction);

		if (norm>0.0){
			direction /= glm::sqrt(norm);
		}else{
			printf("Camera position and camera look at position are equal");
			exit(1);
		}

		TRANSLATE = TRANSLATE + (direction*CAMERA_STEP);

		glutPostRedisplay();
		break;
	case '-':
		//zoom out by amount CAMERA_STEP
		direction = CAMERA_LOOK-CAMERA_POS;

		norm = glm::dot(direction,direction);

		if (norm>0.0){
			direction /= glm::sqrt(norm);
		}else{
			printf("Camera position and camera look at position are equal");
			exit(1);
		}

		TRANSLATE = TRANSLATE - (direction*CAMERA_STEP);

		glutPostRedisplay();
		break;
	}
}

void keySpecial(int key, int x, int y){
	int key_modifier = glutGetModifiers();

	switch (key_modifier){
	case GLUT_ACTIVE_SHIFT:
		//we want to translate
		switch(key){
		case GLUT_KEY_LEFT:
			//translate left (object, not camera)
			break;
		case GLUT_KEY_RIGHT:
			//translate right

			break;
		case GLUT_KEY_UP:
			//translate up

			break;
		case GLUT_KEY_DOWN:
			//translate down

			break;
		}
		break;
	default:
		//we want to rotate
		switch(key){
		case GLUT_KEY_LEFT:
			//rotate left (object, not camera)
			HORIZONTAL_ROT += OBJECT_ROT;

			glutPostRedisplay();
			break;
		case GLUT_KEY_RIGHT:
			//rotate right
			HORIZONTAL_ROT -= OBJECT_ROT;

			glutPostRedisplay();
			break;
		case GLUT_KEY_UP:
			//rotate up
			VERTICAL_ROT += OBJECT_ROT;

			glutPostRedisplay();
			break;
		case GLUT_KEY_DOWN:
			//rotate down
			VERTICAL_ROT -= OBJECT_ROT;

			glutPostRedisplay();
			break;
		}
	}
}

void myReshape(int w, int h) {
	viewport.w = w;
	viewport.h = h;
	float aspect_ratio = ((float)w)/((float)h);

	glViewport (0,0,viewport.w,viewport.h);
	
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();
	//glTranslatef(TRANSLATE[0],TRANSLATE[1],TRANSLATE[2]);
	gluPerspective(45.0f,aspect_ratio,CAMERA_POS[2]-1.0f,CAMERA_POS[2]-1000000.0f);
	//glOrtho(CAMERA_POS[0]-1,CAMERA_POS[0]+1,CAMERA_POS[1]-1,CAMERA_POS[1]+1,CAMERA_POS[2]-1,CAMERA_POS[2]-100000.0f);
	gluLookAt(CAMERA_POS[0],CAMERA_POS[1],CAMERA_POS[2],CAMERA_LOOK[0],CAMERA_LOOK[1],CAMERA_LOOK[2],
			  CAMERA_UP[0],CAMERA_UP[1],CAMERA_UP[2]);

	//glPushMatrix();
	//glTranslatef(TRANSLATE[0],TRANSLATE[1],TRANSLATE[2]);
	 // look into this later
//    glOrtho(-20,20,-20,20,20,-20); // elephant view
//    glOrtho(-1, 1 + (w-400)/200.0 , -1 -(h-400)/200.0, 1, 1, -1); // resize type = add
//    glOrtho(-w/400.0, w/400.0, -h/400.0, h/400.0, 1, -1); // resize type = center
//	glOrtho(-1, 1, -1, 1, 1, -1);    // resize type = stretch
	//glPopMatrix();
}

void myDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// clear the color buffer
    
    if (SMOOTH_SHADING_ON) {
        glShadeModel(GL_SMOOTH);
    } else {
        glShadeModel(GL_FLAT);
    }
    
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();
	glTranslatef(TRANSLATE[0],TRANSLATE[1],TRANSLATE[2]);
	glRotatef(HORIZONTAL_ROT,1,0,0); //horizontal rotate
	glRotatef(VERTICAL_ROT,0,1,0);//vertical rotate
	for (int j = 0; j < scene.patch_list.size(); j++) {
		BezierPatch bez = scene.patch_list[j];
		//cout<<"Tri List: "<<bez.tri_list.size()<<endl;
		//cout<<"Geo List: "<<bez.geo_list.size()<<endl;
		for (int i = 0; i < bez.tri_list.size(); i++) {
			glm::vec3 tri = bez.tri_list[i];
			LocalGeo a,b,c;
			a = bez.geo_list[tri[0]];
			b = bez.geo_list[tri[1]];
			c = bez.geo_list[tri[2]];
//			glColor3f(1.0f, 1.0f, 1.0f);

			if (WIREFRAME_ON){
				glPolygonMode(GL_FRONT, GL_LINE); // wireframe mode
				glPolygonMode(GL_BACK, GL_LINE);
                
                glDisable(GL_LIGHTING);
                glClearColor (0.0, 0.0, 0.0, 0.0);
                glColor3f(1.0f,1.0f,1.0f);
                
				glBegin(GL_POLYGON);
				glNormal3f(a.normal[0],a.normal[1],a.normal[2]);
				glVertex3f(a.point[0],a.point[1],a.point[2]);
                glNormal3f(b.normal[0],b.normal[1],b.normal[2]);
				glVertex3f(b.point[0],b.point[1],b.point[2]);
                glNormal3f(c.normal[0],c.normal[1],c.normal[2]);
				glVertex3f(c.point[0],c.point[1],c.point[2]);
				glEnd();

				glPolygonMode(GL_FRONT, GL_FILL); // fill mode
				glPolygonMode(GL_BACK, GL_FILL);
			}else{
                glEnable(GL_LIGHTING);
                
				glBegin(GL_POLYGON);
				glNormal3f(a.normal[0],a.normal[1],a.normal[2]);
				glVertex3f(a.point[0],a.point[1],a.point[2]);
                glNormal3f(b.normal[0],b.normal[1],b.normal[2]);
				glVertex3f(b.point[0],b.point[1],b.point[2]);
                glNormal3f(c.normal[0],c.normal[1],c.normal[2]);
				glVertex3f(c.point[0],c.point[1],c.point[2]);
				glEnd();
			}
		}
	}
	glPopMatrix();

	glFlush();
	glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}

int main(int argc, char* argv[]){
	/*
	parse command line arguments
	*/
	bool use_adaptive = false;

	if (argc<2){
		cout<<"Not enough paramters"<<endl;
		exit(1);
	}

	string filename = argv[1];
	float subdivision_param = atof(argv[2]); //takes on different meaning depending on whether using uniform or adapative (size vs error)
	scene.step = subdivision_param;

	if (argc > 3){
		//not checking third paramter. Assuming good input. Should handle case of bad input later
		use_adaptive = true;
	}

	/*
	parse data from patch file.
	*/
	int max_patches = 0;
	int current_patch = 0;
	BezierPatch working_patch;

	ifstream inpfile(filename.c_str());
	if(!inpfile.is_open()) {
		cout << "Unable to open file" << endl;
	} else {
		string line;
		while(inpfile.good()) {
			vector<string> splitline;
			string buf;
			getline(inpfile,line);
			stringstream ss(line);
			while (ss >> buf) {
				splitline.push_back(buf);
			}
			//blank lines, increment patch number
			if(splitline.size() == 0) {
				current_patch++;
			}

			else if(splitline.size()==1){
				//a single number input, probably in the first line
				max_patches = atoi(splitline[0].c_str());
				current_patch++;
			}

			else if(splitline.size() > 1) {
				glm::vec3 one(atof(splitline[0].c_str()),atof(splitline[1].c_str()),atof(splitline[2].c_str()));
				glm::vec3 two(atof(splitline[3].c_str()),atof(splitline[4].c_str()),atof(splitline[5].c_str()));
				glm::vec3 three(atof(splitline[6].c_str()),atof(splitline[7].c_str()),atof(splitline[8].c_str()));
				glm::vec3 four(atof(splitline[9].c_str()),atof(splitline[10].c_str()),atof(splitline[11].c_str()));
				vector<glm::vec3> row;
				row.push_back(one);
				row.push_back(two);
				row.push_back(three);
				row.push_back(four);
				working_patch.push_back(row);

				if (working_patch.size() == 4) {
					scene.add_patch(working_patch);
					new (&working_patch) BezierPatch();
				}
			}

			else{
				std::cerr << "Unknown command: " << splitline[0] << std::endl;
			}

		}
	}

	if (use_adaptive){

	}else{
		scene.subdivide_patch();
		scene.make_tri_list();
		scene.set_min_max();
	}

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	// Initalize theviewport size
	viewport.w = 800;
	viewport.h = 800;

	//The size and position of the window
	glutInitWindowSize(viewport.w, viewport.h);
	glutInitWindowPosition(0,0);
	glutCreateWindow(argv[0]);

	initScene();							// quick function to set up scene

	glutDisplayFunc(myDisplay);				// function to run when its time to draw something
	glutReshapeFunc(myReshape);				// function to run when the window gets resized

	glutKeyboardFunc(keyPressed);			// end program when spacebar pressed
	glutSpecialFunc(keySpecial);

	glutMainLoop();

	return 0;
}
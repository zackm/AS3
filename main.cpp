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

/******************
*Global Variables *
******************/
Viewport	viewport;
Scene scene;

const float CAMERA_STEP = .15;
const float OBJECT_STEP = .15;
const float OBJECT_ROT = 5.0f; //taking it to mean five degrees

//Values which will affect the object or camera orientation
glm::vec3 TRANSLATE(0.0,0.0,0.0); //moving object

float ZOOM_FACTOR = 1.0f;
float ZOOM_STEP = .05;
float FOV = 30.0f;
float MAX_FOV = 170.0f;
float MIN_FOV = 10.0f;

float HORIZONTAL_ROT = 0;
float VERTICAL_ROT = 0;

bool WIREFRAME_ON, SMOOTH_SHADING_ON, PROJ_ORTHO,MEAN_CURVATURE_ON,GAUSS_CURVATURE_ON;

void initScene(){
	// Nothing to do here for this simple example.
	SMOOTH_SHADING_ON = true;
	WIREFRAME_ON = false;
	PROJ_ORTHO = true; //false means use perspective instead of ortho.
	GAUSS_CURVATURE_ON = false;
	MEAN_CURVATURE_ON = false;

	GLfloat light_position[] = { -1.0, -1.0, -1.0, 0.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_diffuse[] = { 0.5, 0.0, 0.7, 1.0 };
	GLfloat mat_ambient[] = { 0.1, 0.1, 0.1, 1.0 };
	GLfloat mat_shininess[] = { 20.0 };
	glShadeModel(GL_SMOOTH);

	glClearColor (0.0, 0.0, 0.0, 0.0);

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

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
		break;
	case 'w':
		//toggle between filled and wireframe
		WIREFRAME_ON = !WIREFRAME_ON;
		break;
	case 'p':
		//toggle between ortho and perspective projection
		PROJ_ORTHO = !PROJ_ORTHO;
		break;
	case 'g':
		//toggle between gauss and mean curvature. Only one can be on at a time.
		if (MEAN_CURVATURE_ON){
			MEAN_CURVATURE_ON = false;
		}
		GAUSS_CURVATURE_ON = !GAUSS_CURVATURE_ON;
		break;
	case 'm':
		//hold down h to see mean curvature (not a toggle switch)
		if (GAUSS_CURVATURE_ON){
			GAUSS_CURVATURE_ON = false;
		}
		MEAN_CURVATURE_ON = !MEAN_CURVATURE_ON;
		break;
	case '=':
		if (ZOOM_FACTOR*FOV>MIN_FOV){
			ZOOM_FACTOR -= ZOOM_STEP;
		}

		break;
	case '+':
		if (ZOOM_FACTOR*FOV>MIN_FOV){
			ZOOM_FACTOR -= ZOOM_STEP;
		}

		break;
	case '-':
		if(ZOOM_FACTOR*FOV<MAX_FOV){
			ZOOM_FACTOR += ZOOM_STEP;
		}

		break;
	}
}

void keySpecial(int key, int x, int y){
	int key_modifier = glutGetModifiers();
	glm::vec3 right(1,0,0);
	glm::vec3 up(0,1,0);

	switch (key_modifier){
	case GLUT_ACTIVE_SHIFT:
		//we want to translate
		switch(key){
		case GLUT_KEY_LEFT:
			//translate left (object, not camera)
			TRANSLATE = TRANSLATE - OBJECT_STEP*right;
			break;
		case GLUT_KEY_RIGHT:
			//translate right
			TRANSLATE = TRANSLATE + OBJECT_STEP*right;
			break;
		case GLUT_KEY_UP:
			//translate up
			TRANSLATE = TRANSLATE + OBJECT_STEP*up;
			break;
		case GLUT_KEY_DOWN:
			//translate down
			TRANSLATE = TRANSLATE - OBJECT_STEP*up;
			break;
		}
		break;
	default:
		//we want to rotate
		switch(key){
		case GLUT_KEY_LEFT:
			//rotate left (object, not camera)
			HORIZONTAL_ROT += OBJECT_ROT;
			break;
		case GLUT_KEY_RIGHT:
			//rotate right
			HORIZONTAL_ROT -= OBJECT_ROT;
			break;
		case GLUT_KEY_UP:
			//rotate up
			VERTICAL_ROT += OBJECT_ROT;
			break;
		case GLUT_KEY_DOWN:
			//rotate down
			VERTICAL_ROT -= OBJECT_ROT;
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
	if (PROJ_ORTHO){
		//Orthographic Projection
		glOrtho(scene.left*ZOOM_FACTOR,scene.right*ZOOM_FACTOR,scene.bottom*ZOOM_FACTOR,scene.top*ZOOM_FACTOR,
			scene.z_near,scene.z_far);
	}else{
		//Perspective Projection
		gluPerspective(FOV*ZOOM_FACTOR,aspect_ratio,scene.z_near,scene.z_far);
	}
}

void myDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// clear the color buffer

	int w = viewport.w;
	int h = viewport.h;
	//viewport.w = w;
	//viewport.h = h;
	float aspect_ratio = ((float)w)/((float)h);

	glViewport (0,0,viewport.w,viewport.h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (PROJ_ORTHO){
		//Orthographic Projection
		glOrtho(scene.left*ZOOM_FACTOR,scene.right*ZOOM_FACTOR,scene.bottom*ZOOM_FACTOR,scene.top*ZOOM_FACTOR,
			scene.z_near,scene.z_far);
	}else{
		//Perspective Projection
		gluPerspective(FOV*ZOOM_FACTOR,aspect_ratio,scene.z_near,scene.z_far);
	}

	//determine how to color the model
	if (SMOOTH_SHADING_ON) {
		glShadeModel(GL_SMOOTH);
	} else {
		glShadeModel(GL_FLAT);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(scene.camera_pos.x,scene.camera_pos.y,scene.camera_pos.z,
		scene.camera_look.x,scene.camera_look.y,scene.camera_look.z,
		scene.camera_up.x,scene.camera_up.y,scene.camera_up.z);

	//glLoadIdentity();
	glPushMatrix();
	glTranslatef(TRANSLATE[0],TRANSLATE[1],TRANSLATE[2]);
	glRotatef(HORIZONTAL_ROT,1,0,0); //horizontal rotate
	glRotatef(VERTICAL_ROT,0,1,0);//vertical rotate
	Triangle tri;
	float a_color,b_color,c_color;
	for (int j = 0; j < scene.patch_list.size(); j++) {
		BezierPatch bez = scene.patch_list[j];
		for (int i = 0; i < bez.tri_list.size(); i++) {
			Triangle tri = bez.tri_list[i];
			LocalGeo a,b,c;
			a = tri.a;
			b = tri.b;
			c = tri.c;

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
				if (GAUSS_CURVATURE_ON){
					glDisable(GL_LIGHTING);

					glBegin(GL_POLYGON);

					a_color = glm::max(glm::min(a.gaussian_curvature,1.0f),0.0f);
					b_color = glm::max(glm::min(b.gaussian_curvature,1.0f),0.0f);
					c_color = glm::max(glm::min(c.gaussian_curvature,1.0f),0.0f);

					glClearColor (0.0, 0.0, 0.0, 0.0);
					glColor3f(a_color,a_color,a_color);
					glNormal3f(a.normal[0],a.normal[1],a.normal[2]);
					glVertex3f(a.point[0],a.point[1],a.point[2]);

					glClearColor (0.0, 0.0, 0.0, 0.0);
					glColor3f(b_color,b_color,b_color);
					glNormal3f(b.normal[0],b.normal[1],b.normal[2]);
					glVertex3f(b.point[0],b.point[1],b.point[2]);

					glClearColor (0.0, 0.0, 0.0, 0.0);
					glColor3f(c_color,c_color,c_color);
					glNormal3f(c.normal[0],c.normal[1],c.normal[2]);
					glVertex3f(c.point[0],c.point[1],c.point[2]);

					glEnd();
				}else if(MEAN_CURVATURE_ON){
					glDisable(GL_LIGHTING);

					glBegin(GL_POLYGON);

					a_color = glm::max(glm::min(a.mean_curvature,1.0f),0.0f);
					b_color = glm::max(glm::min(b.mean_curvature,1.0f),0.0f);
					c_color = glm::max(glm::min(c.mean_curvature,1.0f),0.0f);

					glClearColor (0.0, 0.0, 0.0, 0.0);
					glColor3f(a_color,a_color,a_color);
					glNormal3f(a.normal[0],a.normal[1],a.normal[2]);
					glVertex3f(a.point[0],a.point[1],a.point[2]);

					glClearColor (0.0, 0.0, 0.0, 0.0);
					glColor3f(b_color,b_color,b_color);
					glNormal3f(b.normal[0],b.normal[1],b.normal[2]);
					glVertex3f(b.point[0],b.point[1],b.point[2]);

					glClearColor (0.0, 0.0, 0.0, 0.0);
					glColor3f(c_color,c_color,c_color);
					glNormal3f(c.normal[0],c.normal[1],c.normal[2]);
					glVertex3f(c.point[0],c.point[1],c.point[2]);

					glEnd();
				}else{
					glClearColor (0.0, 0.0, 0.0, 0.0);
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
		cout<<"Not enough parameters."<<endl;
		exit(1);
	}

	string filename = argv[1];
	cout<<"Input file :"<<filename<<'\n'<<endl;
	float subdivision_param = atof(argv[2]); //takes on different meaning depending on whether using uniform or adapative (size vs error)
	scene.step = subdivision_param;

	if (argc > 3){
		//not checking third paramter. Assuming good input. Should handle case of bad input later
		use_adaptive = true;
		cout<<"Subdivision type: "<<"adaptive."<<endl;
		cout<<"Error tolerance: "<<subdivision_param<<'\n'<<endl;
	}else{
		cout<<"Subdivision type: "<<"uniform."<<'.\n'<<endl;
		cout<<"Step size: "<<subdivision_param<<'\n'<<endl;
	}

	/*
	parse data from patch file.
	*/
	int max_patches = 0;
	int current_patch = 0;
	BezierPatch working_patch;

	ifstream inpfile(filename.c_str());
	if(!inpfile.is_open()) {
		cout << "Unable to open file." << endl;
		exit(1);
	} else {
		cout<< "File opened successfully. Beginning to parse input file.\n"<<endl;
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

	cout<<"\nFinished parsing input file. Initializing subdivision of patches.\n"<<endl;
	scene.subdivide_patch(use_adaptive); //does uniform tessellation if use_adaptive is false

	cout<<"Finished subdividing patches. Setting scene and camera.\n"<<endl;
	scene.set_min_max();
	scene.set_camera_pos();

	cout<<"Total number of triangles to render: "<<scene.number_of_triangles<<'\n'<<endl;

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);

	// Initalize theviewport size
	viewport.w = 600;
	viewport.h = 600;

	//The size and position of the window
	glutInitWindowSize(viewport.w, viewport.h);
	glutInitWindowPosition(0,0);
	glutCreateWindow("Tyler and Zack AS3");

	cout<<"Initializing the scene and rendering the triangles.\n"<<endl;
	initScene();							// quick function to set up scene

	glutDisplayFunc(myDisplay);				// function to run when its time to draw something
	glutReshapeFunc(myReshape);				// function to run when the window gets resized
	glutIdleFunc(myDisplay);                // function to run when idle

	glutKeyboardFunc(keyPressed);			// end program when spacebar pressed
	glutSpecialFunc(keySpecial);

	glutMainLoop();

	return 0;
}
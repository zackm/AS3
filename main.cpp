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

/*
Simple class to store width and height.
*/
class Viewport {
public:
	int w, h; // width and height
};

/******************
*Global Variables *
******************/
Viewport	viewport;
Scene scene;

float PI = 3.1415926; //used to help map curvatures into the range [-1,1].

const float OBJECT_STEP = .15; //The amount the translation matrix is changed.
const float OBJECT_ROT = 5.0f; //The amount the rotation matrix is changed(5.0 degrees).

glm::vec3 TRANSLATE(0.0,0.0,0.0); //The amount to translate the object.

float ZOOM_FACTOR = 1.0f; //The amount that the camera is zoomed on the object
float ZOOM_STEP = .05; //The amount the ZOOM_FACTOR is changed.
float FOV = 90.0f; //The detault field of view.
float MAX_FOV = 170.0f;
float MIN_FOV = 10.0f;

float HORIZONTAL_ROT = 0; //The amount to rotate the object by horizontally (in degrees)
float VERTICAL_ROT = 0; //Vertically.

bool WIREFRAME_ON, SMOOTH_SHADING_ON,MEAN_CURVATURE_ON, //Booleans which determine type of shading/model.
	GAUSS_CURVATURE_ON,MAX_CURVATURE_ON,MIN_CURVATURE_ON, HIDDEN_LINE_ON;

bool OBJ_ON = false;; // If an obj file is given as input.
bool OBJ_NORM = true; // If vertex normals exist in obj file.
vector<Triangle*> tri_vec; //Vector of triangles from obj file;

vector<vector<Triangle*> > connected_triangles; /*for index i, we have a list of all triangles that use vertex i.
												This vector is used to figure out which triangles share a particular point. We need this to get an apprxomation
												of the gaussian curvature.*/

int COLOR_NUM = 0; // Counter to cycle between colors
vector<glm::vec3> COLOR_ARRAY; // Array of color constants

/*
When toggling between shading modes, this method is called to turn all shading modes off.
Then we can turn on only the single shading mode we want. This prevents us from entering
the wrong if statement if the user has turned on multiple shading modes.
*/
void reset_shading_mode(){
	WIREFRAME_ON = false;
	GAUSS_CURVATURE_ON = false;
	MEAN_CURVATURE_ON = false;
	MAX_CURVATURE_ON = false;
	MIN_CURVATURE_ON = false;
	HIDDEN_LINE_ON = false;
}

/*
When shading the model using curvature, this helper method is called to make a vertex,
and normal and color the vertex accordingly. The color of the vertex is blue if the
curvature is positive, red if it is negative, and black if it is 0. Since curvature 
takes values in the range -inf to inf, we map to the range -1 to 1 by the function
(2/PI) * atan(curvature) in order to color.
*/
void curvature_shading(float curvature,glm::vec3 point, glm::vec3 vector){

	float red,blue;
	if (curvature>0.0f){
		red = 0.0f;
		blue = 2.0f*glm::atan(curvature)/PI;
	}else if(curvature<0.0f){
		red = -2.0f*glm::atan(curvature)/PI;
		blue = 0.0f;
	}else{
		red = 0.0f;
		blue = 0.0f;
	}
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glColor3f(red,0.0f,blue);
	glVertex3f(point[0],point[1],point[2]);
	glNormal3f(vector[0],vector[1],vector[2]);
}

/*
Default the scene to a single light with constant BRDF coefficients.
Default to smooth shading using the light with perspective projection.
*/
void initScene(){
	// Hard code various diffuse and specular constants
	COLOR_ARRAY.push_back( glm::vec3( 0.5, 0.0, 0.7 ));
	COLOR_ARRAY.push_back( glm::vec3( 0.0, 0.7, 0.7 ));
	COLOR_ARRAY.push_back( glm::vec3( 0.5, 0.5, 0.0 ));
	COLOR_ARRAY.push_back( glm::vec3( 0.0, 0.2, 0.9 ));
	COLOR_ARRAY.push_back( glm::vec3( 0.5, 0.9, 0.0 ));
	COLOR_ARRAY.push_back( glm::vec3( 0.7, 0.7, 0.7 ));

	SMOOTH_SHADING_ON = true;
	WIREFRAME_ON = false;
	GAUSS_CURVATURE_ON = false;
	MEAN_CURVATURE_ON = false;
	MAX_CURVATURE_ON = false;
	MIN_CURVATURE_ON = false;
	HIDDEN_LINE_ON = false;

	// Add light and specify material properties
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

	if (!OBJ_NORM) {
		WIREFRAME_ON = true;
	}
}

/* 
Change the diffuse, specular, and ambient terms. Rotates between preset colors.
*/
void change_color(){
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glm::vec3 col = COLOR_ARRAY[COLOR_NUM];
	GLfloat mat_specular[] = { col.x, col.y, col.z, 1.0 };
	GLfloat mat_diffuse[] = { col.x, col.y, col.z, 1.0 };
	GLfloat mat_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
}

/*
Basic interaction with keyboard. Listed keys in switch statement toggle
between different shading and viewing modes. Plus and minus keys change
field of view.
*/
void keyPressed(unsigned char key, int x, int y) {
	switch(key){
	case ' ':
		exit(0);

		break;
	case 's':
		//toggle between flat and smooth, provided we aren't in wireframe mode.
		if(!WIREFRAME_ON){
			SMOOTH_SHADING_ON = !SMOOTH_SHADING_ON;
		}

		break;
	case 'w':
		//toggle between filled and wireframe
		if(WIREFRAME_ON){
			reset_shading_mode();
		}else{
			reset_shading_mode();
			WIREFRAME_ON = true;
		}

		break;
	case 'g':
		//toggle between gauss and mean curvature. Only one can be on at a time.
		if(GAUSS_CURVATURE_ON){
			reset_shading_mode();
		}else{
			reset_shading_mode();
			GAUSS_CURVATURE_ON = true;
		}

		break;
	case 'm':
		//hold down h to see mean curvature (not a toggle switch)
		if(!OBJ_ON){
			if(MEAN_CURVATURE_ON){
				reset_shading_mode();
			}else{
				reset_shading_mode();
				MEAN_CURVATURE_ON = true;
			}
		}

		break;
	case '1':
		//toggle max curvature on
		if(!OBJ_ON){
			if(MAX_CURVATURE_ON){
				reset_shading_mode();
			}else{
				reset_shading_mode();
				MAX_CURVATURE_ON = true;
			}
		}

		break;
	case '2':
		//toggle min curvature on
		if(!OBJ_ON){
			if(MIN_CURVATURE_ON){
				reset_shading_mode();
			}else{
				reset_shading_mode();
				MIN_CURVATURE_ON = true;
			}
		}

		break;
	case '=':
		//Zoom in
		if (ZOOM_FACTOR*FOV>MIN_FOV){
			ZOOM_FACTOR -= ZOOM_STEP;
		}

		break;
	case '+':
		//Also zoom in, when shift is pressed with '=' key.
		if (ZOOM_FACTOR*FOV>MIN_FOV){
			ZOOM_FACTOR -= ZOOM_STEP;
		}

		break;
	case '-':
		//zoom out
		if(ZOOM_FACTOR*FOV<MAX_FOV){
			ZOOM_FACTOR += ZOOM_STEP;
		}

		break;
	case 'c':
		//cycle through colors.
		COLOR_NUM += 1;
		COLOR_NUM = COLOR_NUM % COLOR_ARRAY.size();
		change_color();
		break;
	case 'h':
		//remove hidden lines for wireframes, provided wireframe is on. Else, do nothing.
		if(OBJ_ON){
			if(WIREFRAME_ON){
				if (HIDDEN_LINE_ON){
					reset_shading_mode();
				}else{
					reset_shading_mode();
					HIDDEN_LINE_ON = true;
				}
				WIREFRAME_ON = true;
			}
			OBJ_ON = true;
		}else{
			if(WIREFRAME_ON){
				if (HIDDEN_LINE_ON){
					reset_shading_mode();
				}else{
					reset_shading_mode();
					HIDDEN_LINE_ON = true;
				}
				WIREFRAME_ON = true;
			}
			OBJ_ON = false;
		}
		break;
	}
}

/*
Special key refers to shift key. This is the same as keyPressed but keyPressed
can't do special keys.
*/
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

/*
Re-renders the scene if the user has changed the size of the screen.
*/
void myReshape(int w, int h) {
	viewport.w = w;
	viewport.h = h;
	float aspect_ratio = ((float)w)/((float)h);

	glViewport (0,0,viewport.w,viewport.h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV*ZOOM_FACTOR,aspect_ratio,scene.z_near,scene.z_far);
}

/*
Most of the rendering work done here. This method renders the scene using whichever
render mode is turned on (shading normally, wireframe, hidden line, curvature shading).
It also shades differently if OBJ_ON is true or false; ie, if we are rendering an .obj
file or a .bez file.
*/
void myDisplay() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int w = viewport.w;
	int h = viewport.h;
	float aspect_ratio = ((float)w)/((float)h);

	glViewport (0,0,viewport.w,viewport.h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(FOV*ZOOM_FACTOR,aspect_ratio,scene.z_near,scene.z_far);

	//determine how to color the model
	if (SMOOTH_SHADING_ON) {
		glShadeModel(GL_SMOOTH);
	} else {
		glShadeModel(GL_FLAT);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//set camera
	gluLookAt(scene.camera_pos.x,scene.camera_pos.y,scene.camera_pos.z,
		scene.camera_look.x,scene.camera_look.y,scene.camera_look.z,
		scene.camera_up.x,scene.camera_up.y,scene.camera_up.z);

	//glLoadIdentity();
	glPushMatrix();
	glTranslatef(TRANSLATE[0],TRANSLATE[1],TRANSLATE[2]);
	glRotatef(HORIZONTAL_ROT,1,0,0); //horizontal rotate
	glRotatef(VERTICAL_ROT,0,1,0);//vertical rotate

	// Polygon drawing for OBJ files
	if (OBJ_ON) {
		Triangle* tri;
		for (int i = 0; i < tri_vec.size(); i++) {
			tri = tri_vec[i];
			LocalGeo a,b,c;
			a = (*tri).a;
			b = (*tri).b;
			c = (*tri).c;

			if (WIREFRAME_ON){
				if(HIDDEN_LINE_ON){
					/*renders object twice as white and black with an offset. The offset removes some 
					artifacts from lines behind object.
					*/
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

					glDisable(GL_LIGHTING);
					glClearColor(0.0,0.0,0.0,0.0);
					glColor3f(1.0f,1.0f,1.0f);//color as white for wireframe

					glBegin(GL_POLYGON);
					glVertex3f(a.point[0],a.point[1],a.point[2]);
					glVertex3f(b.point[0],b.point[1],b.point[2]);
					glVertex3f(c.point[0],c.point[1],c.point[2]);
					glEnd();

					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//color as background color.
					glEnable(GL_POLYGON_OFFSET_FILL);
					glPolygonOffset(1.0,1.0);
					glClearColor(0.0,0.0,0.0,0.0);
					glColor3f(0.0,0.0,0.0);

					glBegin(GL_POLYGON);//draw background (black) colored object
					glVertex3f(a.point[0],a.point[1],a.point[2]);
					glVertex3f(b.point[0],b.point[1],b.point[2]);
					glVertex3f(c.point[0],c.point[1],c.point[2]);
					glEnd();

					glDisable(GL_POLYGON_OFFSET_FILL);
				}else{
					//Just normal wirefram mode, without removing hidden lines.
					glPolygonMode(GL_FRONT, GL_LINE);
					glPolygonMode(GL_BACK, GL_LINE);

					glDisable(GL_LIGHTING);
					glClearColor (0.0, 0.0, 0.0, 0.0);
					glColor3f(1.0f,1.0f,1.0f);

					glBegin(GL_POLYGON);

					glVertex3f(a.point[0],a.point[1],a.point[2]);
					glVertex3f(b.point[0],b.point[1],b.point[2]);
					glVertex3f(c.point[0],c.point[1],c.point[2]);
					glEnd();

					glPolygonMode(GL_FRONT, GL_FILL); // fill mode
					glPolygonMode(GL_BACK, GL_FILL);
				}
			}else if(GAUSS_CURVATURE_ON && OBJ_NORM){
				//Colors according the gaussian curvature at each vertex.
				glDisable(GL_LIGHTING);

				glBegin(GL_POLYGON);

				curvature_shading(a.gaussian_curvature,a.point,a.normal);
				curvature_shading(b.gaussian_curvature,b.point,b.normal);
				curvature_shading(c.gaussian_curvature,c.point,c.normal);

				glEnd();
			}else{
				//Colors using typical shading and lights.
				glClearColor (0.0, 0.0, 0.0, 0.0);

				glEnable(GL_LIGHTING);

				glBegin(GL_POLYGON);

				if (OBJ_NORM) { glNormal3f(a.normal[0],a.normal[1],a.normal[2]); }
				glVertex3f(a.point[0],a.point[1],a.point[2]);
				if (OBJ_NORM) { glNormal3f(b.normal[0],b.normal[1],b.normal[2]); }
				glVertex3f(b.point[0],b.point[1],b.point[2]);
				if (OBJ_NORM) { glNormal3f(c.normal[0],c.normal[1],c.normal[2]); }
				glVertex3f(c.point[0],c.point[1],c.point[2]);

				glEnd();
			}
		}

	} else {
		// Polygon drawing for bez files
		Triangle tri;
		for (int j = 0; j < scene.patch_list.size(); j++) {
			BezierPatch bez = scene.patch_list[j];
			for (int i = 0; i < bez.tri_list.size(); i++) {
				Triangle tri = bez.tri_list[i];
				LocalGeo a,b,c;
				a = tri.a;
				b = tri.b;
				c = tri.c;

				if (WIREFRAME_ON){
					if(HIDDEN_LINE_ON){
						/*renders object twice as white and black with an offset. The offset removes some 
						artifacts from lines behind object.
						*/
						glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

						glDisable(GL_LIGHTING);
						glClearColor(0.0,0.0,0.0,0.0);
						glColor3f(1.0f,1.0f,1.0f);//color as white for wireframe

						glBegin(GL_POLYGON);//draw wireframe
						glNormal3f(a.normal[0],a.normal[1],a.normal[2]);
						glVertex3f(a.point[0],a.point[1],a.point[2]);
						glNormal3f(b.normal[0],b.normal[1],b.normal[2]);
						glVertex3f(b.point[0],b.point[1],b.point[2]);
						glNormal3f(c.normal[0],c.normal[1],c.normal[2]);
						glVertex3f(c.point[0],c.point[1],c.point[2]);
						glEnd();

						glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);//color as background color.
						glEnable(GL_POLYGON_OFFSET_FILL);
						glPolygonOffset(1.0,1.0);
						glClearColor(0.0,0.0,0.0,0.0);
						glColor3f(0.0,0.0,0.0);

						glBegin(GL_POLYGON);//draw background (black) colored object
						glNormal3f(a.normal[0],a.normal[1],a.normal[2]);
						glVertex3f(a.point[0],a.point[1],a.point[2]);
						glNormal3f(b.normal[0],b.normal[1],b.normal[2]);
						glVertex3f(b.point[0],b.point[1],b.point[2]);
						glNormal3f(c.normal[0],c.normal[1],c.normal[2]);
						glVertex3f(c.point[0],c.point[1],c.point[2]);
						glEnd();

						glDisable(GL_POLYGON_OFFSET_FILL);

					}else{
						//Usual wireframe mode
						glPolygonMode(GL_FRONT, GL_LINE);
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

					}

				//Color according to the various kinds of curvature.
				}else if(GAUSS_CURVATURE_ON){
					glDisable(GL_LIGHTING);

					glBegin(GL_POLYGON);

					curvature_shading(a.gaussian_curvature,a.point,a.normal);
					curvature_shading(b.gaussian_curvature,b.point,b.normal);
					curvature_shading(c.gaussian_curvature,c.point,c.normal);

					glEnd();
				}else if(MEAN_CURVATURE_ON){
					glDisable(GL_LIGHTING);

					glBegin(GL_POLYGON);

					curvature_shading(a.mean_curvature,a.point,a.normal);
					curvature_shading(b.mean_curvature,b.point,b.normal);
					curvature_shading(c.mean_curvature,c.point,c.normal);

					glEnd();
				}else if(MAX_CURVATURE_ON){
					glDisable(GL_LIGHTING);

					glBegin(GL_POLYGON);

					curvature_shading(a.max_curvature,a.point,a.normal);
					curvature_shading(b.max_curvature,b.point,b.normal);
					curvature_shading(c.max_curvature,c.point,c.normal);

					glEnd();
				}else if(MIN_CURVATURE_ON){
					glDisable(GL_LIGHTING);

					glBegin(GL_POLYGON);

					curvature_shading(a.min_curvature,a.point,a.normal);
					curvature_shading(b.min_curvature,b.point,b.normal);
					curvature_shading(c.min_curvature,c.point,c.normal);

					glEnd();
				}else{
					//Do typicaly color and shading using the light
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
	glutSwapBuffers();
}

/*
Helper method for obj file parsing. Finds the number of slashes in
a face reference. This is due to multiple standards for .obj file format.
*/
int slash_count(string s) {
	int count = 0;
	int pos = 0;
	while (s.find("/", pos) != std::string::npos) {
		count++;
		int curr_pos = s.find("/",pos);
		pos = curr_pos + 1;
	}
	return count;
}

int main(int argc, char* argv[]){
	/*
	parse command line arguments
	*/
	bool use_adaptive = false;

	string filename = argv[1];
	cout<<"Input file : "<<filename<<endl;

	// OBJ File Parsing
	if (filename.find(".obj") != std::string::npos) {
		cout<<"File Format: .obj"<<endl;
		OBJ_ON = true;

		// open file
		ifstream inpfile(filename.c_str());
		if(!inpfile.is_open()) {
			cout << "Unable to open file." << endl;
			exit(1);
		}else{
			cout<< "File opened successfully. Beginning to parse input file.\n"<<endl;
			string line;
			vector<glm::vec3> vert_list;
			vector<glm::vec3> norm_list;

			// fill first position for proper numbering, since obj parsing starts at  instead of 0.
			vert_list.push_back(glm::vec3(0,0,0));
			norm_list.push_back(glm::vec3(0,0,0));
			vector<Triangle*> filler_tri;
			connected_triangles.push_back(filler_tri);

			while(inpfile.good()) {
				vector<string> splitline;
				string buf;
				getline(inpfile,line);
				stringstream ss(line);
				while (ss >> buf) {
					splitline.push_back(buf);
				}

				if(splitline.size() == 0) {
					// ignore blank lines
					continue;
				}
				else if(!splitline[0].compare("#")){
					// ignore comments
					continue;
				}
				else if(!splitline[0].compare("s")){
					// ignore s commands
					continue;
				}
				else if(!splitline[0].compare("g")){
					// ignore g commands
					continue;
				}
				else if(!splitline[0].compare("vt")){
					// ignore vertex texture commands
					continue;
				}
				else if(!splitline[0].compare("usemtl")){
					// ignore material commands
					continue;
				}

				else if(!splitline[0].compare("v")){
					// add vertex to list
					float x = atof(splitline[1].c_str());
					float y = atof(splitline[2].c_str());
					float z = atof(splitline[3].c_str());
					vert_list.push_back(glm::vec3(x,y,z));
				}

				else if(!splitline[0].compare("vn")) {
					// add vertex normal to list
					OBJ_NORM = true;
					float x = atof(splitline[1].c_str());
					float y = atof(splitline[2].c_str());
					float z = atof(splitline[3].c_str());
					norm_list.push_back(glm::vec3(x,y,z));
				}

				//Multiple kinds of face formats. This handles a few of them.
				else if(!splitline[0].compare("f")) {
					int a_point,b_point,c_point,a_point_norm,b_point_norm,c_point_norm;
					glm::vec3 a,b,c;
					// add triangle to list, four different parsing options:
					// v for vertex, t for vextex texture (ignored), n for normal
					// face with vertecies v
					// face with vertex textures v/t
					// face with vertex norms v//n
					// face with text and norms v/t/n
					int count = slash_count(splitline[1]);
					if (count == 1) {
						// case: f v/t
						OBJ_NORM = false;
						int pos = splitline[1].find("/");
						a_point = atoi(splitline[1].substr(0,pos).c_str());
						pos = splitline[2].find("/");
						b_point = atoi(splitline[2].substr(0,pos).c_str());
						pos = splitline[3].find("/");
						c_point = atoi(splitline[3].substr(0,pos).c_str());
					}
					else if (splitline[1].find("//") != std::string::npos) {
						// case: f v//n
						OBJ_NORM = true;
						int pos = splitline[1].find("/");
						a_point = atoi(splitline[1].substr(0,pos).c_str());
						a_point_norm = atoi(splitline[1].substr(pos+2).c_str());
						pos = splitline[2].find("/");
						b_point = atoi(splitline[2].substr(0,pos).c_str());
						b_point_norm = atoi(splitline[2].substr(pos+2).c_str());

						pos = splitline[3].find("/");
						c_point = atoi(splitline[3].substr(0,pos).c_str());
						c_point_norm = atoi(splitline[3].substr(pos+2).c_str());

					}
					else if (count == 2) {
						// case: f v/t/n
						OBJ_NORM = true;
						int pos = splitline[1].find("/");
						a_point = atoi(splitline[1].substr(0,pos).c_str());
						pos = splitline[1].find("/",pos+1);
						a_point_norm = atoi(splitline[1].substr(pos+1).c_str());

						pos = splitline[2].find("/");
						b_point = atoi(splitline[2].substr(0,pos).c_str());
						pos = splitline[2].find("/",pos+1);
						b_point_norm = atoi(splitline[2].substr(pos+1).c_str());

						pos = splitline[3].find("/");
						c_point = atoi(splitline[3].substr(0,pos).c_str());
						pos = splitline[3].find("/",pos+1);
						c_point_norm = atoi(splitline[3].substr(pos+1).c_str());

					} else {
						// case: f v
						OBJ_NORM = false;
						a_point = atoi(splitline[1].c_str());
						b_point = atoi(splitline[2].c_str());
						c_point = atoi(splitline[3].c_str());
					}

					//Make the points of triangle a,b,c.
					a = vert_list[a_point];
					b = vert_list[b_point];
					c = vert_list[c_point];

					glm::vec3 a_norm, b_norm, c_norm;
					if (OBJ_NORM) {
						a_norm = norm_list[a_point_norm];
						b_norm = norm_list[b_point_norm];
						c_norm = norm_list[c_point_norm];
					} else {
						a_norm.x = 0,a_norm.y = 0,a_norm.z = 0;
						b_norm.x = 0,b_norm.y = 0,b_norm.z = 0;
						c_norm.x = 0,c_norm.y = 0,c_norm.z = 0;
					}

					//Create the LocalGeo for the points.
					LocalGeo a_geo(a,a_norm);
					LocalGeo b_geo(b,b_norm);
					LocalGeo c_geo(c,c_norm);

					//Make pointer to this new triangle.
					Triangle* new_tri = new Triangle(a_geo,b_geo,c_geo,a_point,b_point,c_point);

					//resize vector. Obj files don't necessarily tell you how many faces there are going to be.
					float n = glm::max(a_point,glm::max(b_point,c_point));
					if(n+1>connected_triangles.size()){
						connected_triangles.resize(n+1); //+1 because numbering for obj starts at 1.
					}

					//update connected triangles to reflect shared vertices.
					connected_triangles[a_point].push_back(new_tri);
					connected_triangles[b_point].push_back(new_tri);
					connected_triangles[c_point].push_back(new_tri);

					//Add pointer to list of all triangles.
					tri_vec.push_back(new_tri);
				}

				else {
					std::cout << "Unknown command: " << splitline[0] << std::endl;
				}
			}

			// Set the min and max values for coordinates when using obj file format.
			float x_min,y_min,z_min,x_max,y_max,z_max;

			x_min = y_min = z_min = numeric_limits<int>::max();
			x_max = y_max = z_max = numeric_limits<int>::min();

			for (int i = 0; i < vert_list.size(); i++) {
				glm::vec3 p = vert_list[i];
				x_min = glm::min(x_min,p.x);
				y_min = glm::min(y_min,p.y);
				z_min = glm::min(z_min,p.z);
				x_max = glm::max(x_max,p.x);
				y_max = glm::max(y_max,p.y);
				z_max = glm::max(z_max,p.z);
			}
			// Pass min and max values to scene
			scene.max.x = x_max;
			scene.max.y = y_max;
			scene.max.z = z_max;
			scene.min.x = x_min;
			scene.min.y = y_min;
			scene.min.z = z_min;
		}
		cout<<"Input file parsed."<<endl;
		cout<<"Total number of triangles to render: "<<tri_vec.size()<<'\n'<<endl;
		if (!OBJ_NORM) {
			cout<<"No normals detected. View in wireframe mode only."<<endl;
		}else{
			/*at this point, we have a vector of vector of connected triangles. We need to figure out
			the gaussian curvature for each vertex on each triangle. The gaussian curvature can be set for 
			each vertex. We use the gaussian sphere approximation; ie, for each vertex, we sum up the area
			of the triangles that use that vertex, and we sum up the spherical triangle area for the normals
			similarly. Then, we do total_sphere_area/total_area. This converges to the gaussian curvature
			if we use signed area, and we take the triangles to approach an area of 0. If we use small triangles
			and unsigned area, we get an approximation to the unsigned gaussian curvature.
			*/
			vector<Triangle*> shared_triangles;//list of triangles which share a vertex.
			Triangle* temp_tri;
			float curvature, numerator, denom;

			for (int i = 0; i<connected_triangles.size(); i++){
				float curvature = numerator = 0.0;
				denom = 0.0f;
				shared_triangles = connected_triangles[i];

				//first loop is to add up all contributing curvatures
				for (int j = 0; j<shared_triangles.size(); j++){
					temp_tri = shared_triangles[j];
					numerator += (*temp_tri).sphere_area;
					denom += (*temp_tri).area;
				}

				if(denom>0.0f){
					curvature = numerator/denom;
				}else{
					curvature = numeric_limits<int>::max();
				}

				LocalGeo* temp_geo;
				//second loop is to set the correct local geo's curvature value.
				for (int j = 0; j<shared_triangles.size(); j++){
					temp_tri = shared_triangles[j];
					temp_geo = (*temp_tri).geo_at_index(i);
					(*temp_geo).gaussian_curvature = curvature;
				}
			}
		}
	} else {
		// BEZ File Parsing
		cout<<"File Format: .bez"<<endl;

		if (argc<2){
			cout<<"Not enough parameters."<<endl;
			exit(1);
		}

		float subdivision_param = atof(argv[2]); //takes on different meaning depending on whether using uniform or adapative (size vs error)
		scene.step = scene.tolerance = subdivision_param;

		if (argc > 3){
			use_adaptive = true;
			cout<<"Subdivision type: "<<"adaptive."<<endl;
			cout<<"Error tolerance: "<<subdivision_param<<'\n'<<endl;
		}else{
			cout<<"Subdivision type: "<<"uniform."<<'\n'<<endl;
			cout<<"Step size: "<<subdivision_param<<'\n'<<endl;
		}

		int max_patches = 0; //dont need these two 
		int current_patch = 0;
		BezierPatch working_patch;

		//being parsing file
		ifstream inpfile(filename.c_str());
		if(!inpfile.is_open()) {
			cout << "Unable to open file." << endl;
			exit(1);
		}else{
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
					//a single number input, total number of patches.
					max_patches = atoi(splitline[0].c_str());
					current_patch++;
				}

				else if(splitline.size() > 1) {
					//The list of four vertices.
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

		cout<<"Total number of triangles to render: "<<scene.number_of_triangles<<'\n'<<endl;
	}

	//uses max and min in scene to set the bounding sphere and camera location, look_at, up vector.
	scene.set_camera_pos();

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

	initScene();

	glutDisplayFunc(myDisplay);
	glutReshapeFunc(myReshape);
	glutIdleFunc(myDisplay);

	glutKeyboardFunc(keyPressed);
	glutSpecialFunc(keySpecial);

	glutMainLoop();

	return 0;
}
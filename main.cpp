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

float PI = 3.1415926;

const float OBJECT_STEP = .15; //The amount the translation matrix is changed.
const float OBJECT_ROT = 5.0f; //The amount the rotation matrix is changed(5.0 degrees).

glm::vec3 TRANSLATE(0.0,0.0,0.0); //The amount to translate the object.

float ZOOM_FACTOR = 1.0f; //The amount that the camera is zoomed on the object
float ZOOM_STEP = .05; //The amount the ZOOM_FACTOR is changed.
float FOV = 30.0f; //The detault field of view.
float MAX_FOV = 170.0f;
float MIN_FOV = 10.0f;

float HORIZONTAL_ROT = 0; //The amount to rotate the object by horizontally (in degrees)
float VERTICAL_ROT = 0; //Vertically.

bool WIREFRAME_ON, SMOOTH_SHADING_ON, PROJ_ORTHO,MEAN_CURVATURE_ON, //Booleans which determine type of projection (ortho or perspective)
GAUSS_CURVATURE_ON,MAX_CURVATURE_ON,MIN_CURVATURE_ON;			//as well as type of shading.
bool OBJ_ON; // If an obj file is given as input.
bool OBJ_NORM; // If vertex normals exist in obj file.
vector<Triangle> tri_vec; // Vector of triangles from obj file;

/*
 When toggling between shading modes, this method is called to turn all shading modes off.
 Then we can turn on only the single shading mode we want.
 */
void reset_shading_mode(){
	WIREFRAME_ON = false;
	GAUSS_CURVATURE_ON = false;
	MEAN_CURVATURE_ON = false;
	MAX_CURVATURE_ON = false;
	MIN_CURVATURE_ON = false;
}

/*
 When shading the model using curvature, this helper method is called to make a vertex,
 and normal and color the vertex accordingly. The color of the vertex is blue if the
 curvature is positive, red if it is negative, and black if it is 0.
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
 Default to smooth shading using the light with orthographic projection
 */
void initScene(){
	SMOOTH_SHADING_ON = true;
	WIREFRAME_ON = false;
	PROJ_ORTHO = true;
	GAUSS_CURVATURE_ON = false;
	MEAN_CURVATURE_ON = false;
	MAX_CURVATURE_ON = false;
	MIN_CURVATURE_ON = false;
    
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
            //toggle between flat and smooth
            SMOOTH_SHADING_ON = !SMOOTH_SHADING_ON;
            
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
        case 'p':
            //toggle between ortho and perspective projection
            PROJ_ORTHO = !PROJ_ORTHO;
            
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
            if(MEAN_CURVATURE_ON){
                reset_shading_mode();
            }else{
                reset_shading_mode();
                MEAN_CURVATURE_ON = true;
            }
            
            break;
        case '1':
            //toggle max curvature on
            if(MAX_CURVATURE_ON){
                reset_shading_mode();
            }else{
                reset_shading_mode();
                MAX_CURVATURE_ON = true;
            }
            
            break;
        case '2':
            //toggle min curvature on
            if(MIN_CURVATURE_ON){
                reset_shading_mode();
            }else{
                reset_shading_mode();
                MIN_CURVATURE_ON = true;
            }
            
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
    
    // Polygon drawing for obj files
    if (OBJ_ON) {
        Triangle tri;
        for (int i = 0; i < tri_vec.size(); i++) {
            tri = tri_vec[i];
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
                
                if (OBJ_NORM) { glNormal3f(a.normal[0],a.normal[1],a.normal[2]); }
                glVertex3f(a.point[0],a.point[1],a.point[2]);
                if (OBJ_NORM) { glNormal3f(b.normal[0],b.normal[1],b.normal[2]); }
                glVertex3f(b.point[0],b.point[1],b.point[2]);
                if (OBJ_NORM) { glNormal3f(c.normal[0],c.normal[1],c.normal[2]); }
                glVertex3f(c.point[0],c.point[1],c.point[2]);
                glEnd();
                
                glPolygonMode(GL_FRONT, GL_FILL); // fill mode
                glPolygonMode(GL_BACK, GL_FILL);
            } else{
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
    
	string filename = argv[1];
	cout<<"Input file : "<<filename<<endl;
    
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
                
                else if(!splitline[0].compare("v")){
                    float x = atof(splitline[1].c_str());
                    float y = atof(splitline[2].c_str());
                    float z = atof(splitline[3].c_str());
                    vert_list.push_back(glm::vec3(x,y,z));
                }
                
                else if(!splitline[0].compare("f")) {
                    int a_point = atoi(splitline[1].c_str());
                    int b_point = atoi(splitline[2].c_str());
                    int c_point = atoi(splitline[3].c_str());
                    glm::vec3 a = vert_list[a_point];
                    glm::vec3 b = vert_list[b_point];
                    glm::vec3 c = vert_list[c_point];
                    glm::vec3 a_norm, b_norm, c_norm;
                    if (OBJ_NORM) {
                        a_norm = norm_list[a_point];
                        b_norm = norm_list[b_point];
                        c_norm = norm_list[c_point];
                    } else {
                        a_norm.x = 0,a_norm.y = 0,a_norm.z = 0;
                        b_norm.x = 0,b_norm.y = 0,b_norm.z = 0;
                        c_norm.x = 0,c_norm.y = 0,c_norm.z = 0;
                    }
                    LocalGeo a_geo(a,a_norm);
                    LocalGeo b_geo(b,b_norm);
                    LocalGeo c_geo(c,c_norm);
                    tri_vec.push_back(Triangle(a_geo,b_geo,c_geo));
                }
                
                else if(!splitline[0].compare("vn")) {
                    OBJ_NORM = true;
                    float x = atof(splitline[1].c_str());
                    float y = atof(splitline[2].c_str());
                    float z = atof(splitline[3].c_str());
                    norm_list.push_back(glm::vec3(x,y,z));
                }
                
                else {
                    std::cerr << "Unknown command: " << splitline[0] << std::endl;
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
            
            scene.max.x = x_max;
            scene.max.y = y_max;
            scene.max.z = z_max;
            scene.min.x = x_min;
            scene.min.y = y_min;
            scene.min.z = z_min;
        }
        cout<<"Input file parsed."<<endl;
        cout<<"Total number of triangles to render: "<<tri_vec.size()<<'\n'<<endl;
        
    } else {
        
        cout<<"File Format: .bez"<<endl;
        
        if (argc<2){
            cout<<"Not enough parameters."<<endl;
            exit(1);
        }
        
        float subdivision_param = atof(argv[2]); //takes on different meaning depending on whether using uniform or adapative (size vs error)
        scene.step = subdivision_param;
        
        if (argc > 3){
            //not checking third paramter. Assuming good input. Should handle case of bad input later
            use_adaptive = true;
            cout<<"Subdivision type: "<<"adaptive."<<endl;
            cout<<"Error tolerance: "<<subdivision_param<<'\n'<<endl;
        }else{
            cout<<"Subdivision type: "<<"uniform."<<'\n'<<endl;
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
        
        cout<<"Total number of triangles to render: "<<scene.number_of_triangles<<'\n'<<endl;
    }
    
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
	initScene();							// quick function to set up scene
    
	glutDisplayFunc(myDisplay);				// function to run when its time to draw something
	glutReshapeFunc(myReshape);				// function to run when the window gets resized
	glutIdleFunc(myDisplay);                // function to run when idle
    
	glutKeyboardFunc(keyPressed);			// end program when spacebar pressed
	glutSpecialFunc(keySpecial);
    
	glutMainLoop();
    
	return 0;
}
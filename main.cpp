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
Scene s;

void initScene(){
	// Nothing to do here for this simple example.
}

void keyPressed(unsigned char key, int x, int y) {
	if (key == ' ') {
		exit(0);
	}
}

void myReshape(int w, int h) {
	viewport.w = w;
	viewport.h = h;
    
	glViewport (0,0,viewport.w,viewport.h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
    gluLookAt(0.0,1.0,0.2, 0.0,0.0,0.2, 0.0,0.0,1.0); // look into this later
//    glOrtho(-1, 1 + (w-400)/200.0 , -1 -(h-400)/200.0, 1, 1, -1); // resize type = add
//    glOrtho(-w/400.0, w/400.0, -h/400.0, h/400.0, 1, -1); // resize type = center
//	glOrtho(-1, 1, -1, 1, 1, -1);    // resize type = stretch
    glOrtho(-5,5,-5,5,5,-5);
}

void myDisplay() {
    
	glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer
    
	glMatrixMode(GL_MODELVIEW);			        // indicate we are specifying camera transformations
	glLoadIdentity();				        // make sure transformation is "zero'd"
    
    
	// Start drawing
    for (int i = 0; i < s.patch_list.size(); i++) {
        BezierPatch bez = s.patch_list[i];
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_LINES); // for wireframing, pass GL_LINES
        glVertex3f(bez.patch[0][0][0], bez.patch[0][0][1], bez.patch[0][0][2]);
        glVertex3f(bez.patch[0][3][0], bez.patch[0][3][1], bez.patch[0][3][2]);
        glVertex3f(bez.patch[3][3][0], bez.patch[3][3][1], bez.patch[3][3][2]);
        glVertex3f(bez.patch[3][0][0], bez.patch[3][0][1], bez.patch[3][0][2]);
        glEnd();
    }
    
    for (int i = 0; i < s.tri_list.size(); i++) {
        glm::vec3 tri = s.tri_list[i];
        glm::vec3 a,b,c;
        a = s.geo_list[tri[0]].point;
        b = s.geo_list[tri[1]].point;
        c = s.geo_list[tri[2]].point;
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_POLYGON); // for wireframing, pass GL_LINES
        glVertex3f(a[0],a[1],a[2]);
        glVertex3f(b[0],b[1],b[2]);
        glVertex3f(c[0],c[1],c[2]);
        glEnd();
    }
    
	glFlush();
	glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}

int main(int argc, char* argv[]){
    /* Test Area
    BezierPatch test;
    
    vector<glm::vec3> one;
    one.push_back(glm::vec3(0.0,0.0,0.1));
    one.push_back(glm::vec3(0.2,0.0,0.1));
    one.push_back(glm::vec3(0.0,0.3,0.1));
    test.push_back(one);
    test.push_back(one);
    
    cout<<test.patch[1][0][2]<<endl;
    cout<<"Complete!"<<endl;
    
    End Test Area */
    
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
    s.step = subdivision_param;

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
                    s.add_patch(working_patch);
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
        for (int i = 0; i < s.patch_list.size(); i++) {
            s.subdivide_patch(s.patch_list[i]);
        }
        s.make_tri_list();
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
    
	glutMainLoop();
    
	return 0;
}
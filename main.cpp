//
//  main.cpp
//  AS3
//
//  Created by Tyler Brabham and Zack Mayeda
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>

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
	gluOrtho2D(0, viewport.w, 0, viewport.h);
}

void myDisplay() {
    
	glClear(GL_COLOR_BUFFER_BIT);				// clear the color buffer
    
	glMatrixMode(GL_MODELVIEW);			        // indicate we are specifying camera transformations
	glLoadIdentity();				        // make sure transformation is "zero'd"
    
    
	// Start drawing

    
	glFlush();
	glutSwapBuffers();					// swap buffers (we earlier set double buffer)
}

int main(int argc, char * argv[])
{
    if (argc < 3) {
        cout<<"Error: too few arguments given"<<endl;
        exit(1);
    }
    
    string filename = argv[1];
    cout<<"Filename: "<<filename<<" found"<<endl;
    
    float param = atof(argv[2]);
    cout<<"Subdivision Parameter: "<<param<<" found"<<endl;
    
    char subdiv_type;
    if (argc > 3) {
        subdiv_type = argv[3][1];
        cout<<"Using adaptive subdivision"<<endl;  
    } else {
        subdiv_type = 'u';
        cout<<"Using uniform subdivision"<<endl;
    }
    
    cout<<"=========================="<<endl;
    
    // File Parser
	std::ifstream inpfile(filename.c_str());
	if(!inpfile.is_open()) {
		std::cout << "Error: Unable to open file" << std::endl;
	} else {
		std::string line;
        std::getline(inpfile,line);
        cout<<"Number of surface patches: "<<line<<endl;
		while(inpfile.good()) {
            std::vector<std::string> splitline;
			std::string buf;
			std::getline(inpfile,line);
			std::stringstream ss(line);
			while (ss >> buf) {
				splitline.push_back(buf);
			}
            
            if (splitline.size() == 0) {
                continue;
            }
            cout<<splitline[0]<<", "<<splitline[1]<<", "<<splitline[2]<<endl;
        }
    }
    
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    
	// Initalize theviewport size
	viewport.w = 400;
	viewport.h = 400;
    
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


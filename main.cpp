#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

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

	if (argc==3){
		//not checking third paramter. Assuming good input. Should handle case of bad input later
		use_adaptive = true;
	}

	/*
	parse data from patch file.
	*/
	int max_patches = 0;
	int current_patch = 0;
	ifstream inpfile(filename.c_str());
	if(!inpfile.is_open()) {
		cout << "Unable to open file" << endl;
	} else {
		string line;
		//MatrixStack mst;
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
			//Ignore comments
			if(splitline[0][0] == '#') {
				continue;
			}
			
			else if(splitline.size()==1){
				//a single number input, probably in the first line
				max_patches = atoi(splitline[0].c_str());
				current_patch++;
			}
			
			else{
				std::cerr << "Unknown command: " << splitline[0] << std::endl;
			}

		}
	}

	if (use_adaptive){

	}else{

	}
	return 0;
}
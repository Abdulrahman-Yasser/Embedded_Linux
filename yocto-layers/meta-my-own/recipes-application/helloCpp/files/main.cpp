#include <iostream>
#include <fstream>
int main(){
	int how_many_times;
	try{ 
		std::ifstream the_file("/etc/helloCpp.txt", std::ifstream::in);
		the_file >> how_many_times;
		the_file.close();
	}catch(...){
		std::ofstream the_file("/etc/helloCpp.txt");
		the_file << how_many_times;
		the_file.close();
	}
	std::cout << "Here i am, a cpp application in your image !!!\n";
	std::cout << "This is the " << how_many_times <<"'s time the command is called\n";
	std::ofstream the_file("/etc/helloCpp.txt");
	how_many_times++;
	the_file << how_many_times;
	the_file.close();
}

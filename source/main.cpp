#include <iostream>
#include <memory>
#include <windows.h>

#include "ExampleApp.h"

using namespace std;

int main() { 
	hlab::ExampleApp exampleApp;
	
	if (!exampleApp.Initialize()) {
        cout << "Initialize failed." << endl;
        return -1;
	}

	return exampleApp.Run();
}
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include "FileHandler.h"

using namespace std;

class FileHandler {
public:
	static void getFile (string& source, string& path)
	{
		ifstream inFile;
		string s;
		inFile.open ( path );
		while(!inFile.eof()) // To get you all the lines.
		{
			getline(inFile, s); // Saves the line in STRING.
			source.append(s);
			source.append("\n");
		}
		inFile.close();
	}
};
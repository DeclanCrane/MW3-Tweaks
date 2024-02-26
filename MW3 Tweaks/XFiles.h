#pragma once

#include <fstream>
#include <vector>
#include <string>

#include <iostream>

class XFile {
public:
	std::fstream fs;
	std::string filePath;
public:
	XFile(std::string filePath);
	~XFile();
public:
	bool Exists();
	bool Create();
	void Write(const std::string &buff);
	void Read(std::vector<std::string>& lines);
	bool Empty();
};
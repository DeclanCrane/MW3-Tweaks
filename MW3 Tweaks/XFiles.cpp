#include "XFiles.h"

XFile::XFile(std::string filePath)
{
	this->filePath = filePath;
}

XFile::~XFile()
{
	if (fs.is_open())
		fs.close();
}

bool XFile::Exists()
{
	bool exists = false;
	fs.open(this->filePath, std::ios::in);
	exists = fs.is_open();
	fs.close();

	return exists;
}

bool XFile::Create()
{
	bool created = false;
	fs.open(this->filePath, std::ios::out | std::ios::trunc);
	created = fs.is_open();
	fs.close();

	return created;
}

void XFile::Write(const std::string &buff)
{
	fs.open(this->filePath, std::ios::out);
	fs << buff.c_str();
	fs.close();
}

void XFile::Read(std::vector<std::string>& lines)
{
	std::string buff;
	fs.open(this->filePath, std::ios::in);
	while (std::getline(fs, buff)) {
		lines.push_back(buff);
	}
	fs.close();
}

bool XFile::Empty()
{
	bool empty = true;
	fs.open(this->filePath, std::ios::in);
	empty = (fs.peek() == std::ifstream::traits_type::eof());
	fs.close();

	return empty;
}

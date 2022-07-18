#pragma once
#include <fstream>
#include <filesystem>
#include <unordered_map>

template<int NumFiles=1>
class FileWriter {
	std::unordered_map<std::string, int> FilesIndex;
	std::string files_dir[NumFiles];

	std::fstream current_file;
	std::string current_file_dir;
	int tot_files = 0;

public:
	bool haswritten = false;

	FileWriter() {

	}
	~FileWriter() {
		if (current_file.is_open()) current_file.close();
	}

	void AddFile(std::string folder, std::string File_Name,
		std::string extention, std::string code_name) {
		FilesIndex[code_name] = tot_files;
		files_dir[tot_files] = "Data/" + folder + "/" + File_Name + extention;
		if (!std::filesystem::is_directory("Data/" + folder)) {
			std::filesystem::create_directory("Data/" + folder);
		}
		tot_files += 1;
	}

	void WriteLine(std::string Line) {
		current_file << Line << std::endl;
		haswritten = true;
	}

	void ChangeFile(int FileNo) {
		if (current_file.is_open()) current_file.close();
		current_file_dir = files_dir[FileNo];
		current_file.open(current_file_dir, std::ios::app | std::ios::in | std::ios::out);
	}

	void ChangeFile(std::string code_name) {
		if (current_file.is_open()) current_file.close();
		current_file_dir = files_dir[FilesIndex[code_name]];
		current_file.open(current_file_dir, std::ios::app | std::ios::in | std::ios::out);
	}

	void ChangeFile(std::string folder, std::string File_Name, std::string extention) {
		if (current_file.is_open()) current_file.close();
		current_file_dir = "Data/" + folder + "/" + File_Name + extention;
		current_file.open(current_file_dir, std::ios::app | std::ios::in | std::ios::out);
	}

	void ClearFile() {
		if (current_file.is_open()) current_file.close();
		std::filesystem::remove(current_file_dir);
		current_file.open(current_file_dir, std::ios::app | std::ios::in | std::ios::out);
	}

	void ClearAllFiles() {
		if (current_file.is_open()) current_file.close();
		for (int i = 0; i < NumFiles; i++) {
			current_file.open(files_dir[i], std::ios::out | std::ios::trunc);
			current_file.close();
		}
		if(current_file_dir != "") 
		current_file.open(current_file_dir, std::ios::app | std::ios::in | std::ios::out);
	}
};
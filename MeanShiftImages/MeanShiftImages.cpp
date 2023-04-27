#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <windows.h>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

#include "meanshift.h"

const cv::String INPUT_FOLDER = ".\\input\\";
const cv::String OUTPUT_FOLDER = ".\\output\\";
const cv::String OUTPUT_FORMAT = ".tif";
std::vector<std::string> fileNames;

void ShowConsoleCursor(bool showFlag);
std::vector<cv::Mat> readImages();
void writeSummary(std::chrono::milliseconds ms);

int main(void) {
	ShowConsoleCursor(false);
	std::vector<cv::Mat> images = readImages();
	auto start = std::chrono::high_resolution_clock::now();

	// Reset output folder
	std::experimental::filesystem::remove_all(OUTPUT_FOLDER.c_str());
	std::experimental::filesystem::create_directories(OUTPUT_FOLDER.c_str());

	// All parameter combinations to test (one of each for singular tests)
	std::vector<double> bands{ 5.0 };
	std::vector<double> compr{ 6.0 };
	std::vector<double> thrsh{ 100.0 };
	//std::vector<double> bands{ 1.0, 2.0, 3.0, 4.0, 5.0 };
	//std::vector<double> compr{ 4.0, 6.0, 8.0, 10.0, 12.0 };
	//std::vector<double> thrsh{ 90.0, 95.0, 100.0, 105.0, 110.0 };

	int process = 0;
	int total = bands.size() * compr.size() * thrsh.size();
	for (const double& b : bands) {
		for (const double& c : compr) {
			for (const double& t : thrsh) {
				// Tree structured folder names with 1 decimal value
				std::string combination = "BANDW=" + std::to_string(b).substr(0, std::to_string(b).find_last_of(".") + 2) + "\\"
					+ "COMPR=" + std::to_string(c).substr(0, std::to_string(c).find_last_of(".") + 2) + "\\"
					+ "THRSH=" + std::to_string(t).substr(0, std::to_string(t).find_last_of(".") + 2) + "\\";
				std::cout << "Running " << combination << " (" << ++process << "/" << total << "):\n";

				std::string SUBFOLDER = OUTPUT_FOLDER + "\\" + combination;
				std::experimental::filesystem::create_directories(SUBFOLDER.c_str());
				
				for (int i = 0; i < images.size(); ++i) {
					MeanShift msc(images[i]);
					std::string s = "Image " + std::to_string(i + 1) + "/" + std::to_string(images.size()) + ": ";
					msc.setProgressString(s);
					msc.clearProgress();
					msc.setParameters(b, c, t);
					msc.meanShift();
					cv::imwrite(SUBFOLDER + fileNames[i] + "" + OUTPUT_FORMAT, msc.output_image);
					cv::imwrite(SUBFOLDER + fileNames[i] + "_binary" + OUTPUT_FORMAT, msc.output_binary);
				}
				std::cout << "\n";
			}
		}
	}

	auto stop = std::chrono::high_resolution_clock::now();
	writeSummary(std::chrono::duration_cast<std::chrono::milliseconds>(stop - start));
	
	std::cout << "\nProcess Finished\n";
	system("pause");
	return 0;
}

std::vector<cv::Mat> readImages() {
	std::vector<cv::String> names;
	cv::glob(INPUT_FOLDER, names, false);

	std::vector<cv::Mat> images;
	for (const cv::String name : names) {
		cv::Mat img = cv::imread(name);
		if (img.empty()) continue;	// Not valid
		images.push_back(img);

		// Strip and save file name
		std::string fileName = name;
		fileName = fileName.substr(INPUT_FOLDER.length(), 
			fileName.find_last_of(".") - INPUT_FOLDER.length());
		fileNames.push_back(fileName);
	}

	return images;
}

void writeSummary(std::chrono::milliseconds ms) {
	std::ofstream outputfile;
	outputfile.open(OUTPUT_FOLDER + "summary.txt");
	outputfile << std::fixed << std::setprecision(1);
	outputfile << "Time elapsed: " << (ms.count() / 1000.0) << " s\n";
}

//int WinMain(void) {
//	main();
//}

// Hide console cursor, credit: https://stackoverflow.com/a/18028927
void ShowConsoleCursor(bool showFlag) {
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(out, &cursorInfo);
	cursorInfo.bVisible = showFlag; // set the cursor visibility
	SetConsoleCursorInfo(out, &cursorInfo);
}
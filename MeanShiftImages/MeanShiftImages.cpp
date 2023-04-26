#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <iomanip>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

#include "meanshift.h"

const cv::String INPUT_FOLDER = ".\\input\\";
const cv::String OUTPUT_FOLDER = ".\\output\\";
const cv::String OUTPUT_FORMAT = ".tif";
std::vector<std::string> fileNames;

std::vector<cv::Mat> readImages();
void writeOutput(const std::vector<int>& counts, std::chrono::milliseconds ms);

/**
* bandwidth 1, 3, 5, 7
* threshold 90, 95, 100, 105, 110
* compression 4, 6, 8, 10, 12
*/

int main(void) {
	std::vector<cv::Mat> images = readImages();
	auto start = std::chrono::high_resolution_clock::now();

	// Reset output folder
	std::experimental::filesystem::remove_all(OUTPUT_FOLDER.c_str());
	std::experimental::filesystem::create_directories(OUTPUT_FOLDER.c_str());

	std::vector<int> bandwidths = { 1, 3, 5};
	std::vector<int> threshold = { 90, 95, 100, 105, 110 };
	std::vector<int> compression = { 4, 6, 8, 10, 12 };

	const std::vector<std::string> bs = { "bs1", "bs3", "bs5", "7", "9" };
	const std::vector<std::string> ts = { "ts90", "ts95", "ts100", "ts105", "ts110" };
	const std::vector<std::string> cs = { "cs4", "cs6", "cs8", "cs10", "cs12" };

	std::vector<int> counts(images.size(), -1);
	cv::String FILE;

	for (int a = 0; a < 3; a++)
	{
		for (int b = 0; b < 5; b++)
		{
			for (int c = 0; c < 5; c++)
			{
			    //OUTPUT_SUB_FOLDER = ("b" + bandwidths[a] + "t" + threshold[b] + "c:" + compression[c]);
				//std::cout << OUTPUT_SUB_FOLDER;
				//std::experimental::filesystem::create_directories(OUTPUT_SUB_FOLDER.c_str());
				printf("%d%d%d\n", a, b, c);
				
				for (int i = 0; i < images.size(); ++i) {
					MeanShift msc(images[i], bandwidths[a], compression[c], threshold[b] / compression[c]);
					counts[i] = msc.meanShift();
					//cv::imwrite(OUTPUT_FOLDER + fileNames[i] + "" + OUTPUT_FORMAT, msc.output_image);
					cv::imwrite(OUTPUT_FOLDER + bs[a] + ts[b] + cs[c] + fileNames[i] + OUTPUT_FORMAT, msc.output_binary);
				}
			}
		}
	}

	printf("stopped");


	auto stop = std::chrono::high_resolution_clock::now();
	writeOutput(counts, std::chrono::duration_cast<std::chrono::milliseconds>(stop - start));
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

void writeOutput(const std::vector<int>& counts, std::chrono::milliseconds ms) {
	// Write output summary
	std::ofstream outputfile;
	outputfile.open(OUTPUT_FOLDER + "summary.txt");
	for (int i = 0; i < counts.size(); ++i) {
		outputfile << "output" << (i + 1) << " count: " << counts[i] << "\n";
	}
	outputfile << std::fixed << std::setprecision(1);
	outputfile << "Time elapsed: " << (ms.count() / 1000.0) << " s\n";

	outputfile << "\n--- Settings ---\n";
	//outputfile << "Bandwidth: " << BANDWIDTH << "\n";
	//outputfile << "Color Compression: " << COLOR_COMPRESSION << "\n";
}

int WinMain(void) {
	main();
}
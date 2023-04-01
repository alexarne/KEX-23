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

const cv::String INPUT_FOLDER = "./input/";
const cv::String OUTPUT_FOLDER = "./output/";
const cv::String OUTPUT_FORMAT = ".png";

std::vector<cv::Mat> readImages();
void writeOutput(const std::vector<cv::Mat>& images, const std::vector<int>& counts, std::chrono::milliseconds ms);

int main(void) {
	std::vector<cv::Mat> images = readImages();
	auto start = std::chrono::high_resolution_clock::now();

	std::vector<int> counts(images.size(), -1);
	for (int i = 0; i < images.size(); ++i) {
		MeanShift msc(images[i]);
		counts[i] = msc.meanShift();
	}

	auto stop = std::chrono::high_resolution_clock::now();
	writeOutput(images, counts, std::chrono::duration_cast<std::chrono::milliseconds>(stop - start));
	return 0;
}

std::vector<cv::Mat> readImages() {
	std::vector<cv::String> names;
	cv::glob(INPUT_FOLDER, names, false);

	std::vector<cv::Mat> images;
	for (int i = 0; i < names.size(); ++i) {
		cv::Mat img = cv::imread(names[i]);
		if (img.empty()) continue;	// Not valid
		images.push_back(img);
	}

	return images;
}

void writeOutput(const std::vector<cv::Mat>& images, const std::vector<int>& counts, std::chrono::milliseconds ms) {
	// Reset output folder
	std::experimental::filesystem::remove_all(OUTPUT_FOLDER.c_str());
	std::experimental::filesystem::create_directories(OUTPUT_FOLDER.c_str());

	// Write output images
	for (int i = 0; i < images.size(); ++i) {
		bool status = cv::imwrite(OUTPUT_FOLDER + "output" + std::to_string(i + 1) + OUTPUT_FORMAT, images[i]);
		if (status == false) {
			std::cout << "Failed writing image 'output" << std::to_string(i + 1) << OUTPUT_FORMAT << "'\n";
		}
	}

	// Write output summary
	std::ofstream outputfile;
	outputfile.open(OUTPUT_FOLDER + "summary.txt");
	for (int i = 0; i < counts.size(); ++i) {
		outputfile << "output" << (i + 1) << OUTPUT_FORMAT << " count: " << counts[i] << "\n";
	}
	outputfile << std::fixed << std::setprecision(1);
	outputfile << "Time elapsed: " << (ms.count() / 1000.0) << " s\n";
}

int WinMain(void) {
	main();
}
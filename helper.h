#ifndef _CSCI441_HELPER_H_
#define _CSCI441_HELPER_H_

#include <iostream>
#include <string>
#include <vector>
#include <regex>

int chooseString(std::vector<std::string> choices) {
	std::cout << "Please choose:" << std::endl;
	for (int i = 0; i < choices.size(); i++) {
		std::cout << i + 1 << ") " << choices[i] << std::endl;
	}
	std::string input_s;
	int input = 0;
	while (input == 0) {
		std::cout << "Enter your choice> ";
		std::cin >> input_s;
		try {
			input = std::stoi(input_s);
		}
		catch (std::invalid_argument e) { }
		if (input < 1 || input > choices.size()) {
			std::cout << "Please enter a valid selection" << std::endl;
			input = 0;
		}
	}
	return input;
}

bool checkFileExtension(std::string filename, std::string ext) {
	ext = ext.substr(ext.length() - 3);
	filename = filename.substr(filename.length() - 3);
	for (int i = 0; i < 3; i++) {
		filename[i] = std::tolower(filename[i]);
		ext[i] = std::tolower(ext[i]);
	}
	return filename == ext;
}

std::string formatStringifiedFloat(std::string str) {
	if (str.find('.') == std::string::npos) {
		return str;
	}
	while (str[str.length() - 2] != '.') {
		if (str[str.length() - 1] == '0') {
			str = str.substr(0, str.length() - 1);
		}
		else {
			return str;
		}
	}
	return str;
}

void progressBar(std::string header, float* p1, float* p2, int i, int iMax) {
	*p1 = ceil((float)i * 100 / (float)iMax);
	if (*p1 != *p2) {
		*p2 = *p1;
		if (*p1 == 1) {
			std::cout << "--[ " << header << " ]--" << std::endl;
		}
		std::cout << "-" << *p1 << "-";
		if (*p1 == 100) {
			std::cout << std::endl;
		}
	}
}

//https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
std::vector<int> stringSplitI(std::string str, char delim) {
	size_t pos = 0;
	std::vector<int> result;
	while ((pos = str.find(delim)) != std::string::npos) {
		result.push_back(std::stoi(str.substr(0, pos)));
		str.erase(0, pos + 1);
	}
	return result;
}

float mean(std::vector<float> arr) {
	float sum = 0;
	for (int i = 0; i < arr.size(); i++) {
		sum += arr[i];
	}
	return sum / arr.size();
}

float stdDev(std::vector<float> arr, float avg) {
	for (int i = 0; i < arr.size(); i++) {
		arr[i] = pow(arr[i] - avg, 2);
	}
	avg = mean(arr);
	return sqrt(avg);
}

int clamp(int x, int min, int max) {
	if (x < min) {
		return min;
	}
	else if (x > max) {
		return max;
	}
	return x;
}

int maxValue(std::vector<int> arr) {
	int max = INT_MIN;
	for (int i = 0; i < arr.size(); i++) {
		if (arr[i] > max) {
			max = arr[i];
		}
	}
	return max;
}

int similarityScore(int a, int b, int penalty) {
	if (a == b) {
		return 1;
	}
	return penalty;
}

bool isDirectoryFormat(std::string dir) {
	std::regex reg("\D:\\\\([a-zA-Z0-9 ]+\\\\)+$");
	return std::regex_match(dir, reg);
}

float sinc(float x) {
	float M_PI = 3.14159;
	if (x == 0) {
		return 1;
	}
	return sin(M_PI * x) / (M_PI * x);
}

float lanczosKernel(float x, float a) {
	if (x == 0) {
		return 1;
	}
	else if (-a < x && x < a) {
		return sinc(x) * sinc(x / a);
	}
	else {
		return 0;
	}
}

#endif

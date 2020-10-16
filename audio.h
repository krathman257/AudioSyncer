#ifndef _CSCI441_AUDIO_H_
#define _CSCI441_AUIDO_H_

#include <sndfile.h>
#include "helper.h"
#include "bitmap_image.hpp"

class Audio {
private:
	std::string name;
	SNDFILE* file;
	SF_INFO info;
	std::vector<int> peaks;
	bool hasWaveform;
	float* waveform;
	int n;
	//Info
	int channels, format, sampleRate, sections, seekable;
	//Settings
	int lag;
	float threshold;
	float influence;
public:
	//New file
	Audio(std::string fp, int l, float t, float i, int s) {
		if (!checkFileExtension(fp, "wav")) {
			fp += ".wav";
		}
		//Load file
		memset(&info, 0, sizeof(info));
		file = sf_open(fp.c_str(), SFM_READ, &info);

		if (sf_error(file) > 0) {
			std::cout << "ERROR " << sf_error(file) << ": " << sf_error_number(sf_error(file)) << std::endl;
			initializeEmpty();
		}
		else {
			//Get title
			const char* title = sf_get_string(file, SF_STR_TITLE);
			if (title) { name = title; }
			else { name = "NO TITLE"; }

			//Get waveform
			sf_seek(file, 0, SEEK_SET);
			n = info.frames * info.channels;
			waveform = new float[n];
			sf_read_float(file, waveform, n);
			hasWaveform = true;

			lag = l;
			threshold = t;
			influence = i;

			channels = info.channels;
			format = info.format;
			sampleRate = info.samplerate;
			sections = info.sections;
			seekable = info.seekable;

			if (s != sampleRate) {
				resample(s, 3);
				sampleRate = s;
			}

			//Get peaks
			peaks = detectPeaks();

			
		}
	}

	//Import
	Audio(std::string fp) {
		if (!checkFileExtension(fp, "asr")) {
			fp += ".asr";
		}
		std::ifstream importFile(fp);
		if (!importFile.is_open()) {
			std::cout << "Could not open ASR file: " << fp << std::endl;
			initializeEmpty();
		}
		else {
			std::string line;
			std::vector<std::string> importedLines;
			while (std::getline(importFile, line)) {
				importedLines.push_back(line);
			}
			name = importedLines[0];
			n = std::stoi(importedLines[1]);
			lag = std::stoi(importedLines[2]);
			threshold = std::stod(importedLines[3]);
			influence = std::stod(importedLines[4]);
			channels = std::stoi(importedLines[5]);
			format = std::stoi(importedLines[6]);
			sampleRate = std::stoi(importedLines[7]);
			sections = std::stoi(importedLines[8]);
			seekable = std::stoi(importedLines[9]);

			hasWaveform = false;

			std::cout << name << ": Importing peaks..." << std::endl;
			peaks = stringSplitI(importedLines[10], ',');

			importFile.close();
		}
	}

	//Empty
	Audio() {
		initializeEmpty();
	}

	void initializeEmpty() {
		name = "";
		n = 0;
		hasWaveform = false;
		waveform = new float[0];
		peaks = std::vector<int>();
		lag = threshold = influence = 0;
		channels = format = sampleRate = sections = seekable = 0;
	}

	//https://stackoverflow.com/questions/22583391/peak-signal-detection-in-realtime-timeseries-data
	std::vector<int> detectPeaks() {
		if (!(n >= lag + 2)) {
			std::cout << "Lag (" << lag << " + 2) too large for file (" << n << "), initialization canceled" << std::endl;
			initializeEmpty();
			return std::vector<int>();
		}
		else {
			std::vector<int> signals(n + 1, 0);
			std::vector<float> filteredY(signals.begin(), signals.begin() + lag);
			std::vector<float> avgFilter(lag);
			std::vector<float> stdFilter(lag);
			avgFilter[lag - 1] = mean(filteredY);
			stdFilter[lag - 1] = stdDev(filteredY, mean(filteredY));

			float prog1 = 0, prog2 = 0;
			for (int i = lag; i < n - 1; i++) {
				if (abs(waveform[i] - avgFilter[i - 1]) > threshold * stdFilter[i - 1]) {
					if (waveform[i] > avgFilter[i - 1]) {
						signals[i] = 1;
					}
					else {
						signals[i] = -1;
					}
					filteredY.push_back(influence * waveform[i] + (1 - influence) * filteredY[i - 1]);
				}
				else {
					signals[i] = 0;
					filteredY.push_back(waveform[i]);
				}
				float avg = mean(std::vector<float>(filteredY.begin() + i - lag, filteredY.begin() + i));
				avgFilter.push_back(avg);
				stdFilter.push_back(stdDev(std::vector<float>(filteredY.begin() + i - lag, filteredY.begin() + i), avg));

				progressBar("Detecting Peaks", &prog1, &prog2, i, n);
			}
			return signals;
		}
	}

	std::string SmithWaterman(Audio* song, int penalty) {
		std::vector<int> songPeaks = song->getPeaks();
		if (songPeaks.size() > peaks.size()) {
			return "Song longer than video, aborting";
		}
		int song_s = songPeaks.size();
		int audio_s = peaks.size();
		std::vector<std::vector<int>> matrix(2, std::vector<int>(song_s + 1, 0));
		std::vector<int> traceback(4);
		int m_max = INT_MIN;
		int i_max = 0;

		float prog1 = 0, prog2 = 0;
		for (int i = 1; i <= audio_s; i++) {
			for (int j = 1; j <= song_s; j++) {
				traceback[0] = matrix[0][j - 1] + similarityScore(peaks[i - 1], songPeaks[j - 1], penalty);
				traceback[1] = matrix[0][j] + penalty;
				traceback[2] = matrix[1][j - 1] + penalty;
				traceback[3] = 0;
				matrix[1][j] = maxValue(traceback);

				if (matrix[1][j] > m_max) {
					m_max = matrix[1][j];
					i_max = i;
				}
			}
			matrix[0] = matrix[1];
			for (int j = 0; j <= song_s; j++) {
				matrix[1][j] = 0;
			}
			progressBar("Building Smith-Waterman Matrix", &prog1, &prog2, i, audio_s);
		}
		float seconds = ((float)i_max / (float)channels - (float)song_s / (float)song->getChannels()) / sampleRate;
		int timestamp_m = (int)floor(seconds / 60);
		int timestamp_s = (int)floor(seconds) % 60;
		int timestamp_f = (seconds - floor(seconds)) * 60;
		return std::to_string(timestamp_m) + ":" + std::to_string(timestamp_s) + ((timestamp_f < 10) ? "f0" : "f") + std::to_string(timestamp_f) + 
			" (" + std::to_string((int)floor((float)m_max * 100.0 / (float)song_s)) + "% match)";
	}

	//Print waveform
	void printWaveform(int width, int height, std::string filename, int pointSize, bool peaksPrint) {
		bitmap_image image(width, height);
		rgb_t background_c = make_colour(0, 0, 0);
		rgb_t data_c = make_colour(0, 255, 0);
		rgb_t peaks_c = make_colour(255, 0, 255);
		for (int i = 0; i < width; i++) {
			for (int j = 0; j < height; j++) {
				image.set_pixel(i, j, background_c);
			}
		}
		int x, y;
		if (hasWaveform) {
			for (int i = 0; i < n; i++) {
				x = (float)i * (float)width / (float)n;
				y = waveform[i] * height * 0.49 + height / 2;
				for (int a = x - pointSize; a < x + pointSize; a++) {
					for (int b = y - pointSize; b < y + pointSize; b++) {
						image.set_pixel(clamp(a, 0, width - 1), clamp(b, 0, height - 1), data_c);
					}
				}
			}
		}
		if (peaksPrint) {
			for (int i = 0; i < peaks.size(); i++) {
				if (peaks[i] != 0) {
					x = (float)i * (float)width / (float)peaks.size();
					if(hasWaveform){
						y = waveform[i] * height * 0.49 + height / 2;
					}
					else {
						y = height / 2;
					}
					for (int a = x - pointSize * 3; a < x + pointSize * 3; a++) {
						for (int b = y - pointSize * 3; b < y + pointSize * 3; b++) {
							image.set_pixel(clamp(a, 0, width - 1), clamp(b, 0, height - 1), peaks_c);
						}
					}
				}

			}
		}
		if (!checkFileExtension(filename, "png")) {
			filename += ".png";
		}
		image.save_image(filename);
		std::cout << "Waveform Saved" << std::endl;
	}

	void resample(int target_r, float a) {
		int newWaveformLength = ceil((float)n / (float)sampleRate * (float)target_r);
		float* result = new float[newWaveformLength];
		for (int i = 0; i < newWaveformLength; i++) {
			float x = ((float)i / (float)newWaveformLength) * (float)n;
			float sum = 0;
			for (int j = floor(x)-a + 1; j < floor(x)+a; j++) {
				if (j < n && j > 0) {
					sum += waveform[j] * lanczosKernel(x - j, a);
				}
			}
			result[i] = sum;
		}
		n = newWaveformLength;
		delete waveform;
		waveform = result;
	}

	void printInfo() {
		if (sf_error(file) > 0) {
			std::cout << "ERROR " << sf_error(file) << ": " << sf_error_number(sf_error(file)) << std::endl;
		}
		else {
			std::cout << "TITLE: " << name << std::endl;
		}
		std::cout << "CHANNELS: " << channels << std::endl;
		std::cout << "FORMAT: " << format << std::endl;
		std::cout << "FRAMES: " << info.frames << std::endl;
		std::cout << "SAMPLE RATE: " << sampleRate << std::endl;
		std::cout << "SECTIONS: " << sections << std::endl;
		std::cout << "SEEKABLE: " << seekable << std::endl;
	}

	void exportAudio(std::string fileName) {
		if (!checkFileExtension(fileName, "asr")) {
			fileName += ".asr";
		}
		std::ofstream exportFile;
		exportFile.open(fileName);
		exportFile << name << std::endl
			<< n << std::endl
			<< lag << std::endl
			<< threshold << std::endl
			<< influence << std::endl
			<< channels << std::endl
			<< format << std::endl
			<< sampleRate << std::endl
			<< sections << std::endl
			<< seekable << std::endl;
		for (int i = 0; i < peaks.size(); i++) {
			exportFile << peaks[i] << ",";
		}
		exportFile << std::endl;
		exportFile.close();
		std::cout << "Exported: " << fileName << std::endl;
	}

	std::string menuPrint() {
		return name + " (" +
			std::to_string(lag) + "/" +
			formatStringifiedFloat(std::to_string(threshold)) + "/" +
			formatStringifiedFloat(std::to_string(influence)) + ")" +
			" Sample Rate: " + std::to_string(sampleRate);
	}

	std::vector<int> getPeaks() {
		return peaks;
	}

	int getChannels() {
		return channels;
	}

	int getSampleRate() {
		return sampleRate;
	}

	~Audio() {
		sf_close(file);
		delete waveform;
	}
};

#endif

#include "audio.h"

using namespace std;

//const char* filePath = "C:\\Users\\Krathman257\\Videos\\Desktop\\Desktop 2020.09.02 - 19.41.05.01";
string projectPath = "D:\\Krathman\\Desktop\\Projects\\AudioSyncer\\";
string audioStoragePath = "AudioStorage\\";
string waveformExportPath = "Waveforms\\";

vector<string> choices_mm;
vector<string> choices_cs;
Audio* video = new Audio();
Audio* song = new Audio();
Audio* selection = video;

//Default settings
int lag = 100;
float threshold = 2;
float influence = 0.98;
int targetSR = 100;

int height = 1000;
int width = 2000;
int pointSize = 1;
bool peaksPrint = true;

int mainMenuChoice();
int changeSettings();

int mainMenuChoice() {
	cout << "***************\nAudioSyncer by krathman257" << endl;
	cout << "Lag / Threshold / Influence / Target Sample Rate: " << lag << " / " << threshold << " / " << influence << " / " << targetSR << endl;
	if (selection == video) {
		cout << "* Loaded Video: " << video->menuPrint() << endl;
		cout << "  Loaded Song:  " << song->menuPrint() << endl;
	}
	else if (selection == song) {
		cout << "  Loaded Video: " << video->menuPrint() << endl;
		cout << "* Loaded Song:  " << song->menuPrint() << endl;
	}
	else {
		cout << "FATAL ERROR: selection pointer out of bounds. Exiting program." << endl;
		exit(EXIT_FAILURE);
	}

	string temp;
	switch (chooseString(choices_mm)) {
	//Switch Selection
	case 1:
		if (selection == video) {
			selection = song;
		}
		else {
			selection = video;
		}
		return 0;
	//Open New
	case 2:
		cout << "Enter the name of the .WAV file> ";
		cin >> temp;
		if (selection == video) {
			video = new Audio(projectPath + audioStoragePath + temp, lag, threshold, influence, targetSR);
			selection = video;
		}
		else if (selection == song) {
			song = new Audio(projectPath + audioStoragePath + temp, lag, threshold, influence, targetSR);
			selection = song;
		}
		else {
			cout << "Import failed: selection incorrectly assigned" << endl;
		}
		return 0;
	//Import
	case 3:
		
		cout << "Enter the name of the .ASR file> ";
		cin >> temp;
		if (selection == video) {
			video = new Audio(projectPath + audioStoragePath + temp);
			selection = video;
		}
		else if (selection == song) {
			song = new Audio(projectPath + audioStoragePath + temp);
			selection = song;
		}
		else {
			cout << "Import failed: selection incorrectly assigned" << endl;
		}
		return 0;
	//Export
	case 4:
		cout << "Enter the name of the export file> ";
		cin >> temp;
		selection->exportAudio(projectPath + audioStoragePath + temp);
		return 0;
	//Print Waveform
	case 5:
		cout << "Enter the name of the waveform image file> ";
		cin >> temp;
		selection->printWaveform(width, height, projectPath + waveformExportPath + temp, pointSize, peaksPrint);
		return 0;
	//Sync
	case 6:
		if (video->getSampleRate() != song->getSampleRate() || video->getSampleRate() == 0) {
			cout << "SYNC FAILED: Both audio files must be loaded and have identical sample rates" << endl;
		}
		else {
			cout << "Sync completed:\n" + video->SmithWaterman(song, -1) + "\n";
		}
		return 0;
	//Change Settings
	case 7:
		changeSettings();
		return 0;
	//Quit
	case 8:
		return 1;
	default:
		return 0;
	}
}

int changeSettings() {
	cout << "***************\nCurrent Settings:\nLag: " << lag << "  Threshold: " << threshold << endl;
	cout << "Influence: " << influence << "  Target Sample Rate: " << targetSR << endl;
	cout << "Image Height: " << height << "  Image Width: " << width << endl;
	cout << "Point Size: " << pointSize << "  Denote Peaks: " << ((peaksPrint==0)?"False":"True") << endl;
	cout << "Project Directory: " << projectPath << endl;
	float temp;
	string temp_s;
	switch (chooseString(choices_cs)) {
	//Lag
	case 1:
		cout << "Please enter a new Lag value> ";
		cin >> temp;
		lag = (int)temp;
		return 0;
	//Threshold
	case 2:
		cout << "Please enter a new Threshold value> ";
		cin >> temp;
		threshold = temp;
		return 0;
	//Influence
	case 3:
		cout << "Please enter a new Influence value> ";
		cin >> temp;
		if (0.0 > temp || 1.0 < temp) {
			cout << "Influence must be between 0.0 and 1.0, no change made" << endl;
		}
		else {
			influence = temp;
		}
		return 0;
	//Lag
	case 4:
		cout << "Please enter a new Target Sample Rate> ";
		cin >> temp;
		targetSR = (int)temp;
		return 0;
	//Height
	case 5:
		cout << "Please enter a new image Height> ";
		cin >> temp;
		height = (int)temp;
		return 0;
	//Width
	case 6:
		cout << "Please enter a new image Width> ";
		cin >> temp;
		width = (int)temp;
		return 0;
	//Point Size
	case 7:
		cout << "Please enter a new image Point Size> ";
		cin >> temp;
		pointSize = (int)temp;
		return 0;
	//Print Peaks
	case 8:
		cout << "Should the printed waveform denote peaks? (No = 0, Yes = 1)> ";
		cin >> temp;
		if ((int)temp == 0) {
			peaksPrint = false;
		}
		else if ((int)temp == 1) {
			peaksPrint = true;
		}
		else {
			cout << "Invalid choice, no change made" << endl;
		}
		return 0;
	default:
		return 0;
	}
}

int main(void) {
	choices_mm.push_back("Switch Selection");
	choices_mm.push_back("Open New");
	choices_mm.push_back("Import");
	choices_mm.push_back("Export");
	choices_mm.push_back("Print Waveform");
	choices_mm.push_back("Sync");
	choices_mm.push_back("Change Settings");
	choices_mm.push_back("Quit");

	choices_cs.push_back("Lag");
	choices_cs.push_back("Threshold");
	choices_cs.push_back("Influence");
	choices_cs.push_back("Target Sample Rate");
	choices_cs.push_back("Waveform - Height");
	choices_cs.push_back("Waveform - Width");
	choices_cs.push_back("Waveform - Point Size");
	choices_cs.push_back("Waveform - Print Peaks");
	choices_cs.push_back("Return To Main Menu");

	while (mainMenuChoice() != 1);

	delete video;
	delete song;
}

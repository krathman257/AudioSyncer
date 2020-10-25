## Basic Running Instructions
There are better ways to compile C++ programs on Windows (a simple Cmake configuration, for instance), but these instructions are for compiling and running the program within the Microsoft Visual Studio environment.

# Creating the project

Start a new C++ project in Visual Studio. No files are needed to begin with. In the Solution Explorer to the far right, right click on the directory labeled Source Files. From there, select Add -> Existing Item... to browse for the three main C++ files (main, audio, and helper). Strictly speaking, audio and helper can be placed in the Header Files directory instead of Source Files, but as far as I know the difference is on of organization.

# Including Libraries

Many of the function require use of external libraries. To include these, select Project from the toolbar at the top, and select ProjectName Properties (making sure one of the three main files is the selected window). From the Property Pages, select VC++ Directories -> Include Directories. Click with the arrow on the right, click Edit... to add any directories to include when compiling the program.

Make sure the directories you choose directly include the library files. In this case, I believe you'll want to include [PATH]/bitmap/include/bitmap and [PATH]/libsndfile/include.

# Running

Once the error messages are gone, press the Local Windows Debugger at the top of the window (with the green arrow). This will compile the program and generate an .EXE that will run automatically.

## The Program Itself

The program is far from finished, and until it is, there are certain things that should be minded when running it. Most notably, the projectPath variable (line 6) specifies a directory on MY computer. Naturally, this won't find anything properly on anyone ELSE'S computer, but that's a feature to add later. For now, change line 6 to a directory on your own computer (in the same format, being "DRIVE:\\Folder\\Other Folder\\").

Similarly, the chosen projectPath must contain two folders, called AudioStorage and Waveforms. AudioStorage is where the program searches for .WAV files to import, .ASR (a custom extension) files to import, and where it exports new .ASR files. Waveforms is where any visual waveforms are printed as .BMP files.

To my knowledge, there are no fatal errors that result in crashes, though the wrong input to the menus might result in no action taking place. I believe I've plugged any memory leaks, but some may still exist, and Sync runtime can take a while. The printing progress bar should give you some clue as to how long each process should take.

# Usage

I recommend the free audio editing software Audicity to convert files to .WAV. Ideally, this program will do that automatically in the future. Tutorials can be found online for converting .MP3 audio files to .WAV, as well as ripping a .WAV out of a .MP4 video file.

Set the settings you want the audio files to be imported at. Sample rate should be the same, but Lag, Threshold and Influence can vary.

Switch the selection to change which file to operate on. The Song file should be shorter than the Video file.

Print Waveforms to visualize how well the peaks of the waveform were detected.

Sync the files. This can take a while depending on sample rate. Once synced, the program will print a timestamp within the video. Ideally, the song should be started at this timestamp for an optimal sync (minutes : seconds f frames, assuming 60 frames per second). The audio file can be overlayed on top of the video file using an external video editing software (I used the paid program Adobe After Effects).

# Settings

Lag -- How many datapoints before the current are used to measure the peak. More doesn't necessarily mean better.

Threshold -- Standard Deviation threshold that a datapoint must cross to be considered a peak. Higher threshold means more defined peaks.

Influence -- Influence of current peak on calculations. Should be between 0 and 1.

Target Sample Rate -- If the imported .WAV isn't the target sample rate, it will be resampled. No affect on imported .ASR files. When Syncing, the two waveforms should have the same sample rate. Lower sample rate means fewer datapoints to process means faster runtime. For reference, music is typically sampled at greater than 44,000 samples per second; the default setting to resample to is 100 samples per second.

Image Height / Width -- Dimensions of printed waveform .BMP files.

Point Size -- Size of each datapoint in printed waveform

Denote Peaks -- Whether or not detected peaks are shown in the printed waveform.

# Known Issues

Long audio files can stop detecting peaks after decent amount of time. Likely a float equality issue.

Various functions out-of-date or no longer used. Should clean up / comment.
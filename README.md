QS
==

QtScoerur GUI
This Qt project is the GUI of Scoreur project, which is meant to implement the whole process from audio input to score output.
Programs of algorithm group runs under this GUI. Other basic functions of common audio edit programs may be included as well.

### Functions of current version
---
1. Showing wavforms of .wav or .mp3 file along with the spectrum in the WavView;
2. Recording and playing audio;
3. Typesetting the scores according to formatted input (pitch & duration);
4. Simulating the piano keyboard in KeyScene, allowing input the notes from computer keyboard;
5. Provide score to wav function, with simple overtones and filtering;
6. Provide onset detection and svm prediction of audio source, together with rhythm quantizing;
... ...


### Structure of src files
---
Format: File names all in lower cases, each with two files (.cpp and .h).
1.General - This folder includes src files that define the main windows and base classes of graphics views and scenes.<br />
`qswindow`: Define main window, include wavTab, scoreTab, staffTab, webTab which presents associated views and scenes;<br />
`qsview`: Define base class of WavView, ScoreView and StaffView;<br />
`qsscene`: Define KeyScene and base class of WavScene, ScoreScene, StaffScene;<br />
`qsplayer`: For playing the audio file in another thread;<br />
`qsrecorder`: For recording audio input from build-in microphone;<br />
`pianokey`: Define graphics item for single key, which can be controlled by the computer keyboard to input notes to the ScoreView.<br />

2.Core - This folder bridge the GUI to the programs of algorithm group by importing python modules or calling external process;<br />
`spectrum`: Implement spectrum analysis of wave file, provide multiple transforms like FFT, STFT, DCT, CQT, etc;<br />
`pywrap`: Wrap the python module in the subfolder `python2.7` and `py`;<br />
`svm`:  C++ source of libsvm, for pitch prediction;<br />
`detector`: Provide basic functions for onset detecting and beats quantizing.<br />

3.Wave - This folder includes src files for WavView and audio processing.<br />
`wavfile`: Handle I/O operations of wave files, provide basic transformation between .wav and other formats;<br />
`wavscene`: Define WavScene for displaying and wavforms and spectrum graphs;<br />
`wavframe`: For displaying wavforms of limited size, updated when the view move forward;<br />
`wavChannel`: For placing wavframes, surport both single channel and double channels; <br />
`spectrumgraph`: For drawing spectrums in a logarithmic scale, updated when the view move forward;<br />
`lame.h`: header file of libmp3lame, for mp3 codec.<br />

4.Score - This folder includes src files for simple score typesetting and editing.<br />
 `scorescene`: Define ScoreScene for displaying/editing simple scores, able to arrange the notes according to formatted input;<br />
 `scoreitem`: Define graphics item for single notes in simple scores.<br />

5.Staff - This folder includes src files for staff typesetting.<br />
`staffscene`: Define StaffScene for displaying staves, in a similar structure as ScoreScene, which has not been finished<br />
(a web version supported by js backend is available on webpage of Scoreur Project);<br />
`staffitem`: Define graphics item for single notes in staves.<br />

6.html5applicationviewer: This folder is a subproject of Qt Html5, displaying webView and handling the connections with the server of Scoreur Project.<br />

7.`qspreset`: This class handling the settings and all preset parameters of the programs which can be modified by the user.<br />

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
> 
... ...


### Structure of src files
---
Format: File names all in lower cases, each with two files (.cpp and .h).<br />
1.General - This folder includes src files that define the main windows and base classes of graphics views and scenes.
> 
`qswindow`: Define main window, include wavTab, scoreTab, staffTab, webTab which presents associated views and scenes;
> 
`qsview`: Define base class of WavView, ScoreView and StaffView;
> 
`qsscene`: Define KeyScene and base class of WavScene, ScoreScene, StaffScene;
> 
`qsplayer`: For playing the audio file in another thread;
> 
`qsrecorder`: For recording audio input from build-in microphone;
> 
`pianokey`: Define graphics item for single key, which can be controlled by the computer keyboard to input notes to the ScoreView.

2.Core - This folder bridge the GUI to the programs of algorithm group by importing python modules or calling external process.
> 
`spectrum`: Implement spectrum analysis of wave file, provide multiple transforms like FFT, STFT, DCT, CQT, etc;
> 
`pywrap`: Wrap the python module in the subfolder `python2.7` and `py`;
`svm`:  C++ source of libsvm, for pitch prediction;
> 
`detector`: Provide basic functions for onset detecting and beats quantizing.

3.Wave - This folder includes src files for WavView and audio processing.
> 
`wavfile`: Handle I/O operations of wave files, provide basic transformation between .wav and other formats;
> 
`wavscene`: Define WavScene for displaying and wavforms and spectrum graphs;
> 
`wavframe`: For displaying wavforms of limited size, updated when the view move forward;
> 
`wavChannel`: For placing wavframes, surport both single channel and double channels; 
> 
`spectrumgraph`: For drawing spectrums in a logarithmic scale, updated when the view move forward;
> 
`lame.h`: header file of libmp3lame, for mp3 codec.

4.Score - This folder includes src files for simple score typesetting and editing.
> 
 `scorescene`: Define ScoreScene for displaying/editing simple scores, able to arrange the notes according to formatted input;
> 
 `scoreitem`: Define graphics item for single notes in simple scores.

5.Staff - This folder includes src files for staff typesetting.
> 
`staffscene`: Define StaffScene for displaying staves, in a similar structure as ScoreScene, which has not been finished
(a web version supported by js backend is available on webpage of Scoreur Project);
> 
`staffitem`: Define graphics item for single notes in staves.

6.html5applicationviewer: This folder is a subproject of Qt Html5, displaying webView and handling the connections with the server of Scoreur Project.

7.`qspreset`: This class handling the settings and all preset parameters of the programs which can be modified by the user.

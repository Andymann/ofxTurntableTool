#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "ofxMidi.h"
#include "ofxXmlSettings.h"
#include "ofxAudioFile.h"


class ofApp : public ofBaseApp, public ofxMidiListener{
	public:
		void setup() override;
		void update() override;
		void draw() override;
		void exit() override;

		void keyPressed(int key) override;
		void keyReleased(int key) override;
		void mouseMoved(int x, int y ) override;
		void mouseDragged(int x, int y, int button) override;
		void mousePressed(int x, int y, int button) override;
		void mouseReleased(int x, int y, int button) override;
		void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
		void mouseEntered(int x, int y) override;
		void mouseExited(int x, int y) override;
		void windowResized(int w, int h) override;
		void dragEvent(ofDragInfo dragInfo) override;
		void gotMessage(ofMessage msg) override;

	void processMidi_ControlChange(ofxMidiMessage& message);
	void processMidi_NoteOn(ofxMidiMessage& message);
	void processMidi_NoteOff(ofxMidiMessage& message);
	
		
	ofTrueTypeFont font;

	const string TITLE="Andyland Tool";
	const string VERSION="0.1";
	bool bShowGui = true;
	const string LBL_NONE = "[none]";
	const string LBL_CMD_MIDI_IN = "Midi In: ";

	const float JOGRESOLUTION = 256.00;
	const float MAXFADERPITCH = 0.08;
	const float PLATTERSPEED = 33.33;

	vector<string>logText;
	void addLog(string p);
    void showLog();
	void clearLog();

	ofxXmlSettings xmlSettings;
    void saveSettings();
	void restoreSettings();



	ofxMidiIn midiIn;
	void newMidiMessage(ofxMidiMessage& eventArgs);
	void setMidiPort(string pPortName);
	void onDropdownEvent(ofxDatGuiDropdownEvent e);
	void buildGui();

	ofxDatGui* gui;
	ofxDatGuiButton* btnClear;
	ofxDatGuiDropdown* cmbMidiIn;

	double pitch;
	double faderpitch;	//einmal fader, einmal scratchen
	float platterDegree;
	float markerPositionOffset;
	float scratchOffset;
	bool bReversePlay = false;
	bool bJogwheelTouch;
	float updateInterval;
	long lastTimestamp;
	double realPitch; //ganz am ende, nach scratching, nudging etc.
	//bool bNewJogWheelTick;

	ofxAudioFile audiofile;
	double playhead;
	double playhead_old;
	std::atomic<double> playheadControl;
	double step;
	double sampleRate;


	void audioOut(ofSoundBuffer & buffer);
	void play(ofSoundBuffer & buffer, int i);

	void drawVinyl(float x, float y, float scale);

	void setCuePoint(uint8_t pCueID, int pMS);
	void recallCuePoint(uint8_t pCueID);

};

class myCustomTheme : public ofxDatGuiTheme{
public:
    myCustomTheme(){
        font.size = 12;
        //font.file = "path/to/font.ttf";
        init();
    }
};
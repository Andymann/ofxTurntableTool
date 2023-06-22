#include "ofApp.h"

static const ofColor cyanPrint = ofColor::fromHex(0x00abec);
static const ofColor magentaPrint = ofColor::fromHex(0xec008c);
static const ofColor yellowPrint = ofColor::fromHex(0xffee00);
static const int maxData = 512;

uint64_t xTime;
uint64_t xTimeBuff;


//--------------------------------------------------------------
void ofApp::setup(){
    //ofSetEscapeQuitsApp(false);
    ofSetWindowTitle(TITLE + " v" + VERSION);
    ofSetBackgroundColor(0, 0, 0);
    ofSetVerticalSync(true);
    //ofSetFrameRate(30);
    buildGui();
    
    restoreSettings();

    std::string filepath = ofToDataPath("Track1.mp3"); // mono(?) original file
    
    audiofile.setVerbose(true); 
    ofSetLogLevel(OF_LOG_VERBOSE);

    if( ofFile::doesFileExist( filepath ) ){
        audiofile.load( filepath );
        if (!audiofile.loaded()){
            ofLogError()<<"error loading file, double check the file path";
        }
        addLog("Audiofile:" + filepath + " loaded sucecessfully" );
        addLog("Audiofile.length():" + ofToString( audiofile.length() ));
        addLog("Audiofile channels:" + ofToString( audiofile.channels() ));
        addLog("Audiofile samplerate:" + ofToString( audiofile.samplerate() ));
        
    }else{
        ofLogError()<<"input file does not exists";
    }
    
    // audio setup for testing audio file stream 
	ofSoundStreamSettings settings;
    sampleRate = 44100.0;
    settings.setOutListener(this);
	settings.sampleRate = sampleRate;
	settings.numOutputChannels = 2;
	settings.numInputChannels = 0;
	settings.bufferSize = 512;
	ofSoundStreamSetup(settings);


    playhead = std::numeric_limits<int>::max(); // because it is converted to int for check
    playheadControl = 0.0;//-1.0;
    
    step = audiofile.samplerate() / sampleRate;
    addLog("step size:" + ofToString(step) );

    //bJogwheelTouch = false;
    //bReversePlay  = false;
    //bNewJogWheelTick = false;

    addLog("playHead:" + ofToString(playhead) );


    pitch = 1.0;
    faderpitch = 1.0;
    platterDegree = 0.0;
    markerPositionOffset = 180.0;
    scratchOffset = 0.0;
    updateInterval = 0.0;
    realPitch = 1.0;

    ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL_BILLBOARD);
	ofSetCircleResolution(50);
	ofSetLineWidth(2);

    lastTimestamp = ofGetElapsedTimeMillis();
}

float lastPlatterDegree;
float platterDegreeDelta;

//--------------------------------------------------------------
void ofApp::update(){
    float factor = pitch * PLATTERSPEED*360.0/60.0/1000.0;

    if(!bJogwheelTouch){
        xTime = ofGetElapsedTimeMillis() - xTimeBuff;
    }else{
        xTimeBuff = ofGetElapsedTimeMillis()-xTime;
    }
    platterDegree = (xTime %(int(360/factor)))*factor + scratchOffset;

    long tTime = ofGetElapsedTimeMillis();

    updateInterval = tTime-lastTimestamp;
    lastTimestamp = tTime;

    platterDegreeDelta = platterDegree - lastPlatterDegree;
    lastPlatterDegree = platterDegree;
    realPitch = (double)(5.0* platterDegreeDelta/updateInterval);

    if(realPitch != 0.){
        //addLog("realPitch:" + ofToString( realPitch));
    }

    // Player: für t= updateInterval wird mit realPitch wiedergegeben
}

//--------------------------------------------------------------
void ofApp::draw(){
    showLog();
    drawVinyl(ofGetWidth()-150,ofGetHeight()-150,128);
    
}

//--------------------------------------------------------------
void ofApp::exit(){
    saveSettings();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key==' '){
        bShowGui=!bShowGui;
        gui->setVisible(bShowGui);
    }

    if(key=='c'){
        clearLog();
    }

    if(key=='1'){
        recallCuePoint(1);
    }
/*
    if(key=='+'){
        playhead +=50000;
    }

    if(key=='r'){
        bReversePlay = !bReversePlay;
    }

    if(key=='f'){
        pitch = 2.0;
    }

    if(key=='s'){
        pitch = 0.5;
    }
*/
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if(key=='f'){
        pitch = 1.0;
    }

    if(key=='s'){
        pitch = 1.0;
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::onDropdownEvent(ofxDatGuiDropdownEvent e)
{
    cout << "onDropdownEvent: " << e.target->getLabel() << " Selected" << endl;
    setMidiPort( e.target->getLabel() );
}

//--------------------------------------------------------------
void ofApp::setMidiPort(string pPortName){
    midiIn.closePort();
    midiIn.removeListener(this);
    
    midiIn.openPort( pPortName ); // open a virtual port
    //midiIn.ignoreTypes(false, // sysex  <-- don't ignore timecode messages!
    //                   false, // timing <-- don't ignore clock messages!
    //                   true); // sensing
    
    // add ofApp as a listener
    midiIn.addListener(this);
    cout << "setMidiPoprt: " << pPortName << " Selected" << endl;
}

//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& message) {
   //addLog("Midi INcoming:" + message.getStatusString(message.status));
   //addLog("Midi INcoming:" + ofToString(message.value));
    switch(message.status) {
        case MIDI_TIME_CLOCK:
            break;
        case MIDI_START: case MIDI_CONTINUE:
            break;
        case MIDI_STOP:
            break;
        case MIDI_NOTE_ON:
            processMidi_NoteOn(message);
            break;
        case MIDI_NOTE_OFF:
            processMidi_NoteOff(message);
            break;
        case MIDI_CONTROL_CHANGE:
            processMidi_ControlChange(message);
            break;
        default:
            break;
    }
}

//--------------------------------------------------------------
void ofApp::buildGui(){
    font.load(OF_TTF_MONO, 11);
    
    gui = new ofxDatGui( ofxDatGuiAnchor::TOP_LEFT );
    vector<string> opts = {midiIn.getInPortList()/*"option - 1", "option - 2", "option - 3", "option - 4"*/};
    cmbMidiIn = gui->addDropdown("Click hier to select MidiPort", opts);
    btnClear = gui->addButton("Click it like Beckham");


    // once the gui has been assembled, register callbacks to listen for component specific events //
    //gui->onSliderEvent(this, &ofApp::onSliderEvent);
    gui->onDropdownEvent(this, &ofApp::onDropdownEvent);

    gui->setTheme(new myCustomTheme() );
    gui->setWidth( ofGetScreenWidth() );
}

//--------------------------------------------------------------
void ofApp::addLog(string p){
    if(logText.size()>17){
        logText.erase( logText.begin() );
    }
    logText.push_back( p );
}

//--------------------------------------------------------------
void ofApp::showLog(){
    string sTmp;
    //string sOscInfo="OSC Format: NoteOn/Channel/Pitch  ControlChange/Channel/Value\n\r";
    //sOscInfo += "OSC In Port:" + ofToString(incomingPortOsc) + "   OSC Out Port:" + ofToString(outGoingPortOsc) + " (see settings.xml)";
    //font.drawString(sOscInfo, 10,btnClear->getY() + 100);
    
    for(int i=0; i<logText.size(); i++){
        sTmp += logText[i] + "\n";
    }
    font.drawString(sTmp, 10,btnClear->getY() + btnClear->getHeight() +25);
}

void ofApp::clearLog(){
    std::fill( std::begin( logText ), std::end( logText ), 0 );
}

void ofApp::restoreSettings(){
    if (xmlSettings.loadFile("settings.xml")) {
        addLog("XML loaded");
    }else{
        addLog("Could not load xml. Reverting to default values.");
    }
    
    string sMidiInPort = xmlSettings.getValue("midiInPort", "");
    if((!sMidiInPort.empty()) && (sMidiInPort.compare(LBL_NONE)!=0) ){
        vector<string> optsMidi_In = {midiIn.getInPortList()};
        optsMidi_In.push_back(LBL_NONE);

        for(int i=0; i<optsMidi_In.size(); i++){
            if(optsMidi_In[i] == sMidiInPort){
                cmbMidiIn->select(i);
                setMidiPort(sMidiInPort);
                cmbMidiIn->setLabel(LBL_CMD_MIDI_IN + sMidiInPort);
                //bMidiIn_Active=true;
                break;
            }
        }
    }else{
        cout << "LoadFromSettings: Midi in disabled";
        cmbMidiIn->setLabel(LBL_CMD_MIDI_IN + sMidiInPort);
        //bMidiIn_Active = false;
    }
}

void ofApp::saveSettings(){
    //ofLogNotice(ofToString( cmbMidiOut->getSelected()->getName() ));
    
    if(!cmbMidiIn->getSelected()->getName().empty())
        xmlSettings.setValue("midiInPort", cmbMidiIn->getSelected()->getName() );
    xmlSettings.save();
}


void ofApp::audioOut(ofSoundBuffer & buffer){
    //addLog("audioOut");
    if( playheadControl >= 0.0 ){
        playhead = playheadControl;
        playheadControl = -1.0;
    }

    for (size_t i = 0; i < buffer.getNumFrames(); i++){
        addLog("playhead: " + ofToString(playhead) );
        //double testHead;
        if( playhead < audiofile.length()-1 ){
            play(buffer, i);
            if(realPitch!=0){
                playhead += step*pitch;//*realPitch;
            }
            //testHead += step*realPitch;
            //addLog("playHead:" + ofToString(playhead) + " pitch:" + ofToString(pitch) + " realPitch:" + ofToString(realPitch) );
            //addLog("playHead:" + ofToString(playhead) + " testhead:" + ofToString(testHead) );
            //addLog("realpitch:" + ofToString(realPitch));// + " realPitch:" + ofToString(realPitch) );

        }else{
            addLog("Trackende");
            buffer[i*buffer.getNumChannels()    ] = 0.0f;
            buffer[i*buffer.getNumChannels() + 1] = 0.0f;
            playhead = std::numeric_limits<int>::min();
        }
    }
    
/*
   for (size_t i = 0; i < buffer.getNumFrames(); i++){
        //addLog("playhead: " + ofToString(playhead) );
        if(!bReversePlay){
            if( playhead < audiofile.length()-1 ){
                play(buffer, i);
                if(!bJogwheelTouch){
                    playhead += step*pitch;
                }else{
                    if(bNewJogWheelTick){
                        playhead += step*pitch;
                    }
                }
            }else{
                buffer[i*buffer.getNumChannels()    ] = 0.0f;
                buffer[i*buffer.getNumChannels() + 1] = 0.0f;
                playhead = std::numeric_limits<int>::max();
            }
        }else{
            if( playhead > 0 ){
                play(buffer, i);
                //playhead -= step*pitch;
                if(!bJogwheelTouch){
                    playhead -= step*pitch; //dead branch?
                }else{
                    if(bNewJogWheelTick){
                        playhead -= step*pitch;
                    }
                }
                
            }else{
                buffer[i*buffer.getNumChannels()    ] = 0.0f;
                buffer[i*buffer.getNumChannels() + 1] = 0.0f;
                playhead = std::numeric_limits<int>::min();
            }
        }
    }
*/
}


void ofApp::play(ofSoundBuffer & buffer, int i){
    for( size_t k=0; k<buffer.getNumChannels(); ++k){
        if( k < audiofile.channels() ){
            float fract = playhead - (double) playhead;
            float s0 = audiofile.sample( playhead, k );
            float s1 = audiofile.sample( playhead+1, k );
            float isample = s0*(1.0-fract) + s1*fract; // linear interpolation
            buffer[i*buffer.getNumChannels() + k] = isample;
        }else{
            buffer[i*buffer.getNumChannels() + k] = 0.0f;
        }
    }
}


void ofApp::processMidi_ControlChange(ofxMidiMessage& message){
    if(message.channel==2){
        if(message.control==8){
            //pitchfader
            float val = message.value/127.0;
            val = ofMap(val, 0.0, 1.0, 1.0, -1.0);
            val = 1.0 + val*MAXFADERPITCH;
            pitch=val;
            faderpitch = val;
            addLog("Pitch:" + ofToString(val) );
        }
        if(message.control==10){
            if(message.value==1){
                //addLog("Jogwheel CW );
                //bNewJogWheelTick = true;
                bReversePlay  = false;
                scratchOffset += 360.0/JOGRESOLUTION;
            }
            if(message.value==127){
                //addLog("Jogwheel CCW");
                //bNewJogWheelTick = true;
                bReversePlay = true;
                scratchOffset -= 360/JOGRESOLUTION;
            }

            //addLog("scratchOffset " + ofToString(scratchOffset) );
            //pitch=1.0+(1.66/message.deltatime);
            //addLog("-------- " + ofToString(6.66/message.deltatime));
        }
    }
}

void ofApp::processMidi_NoteOn(ofxMidiMessage& message){
    if(message.channel==2){
        if(message.pitch==8){
            if(message.velocity==127){
                //addLog("Jog touch");
                bJogwheelTouch=true;
            }
            if(message.velocity==0){
                //addLog("Jog release");
                bJogwheelTouch=false;
                bReversePlay = false;
                pitch=faderpitch;
            }
        }
    }
}

void ofApp::processMidi_NoteOff(ofxMidiMessage& message){
}

void ofApp::drawVinyl(float x, float y, float scale) {
	scale /= 2;
	ofPushStyle();
	ofPushMatrix();
	
	ofTranslate(x, y);
	ofScale(scale, scale);
	ofTranslate(1, 1);
	ofNoFill();
	ofSetColor(255);
	ofCircle(0, 0, 1);
	
	ofPushMatrix();
	ofSetColor(yellowPrint);
//	ofRotate(ofxXwax::millisToDegrees(xwax.getAbsolute()));
	ofLine(0, 0, 0, 1);
	ofPopMatrix();
	
	ofPushMatrix();
	ofSetColor(magentaPrint);
//	ofRotate(ofxXwax::millisToDegrees(xwax.getRelative()));
    ofRotate( platterDegree + markerPositionOffset );
	ofLine(0, 0, 0, 1);
	ofPopMatrix();
	
	ofPopMatrix();
	ofPopStyle();
}

void ofApp::setCuePoint(uint8_t pCueID, int pMS){

}
	
    
void ofApp::recallCuePoint(uint8_t pCueID){
    if(pCueID==1){
        playhead = 532500;
    }
    markerPositionOffset=180-platterDegree;
}
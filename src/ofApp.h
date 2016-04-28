#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"

#include "ofxFaceTracker.h"

using namespace ofxCv;
using namespace cv;

// single-person ear detector
class EarDetector {
protected:
    ofRectangle roi;
    
public:
    ObjectFinder profileFinder, leftEarFinder, rightEarFinder;
    
    void setup() {
        profileFinder.setup("haarcascade_profileface.xml");
        profileFinder.setPreset(ObjectFinder::Accurate);
        profileFinder.setFindBiggestObject(true);

        leftEarFinder.setup("haarcascade_mcs_leftear.xml");
        leftEarFinder.setPreset(ObjectFinder::Accurate); // Fast, Accurate, or Sensitive
        leftEarFinder.setMinNeighbors(0);

        rightEarFinder.setup("haarcascade_mcs_rightear.xml");
        rightEarFinder.setPreset(ObjectFinder::Accurate);
        rightEarFinder.setMinNeighbors(0);
    }
    template <class T>
    void update(T& img) {
        update(toCv(img));
    }
    void update(const Mat& mat) {
        profileFinder.update(mat);
        
        if (profileFinder.size()) {
            cv::Mat faceMat = mat;
            
            leftEarFinder.update(faceMat);
            rightEarFinder.update(faceMat);
        }
    }
    void draw() const {
        if (profileFinder.size()) {
            // uncomment this line for debugging
            profileFinder.draw();
            if (leftEarFinder.size() && rightEarFinder.size()) {
                drawEar(leftEarFinder);
            } else if (leftEarFinder.size()) {
                drawEar(leftEarFinder);
            } else if (rightEarFinder.size()) {
                drawEar(rightEarFinder);
            }
//            if (leftEarFinder.size()) drawEar(leftEarFinder);
//            if (rightEarFinder.size()) drawEar(rightEarFinder);
        }
    }
    void drawEar(ofxCv::ObjectFinder earFinder) const {
        ofRectangle ear = earFinder.getObject(0);
        ofImage blownupEar;
        blownupEar.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
        float earWidth = ear.width;
        float earHeight = ear.height;
        blownupEar.crop(ear.x, ear.y, earWidth, earHeight);
        blownupEar.resize(earWidth * 3, earHeight * 3);
        ofPushMatrix();
        ofTranslate(ear.x - earWidth, ear.y - earHeight);
        blownupEar.draw(0, 0);
        ofPopMatrix();
    }
};

class Highpass {
public:
    Mat lab, lowpass, highpass;
    vector<Mat> labChannels;
    
    template <class S, class D>
    void filter(S& src, D& dst, int size, float contrast = 1) {
        ofxCv::convertColor(src, lab, CV_RGB2Lab);
        cv::split(lab, labChannels);
        cv::Mat& lightness = labChannels[0];
        ofxCv::blur(lightness, lowpass, size);
        // could convert to 16s instead of 32f for extra speed
        cv::subtract(lightness, lowpass, highpass, cv::noArray(), CV_32F);
        if (contrast != 1) {
            highpass *= contrast;
        }
        highpass += 128; // should be diff for other datatypes
        highpass.convertTo(lightness, lightness.type());
        ofxCv::imitate(dst, src);
        cv::merge(labChannels, ofxCv::toCv(dst));
        ofxCv::convertColor(dst, dst, CV_Lab2RGB);
    }
};

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
	ofVideoGrabber cam;
	ofxFaceTracker tracker;
    ofImage blownupMouth;
    EarDetector ear;
    
    ofImage img;
    ofPixels pixels;
    ofxPanel gui;
    ofParameter<int> radius;
    ofParameter<bool> useGaussian;

    Highpass hp;
    ofImage filtered;
    ofParameter<float> size, contrast;
};
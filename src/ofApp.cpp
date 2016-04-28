#include "ofApp.h"

using namespace ofxCv;

void ofApp::setup() {
    ofSetVerticalSync(true);
    // comment this if performance degrades
    ofSetFrameRate(120);
    
    cam.initGrabber(640, 480);
	tracker.setup();
    ear.setup();
    
//    gui.setup();
//    gui.add(useGaussian.set("Use Gaussian", false));
//    gui.add(radius.set("Radius", 50, 0, 100));
//    gui.add(size.set("size", 0, 0, 128));
//    gui.add(contrast.set("contrast", 1.5, .5, 5));
}

void ofApp::update() {
    
	cam.update();
	if (cam.isFrameNew()) {
		tracker.update(toCv(cam));
        ear.update(cam);
        
        //>>
//        ofxCv::copy(cam, img);
//        if (useGaussian) {
//            GaussianBlur(img, radius);
//        } else {
//            blur(img, radius);
//        }
//        img.update();

        //>>
//        hp.filter(cam, filtered, size, contrast);
//        hp.filter(tracker, filtered, size, contrast);
//        filtered.update();
	}
}

void ofApp::draw() {
    ofSetColor(255);
    ofBackground(0, 0, 0);
    //>>
//    filtered.draw(0, 0);

    //>>
//    if (img.isAllocated()) {
//        img.draw(0, 0);
//    }
//    gui.draw();
//    cam.draw(0,0);
    ear.draw();
    
    // uncomment if you wanna see tracker outlines
//    ofSetLineWidth(2);
//    tracker.draw();
    
    //>>
    ofPushMatrix();
    ofPolyline mouth = tracker.getImageFeature(ofxFaceTracker::OUTER_MOUTH);
//    ofPolyline mouth = tracker.getObjectFeature(ofxFaceTracker::OUTER_MOUTH);
    ofRectangle mouthBox = mouth.getBoundingBox();
    float mouthBoxWidth = mouthBox.width;
    float mouthBoxHeight = mouthBox.height;
    pixels = cam.getPixels();
//    blownupMouth.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
    blownupMouth.setFromPixels(pixels);
    blownupMouth.crop(mouthBox.x, mouthBox.y, mouthBoxWidth, mouthBoxHeight);
    blownupMouth.resize(mouthBoxWidth * 3, mouthBoxHeight * 3);
    ofTranslate(mouthBox.x - mouthBoxWidth, mouthBox.y - mouthBoxHeight);
    blownupMouth.draw(0, 0);
    ofPopMatrix();
    
	ofDrawBitmapString(ofToString((int) ofGetFrameRate()), 10, 20);
}

void ofApp::keyPressed(int key) {
	if(key == 'r') {
		tracker.reset();
	}
}

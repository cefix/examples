/*
 *  Sketch.h
 *  cefixSketch
 *
 *  Created by Stephan Huber on 11.05.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#ifndef SKETCH_HEADER
#define SKETCH_HEADER

#include <cefix/Sketch.h>
#include "ExtrudedLineGeometry.h"
#include "LiveDrawing.h"

class Sketch : public cefix::Sketch {
public:
	void setup();
	void draw();
	
	void mousePressed();
	void mouseReleased();
    void keyPressed() {
        if (key == 'd') {
            _liveDrawing->setEnabled(!_liveDrawing->getEnabled());
        }
    }
    
private:
    
    osg::Geode* createStaticExtrudedLine();
    osg::Geode* createTexturedLineWithCaps(); 



	bool _down;
    osg::ref_ptr<LiveDrawing> _liveDrawing;
};


#endif
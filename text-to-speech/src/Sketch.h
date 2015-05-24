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


#include "Text2Speech.h"

class Sketch : public cefix::Sketch {
public:
	void setup();
	void draw();
	
	void mousePressed();
	void mouseReleased();
    
    void update();

private:
	osg::ref_ptr<osg::ShapeDrawable> _drawable;
	bool _down;
    osg::ref_ptr<cefix::Text2Speech> _t2t, _t2t2;
};


#endif
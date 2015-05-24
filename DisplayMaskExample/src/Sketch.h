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
#include <cefix/DisplayMaskController.h>

class Sketch : public cefix::Sketch {
public:
    void init();
	void setup();
	void draw();
	
	void mousePressed();
	void mouseReleased();
    
    void keyPressed();

private:
	osg::ref_ptr<osg::ShapeDrawable> _drawable;
    osg::ref_ptr<cefix::DisplayMaskController> _dmc;
	bool _down;

};


#endif
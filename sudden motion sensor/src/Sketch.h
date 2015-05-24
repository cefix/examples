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
#include <osg/ShapeDrawable>

class Sketch : public cefix::Sketch {
public:
	void setup();
	void draw();
	
	void mousePressed();
	void mouseReleased();

private:
	osg::ref_ptr<osg::ShapeDrawable> _drawable;
	bool _down;

};


#endif
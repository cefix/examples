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
#include <cefix/Quad2DGeometry.h>
#include <cefix/LineStripGeometry.h>

class Sketch : public cefix::Sketch {
public:
	void setup();
	void update();
	
	void mousePressed();
	void mouseReleased();

private:
	bool _down;
    std::vector<osg::ref_ptr<cefix::Quad2DGeometry> > _corners;
    osg::ref_ptr<cefix::Quad2DGeometry> _back;
    osg::ref_ptr<cefix::LineStripGeometry> _shape;
    osg::ref_ptr<osg::Projection> _projection;
	std::vector<osg::ref_ptr<osg::MatrixTransform> > _mats;
    
};


#endif
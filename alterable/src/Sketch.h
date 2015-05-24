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
#include <cefixNet/AutoDiscovery.h>


class Sketch : public cefix::Sketch {
public:
	void setup();
	void draw();
	
	void mousePressed();
	void mouseReleased();
    
    void reset(int i = 1);
	
	void setColor(osg::Vec4 color) { _color = color; }
	osg::Vec4 getColor() const { return _color; }

private:
	osg::ref_ptr<osg::ShapeDrawable> _drawable;
	bool _down;
    float _demping;
    float _distance;
    osg::Vec4 _color;    
    std::string _fps;
    osg::ref_ptr<cefix::net::AutoDiscovery> _bonjour;

};


#endif
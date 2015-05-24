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

class Sketch : public cefix::Sketch {
public:
	void setup();
	void draw();
	
	void mousePressed();
	void mouseReleased();

private:

	void createSceneGraph1(osg::Node* d3, osg::Node* d2);
	void createBlurringSceneGraph1(osg::Node* d3, osg::Node* d2);
    void createBlurringSceneGraphWithPrecomputedKernel(osg::Node* d3, osg::Node* d2);
	void createDOFSceneGraph(osg::Node* d3, osg::Node* d2);
	void createDOFSceneGraph2(osg::Node* d3, osg::Node* d2); 

	
	osg::ref_ptr<osg::Camera> _camera;
	bool _down;
	osg::ref_ptr<osg::Uniform>  _blurAmount, _sigma;
    osg::ref_ptr<osg::Image>    _gaussianKernel;

};


#endif
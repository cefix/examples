/*
 *  Sketch.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 11.05.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "Sketch.h"
#include "SpaceNavigatorCameraManipulator.h"
#include "SpaceNavigatorDevice.h"

void Sketch::setup() 
{
	// set window size
	size(1024, 768);
	
	// create a box + drawable
	osg::Box* box = new osg::Box();
	_drawable = new osg::ShapeDrawable(box);
	osg::Geode* geode = new osg::Geode();
	geode->addDrawable(_drawable.get());
	getWorld()->addChild(geode);
	
	// allow osg-event handler:
	allowOsgHandler(true);
    
    getMainWindow()->addDevice(new SpaceNavigatorDevice());
    getMainWindow()->setCameraManipulator(new SpaceNavigatorCameraManipulator());
	
	_down = 0;
}

void Sketch::draw()
{
	osg::Vec4 c(0,0,0,1);
	c[0] = mouseX / 1024.0;
	c[1] = mouseY / 768.0;
	c[2] = _down;
	
	_drawable->setColor(c);

}



void Sketch::mouseReleased() 
{
	_down = 0;
}

void Sketch::mousePressed() 
{
	_down = 1;
}


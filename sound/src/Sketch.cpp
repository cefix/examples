/*
 *  Sketch.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 11.05.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "Sketch.h"
#include "SoundCube.h"
#include <cefix/StringUtils.h>

void Sketch::setup() 
{
	// set window size
	size(1024, 768);
	
	for(unsigned int i = 0;i < 10; i++) {
        getWorld()->addChild(new SoundCube(osg::Vec3(i, 0, 0), "sound_"+cefix::intToString(i)+".wav"));
    }
    	
	// allow osg-event handler:
	allowOsgHandler(true);
	
}

void Sketch::draw()
{
}



void Sketch::mouseReleased() 
{
}

void Sketch::mousePressed() 
{

}


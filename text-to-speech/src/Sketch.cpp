/*
 *  Sketch.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 11.05.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "Sketch.h"


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
	
    _t2t = new cefix::Text2Speech();
    unsigned int num_voices = _t2t->getNumVoices();
    for(unsigned int i = 0; i < num_voices; ++i) {
        std::cout << i << ": " << _t2t->getVoiceAt(i) << std::endl;
    }
    _t2t->setVoice("com.apple.speech.synthesis.voice.yannick.premium");
    
    _t2t2 = new cefix::Text2Speech("com.apple.speech.synthesis.voice.steffi.premium");
    
	_down = 0;
}


void Sketch::update()
{
    if (!_t2t->isSpeaking())
        _t2t->speak(cefix::intToString(mouseX));
    
    if (!_t2t2->isSpeaking())
        _t2t2->speak(cefix::intToString(mouseY));
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


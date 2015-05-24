/*
 *  Sketch.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 11.05.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "Sketch.h"
#include <cefix/TextGeode.h>

void Sketch::init() 
{
    // DisplayCollection erzeugen
    cefix::DisplayCollection* dc = new cefix::DisplayCollection();
    
    //Fenster-Konfiguration erzeugen
    cefix::WindowConfiguration* conf = cefix::WindowConfiguration::createFullScreen(0);
    
    // Standard-Display für das Fenster erzeugen
    cefix::Display* display = conf->createStandardDisplay("main");
    
    // der Fenster-Konfiguration hinzufügen
    conf->addDisplay(display);
    
    // DisplayMaskController erzeugen und dem display hinzufügen
    _dmc = new cefix::DisplayMaskController();
    _dmc->setEditable(true);
    display->setDisplayMaskController(_dmc);
    
    // Fenster-Konfiguration der DisplayCollection hinzufügen und dem Sketch/AppController bekanntmachen
    dc->addWindowConfiguration(conf);
    setDisplayCollection(dc);
}

void Sketch::setup() 
{
	getMainWindow()->getCamera()->setClearColor(osg::Vec4(0.5,0.5,0.5,1));
    
    // create a box + drawable
	osg::Box* box = new osg::Box();
	_drawable = new osg::ShapeDrawable(box);
	osg::Geode* geode = new osg::Geode();
	geode->addDrawable(_drawable.get());
	getWorld()->addChild(geode);
    
    cefix::Utf8TextGeode* tg = new cefix::Utf8TextGeode("system.xml", 8, "F1 -- toggle DisplayMaskController");
    tg->setPosition(osg::Vec3(10, getHeight() - 20, 0));
    tg->setTextColor(osg::Vec4(1,1,1,1));
    get2DLayer()->addChild(tg);
	
	// allow osg-event handler:
	allowOsgHandler(true);
	
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


void Sketch::keyPressed()
{
    if (key == osgGA::GUIEventAdapter::KEY_F1) {
        _dmc->setEditable(!_dmc->isEditable());
    }
}

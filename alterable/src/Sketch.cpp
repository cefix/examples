/*
 *  Sketch.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 11.05.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "Sketch.h"
#include <cefix/AlterableController.h>
#include <cefix/WidgetsAlterableControllerImplementation.h>
#include <cefixNet/MrmrAlterableControllerImplementation.h>
#include <cefixNet/OscAlterableControllerImplementation.h>
#include <cefix/WidgetFactory.h>
#include <cefix/WidgetThemeManager.h>

#include "WebAlterableControllerImplementation.h"
static int static_test_var = 0;


void Sketch::setup() 
{
	cefix::WidgetFactory::instance()->setWidgetImplementationSet("themed");
	cefix::WidgetThemeManager::instance()->loadThemeAndMakeCurrent("sw_widget_theme.xml");
    
    // set window size
	size(1024, 768);
	
	// create a box + drawable
	osg::Box* box = new osg::Box();
	_drawable = new osg::ShapeDrawable(box);
	osg::Geode* geode = new osg::Geode();
	geode->addDrawable(_drawable.get());
    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	getWorld()->addChild(geode);
	
	// allow osg-event handler:
	allowOsgHandler(true);
    
    // ein paar Alterables erzeugen und in verschiedene Gruppen packen
    
    cefix::AlterableController::add("Sketch", "Demping", this, _demping, cefix::doubleRange(-1,1) );
	cefix::AlterableController::add("Sketch", "Distance", this, _distance);
    cefix::AlterableController::add("Sketch", "fps", this, _fps);
    cefix::AlterableController::add("Sketch Settings", "Down", this, _down);
    cefix::AlterableController::add("Sketch Settings", "Reset", this, &Sketch::reset);
    cefix::AlterableController::add("Cube", "Color", this, &Sketch::getColor, &Sketch::setColor, cefix::doubleRange(0,1));
    
    cefix::AlterableController::add("Sketch", "Static test",  static_test_var, cefix::doubleRange(0,500));
        
    // nach std::cout dumpen
    cefix::AlterableController::dump(std::cout);
    
    
    // widgets backend
    {
        cefix::WidgetsAlterableControllerImplementation* widgets_impl = new cefix::WidgetsAlterableControllerImplementation();
        cefix::AlterableController::addImplementation(widgets_impl);
        get2DLayer()->addChild(widgets_impl->getNode());
	}
    
    
    // mrmr-backend
    if (0){
        cefix::net::MrmrAlterableControllerImplementation* widgets_impl = new cefix::net::MrmrAlterableControllerImplementation();
        widgets_impl->setPassword("cefix");
        cefix::AlterableController::addImplementation(widgets_impl);
    }
    
    // osc-backend
    if (0){
        cefix::net::OscAlterableControllerImplementation* widgets_impl = new cefix::net::OscAlterableControllerImplementation();
        cefix::AlterableController::addImplementation(widgets_impl);
        _bonjour = new cefix::net::AutoDiscovery();
        _bonjour->registerService("_osc._udp", 8000);
    }
    
     // web-backend
    if (1){
        cefix::net::WebAlterableControllerImplementation* widgets_impl = new cefix::net::WebAlterableControllerImplementation();
        cefix::AlterableController::addImplementation(widgets_impl);
    }
        
    reset();
    
   //setRunFrameScheme(osgViewer::ViewerBase::ON_DEMAND);
}

void Sketch::reset(int i)
{
    if (i) {
        std::cout << "reset" << std::endl;
        
        _color = osg::Vec4(1,0,0.2, 1);
        _down = true;
    }
}

void Sketch::draw()
{
    static int i=0;
	if (_down)
        _fps = cefix::intToString(i++)+ " frames";
    
    _drawable->setColor(_color);

}



void Sketch::mouseReleased() 
{
}

void Sketch::mousePressed() 
{
}


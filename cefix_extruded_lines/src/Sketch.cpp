/*
 *  Sketch.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 11.05.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "Sketch.h"

#include <cefix/Perlin.h>
#include <cefix/PickEventHandler.h>
#include "LiveDrawing.h"

#include "TexturedLineWithCapsGeometry.h"



osg::Geode* Sketch::createStaticExtrudedLine() 
{
    
    typedef cefix::ExtrudedLineGeometryT<cefix::ExtrudedLine::QuadSliceGenerator> MyExtrudedLineGeometry;
    typedef cefix::ExtrudedLineGeometryT<cefix::ExtrudedLine::DebugGenerator> MyDebugExtrudedLineGeometry;

    osg::ref_ptr<MyExtrudedLineGeometry> _drawable;
    _drawable = new MyExtrudedLineGeometry();
    

	cefix::Perlin perlin(8, 0.3, 1.0f, 0);

    osg::Vec3 v, up;
    double wx,wy; 
    
    for(unsigned int i = 0; i < 500; ++i) {
        v[0] += 0.75; //std::max(0.2f, perlin.get(i/5.0f));
        

        v[1] =  /* 1 - cefix::randomf(2.0f) + */ sin(v[0] /20.0)* (50 + i/20.0);
        v[2] =  /* 1 - cefix::randomf(2.0f) + */ cos(v[0] /20.0)* (50 + i/20.0) ;
        
        double width = (1.0f+perlin.get(v[0]/20.0)) * 20.0;
        wx = width;
        wy = width;
        
        //wx = 20; wy = 20;
        //w.set(5+perlin.get(i/200.0)*10, 0,0);
        //w.set(20,0,0);
        
        up.set(cos(v[0] / 50.0 * 2*osg::PI), sin(v[0] / 50.0 * 2*osg::PI), 0);
        
        
        _drawable->add(cefix::ExtrudedLine::Point(v, wx, wy, up));
    
    }
    
    _drawable->update();
    
    MyDebugExtrudedLineGeometry* debug = new MyDebugExtrudedLineGeometry();
    debug->setLineVertices(_drawable->getLineVertices());
    debug->update();
    
    
    osg::Geode* geode = new osg::Geode();
	geode->addDrawable(_drawable.get());
    //geode->addDrawable(debug);

    return geode;
}

osg::Geode* Sketch::createTexturedLineWithCaps() 
{

    osg::Geode* geode = new osg::Geode();
    
    {
        cefix::TexturedLineWithCapsGeometry* line = new cefix::TexturedLineWithCapsGeometry();
        line->add(
            osg::Vec3(50, 50, 0),
            10,
            0
        );
        
        line->add(
            osg::Vec3(getWidth()-50, getHeight()-50, 0),
            10,
            0
        );
        line->update();
        line->getColorPolicy().setColor(osg::Vec4(1,1,1,1));
        geode->addDrawable(line);
    }

    for(unsigned int i = 0; i < 100; ++i) {
        cefix::TexturedLineWithCapsGeometry* line = new cefix::TexturedLineWithCapsGeometry();
        float w = cefix::in_between(1, 30);
        line->add(
            osg::Vec3(cefix::in_between(0, getWidth()), cefix::in_between(0, getHeight()), 0),
            w,
            0
        );
        
        line->add(
            osg::Vec3(cefix::in_between(0, getWidth()), cefix::in_between(0, getHeight()), 0),
            w,
            0
        );
        line->update();
        line->getColorPolicy().setColor(osg::Vec4(cefix::in_between(0, 1),cefix::in_between(0, 1),cefix::in_between(0, 1),cefix::in_between(0, 1)));
        geode->addDrawable(line);
    }
    
	osg::Image* img = osgDB::readImageFile("black_dot2.png");
    if (img) {
        osg::Texture2D* tex = new osg::Texture2D(img);
        geode->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON);
    }
    return geode;
}


void Sketch::setup() 
{
	getMainWindow()->getCamera()->setClearColor(osg::Vec4(0.8,0.8, 0.8, 1));
    // set window size
	size(1024, 768);
	
    if (0) {
        getWorld()->addChild(createStaticExtrudedLine());
    }
    if (0) {
        get2DLayer()->addChild(createTexturedLineWithCaps());
        // getWorld()->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    }
    if (1) {
        _liveDrawing = new LiveDrawing();
        getMainWindow()->getPickEventHandler()->setWindowPickable(_liveDrawing);
        get2DLayer()->addChild(_liveDrawing->getNode());
    }

    getWorld()->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

    allowOsgHandler(true);
	
	_down = 0;
}

void Sketch::draw()
{
	osg::Vec4 c(0,0,0,1);
	c[0] = mouseX / 1024.0;
	c[1] = mouseY / 768.0;
	c[2] = _down;
}



void Sketch::mouseReleased() 
{
	_down = 0;
}

void Sketch::mousePressed() 
{
	_down = 1;
}


/*
 *  Sketch.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 11.05.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "Sketch.h"

static const int NUM_VERTICES = 100000;

void WorkerThread::run() 
{
    while(!_isFinished) 
    {
        osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array(NUM_VERTICES);
        for(unsigned int i =0; i < NUM_VERTICES; ++i) {
            (*v)[i].set(cefix::in_between(-1,1), cefix::in_between(-1,1), cefix::in_between(-1,1));
        }
        
        if (_buffer->beginWriting()) { // haben wir platz zum schreiben
            _buffer->write(0, v);
            _buffer->endWriting(); // fertig mit schreiben
        }
        
        OpenThreads::Thread::microSleep(10*1000); // 0.1s schlafen
    }
}

WorkerThread::~WorkerThread() 
{
    _isFinished = true;
    join();
}

void Sketch::setup() 
{
	// set window size
	size(1024, 768);
    
    _drawable = new osg::Geometry();
    osg::Vec3Array* v = new osg::Vec3Array(NUM_VERTICES);
    _drawable->setVertexArray(v);
    _drawable->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, NUM_VERTICES));
    
    osg::Vec4Array* c = new osg::Vec4Array();
    c->push_back(osg::Vec4(1,1,1,0.1));
    _drawable->setColorArray(c);
    _drawable->setColorBinding(osg::Geometry::BIND_OVERALL);
    
    osg::BoundingBox bb;
    bb.expandBy(osg::Vec3(-1,-1, -1));
    bb.expandBy(osg::Vec3( 1, 1,  1));
    
    _drawable->setInitialBound(bb);
	
	// create a box + drawable
    osg::Geode* geode = new osg::Geode();
    geode->addDrawable(_drawable.get());
    geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
	getWorld()->addChild(geode);
	
	// allow osg-event handler:
	allowOsgHandler(true);
	
	_buffer = new Vec3ArrayTripleBuffer();
    _buffer->resize(1); // wir wollen nur ein Vec3Array
    
    _thread = new WorkerThread(_buffer);
    _thread->start();
    
}

void Sketch::draw()
{	
    if (_buffer->beginReading()) { // gibts denn schon daten?
        osg::ref_ptr<osg::Vec3Array> a;
        _buffer->read(0, a);
        if(a.valid()) {
            _drawable->setVertexArray(a);
            _drawable->dirtyBound();
        }
        _buffer->endReading(); // signalisieren, dass wir mit dem Lesen fertig sind
    }
}



void Sketch::mouseReleased() 
{
	_down = 0;
}

void Sketch::mousePressed() 
{
	_down = 1;
}


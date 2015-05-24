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
#include <cefix/TripleBuffer.h>
#include <OpenThreads/Thread>

typedef cefix::TripleBufferRef< osg::ref_ptr<osg::Vec3Array> > Vec3ArrayTripleBuffer;


class WorkerThread : public osg::Referenced, public OpenThreads::Thread {
public:
    WorkerThread(Vec3ArrayTripleBuffer* buffer) : osg::Referenced(), OpenThreads::Thread(), _buffer(buffer), _isFinished(false) {}
    
    virtual void run();
    
    void setFinished() { _isFinished = true; }
protected:
    ~WorkerThread();
private:
    osg::ref_ptr<Vec3ArrayTripleBuffer> _buffer;
    bool _isFinished;

};

class Sketch : public cefix::Sketch {
public:
	void setup();
	void draw();
	
	void mousePressed();
	void mouseReleased();

private:
	osg::ref_ptr<osg::Geometry> _drawable;
	bool _down;
    
    osg::ref_ptr<Vec3ArrayTripleBuffer> _buffer;
    osg::ref_ptr<WorkerThread> _thread;

};


#endif
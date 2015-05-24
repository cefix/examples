/*
 *  DummyParticle.h
 *  springs
 *
 *  Created by Stephan Huber on 10.02.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#ifndef DUMMY_PARTICLE_HEADER
#define DUMMY_PARTICLE_HEADER

#include <cefix/Draggable.h>

#include <cefix/VerletParticle.h>
#include <cefix/RK2Particle.h>

#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>

class DummyParticle;
class DummyParticleDraggable : public cefix::Draggable, public osg::Referenced {

public:
	DummyParticleDraggable(DummyParticle* p);
	virtual bool dragStarted();
	virtual bool drag(const osg::Vec3& delta);
	virtual void dragStopped();
	virtual bool mouseDown();
private:
	DummyParticle*	_p;
	osg::Vec3		_startPos;
	bool			_isFixed;

};


typedef cefix::VerletParticle DummyParticleParent;


class DummyParticle : public DummyParticleParent {

public:
	DummyParticle() 
	:	DummyParticleParent(),
		_mat(new osg::MatrixTransform() ) 
	{
		osg::Geode * geode = new osg::Geode();
		geode->setUserData(new DummyParticleDraggable(this));
		setMass(cefix::in_between(3,30));
		geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0,0,0), getMass())) );
		_mat->addChild(geode);
		setPosition(osg::Vec3(cefix::in_between(-250, 250),cefix::in_between(-250, 250), cefix::in_between(-250, 250))); 
		//setLifeTime(10.0f);
		
	}
	
	
	inline void updateRepresentation() 
	{
		_mat->setMatrix(osg::Matrix::translate(getPosition()));
	}
	
	inline osg::Node* getNode() { return _mat.get(); }
	
	virtual ~DummyParticle();
	
private:
	osg::ref_ptr<osg::MatrixTransform> _mat;
	

};


#endif
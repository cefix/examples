/*
 *  DummyParticle.cpp
 *  springs
 *
 *  Created by Stephan Huber on 10.02.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "DummyParticle.h"




DummyParticle::~DummyParticle() 
{
	//osg::Group* g = _mat->getParent(0);
	//if (g) g->removeChild(_mat.get());
}



DummyParticleDraggable::DummyParticleDraggable(DummyParticle* p)
:	cefix::Draggable(),
	_p(p)
{
}

bool DummyParticleDraggable::mouseDown() 
{
	setBlockSuccessiveHandler(true);
	return cefix::Draggable::mouseDown();
}

bool DummyParticleDraggable::dragStarted()
{
	_startPos	= _p->getPosition();
	_isFixed	= _p->isFixed();
	_p->setFixed(true);
	return true;
}
bool DummyParticleDraggable::drag(const osg::Vec3& delta)
{
	_p->setPosition( _startPos + delta );
	return true;
}
void DummyParticleDraggable::dragStopped() 
{
	_p->setFixed(_isFixed);
}

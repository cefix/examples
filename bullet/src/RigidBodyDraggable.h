#ifndef RIGID_BODY_DRAGGABLE
#define RIGID_BODY_DRAGGABLE


#include <cefix/MathUtils.h>

#include <cefixbt/Constraints.h>
#include <cefixbt/MotionState.h>
#include <cefixbt/RigidBody.h>
#include <cefixbt/Shape.h>
#include <cefixbt/Utils.h>
#include <cefixbt/AppController.h>
#include <cefix/Draggable.h>

namespace cefixbt {

class RigidBodyDraggable : public cefix::Draggable, public osg::Referenced {
    
public:
	RigidBodyDraggable(btRigidBody* body, cefixbt::AppController* world)
	:	cefix::Draggable(),
    osg::Referenced(),
    _body(body),
    _world(world)
	{
	}
	
	virtual bool dragStarted() 
	{
		setBlockSuccessiveHandler(true);
		
		
		_body->setActivationState(DISABLE_DEACTIVATION);
		std::cout << hit().getWorldIntersectPoint() << std::endl; 
		_pickPos = hit().getWorldIntersectPoint();// + cefix::MouseLocationProvider::instance()->getNear();
		
		_worldDragPlane.set(cefix::MouseLocationProvider::instance()->getRay(),hit().getWorldIntersectPoint()); 
        
		_p2p = new cefixbt::Point2PointConstraint(_body, cefixbt::asBtVector3(hit().getLocalIntersectPoint()) );
		//_p2p->m_setting.m_impulseClamp = mousePickClamping;
		_p2p->m_setting.m_tau = 0.8f;
        
		_world->addConstraint(_p2p);
        
		return true;
	}
	
	virtual bool drag (const osg::Vec3 &delta) {
		// wir können hier das delta nicht gebrauchen, da es im lokalen Koordinaten-System des Nodes sich befindet.
		
		osg::Vec3 p;
		cefix::getRayPlaneIntersection(_worldDragPlane, cefix::MouseLocationProvider::instance()->getNear(), cefix::MouseLocationProvider::instance()->getRay(), p); 
        
		std::cout << "delta: " << p << std::endl;
		_p2p->setPivotB( cefixbt::asBtVector3(p));
		return true;
	}
	
	virtual void dragStopped()
	{
		_world->removeConstraint(_p2p.get());
		_body->forceActivationState(ACTIVE_TAG);
		_body->setDeactivationTime( 0.f );
	}
	
private:
	btRigidBody* _body;
	cefixbt::AppController* _world;
	osg::ref_ptr<cefixbt::Point2PointConstraint> _p2p;
	osg::Vec3 _pickPos;
	osg::Plane _worldDragPlane;
	
};

}


#endif // RIGID_BODY_DRAGGABLE
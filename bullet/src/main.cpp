//
//  main.c
//  cefixTemplate
//
//  Created by Stephan Huber on 26.11.06.
//  Copyright __MyCompanyName__ 2006. All rights reserved.
//

#include "btBulletDynamicsCommon.h"

#include <cefixbt/Utils.h>
#include <cefixbt/AppController.h>

#include <cefixbt/Shape.h>
#include <cefixbt/MotionState.h>
#include <cefixbt/Constraints.h>

#include <cefix/DataFactory.h>
#include <cefix/LineGridGeometry.h>
#include <cefix/MathUtils.h>
#include <cefix/StatisticsGroup.h>
#include <cefix/SoundManager.h>
#include <cefix/RingBuffer.h>
#include <cefix/Draggable.h>
#include <cefix/SoundPool.h>
#include "RigidBodyDraggable.h"

#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>


class GroundContactCallback : public cefixbt::ContactCallback {
public:
    GroundContactCallback() 
    :   cefixbt::ContactCallback() 
    {
        _sounds = new cefix::SoundPool<32>("sound.wav");
    }
    
    virtual void newContactWith(const btCollisionObject* o, const btManifoldPoint& contact_point) 
    {
        cefix::Sound* sound = _sounds->getNext();
        
        if (!sound->isPlaying()) {
            sound->seek(0);
            sound->play();
            sound->setVolume(1.0);
        }
    }
    
    
    virtual void lostContactWith(const btCollisionObject* o) {}
private:
    osg::ref_ptr<cefix::SoundPool<32> > _sounds;
    
};



class MyAppController: public cefixbt::AppController 
{

	public:
		MyAppController(): cefixbt::AppController(true) 
		{
			subscribeTo("keypress", this, &MyAppController::handleKeyPress);
		}
		
		~MyAppController() {
			std::cout << "~MyAppController" << std::endl;
		}
			
		void handleKeyPress(const int& key) 
		{
			if (key == osgGA::GUIEventAdapter::KEY_F1) {
				if (_scene->getNodeMask() == 0x0) 
					_scene->setNodeMask(0xFFFFFFF);
				else
					_scene->setNodeMask(0x0);
			}
			
			getMainWindow()->requestPicking();
			getMainWindow()->getPickEventHandler()->setHierarchicalPickingEnabled(true);
            
            if(key == osgGA::GUIEventAdapter::KEY_F2) {
                getPhysics()->pause();
                for(unsigned int i = 0; i < _rigidBodies.size(); ++i) {
					removeRigidBody(_rigidBodies[i]);
				}
                getPhysics()->resume();
            }
            
            if(key == osgGA::GUIEventAdapter::KEY_F3) {
                getPhysics()->pause();
                _rigidBodies[0]->setWorldTransform(cefixbt::asBtTransform(osg::Matrix::translate(100,100,100)));
                getPhysics()->resume();
            }

            if(key == osgGA::GUIEventAdapter::KEY_F4) {
                bool visible = (_debugNode->getNodeMask() > 0); 
                _debugNode->setNodeMask(0xffff * (!visible));
            }

            
		}
		
        		
		osg::Group * createWorld() {
        
            setupPerformanceStatistics();
            
            //getPhysics()->setUseFixedTimeSteps(false);
            //getPhysics()->setMaxSubSteps(5);
            
            getPhysics()->addPerformanceStatistics(this);
            
            cefix::log::setInfoLevel(osg::ALWAYS);
		
			getMainWindow()->getCamera()->setClearColor(osg::Vec4(0.2, 0.2, 0.2, 1.0));
		
			// statistics
			if (0) {
				enablePerformanceStatistics(true);
				get2DLayer()->addChild(getPerformanceStatisticsGroup());
			}
			
			setUseOptimizerFlag(false);
			
			osg::Group* g = new osg::Group();
			
			{
				//btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,1,0),1);
				cefixbt::StaticPlaneShape* groundShape = new cefixbt::StaticPlaneShape(osg::Vec3(0,0,1),0);
				cefixbt::MotionState* groundMotionState = new cefixbt::MotionState( btTransform(btQuaternion(0,0,0,1),btVector3(-250,-250,-1)) );
				
				btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
				osg::ref_ptr<cefixbt::RigidBody>  groundRigidBody = new cefixbt::RigidBody(groundRigidBodyCI);
                groundRigidBody->setContactCallback(new GroundContactCallback());
                
				addRigidBody(groundRigidBody);
				
				osg::Geode* groundgeode = new osg::Geode();
				groundgeode->addDrawable(new cefix::LineGridGeometry(osg::Vec3(500,500,0)));
				groundMotionState->addChild(groundgeode);
				g->addChild(groundMotionState);

			}
			
			for (unsigned int i = 0; i < 00; ++i) {
				cefixbt::SphereShape* fallShape = new cefixbt::SphereShape(1);
				cefixbt::MotionState* fallMotionState =  new cefixbt::MotionState( btTransform(btQuaternion(0,0,0,1),btVector3(cefix::in_between(-5,5), cefix::in_between(-5,5), cefix::in_between(50,500))) );
			
				btScalar mass = cefix::in_between(1,10);
				btVector3 fallInertia(0,0,0);
				fallShape->calculateLocalInertia(mass,fallInertia);
				
				btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,fallMotionState,fallShape,fallInertia);
				osg::ref_ptr<cefixbt::RigidBody>  fallRigidBody = new cefixbt::RigidBody(fallRigidBodyCI);
			
				addRigidBody(fallRigidBody);
			
				osg::Geode* fallgeode = new osg::Geode();
				osg::ShapeDrawable* drawable = new osg::ShapeDrawable(fallShape);
				drawable->setColor(osg::Vec4(cefix::randomf(1.0),cefix::randomf(1.0),cefix::randomf(1.0),1.0));
				fallgeode->addDrawable(drawable);
				fallMotionState->addChild(fallgeode);
				g->addChild(fallMotionState);
			}
			
			std::vector<osg::ref_ptr<cefixbt::RigidBody> > boxes;
			for (unsigned int i = 0; i < 00; ++i) {
				cefixbt::BoxShape* fallShape = new cefixbt::BoxShape(osg::Vec3(1,1,1));
				cefixbt::MotionState* fallMotionState =  new cefixbt::MotionState( btTransform(btQuaternion(0,0,0,1),btVector3(cefix::in_between(-5,5), cefix::in_between(-5,5), cefix::in_between(50,500))) );
			
				btScalar mass = cefix::in_between(1,10);
				btVector3 fallInertia(0,0,0);
				fallShape->calculateLocalInertia(mass,fallInertia);
			
				btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,fallMotionState,fallShape,fallInertia);
				osg::ref_ptr<cefixbt::RigidBody>  fallRigidBody = new cefixbt::RigidBody(fallRigidBodyCI);
			
				addRigidBody(fallRigidBody);
			
						
				osg::Geode* fallgeode = new osg::Geode();
				osg::ShapeDrawable* drawable = new osg::ShapeDrawable(fallShape);
				drawable->setColor(osg::Vec4(cefix::randomf(1.0),cefix::randomf(1.0),cefix::randomf(1.0),1.0));
				fallgeode->addDrawable(drawable);
				fallMotionState->addChild(fallgeode);
				g->addChild(fallMotionState);
				boxes.push_back(fallRigidBody);
			}
			
			
			
			if (1) {
				osg::Node* node = osgDB::readNodeFile("w.obj");
				osgUtil::Optimizer o;
				o.optimize(node);
				cefixbt::ConvexHullShape* fallShape = new cefixbt::ConvexHullShape(node, false);
				//cefixbt::ConvexTriangleMeshShape* fallShape = new cefixbt::ConvexTriangleMeshShape(node, false);
				osg::ref_ptr<cefixbt::RigidBody>  last(NULL), current(NULL);
				for (unsigned int i = 0; i < 10; ++i) {
					cefixbt::MotionState* fallMotionState =  new cefixbt::MotionState( btTransform(btQuaternion(0,0,0,1),btVector3(cefix::in_between(-50,50), cefix::in_between(-50,50), cefix::in_between(50,50))) );
				
					btScalar mass = cefix::in_between(1,10);
					btVector3 fallInertia(0,0,0);
					fallShape->calculateLocalInertia(mass,fallInertia);
				
					btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,fallMotionState,fallShape,fallInertia);
					osg::ref_ptr<cefixbt::RigidBody>  fallRigidBody = new cefixbt::RigidBody(fallRigidBodyCI);
				
					fallRigidBody->setDamping(0.4, 0.4);
					
					addRigidBody(fallRigidBody);
				
                    
					fallMotionState->addChild(node);
					fallMotionState->setUserData(new cefixbt::RigidBodyDraggable(fallRigidBody, this));
					g->addChild(fallMotionState);
					last = current;
					current = fallRigidBody;
					_rigidBodies.push_back(current);
                    
					if (last && current) {
						const int mode(0);
						switch (mode) {
						case -1:
							break;
						case 0:
							{
								cefixbt::Point2PointConstraint* constraint = new cefixbt::Point2PointConstraint(last, current, osg::Vec3(7, 7, 0), osg::Vec3(-7, 0 , 0));
								addConstraint(constraint);
							}
							break;
						case 1:
							{
								cefixbt::Generic6DofConstraint* constraint = new cefixbt::Generic6DofConstraint(
									last, 
									current, 
									osg::Matrix::translate(osg::Vec3(5, 0, 0)), 
									osg::Matrix::translate(osg::Vec3(-15, 0 , 0)),
									true
								);
								constraint->setLinearLowerLimit(btVector3(0.0f, 0.0f, 0.0f));
								constraint->setLinearUpperLimit(btVector3(3.0f, 0.0f, 0.0f));

								constraint->setAngularLowerLimit(btVector3(0,0,0));
								constraint->setAngularUpperLimit(btVector3(0.2,0,0));
								addConstraint(constraint);
							}
							break;						
						
						case 2:
							{
								cefixbt::Generic6DofSpringConstraint* constraint = new cefixbt::Generic6DofSpringConstraint(
									last, 
									current, 
									osg::Matrix::translate(osg::Vec3(5, 0, 0)), 
									osg::Matrix::translate(osg::Vec3(-15, 0 , 0)),
									true
								);
								constraint->setLinearLowerLimit(btVector3(-1.0f, -1.0f, -1.0f));
								constraint->setLinearUpperLimit(btVector3(1.0f, 1.0f, 1.0f));

								constraint->setAngularLowerLimit(btVector3(-0.5,0,0));
								constraint->setAngularUpperLimit(btVector3(0.5,0.0,0));
								for(unsigned int i=0; i < 6; ++i) {
									constraint->enableSpring(i, true);
									constraint->setStiffness(i, 0.01);
									constraint->setDamping (i, 0.01);
								}
								
								addConstraint(constraint);
							}
                        
                        
						}
					}
				}
				
				
			}
			osg::Group* world = new osg::Group();
			world->addChild(g);
			_scene = g;	
			
            // debug-world
			if (1) {
				_debugNode = getDebugDrawNode(
                    /*cefixbt::DebugPhysicsDrawer::DBG_DrawAabb | */
                    cefixbt::DebugPhysicsDrawer::DBG_DrawConstraints | 
                    cefixbt::DebugPhysicsDrawer::DBG_DrawConstraintLimits |
                    cefixbt::DebugPhysicsDrawer::DBG_DrawFeaturesText |
                    cefixbt::DebugPhysicsDrawer::DBG_DrawContactPoints |
                    cefixbt::DebugPhysicsDrawer::DBG_NoDeactivation |
                    cefixbt::DebugPhysicsDrawer::DBG_NoHelpText |
                    cefixbt::DebugPhysicsDrawer::DBG_DrawText |
                    cefixbt::DebugPhysicsDrawer::DBG_ProfileTimings |
                    cefixbt::DebugPhysicsDrawer::DBG_DrawWireframe
                );
                world->addChild(_debugNode);
                _debugNode->setNodeMask(0x0);
			}			
			return world;
		}
	private:
		osg::Group* _scene;
        std::vector< cefixbt::RigidBody* > _rigidBodies;
        osg::Node* _debugNode;
};


int main(int argc, char* argv[])
{
    cefix::DataFactory::instance()->setApplicationPath(argv[0]);
	osg::ref_ptr<MyAppController> app = new MyAppController();
	// create the world and apply it
    app->applyWorld(app->createWorld());

    //app->requestPicking();
    //app->requestPostScriptWriter();
    
    //create a viewer-window/fullscreen
    app->realize();
	app->run();
	
	app = NULL;

	return 0;
}



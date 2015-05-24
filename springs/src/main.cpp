//
//  main.c
//  cefixTemplate
//
//  Created by Stephan Huber on 26.11.06.
//  Copyright __MyCompanyName__ 2006. All rights reserved.
//

#include <cefix/AppController.h>
#include <osg/ShapeDrawable>

#include <cefix/DataFactory.h>
#include <cefix/ParticleProcessor.h>
#include <cefix/ParticleSteadyRateCounter.h>
#include <cefix/ParticleBoxPlacer.h>
#include <cefix/ParticleFixedDirectionShooter.h>
#include <cefix/ParticleEmitter.h>
#include <cefix/AnimationFactory.h>
#include <cefix/CommonParticleOperators.h>

#include <cefix/WidgetFactory.h>
#include <cefix/WidgetThemeManager.h>
#include <cefix/GroupWidget.h>
#include <cefix/VecXSliderWidget.h>

#include "DummyParticle.h"
#include <cefix/SpringParticleProcessor.h>
#include <cefix/HSliderWithCaptionAndValueWidget.h>

typedef cefix::SpringParticleProcessor<DummyParticle> UsedSpringParticleProcessor;


class RotateCallback : public osg::NodeCallback {
public:
	RotateCallback(osg::MatrixTransform* mat) : osg::NodeCallback(), _mat(mat) {}
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		_angle += 0.001;
		_mat->setMatrix(osg::Matrix::rotate(_angle, osg::Z_AXIS));
		traverse(node, nv);
	}
private:
	osg::MatrixTransform* _mat;
	float _angle;
};



class MyAppController: public cefix::AppController {
	private:
		osg::ref_ptr<UsedSpringParticleProcessor >							_pp;
		std::vector<DummyParticle* >										_particles;
		std::vector<cefix::ParticleSpringConstraint<DummyParticle>*>		_springs;
		osg::MatrixTransform*												_world;
		float																_strength, _restLength;
		osg::ref_ptr<cefix::GroupWidget>									_widgets;
		
	public:
		MyAppController()
		: cefix::AppController(),
			_strength(0.05),
			_restLength(50)
		{
			// themed widgets benutzen
			cefix::WidgetThemeManager::instance()->loadThemeAndMakeCurrent("sw_widget_theme.xml");			
			subscribeTo("keypress", this, &MyAppController::handleKeyPress);
		}
		
				
		inline float getStrength() { return _strength; }
		void setStrength(float strength) 
		{ 
			_strength = strength;
			
			for(unsigned int i = 0; i < _springs.size(); ++i) {
				_springs[i]->setStrength(_strength);
			}
		}
		
		inline float getWishDistance() { return _restLength; }
		void setWishDistance(float f) 
		{ 
			_restLength = f;
			
			for(unsigned int i = 0; i < _springs.size(); ++i) {
				_springs[i]->setWishDistance(f);
			}
		}
		
				
		void handleKeyPress(const int& key) 
		{
			if (key == '-') {
				addPyramide();
			} else if (key == 'a') {
				addAttractedParticle();
			} else if (key == 'r') {
				addRepulsedParticle();
			} else if (key == '.') {
				addParticle();
			} else if (key == ',') {
				for(unsigned int i=4; i < _particles.size(); ++i) {
					_particles[i]->setPosition( osg::Vec3(cefix::in_between(-250,250), cefix::in_between(-250,250), cefix::in_between(-250, 250)) );
				}
			}
		}	
		

		
		osg::Node* createWidgets() {

			// Widgets erzeugen
			cefix::WidgetFactory* wf = cefix::WidgetFactory::instance();
			cefix::AbstractProperty* widgetsprop = cefix::DataFactory::instance()->getPreferencesFor("/groupwidget");
			if ((widgetsprop != NULL) && (widgetsprop->getType() == cefix::AbstractProperty::PROPERTY_LIST)) 
			{ 
				// responder-map:
				cefix::WidgetFactory::ResponderMap respondermap;
				std::cout << _pp->getNumIterations() << std::endl;
				respondermap.add<cefix::HSliderWithCaptionAndValueWidget>("num_iterations", _pp.get(), &UsedSpringParticleProcessor::getNumIterations, &UsedSpringParticleProcessor::setNumIterations);
				respondermap.add<cefix::Vec3SliderWidget>("gravity_slider", _pp.get(), &UsedSpringParticleProcessor::getGravity, &UsedSpringParticleProcessor::setGravity);
				respondermap.add<cefix::HSliderWithCaptionAndValueWidget>("damping_slider", _pp.get(), &UsedSpringParticleProcessor::getDamping, &UsedSpringParticleProcessor::setDamping);	
				respondermap.add<cefix::HSliderWithCaptionAndValueWidget>("spring_strength", this, &MyAppController::getStrength, &MyAppController::setStrength);
				respondermap.add<cefix::HSliderWithCaptionAndValueWidget>("spring_min_distance", this, &MyAppController::getWishDistance, &MyAppController::setWishDistance);
				
				
				_widgets =  dynamic_cast<cefix::GroupWidget*>(wf->create(widgetsprop, respondermap));
			}
			requestPicking();
			
			return (_widgets.valid()) ? _widgets->getNode() : NULL;
		}
		
		
		void addParticle(DummyParticle* p) {
			_world->addChild(p->getNode());
			_pp->addParticle(p);
			_particles.push_back(p);
		}
		
		void addParticle() 
		{
			
			// Partikel erzeugen + positionieren
			DummyParticle* p = new DummyParticle();
			p->setPosition( osg::Vec3(cefix::in_between(-250,250), cefix::in_between(-250,250), cefix::in_between(-250,250)) );
			p->update(0);
			
			// 2 Springs erzeugen und dem ConstraintSolver bekannt geben
			DummyParticle* p1 = _particles[cefix::randomf( cefix::randomf(10.0) < 1.0 ? 4 : _particles.size())];
			_springs.push_back(_pp->addSpring(p1,p, _strength, _restLength));
			

			
			addParticle(p);

		}
		
		void addAttractedParticle() 
		{
			
			// Partikel erzeugen + positionieren
			DummyParticle* p = new DummyParticle();
			p->setPosition( osg::Vec3(cefix::in_between(-250,250), cefix::in_between(-250,250), cefix::in_between(-250,250)) );
			p->update(0);
			
			DummyParticle* p1 = _particles[cefix::randomf( cefix::randomf(10.0) < 1.0 ? 4 : _particles.size())];
			_springs.push_back(_pp->addSpring(p1,p, _strength, _restLength));
			
			for(unsigned int i=0; i < 3; ++i) 
			{
				DummyParticle* p2 = _particles[cefix::randomf(_particles.size())];
				_pp->addAttraction(p2,p, 0.001);
			}

			
			addParticle(p);

		}
		
		void addRepulsedParticle() 
		{
			
			// Partikel erzeugen + positionieren
			DummyParticle* p = new DummyParticle();
			p->setPosition( osg::Vec3(cefix::in_between(-250,250), cefix::in_between(-250,250), cefix::in_between(-250,250)) );
			p->update(0);
			
			DummyParticle* p1 = _particles[cefix::randomf( cefix::randomf(10.0) < 1.0 ? 4 : _particles.size())];
			_springs.push_back(_pp->addSpring(p1,p, _strength, _restLength));
			
			for(unsigned int i=0; i < 3; ++i) 
			{
				DummyParticle* p2 = _particles[cefix::randomf(_particles.size())];
				_pp->addAttraction(p2,p, -0.001);
			}

			
			addParticle(p);

		}
		
		void addPyramide() {
			
			DummyParticle* anchor = _particles[cefix::randomf(_particles.size())];
			// Pyramide aus 8 Partikeln bauen
			
			DummyParticle* p1 = new DummyParticle();
			DummyParticle* p2 = new DummyParticle();
			DummyParticle* p3 = new DummyParticle();
			DummyParticle* p4 = new DummyParticle();
			
			addParticle(p1);
			addParticle(p2);
			addParticle(p3);
			addParticle(p4);
			
						
			_pp->addFixed(p1, p2, 150);
			_pp->addFixed(p1, p3, 150);
			_pp->addFixed(p1, p4, 150);
			_pp->addFixed(p2, p3, 150);
			_pp->addFixed(p2, p4, 150);
			_pp->addFixed(p3, p4, 150);
			
		
			_pp->addFixed(p1, anchor, 300);
		}
		
		osg::Group * createWorld() 
		{
		
			
			_world = new osg::MatrixTransform();
			
			// SpringParticleProcessor erzeugen
			_pp = new UsedSpringParticleProcessor();
			cefix::AnimationFactory::instance()->add(new cefix::ParticleProcessorAnimationAdapter<DummyParticle>(_pp.get(), false, 0.03));
			
			const int maxx(50);
			const int maxy(50);
			
			for (int y = 0; y < maxy; ++y) {
				for(int x = 0; x < maxx; ++x) {
					DummyParticle* p = new DummyParticle();
					p->setPosition( osg::Vec3(x*50.0 - 250.0, y*50.0 - 250.0, -250.0) );
					_pp->addParticle(p);
					_world->addChild(p->getNode());
					
					if (((x==0) && ((y == 0) || (y == maxy-1))) || ((x==maxx-1) && ((y==0) || (y==maxy-1))))
						p->setFixed( true );
				
					_particles.push_back(p);

				}
			}
			
			for (unsigned int y = 0; y < maxy; ++y) {
				for(unsigned int x = 0; x < maxx; ++x) {
					DummyParticle* p1(NULL);
					if (x>=1) 
						p1 = _particles[ (y)* maxx +(x-1) ];
					DummyParticle* p2(NULL);
					if (y>=1)
						p2 = _particles[ (y-1)* maxx +(x) ];
					DummyParticle* p = _particles[ (y)* maxx +(x) ];
					if (p1)
						_springs.push_back(_pp->addSpring(p1,p, _strength, _restLength));
					if (p2)
						_springs.push_back(_pp->addSpring(p2,p, _strength, _restLength));
				}
			}

		
			
				
			
			osg::Group* group = new osg::Group();
			group->addChild(_world);
			//_world->addChild(_pp->enableDebug());
			//_world->setUpdateCallback(new RotateCallback(_world));
			
			get2DLayer()->addChild(createWidgets());
			return group;
		}

};


int main(int argc, char* argv[])
{
    cefix::DataFactory::instance()->setApplicationPath(argv[0]);
	osg::ref_ptr<MyAppController> app = new MyAppController();
	app->setUseOptimizerFlag(false);
	// create the world and apply it
    app->applyWorld(app->createWorld());

    //app->requestPicking();
    //app->requestPostScriptWriter();
    
    //create a viewer-window/fullscreen
    app->realize();
	app->run();

	return 0;
}



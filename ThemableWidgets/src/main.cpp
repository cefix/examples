//
//  main.c
//  cefixTemplate
//
//  Created by Stephan Huber on 26.11.06.
//  Copyright __MyCompanyName__ 2006. All rights reserved.
//

#include <cefix/AppController.h>
#include <osg/ShapeDrawable>
#include <cefix/Quad2DGeometry.h>
#include <cefix/SequenceGrabberManager.h>

#include <cefix/DataFactory.h>
#include <cefix/WidgetFactory.h>
#include <cefix/ProgressBarWidget.h>
#include <cefix/AnimationFactory.h>
#include <cefix/TextInputWidgetManager.h>
#include <cefix/WidgetThemeManager.h>
#include <cefix/GroupWidget.h>

class AnimateProgressBar : public cefix::AnimationBase {
public:
	AnimateProgressBar(cefix::ProgressBarWidget* w) : cefix::AnimationBase(0), _w(w) 
	{
		_w->setMaximum(1000);
		_cnt = 0;
	}
	
protected:
	virtual void animate(float elapsed) {
		++_cnt;
		_w->setValue(_cnt);
		if ((_cnt > 100) && (_cnt < 400)) {
			if (_cnt == 101) _w->setMaximum(-1);
		}else
			_w->setMaximum(1000);
		if (_cnt >= 1000) _cnt = 0;
	}
private:
	osg::ref_ptr<cefix::ProgressBarWidget> _w;
	unsigned int _cnt;

};


class MyAppController: public cefix::AppController {

	public:
		osg::Geode* createQuad(osg::Image* img, const osg::Vec3& v) {
			osg::Geode* geode = new osg::Geode();
			cefix::Quad2DGeometry* geo = new cefix::Quad2DGeometry(osg::Vec4(v[0],v[1],v[0]+320,v[1] + 240));
			geo->setTextureFromImage(img);
			geo->setTextureRect(osg::Vec4(0,0,320,240));
			geode->addDrawable(geo);
			return geode;
		}
		
		
		osg::Node* createWidgets() {

			// Widgets erzeugen
			cefix::WidgetFactory* wf = cefix::WidgetFactory::instance();
			cefix::AbstractProperty* widgetsprop = cefix::DataFactory::instance()->getPreferencesFor("/groupwidget");
			if ((widgetsprop != NULL) && (widgetsprop->getType() == cefix::AbstractProperty::PROPERTY_LIST)) 
			{ 
				// responder-map:
				cefix::WidgetFactory::ResponderMap respondermap;
				
				_widgets =  dynamic_cast<cefix::GroupWidget*>(wf->create(widgetsprop, respondermap));
							
				try {
					cefix::AnimationFactory::instance()->add(new AnimateProgressBar(_widgets->find<cefix::ProgressBarWidget*>("progressbar")));
					//_widgets->find<cefix::ProgressBarWidget*>("progressbar")->setMaximum(-1);
				}
				catch (...)
				{}
				
			}
			requestPicking();
			
			return (_widgets.valid()) ? _widgets->getNode() : NULL;
		}

		
		osg::Group * createWorld() {
		
			// themed widgets benutzen
			cefix::WidgetFactory::instance()->setWidgetImplementationSet("themed");
			
			cefix::WidgetThemeManager::instance()->loadThemeAndMakeCurrent("grey_theme.xml");
			
			osg::Group* g = new osg::Group();
			
			// Widgets erzeugen
			osg::Node* node = createWidgets();
			get2DLayer()->addChild( node );
			
			// Wichtig für TextInputWidgets: den TextINputWidgetManager als EventHandler registriern
			
			getMainWindow()->getEventHandlers().push_front(cefix::TextInputWidgetManager::instance());
		
		
			float grey = 0.7f;
			getMainWindow()->getCamera()->setClearColor(osg::Vec4(grey, grey, grey,1));
			return g;
		}
	private:
		osg::ref_ptr<cefix::GroupWidget> _widgets;

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



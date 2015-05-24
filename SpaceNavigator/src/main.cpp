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
#include <cefix/InputDeviceManager.h>
#include <cefix/Logger.h>

class TransformControlCallback : public osg::NodeCallback {
	public:
		TransformControlCallback(osg::MatrixTransform* mat) 
		:	osg::NodeCallback(),
			_mat(mat) 
		{
			cefix::InputDeviceManager* dm = cefix::InputDeviceManager::instance();
			_device = dm->open("spacenavigator");
            if (_device.valid()) {
                _transId = _device->getKey("translation");
                _rotId   = _device->getKey("rotation");
                
                std::cout << (*_device) << std::endl;
            }
			
		}
		
		virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
		
			if (_device.valid()) {
				std::cout << "trans: " << _device->getVec3Value(_transId) << " rot: " << _device->getVec3Value(_rotId) << std::endl;
				osg::Vec3d trans(_device->getVec3Value(_transId) / 1024.0);
				osg::Vec3d rot(_device->getVec3Value(_rotId) / 1024.0 * osg::PI / 20.0);
				
                _trans += osg::Vec3(trans[0], trans[2], trans[1]);
                _rot *= osg::Quat(rot[0], osg::X_AXIS, rot[1], osg::Y_AXIS, rot[2], osg::Z_AXIS);
				_mat->setMatrix(osg::Matrix::rotate(_rot) * osg::Matrix::translate(_trans));
			}
			traverse(node, nv);
		}
	private:
		osg::ref_ptr<cefix::InputDevice> _device;
        unsigned int _transId, _rotId;
        osg::Vec3d _trans;
        osg::Quat _rot;
		osg::MatrixTransform* _mat;
};
		

class MyAppController: public cefix::AppController {

	public:
		MyAppController() : cefix::AppController(cefix::Logger::TEXT_GEODE | cefix::Logger::REDIRECT_CONSOLE | cefix::Logger::REDIRECT_ERR_CONSOLE) {}
		osg::Group * createWorld() {
		
			osg::Group* g = new osg::Group();
			/*osg::Geode* geode = new osg::Geode();
			geode->addDrawable(new osg::ShapeDrawable(new osg::Box(osg::Vec3(0,0,0), 1)));
			*/
			osg::Node* geode = osgDB::readNodeFile("cow.osg");
			osg::MatrixTransform* mat = new osg::MatrixTransform();
			mat->addChild(geode);
			mat->setUpdateCallback(new TransformControlCallback(mat));
			g->addChild(mat);
			
			std::cout << "world created" << std::endl;
			
					
			return g;
		}
		
		virtual  void realize() 
        {
			{
                cefix::AppController::realize();
                cefix::InputDeviceManager* dm = cefix::InputDeviceManager::instance();
                cefix::InputDevice* device = dm->open("spacenavigator");
                                
                if (device) {
                    std::cout << (*device) << std::endl;
                }
            }            
		}
		
};


int main(int argc, char* argv[])
{
    cefix::DataFactory::instance()->setApplicationPath(argv[0]);
	osg::ref_ptr<MyAppController> app = new MyAppController();
	
	// create the world and apply it
    app->setUseOptimizerFlag(false);
	app->applyWorld(app->createWorld());

    //app->requestPicking();
    //app->requestPostScriptWriter();
    
    //create a viewer-window/fullscreen
    app->realize();
	app->run();

	return 0;
}



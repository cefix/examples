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
#include <cefix/WindowRotation.h>
#include <cefix/Quad2DGeometry.h>


class RotateCallback : public osg::NodeCallback {
public:
	RotateCallback(osg::MatrixTransform* mat) : osg::NodeCallback(), _mat(mat) {}
	
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
	
		_angle += 0.01;
		_mat->setMatrix(osg::Matrix::rotate(_angle, osg::Z_AXIS) * osg::Matrix::rotate(_angle/2, osg::X_AXIS));
		
		traverse(node, nv);
	};
	
private:
	osg::MatrixTransform* _mat;
	float _angle;
};


class MyAppController: public cefix::AppController {

	public:
		
		void createOffscreenRendering(const osg::Vec4& clearColor, float w, float h);
		
		osg::Group * createWorld() 
		{
			createOffscreenRendering(osg::Vec4(0.2,0.2,0.2,0.1), 800, 200);
			
			// viewmatrix setzen
			_camera->setViewMatrix(osg::Matrix::translate(osg::Vec3(0,0,-10)));
			
			// szene, die in die textur gerendert werden 
			{
				osg::Geode* geode = new osg::Geode();
				osg::ShapeDrawable* shapedrawable = new osg::ShapeDrawable(new osg::Box(osg::Vec3(0,0,0), 1));
				shapedrawable->setColor(osg::Vec4(1,0,0,1));
				geode->addDrawable(shapedrawable);
				
				osg::MatrixTransform* mat = new osg::MatrixTransform();
				mat->setUpdateCallback(new RotateCallback(mat));
				mat->addChild(geode);
				_camera->addChild(mat);
			}
			
			osg::Group* g = new osg::Group();
			g->addChild(_camera.get());
			for(unsigned int i = 0; i < 10; ++i) {
				osg::MatrixTransform* mat = new osg::MatrixTransform();
				mat->setMatrix(osg::Matrix::translate(0,0,i) * osg::Matrix::rotate(i*30 / osg::PI, osg::Vec3(0,0,1)));
				
				g->addChild(mat);

				{
					cefix::Quad2DGeometry* quad = new cefix::Quad2DGeometry(0,0,800,200);
					quad->setTexture(_colortexture.get());
					quad->setTextureRect(0,0,800,200);
					osg::Geode* geode = new osg::Geode();
					geode->addDrawable(quad);
					geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
					geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
					mat->addChild(geode);
				}
				
				g->addChild(mat);

				{
					cefix::Quad2DGeometry* quad = new cefix::Quad2DGeometry(900,0,800,200);
					quad->setTexture(_depthtexture.get());
					quad->setTextureRect(0,0,800,200);
					osg::Geode* geode = new osg::Geode();
					geode->addDrawable(quad);
					geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);
					geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
					mat->addChild(geode);
				}
			}
			return g;
		}
		
	private:
		osg::ref_ptr<osg::Camera> _camera;
		osg::ref_ptr<osg::TextureRectangle> _colortexture, _depthtexture;

};


void MyAppController::createOffscreenRendering(const osg::Vec4& clearColor, float width, float height) 
{
	cefix::WindowRotation w_rot(getMainWindow()->getRotation());
	w_rot.setViewport(0, 0, width, height);

	_camera = new osg::Camera();
	_camera->setClearColor(clearColor);
	_camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	osg::ref_ptr<osg::Viewport> vp = w_rot.getViewport();

	_camera->setViewport( vp.get() );
		
	// set up projection.
	_camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);

	float hfov = 0;
	float vfov = 0;
	double ratio = (height) / (double)(width);
	
	if ((hfov == 0) && (vfov == 0))
		hfov = 60.0f;
	
	if ((hfov == 0) || (vfov == 0)) {
		if (hfov == 0)
			hfov = osg::RadiansToDegrees(2.0 * atan (tan(osg::DegreesToRadians(vfov)/2.0) * (1/ratio) ));
		else if (vfov == 0)
			vfov = osg::RadiansToDegrees(2.0 * atan (tan(osg::DegreesToRadians(hfov)/2.0) * (ratio) ));
	}
	
	_camera->setProjectionMatrix(w_rot.computeProjectionMatrix(hfov, vfov, 1, 1000));
	
	_camera->setViewMatrix(osg::Matrix::identity());
	
	// set the camera to render before the main camera.
	_camera->setRenderOrder(osg::Camera::POST_RENDER);

	_camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);

	
	//Textur aufsetzen
	{
		osg::TextureRectangle* tex = new osg::TextureRectangle;
		tex->setTextureSize(width, height);
		tex->setInternalFormat(GL_RGBA);
		tex->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
		tex->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);	
		
		_camera->attach(osg::Camera::COLOR_BUFFER, tex, 0, 0, false, 4, 4);
		
		_colortexture = tex;
	}
	{
		osg::TextureRectangle* tex = new osg::TextureRectangle;
		tex->setTextureSize(width, height);
		tex->setInternalFormat(GL_DEPTH_COMPONENT);
		tex->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
		tex->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);	
		
		_camera->attach(osg::Camera::DEPTH_BUFFER, tex);
		
		_depthtexture = tex;
	}
	
}


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

	return 0;
}



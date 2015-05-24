/*
 *  Sketch.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 11.05.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "Sketch.h"
#include <cefix/Quad2DGeometry.h>
#include <cefix/DisplayCollection.h>
#include <cefix/TextGeode.h>
#include <cefix/RttTools.h>
#include <cefix/RttUnit.h>
#include <cefix/ShaderTools.h>

static float max_uniform_val = 100.0f;
static bool do_rotate = true;


class RotateCallback : public osg::NodeCallback {
public:
	RotateCallback(osg::MatrixTransform* mat, const osg::Vec3& v, float speed = 0.01) : osg::NodeCallback(), _mat(mat), _v(v), _speed(speed) { _angle = 0; }
	
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
	
		if (do_rotate) 
			_angle += _speed;
		_mat->setMatrix(osg::Matrix::rotate(_angle, osg::Z_AXIS) * osg::Matrix::rotate(_angle/2, osg::X_AXIS) * osg::Matrix::translate(_v));
		
		traverse(node, nv);
	};
	
private:
	osg::MatrixTransform* _mat;
	osg::Vec3 _v;
	float _angle,_speed;
};


void Sketch::createSceneGraph1(osg::Node* d3, osg::Node* d2)
{
	unsigned int w(1024), h(768);
	
	if (0) {
		cefix::Display* display = new cefix::Display("offscreen_1");
		display->setViewport(0,0, w, h);
		display->set2DConfiguration(cefix::Display::Configuration2D(w, h));
		display->set3DConfiguration(cefix::Display::Configuration3D(60, 40, 1, 1000));
		
		osg::TextureRectangle* tex = cefix::RttTools::createTextureRectangle(display, osg::Camera::COLOR_BUFFER, GL_RGBA, false, 4);
		
		cefix::Quad2DGeometry* quad = new cefix::Quad2DGeometry(0,0, w, h);
		quad->setTexture(tex);
		quad->setTextureRect(0,0,800,600);
		
		osg::Geode* quad_geode = new osg::Geode();
		quad_geode->addDrawable(quad);
		getWorld()->addChild(quad_geode);
	}
	
	osg::ref_ptr<cefix::Quad2DGeometry> step_1_quad, step_2_quad, step_3_quad;
	// 1. step 3d-welt rendern
	{
		// RTT-Textur erzeugen und einem erzeugten Quad zuweisen
		step_1_quad = cefix::RttTools::createQuadWithTextureRectangle("step_1", w, h, osg::Camera::COLOR_BUFFER, GL_RGBA, false, 4);
		
		// Display holen und 2d + 3d welt dazuklemmen
		cefix::Display* display = cefix::DisplayCollection::instance()->getDisplay("step_1");
		display->get3DWorld()->addChild(d3);
		display->get2DWorld()->addChild(d2);
		
		_camera = display->get3DCamera();
		_camera->setViewMatrix(osg::Matrix::lookAt(osg::Vec3(3, 0, 0), osg::Vec3(0,0,0), osg::Vec3(0,1,0)));
	}
	
	// 2. textur aus step 1 rendern und horizontal blur shader anwenden
	{
		step_2_quad = cefix::RttTools::createQuadWithTextureRectangle("step_2", w, h, osg::Camera::COLOR_BUFFER, GL_RGBA);
		
		cefix::Display* display = cefix::DisplayCollection::instance()->getDisplay("step_2");
		
		osg::Geode* geode = cefix::RttTools::createGeode(step_1_quad);
		osg::StateSet* ss = geode->getOrCreateStateSet();
		cefix::ShaderTools::attachShaders("shaders/simpleBlurHorizontal", ss);
		
		_blurAmount = new osg::Uniform("blurAmnt", 3.0f);
		ss->addUniform(_blurAmount);
		
		display->get2DWorld()->addChild(geode);

	}
	
	// 3. textur aus step 2 rendern und vertical blur shader anwenden
	{
		step_3_quad = cefix::RttTools::createQuadWithTextureRectangle("step_3", w, h, osg::Camera::COLOR_BUFFER, GL_RGBA);
		
		cefix::Display* display = cefix::DisplayCollection::instance()->getDisplay("step_3");
		
		osg::Geode* geode = cefix::RttTools::createGeode(step_2_quad);
		osg::StateSet* ss = geode->getOrCreateStateSet();
		cefix::ShaderTools::attachShaders("shaders/simpleBlurVertical", ss);
		
		ss->addUniform(_blurAmount);
		
		display->get2DWorld()->addChild(geode);

	}
	
	// 4. textur aus step 3 auf den screen rendern
	{
		getWorld()->addChild(cefix::DisplayCollection::instance()->getDisplay("step_1")->get3DCamera());
		getWorld()->addChild(cefix::DisplayCollection::instance()->getDisplay("step_2")->get3DCamera());
		getWorld()->addChild(cefix::DisplayCollection::instance()->getDisplay("step_3")->get3DCamera());
		get2DLayer()->addChild(cefix::RttTools::createGeode(step_3_quad));
	}
}

void Sketch::createBlurringSceneGraph1(osg::Node* d3, osg::Node* d2) 
{
	unsigned int w(1024), h(768);
	float sigma = 1000.0f;
	
	// Step 1
	cefix::SceneRttUnit* step_1 = new cefix::SceneRttUnit("step_1", w, h, true);
	step_1->createOutputTexture("color_buffer", osg::Camera::COLOR_BUFFER, GL_RGBA, false, 4);
	step_1->createOutputTexture("adepth_buffer", osg::Camera::DEPTH_BUFFER, GL_DEPTH_COMPONENT);
	
	step_1->get3DWorld()->addChild(d3);
	step_1->get2DWorld()->addChild(d2);
	step_1->getSceneCamera()->setComputeNearFarMode(osg::Camera::DO_NOT_COMPUTE_NEAR_FAR);
    
	_camera = step_1->getSceneCamera();
	
    
	// Step 2
	cefix::RenderRttUnit* step_2 = new cefix::RenderRttUnit("step_2", w, h, true);
	step_2->createOutputTexture("color_buffer", osg::Camera::COLOR_BUFFER, GL_RGBA);
	
	
	step_2->addInput(step_1);
	step_2->attachShaders("shaders/gauss_convolution_1d_x");
	
	_blurAmount = new osg::Uniform("radius", 3.0f);
	_sigma = new osg::Uniform("sigma", 10.0f);
	step_2->attachUniform(_blurAmount);
	step_2->attachUniform(_sigma);
	step_2->attachUniform("color_buffer");
	
	//step_1->get2DWorld()->addChild(cefix::RttTools::createGeode(step_2->getOutputAsQuad("color_buffer", 200, 100)));
	
	// Step3 
	
	cefix::RenderRttUnit* step_3 = new cefix::RenderRttUnit("step_3", w, h, true);
	step_3->createOutputTexture("color_buffer", osg::Camera::COLOR_BUFFER, GL_RGBA);
    
	step_3->addInput(step_2);
	
	step_3->attachShaders("shaders/gauss_convolution_1d_y");
	
	
	step_3->attachUniform(_blurAmount);
	step_3->attachUniform(_sigma);
	step_3->attachUniform("color_buffer");
	
	// Result
	
	getWorld()->addChild(step_1);
	
	get2DLayer()->addChild(cefix::RttTools::createGeode(step_3->getOutputAsQuad("color_buffer")));	
	
	if (0) {
		cefix::RttUnit* units[3] = { step_1, step_2, step_3 };
		osg::Geode* geode = new osg::Geode();
		
		for(unsigned int i = 0; i < 3; ++i) {
			cefix::Quad2DGeometry* geo = units[i]->getOutputAsQuad("color_buffer", osg::Vec4(i * w/3, 0,i*w/3 + w/3, h/3));
			geo->setLocZ(100);
			geode->addDrawable(geo);
		}
		get2DLayer()->addChild(geode);
	}
}



void Sketch::createBlurringSceneGraphWithPrecomputedKernel(osg::Node* d3, osg::Node* d2) 
{
	unsigned int w(1024), h(768);
	float sigma = 1000.0f;
    
    osg::Texture1D* gaussian_kernel_tex = cefix::ShaderTools::precomputeGaussianKernel(256, sigma);
	_gaussianKernel = gaussian_kernel_tex->getImage();
    
	// Step 1
	cefix::SceneRttUnit* step_1 = new cefix::SceneRttUnit("step_1", w, h, true);
	step_1->createOutputTexture("color_buffer", osg::Camera::COLOR_BUFFER, GL_RGBA, false, 4);
	step_1->createOutputTexture("adepth_buffer", osg::Camera::DEPTH_BUFFER, GL_DEPTH_COMPONENT);
	
	step_1->get3DWorld()->addChild(d3);
	step_1->get2DWorld()->addChild(d2);
	step_1->getSceneCamera()->setComputeNearFarMode(osg::Camera::DO_NOT_COMPUTE_NEAR_FAR);
		
	_camera = step_1->getSceneCamera();
	
			
	// Step 2
	cefix::RenderRttUnit* step_2 = new cefix::RenderRttUnit("step_2", w, h, true);
	step_2->createOutputTexture("color_buffer", osg::Camera::COLOR_BUFFER, GL_RGBA);
	
	
	step_2->addInput(step_1);
    step_2->addInput("gaussian_kernel", gaussian_kernel_tex);
	step_2->attachShaders("shaders/precomputed_gauss_convolution_1d_x");
	
	_blurAmount = new osg::Uniform("radius", 3.0f);
	_sigma = new osg::Uniform("sigma", 10.0f);
	step_2->attachUniform(_blurAmount);
	step_2->attachUniform(_sigma);

    
	 
	//step_1->get2DWorld()->addChild(cefix::RttTools::createGeode(step_2->getOutputAsQuad("color_buffer", 200, 100)));
	
	// Step3 
	
	cefix::RenderRttUnit* step_3 = new cefix::RenderRttUnit("step_3", w, h, true);
	step_3->createOutputTexture("color_buffer", osg::Camera::COLOR_BUFFER, GL_RGBA);

	step_3->addInput(step_2);
	step_3->addInput("gaussian_kernel", gaussian_kernel_tex);
	step_3->attachShaders("shaders/precomputed_gauss_convolution_1d_y");
	
	
	step_3->attachUniform(_blurAmount);
	step_3->attachUniform(_sigma);

	
	// Result
	
	getWorld()->addChild(step_1);
	
	get2DLayer()->addChild(cefix::RttTools::createGeode(step_3->getOutputAsQuad("color_buffer")));	
	
	if (0) {
		cefix::RttUnit* units[3] = { step_1, step_2, step_3 };
		osg::Geode* geode = new osg::Geode();
		
		for(unsigned int i = 0; i < 3; ++i) {
			cefix::Quad2DGeometry* geo = units[i]->getOutputAsQuad("color_buffer", osg::Vec4(i * w/3, 0,i*w/3 + w/3, h/3));
			geo->setLocZ(100);
			geode->addDrawable(geo);
		}
		get2DLayer()->addChild(geode);
	}
}


void Sketch::createDOFSceneGraph(osg::Node* d3, osg::Node* d2) 
{
	unsigned int w(1024), h(768);
	
	// Step 1
	cefix::SceneRttUnit* step_1 = new cefix::SceneRttUnit("step_1", w, h, true);
	step_1->createOutputTexture("color_buffer", osg::Camera::COLOR_BUFFER, GL_RGBA, false, 4);
	step_1->createOutputTexture("depth_buffer", osg::Camera::DEPTH_BUFFER, GL_DEPTH_COMPONENT,false, 4);
	
	step_1->get3DWorld()->addChild(d3);
	step_1->get2DWorld()->addChild(d2);
	step_1->getSceneCamera()->setComputeNearFarMode(osg::Camera::DO_NOT_COMPUTE_NEAR_FAR);
		
	_camera = step_1->getSceneCamera();
	
	_blurAmount = new osg::Uniform("focus", 3.0f);
	
	// Step 1
	cefix::RenderRttUnit* step_2 = new cefix::RenderRttUnit("step_2", w, h, true);
	step_2->createOutputTexture("color_buffer", osg::Camera::COLOR_BUFFER, GL_RGBA);
	
	step_2->addInput(step_1);
	step_2->attachUniform(_blurAmount);
	step_2->attachShaders("shaders/dof");
	
	// reinhängen
	
	getWorld()->addChild(step_1);
	
	get2DLayer()->addChild(cefix::RttTools::createGeode(step_2->getOutputAsQuad("color_buffer")));	

	max_uniform_val = 1.0f;
}


void Sketch::createDOFSceneGraph2(osg::Node* d3, osg::Node* d2) 
{
	unsigned int w(1024), h(768);
	
	std::vector<osg::ref_ptr<cefix::RttUnit> > steps;
	cefix::RttUnit* blurred_small  = NULL;
	cefix::RttUnit* blurred_smaller = NULL;
	
	
	// Step 1
	{
		cefix::SceneRttUnit* step = new cefix::SceneRttUnit("step_1", w, h, true);
		step->createOutputTexture("color_buffer", osg::Camera::COLOR_BUFFER, GL_RGBA, false, 4);
		step->createOutputTexture("depth_buffer", osg::Camera::DEPTH_BUFFER, GL_DEPTH_COMPONENT,false, 4);
		
		step->get3DWorld()->addChild(d3);
		step->get2DWorld()->addChild(d2);
		step->getSceneCamera()->setComputeNearFarMode(osg::Camera::DO_NOT_COMPUTE_NEAR_FAR);
		
		_camera = step->getSceneCamera();

		
		steps.push_back(step);
	}
	{
		// 1/2 kleiner + blur
		
		cefix::RenderRttUnit* blur_hor = new cefix::RenderRttUnit("half_step_2", w/2, h/2, true);
		blur_hor->createOutputTexture("color_buffer", osg::Camera::COLOR_BUFFER, GL_RGBA);
		blur_hor->addInput(steps[0]);
		blur_hor->attachShaders("shaders/gauss_convolution_1d_x");
		
		osg::Uniform* blurAmount = new osg::Uniform("radius", 7.0f);
		osg::Uniform* sigma = new osg::Uniform("sigma", 4.0f);
		
		blur_hor->attachUniform(blurAmount);
		blur_hor->attachUniform(sigma);
		blur_hor->attachUniform("color_buffer");

		cefix::RenderRttUnit* blur_ver = new cefix::RenderRttUnit("half_step_3", w/2, h/2, true);
		blur_ver->createOutputTexture("color_buffer", osg::Camera::COLOR_BUFFER, GL_RGBA);
		blur_ver->addInput(blur_hor);
		blur_ver->attachShaders("shaders/gauss_convolution_1d_y");
		
		blur_ver->attachUniform(blurAmount);
		blur_ver->attachUniform(sigma);
		blur_ver->attachUniform("color_buffer");
		
		blurred_small = blur_ver;
		steps.push_back(blur_hor);
		steps.push_back(blur_ver);
	}
	
	{
		// 1/4 kleiner + blur
		
		cefix::RenderRttUnit* blur_hor = new cefix::RenderRttUnit("quarter_step_2", w/4, h/4, true);
		blur_hor->createOutputTexture("color_buffer", osg::Camera::COLOR_BUFFER, GL_RGBA);
		blur_hor->addInput(steps[0]);
		blur_hor->attachShaders("shaders/gauss_convolution_1d_x");
		
		osg::Uniform* blurAmount = new osg::Uniform("radius", 7.0f);
		osg::Uniform* sigma = new osg::Uniform("sigma", 4.0f);
		
		blur_hor->attachUniform(blurAmount);
		blur_hor->attachUniform(sigma);
		blur_hor->attachUniform("color_buffer");

		cefix::RenderRttUnit* blur_ver = new cefix::RenderRttUnit("quarter_step_3", w/4, h/4, true);
		blur_ver->createOutputTexture("color_buffer", osg::Camera::COLOR_BUFFER, GL_RGBA);
		blur_ver->addInput(blur_hor);
		blur_ver->attachShaders("shaders/gauss_convolution_1d_y");
		
		blur_ver->attachUniform(blurAmount);
		blur_ver->attachUniform(sigma);
		blur_ver->attachUniform("color_buffer");
		
		blurred_smaller = blur_ver;
		steps.push_back(blur_hor);
		steps.push_back(blur_ver);
	}

	{
		// so jetzt alles zusammenfügen
		cefix::RenderRttUnit* result = new cefix::RenderRttUnit("result", w, h, true);
		result->createOutputTexture("color_buffer", osg::Camera::COLOR_BUFFER, GL_RGBA);
		
		result->addInput("color_buffer", steps[0]->getOutput("color_buffer"));
		result->addInput("depth_buffer", steps[0]->getOutput("depth_buffer"));
		result->addInput("color_buffer_blurred", blurred_small->getOutput("color_buffer"));
		result->addInput("color_buffer_blurred_more", blurred_smaller->getOutput("color_buffer"));
		
		_blurAmount = new osg::Uniform("focal_length", 0.0f);
		_sigma = new osg::Uniform("focal_range", 0.0f);
		
		result->attachUniform(_blurAmount);
		result->attachUniform(new osg::Uniform("focal_range", 10.0f));
		result->attachUniform(new osg::Uniform("zNear", 1.0f));
		result->attachUniform(new osg::Uniform("zFar", 1000.0f));
		result->attachShaders("shaders/better_dof");
		
		steps.push_back(result);
		
		max_uniform_val = 10.0f;
		
		getWorld()->addChild(steps[0]);
		
		steps[0]->addChild(result);
		
		get2DLayer()->addChild(cefix::RttTools::createGeode(result->getOutputAsQuad("color_buffer", osg::Vec4(0,0,w,h))));	
	}
	
	if (1) {
		
		osg::Geode* geode = new osg::Geode();
		unsigned int m(steps.size());
		for(unsigned int i = 0; i < m; ++i) {
			cefix::Quad2DGeometry* geo = steps[i]->getOutputAsQuad("color_buffer", osg::Vec4(i * w/m, 0,i*w/m + w/m, h/m));
			geo->setLocZ(100);
			geode->addDrawable(geo);
		}
		get2DLayer()->addChild(geode);
	}
}



void Sketch::setup() 
{
	// set window size
	size(1024, 768);
	
	osg::MatrixTransform* root_mat = new osg::MatrixTransform();
	root_mat->setUpdateCallback(new RotateCallback(root_mat, osg::Vec3(0,0,0), 0.005));
	
	// create a box + drawable
	for(unsigned int i = 0; i < 100; ++i) {
		float dx = 3;
		osg::Box* box = new osg::Box(osg::Vec3(0, 0, 0), 0.3f);
		osg::Drawable* drawable = new osg::ShapeDrawable(box);
		osg::Geode* geode = new osg::Geode();
		geode->addDrawable(drawable);
		osg::MatrixTransform* mat = new osg::MatrixTransform();
		mat->addChild(geode);
		mat->setUpdateCallback(new RotateCallback(mat, osg::Vec3(cefix::in_between(-dx, dx), cefix::in_between(-dx, dx), cefix::in_between(-dx, dx)), cefix::in_between(0.01, 0.1)));
		
		root_mat->addChild(mat);
	}
	
	cefix::Utf8TextGeode* tg = new cefix::Utf8TextGeode("system.xml", 8, "huhu");
	tg->setPosition(osg::Vec3(10,10,10));
	tg->setTextColor(osg::Vec4(1,1,1,1));
	
	//createBlurringSceneGraph1(root_mat, tg);
	//createBlurringSceneGraphWithPrecomputedKernel(root_mat, tg);
	createDOFSceneGraph2(root_mat, tg);
	
	
	
	osgDB::writeNodeFile(*getWorld(), "/Users/stephan/Desktop/camera.osg");
	
	// allow osg-event handler:
	allowOsgHandler(true);
	
	_down = 0;
}







void Sketch::draw()
{
	if(_camera) _camera->setViewMatrix(getMainWindow()->getTrackballManipulator()->getInverseMatrix());
	osg::Vec4 c(0,0,0,1);
	c[0] = mouseX / 1024.0;
	c[1] = mouseY / 768.0;
	c[2] = _down;
	c[3] = 1.0f;
	
	//_camera->setClearColor(c);
	_blurAmount->set(mouseX / 1024.0f * max_uniform_val);
	_sigma->set(mouseY / 768.0f * max_uniform_val);
	
	
	float blur, sigma;
	_blurAmount->get(blur);
	_sigma->get(sigma);
	std::cout << "blur: " << blur << " sigma: " << sigma << std::endl;
    
    if(_gaussianKernel.valid()) 
    {
        cefix::ShaderTools::precomputeGaussianKernel(_gaussianKernel, 256, sigma);
    }
}



void Sketch::mouseReleased() 
{
	_down = 0;
	do_rotate = true;
}

void Sketch::mousePressed() 
{
	_down = 1;
	do_rotate = false;
}


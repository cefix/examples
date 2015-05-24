/*
 *  Sketch.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 11.05.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */


#include "Sketch.h"
#include <cefix/Draggable.h>
#include <cefix/EulerAngle.h>
#include <cefix/MathUtils.h>

static std::vector<osg::Vec2> source;


class CornerDraggable : public osg::Referenced, public cefix::Draggable {
public: 
    CornerDraggable(cefix::Quad2DGeometry* geo) : osg::Referenced(), cefix::Draggable(), _geo(geo) {}
    
    virtual bool dragStarted() 
    {
        setBlockSuccessiveHandler(true);
		_startP = _geo->getCenter();
        return true;
    }
    
    virtual bool drag(const osg::Vec3& delta) {
        _geo->setCenter(delta[0] + _startP[0], delta[1] + _startP[1]);
        return true;
    }
private:   
    cefix::Quad2DGeometry* _geo;
    osg::Vec2 _startP;
};


void Sketch::setup() 
{
	// set window size
	size(1150, 768);
	
    {
        osg::Geode* geode = new osg::Geode();
        
        _back = new cefix::Quad2DGeometry(0,0, 1150,768);
        _back->setTextureFromImage(osgDB::readImageFile("test.jpg"), false);
        _back->setTextureRect(0,0,1,1);
        _back->getOrCreateStateSet()->setRenderBinDetails(-1, "RenderBin");
        _back->setColor(osg::Vec4(1,1,1,0.3));
        
        geode->addDrawable(_back);
        get2DLayer()->addChild(geode);
        
        _shape = new cefix::LineStripGeometry(true, 4);
        _shape->setColor(osg::Vec4(1,1,1,0.5));
        _shape->getOrCreateStateSet()->setRenderBinDetails(20, "DepthSortedBin");
        
        geode->addDrawable(_shape);

    }
    
    float x[4] = { 0, 1150, 1150, 0 };
    float y[4] = { 0, 0, 768, 768};
    source.resize(4);
    
    for(unsigned int i = 0; i < 4; ++i) {
        cefix::Quad2DGeometry* geo = new cefix::Quad2DGeometry(-5, -5, 10, 10);
        geo->setCenter(x[i], y[i]);
        geo->getOrCreateStateSet()->setRenderBinDetails(20, "DepthSortedBin");
        osg::Geode* geode = new osg::Geode();
        geode->addDrawable(geo);
        get2DLayer()->addChild(geode);
        
        _corners.push_back(geo);
        
        geode->setUserData(new CornerDraggable(geo));
        source[i].set(x[i], y[i]);
    }
    
    {
        osg::Geode* geode = new osg::Geode();
        cefix::Quad2DGeometry* drawable = new cefix::Quad2DGeometry(0,0, 1150, 768);
		drawable->setTextureFromImage(osgDB::readImageFile("test.jpg"), false);
        drawable->setTextureRect(0,0,1,1);
		
		drawable->setColor(osg::Vec4(0.3,0.3,1,0.8));
		geode->addDrawable(drawable);
        //getWorld()->addChild(geode);
		osg::Group* parent = get2DLayer();
		for(unsigned int i = 0; i < 10; ++i) 
		{
			
			osg::MatrixTransform* mat = new osg::MatrixTransform();
			mat->addChild(geode);
			parent->addChild(mat);
			parent = mat;
			_mats.push_back(mat);
		}
        
    }
       
	allowOsgHandler(true);
	
	_down = 0;
}

void Sketch::update()
{
	_shape->clear();
    std::vector<osg::Vec2> dest;
    
    for(unsigned int i = 0; i < _corners.size(); ++i) {
        osg::Vec2 c(_corners[i]->getCenter());
        _shape->add(osg::Vec3(c[0], c[1], 0));
        dest.push_back(osg::Vec2(c[0], c[1]));
    }
    for(unsigned int i = 0; i < 4; ++i) {
		std::cout << source[i] << " -> " << dest[i] << std::endl;
	}
    osg::Matrix matrix;
    
    matrix = cefix::findHomography(source, dest);
    osg::Quat rot = matrix.getRotate();
	
	for(unsigned int i = 0; i < _mats.size(); ++i)
		_mats[i]->setMatrix(matrix);


    std::cout <<  "rot  : " << cefix::toEuler(rot) << std::endl;
    std::cout <<  "trans: " << matrix.getTrans() << std::endl;
}



void Sketch::mouseReleased() 
{
}

void Sketch::mousePressed() 
{
}


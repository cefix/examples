/*
 *  Sketch.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 11.05.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "Sketch.h"
#include "SuddenMotionSensorInputDevice.h"
#include <osg/io_utils>
#include <osg/ValueObject>

class SMSEventHandler : public osgGA::GUIEventHandler {
public:
    SMSEventHandler(osg::MatrixTransform* mat)
        : osgGA::GUIEventHandler()
        , _mat(mat)
    {
    }
    
    virtual bool handle (const osgGA::GUIEventAdapter & ea, osgGA::GUIActionAdapter & aa) {
        
        if ((ea.getEventType() == osgGA::GUIEventAdapter::USER) && (ea.getName() == "sms")) {
            osg::Vec3d value;
            if (ea.getUserValue("acceleration", value)) {
                _mat->setMatrix(osg::Matrix::rotate(
                -value[0]*osg::PI_2, osg::X_AXIS,
                value[1]*osg::PI_2, osg::Y_AXIS,
                0, osg::Z_AXIS));
                return true;
            }
        }
        
        return false;
    }
private:
    osg::ref_ptr<osg::MatrixTransform> _mat;
    
};

void Sketch::setup() 
{
	// set window size
	size(1024, 768);
	
	// create a box + drawable
	osg::Box* box = new osg::Box();
	_drawable = new osg::ShapeDrawable(box);
	osg::Geode* geode = new osg::Geode();
    osg::MatrixTransform* mat = new osg::MatrixTransform();
    mat->addChild(geode);
	geode->addDrawable(_drawable.get());
	getWorld()->addChild(mat);
    
    getMainWindow()->addEventHandler(new SMSEventHandler(mat));
	
	// allow osg-event handler:
	allowOsgHandler(true);
	
	_down = 0;
    
    osgGA::Device* device = new SuddenMotionInputDevice();
    getMainWindow()->addDevice(device);
}

void Sketch::draw()
{
	osg::Vec4 c(0,0,0,1);
	c[0] = mouseX / 1024.0;
	c[1] = mouseY / 768.0;
	c[2] = _down;
	
	_drawable->setColor(c);

}



void Sketch::mouseReleased() 
{
	_down = 0;
}

void Sketch::mousePressed() 
{
	_down = 1;
}


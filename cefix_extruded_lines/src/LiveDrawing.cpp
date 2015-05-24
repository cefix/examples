//
//  LiveDrawing.cpp
//  cefixSketch
//
//  Created by Stephan Huber on 07.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "LiveDrawing.h"
#include <cefix/AnimationFactory.h>

class LiveDrawingAnimation : public cefix::AnimationBase {
public:
    LiveDrawingAnimation(LiveDrawing* parent) : cefix::AnimationBase(0), _parent(parent) {}
    
    virtual void animate(float elapsed) {
        _parent->update();
    }
    using AnimationBase::setFinishedFlag;
private:
    LiveDrawing* _parent;
};




LiveDrawing::LiveDrawing() 
:   cefix::WindowPickable(),
    _enabled(true)
{
    _line = new Line();
    //_debugLine = new DebugLine();
    
    _node = new osg::Geode();
    _node->addDrawable(_line);
    if (_debugLine) _node->addDrawable(_debugLine);
    
    osg::Image* img = osgDB::readImageFile("black_dot2.png");
    if (img) {
        osg::Texture2D* tex = new osg::Texture2D(img);
        _line->getOrCreateStateSet()->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON);
    }
}


void LiveDrawing::mouseDown()
{
    if (!_enabled) return;
    _line->getColorPolicy().setColor(osg::Vec4(0,0,0,0.3));
    _buffer.clear();
    _line->clear();
    if (_debugLine) _debugLine->clear();
    _animation = new LiveDrawingAnimation(this);
    cefix::AnimationFactory::instance()->add(_animation);
    
    _lastP.set(getScreenX(), getNativeY());
}

void LiveDrawing::mouseUp()
{
    if (!_enabled) return;
    
    if (_animation.valid()) {
        static_cast<LiveDrawingAnimation*>(_animation.get())->setFinishedFlag();
        _animation = NULL;
    }
}
  
void LiveDrawing::update()
{
    osg::Vec2 p(getScreenX(), getNativeY());
    _buffer.add(p -_lastP);
    float w = std::max(2.0f, _buffer.getAverage().length());
    
    _line->add(cefix::ExtrudedLine::Point(osg::Vec3(p[0], p[1],0), w, w));
    _line->update();
    
    if (_debugLine) _debugLine->add(cefix::ExtrudedLine::Point(osg::Vec3(p[0], p[1],0), w, w));
    if (_debugLine) _debugLine->update();
    
    _lastP = p;
}
    
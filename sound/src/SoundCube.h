/*
 *  SoundCube.h
 *  sound
 *
 *  Created by Stephan Huber on 20.01.11.
 *  Copyright 2011 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#ifndef SOUND_CUBE_HEADER
#define SOUND_CUBE_HEADER

#include <osg/Geode>
#include <osg/ShapeDrawable>
#include <cefix/Sound.h>
#include <cefix/Pickable.h>

class SoundCube : public osg::Geode, public cefix::Pickable {

public:
    SoundCube(const osg::Vec3& center, const std::string& sound_file);
    
    virtual bool mouseEnter() {
        _drawable->setColor(osg::Vec4(1,0,0,1));
        return cefix::Pickable::mouseEnter();
    }
    
    virtual bool mouseLeave() {
        _drawable->setColor(osg::Vec4(1,!_sound->isPlaying(),1,1));
        return cefix::Pickable::mouseLeave();
    }
    
    virtual bool mouseDown() {
        if (_sound->isPlaying()) 
            _sound->pause();
        else 
            _sound->play();
            
        return cefix::Pickable::mouseDown();
    }
    
private:
    osg::ref_ptr<cefix::Sound> _sound;
    
    osg::ref_ptr<osg::ShapeDrawable> _drawable;
};


#endif
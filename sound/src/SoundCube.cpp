/*
 *  SoundCube.cpp
 *  sound
 *
 *  Created by Stephan Huber on 20.01.11.
 *  Copyright 2011 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "SoundCube.h"
#include <cefix/SoundManager.h>

SoundCube::SoundCube(const osg::Vec3& center, const std::string& sound_file)
:   osg::Geode()
{
    _sound = cefix::SoundManager::readSoundFile(sound_file, false);
    _sound->setLooping(true);
    _drawable = new osg::ShapeDrawable(new osg::Box(center, 0.5));
    addDrawable(_drawable);
}

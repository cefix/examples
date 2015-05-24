//
//  LiveDrawing.h
//  cefixSketch
//
//  Created by Stephan Huber on 07.03.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include <cefix/WindowPickable.h>
#include <cefix/RingBuffer.h>
#include <cefix/AnimationBase.h>
#include "ExtrudedLineGeometry.h"
#include "TexturedLineWithCapsGeometry.h"

class TexturedLineTexturePolicy : public cefix::ExtrudedLine::TexturePolicyBase {
public:
    TexturedLineTexturePolicy()
    :   cefix::ExtrudedLine::TexturePolicyBase(),
        _texCoords(new osg::Vec2Array())
    {
    }
    
    void setupTextureCoords(cefix::ExtrudedLine::BaseGeometry* geo, unsigned int num_vertices_per_slice) 
    {
        geo->setTexCoordArray(0, _texCoords);
        _numVerticesPerSlice = num_vertices_per_slice;
        
    }
    void beginTextureCoords(cefix::ExtrudedLine::BaseGeometry* geo, unsigned int num_vertices_per_slice) {
        _texCoords->resize(geo->getNumLineVertices() * num_vertices_per_slice);
        _numVerticesPerSlice = num_vertices_per_slice;
    }
    
    void computeTextureCoords(cefix::ExtrudedLine::BaseGeometry* geo, unsigned int i, const cefix::ExtrudedLine::Point& p) 
    {
        const double scalar = 1 / static_cast<double>(_numVerticesPerSlice-1);
        
        for(unsigned int k = 0; k < _numVerticesPerSlice; ++k) {
            (*_texCoords)[i * _numVerticesPerSlice + k].set( i / static_cast<double>(geo->getNumLineVertices()-1), k * scalar);
        }
    }
private:
    osg::Vec2Array* _texCoords;
    unsigned int _numVerticesPerSlice;


};


class LiveDrawing: public cefix::WindowPickable {

public:
    /*
    typedef cefix::ExtrudedLineGeometryT<
        cefix::ExtrudedLine::LineSliceGenerator, 
        cefix::ExtrudedLine::SingleColorPolicy,
        TexturedLineTexturePolicy> Line;
    */
    typedef cefix::TexturedLineWithCapsGeometry Line;
    typedef cefix::ExtrudedLineGeometryT<
        cefix::ExtrudedLine::DebugGenerator> DebugLine;

    
    LiveDrawing();
    
    virtual void mouseDown();
    virtual void mouseUp();
    
    virtual void update();
    osg::Node* getNode() { return _node.get(); }
    
    void setEnabled(bool b) { _enabled = b; }
    bool getEnabled() const { return _enabled; }
    
private:
    osg::ref_ptr<Line> _line;
    osg::ref_ptr<DebugLine> _debugLine;
    osg::ref_ptr<osg::Geode> _node;
    cefix::RingBuffer<osg::Vec2, 10> _buffer;
    osg::ref_ptr<cefix::AnimationBase> _animation;
    osg::Vec2 _lastP;
    bool _enabled;
};
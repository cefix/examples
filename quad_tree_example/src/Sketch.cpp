/*
 *  Sketch.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 11.05.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "Sketch.h"


/** 
 * da cefix::Quad2DGeometry keinen getter getPosition hat, müssen wir hier einen durch 
 * template-spezialisierung erzeugen, damit der tree die position des objekts ermitteln kann
 */
namespace cefix {
template<> inline osg::Vec2 get_position_for_quad_tree(const cefix::Quad2DGeometry& t) 
{ 
	return osg::Vec2(t.getLeft(), t.getTop());
}

}

Sketch::Sketch() 
:	cefix::Sketch(),
	_quadTree(1024, 768)
{

}

void Sketch::setup() 
{
	// set window size
	size(1024, 768);
	
	osg::Geode* geode = new osg::Geode();
	
	// einen haufen rechtecke erzeugen + zufällig platzieren
	for (unsigned int i = 0; i < 500; ++i) 
	{
		osg::Vec2 pos = osg::Vec2(cefix::in_between(0,1024), cefix::in_between(0,768));
		
		cefix::Quad2DGeometry* geo = new cefix::Quad2DGeometry(pos[0] - 5, pos[1] - 5, 10, 10);
		geode->addDrawable(geo);
		_quadTree.add(geo);
	}	
	
	// nur zum visualisieren
	cefix::QuadTreeGeometry* quad_tree_geo = new cefix::QuadTreeGeometry();
	quad_tree_geo->visualize(_quadTree);
	geode->addDrawable(quad_tree_geo);
	
	get2DLayer()->addChild(geode);
}

void Sketch::draw()
{
	const float radius = 150.0f;
	
	// alle alten results zurückstellen
	for(unsigned int i = 0; i < _results.size(); ++i) {
		_results[i]->setColor(osg::Vec4(1,1,1,1));
		_results[i]->dirtyDisplayList();
	}	
	
	// neue results finden
	_results.clear();
	_quadTree.find_in_range(mouseX - radius, mouseY - radius, mouseX + radius, mouseY + radius, _results);
	
	// und einfärben
	for(unsigned int i = 0; i < _results.size(); ++i) {
		_results[i]->setColor(osg::Vec4(0,1,0 ,1));
		_results[i]->dirtyDisplayList();
	}	
}



void Sketch::mouseReleased() 
{
}

void Sketch::mousePressed() 
{
}


/*
 *  Sketch.cpp
 *  cefixSketch
 *
 *  Created by Stephan Huber on 11.05.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#include "Sketch.h"
#include <cefix/MouseLocationProvider.h>

/** 
 * da cefix::Quad2DGeometry keinen getter getPosition hat, müssen wir hier einen durch 
 * template-spezialisierung erzeugen, damit der tree die position des objekts ermitteln kann
 */
namespace cefix {
template<> inline osg::Vec3 get_position_for_oct_tree(const cefix::Quad2DGeometry& t) 
{ 
	return osg::Vec3(t.getLeft(), t.getTop(), t.getLocZ());
}

}

Sketch::Sketch() 
:	cefix::Sketch(),
	_octTree(1000, 1000, 1000)
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
		osg::Vec2 pos = osg::Vec2(cefix::in_between(0,1000), cefix::in_between(0,1000));
		
		cefix::Quad2DGeometry* geo = new cefix::Quad2DGeometry(pos[0] - 5, pos[1] - 5, 10, 10);
		geo->setLocZ(cefix::in_between(0,1000));
		geode->addDrawable(geo);
		_octTree.add(geo);
	}	
	
	// nur zum visualisieren
	cefix::OctTreeGeometry* oct_tree_geo = new cefix::OctTreeGeometry();
	oct_tree_geo->visualize(_octTree);
	geode->addDrawable(oct_tree_geo);
	
	getWorld()->addChild(geode);
	
	allowOsgHandler(true);
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
	osg::Vec3 p( (cefix::MouseLocationProvider::instance()->getFar() + cefix::MouseLocationProvider::instance()->getNear()) / 2.0);
	std::cout << p << std::endl;
	_octTree.find_in_range(p, radius, _results);
	
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


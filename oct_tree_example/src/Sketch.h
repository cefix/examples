/*
 *  Sketch.h
 *  cefixSketch
 *
 *  Created by Stephan Huber on 11.05.09.
 *  Copyright 2009 Stephan Maximilian Huber, digital mind. All rights reserved.
 *
 */

#ifndef SKETCH_HEADER
#define SKETCH_HEADER

#include <cefix/Sketch.h>
#include <cefix/OctTree.h>
#include <cefix/Quad2DGeometry.h>


class Sketch : public cefix::Sketch {
public:
	Sketch();
	void setup();
	void draw();
	
	void mousePressed();
	void mouseReleased();

private:
	cefix::OctTree<
		cefix::Quad2DGeometry, //art des objekts, das im tree gespeicher werden soll
		float, //genereller typ, meist float oder double
		osg::Vec3, //zum bestimmen von abständen, etc
		4, // = maxdepth, wie tief wird verschachtelt
		10 // = MaxChildsBeforeSubDivision, wieviel nodes nimmt eine Zelle auf, bevor sie gesplittet wird
	>	_octTree;
	
	std::vector<cefix::Quad2DGeometry*> _results;
	

};


#endif
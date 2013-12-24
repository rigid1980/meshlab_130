/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *   
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include <GL/glew.h>
#include "mesh_grid.h"
#include <QGLFramebufferObject>
#include <vcg/math/gen_normal.h>
#include <wrap/qt/checkGLError.h>
#include <iostream>
#include <meshlab/mainwindow.h>



#include <vcg/complex/algorithms/create/platonic.h>
#include <vcg/complex/algorithms/create/marching_cubes.h>
#include <vcg/complex/algorithms/create/mc_trivial_walker.h>

using namespace std;
using namespace vcg;

MeshGridPlugin::MeshGridPlugin() 
{ 
	typeList 
	<< FP_MESH_GRID;
	
    foreach(FilterIDType tt , types())
        actionList << new QAction(filterName(tt), this);

}

MeshGridPlugin::~MeshGridPlugin()
{
}

QString MeshGridPlugin::filterName(FilterIDType algoId) const
{
	switch(algoId)
	{
    case FP_MESH_GRID :  return QString("Grid Mesh Generator");
    	default : assert(0); 
	}
	
	return QString("");
}

QString MeshGridPlugin::filterInfo(FilterIDType algoId) const
{
	switch(algoId)
	{
    case  FP_MESH_GRID: return tr("Generate a new 2D Grid mesh with number of vertices on X and Y axis specified by user<br>"
						         "within unit square.");
    
		default : assert(0); 
	}

	return QString("");
}

MeshGridPlugin::FilterClass MeshGridPlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_MESH_GRID :return MeshFilterInterface::Mesh;
    default : assert(0);  
	}
}

int MeshGridPlugin::getRequirements(QAction */*action*/)
{
	//no requirements needed
	return 0;
}

void MeshGridPlugin::initParameterSet(QAction *action, MeshModel &m, RichParameterSet &parlst)
{

    

								
    switch(ID(action))
    {
          case FP_MESH_GRID:
            parlst.addParam(new RichInt("numVertX", 100, "num vertices on x", "number of vertices on x. it must be positive"));
			parlst.addParam(new RichInt("numVertY", 100, "num vertices on y", "number of vertices on y. it must be positive"));
            break;
        default: break; // do not add any parameter for the other algos
    }
	
}
bool MeshGridPlugin::applyFilter(QAction *algo, MeshDocument &md,
                             RichParameterSet & par, vcg::CallBackPos *cb)
{
    md.addNewMesh("","this->filterName(ID(filter))");
  MeshModel &m=*(md.mm());

  int cols = par.getInt("numVertX");
    int rows = par.getInt("numVertY");
	
	float totalw = 1.0;
  float totalh = 1.0;
	int w = cols+1;
    int h = rows+1;
    float wl = 1.0/cols;
    float hl = 1.0/rows;

    qDebug("w %d h %d",w,h);
    if(w <= 0 || h <= 0) {
      return false;
    }


    // use Grid function to generate Grid
    std::vector<float> data(w*h,0);
    tri::Grid<CMeshO>(m.cm, w, h, 1, 1, &data[0]);

    
    {
      // move x and y
      double halfw = double(w-1)/2;
      double halfh = double(h-1)/2;
      double wld = wl/double(w);
      double hld = hl/float(h);


//      CMeshO::VertexIterator vi;
//      for(vi = m.cm.vert.begin(); vi != m.cm.vert.end(); ++vi)
//      {
//         // qDebug("pos x: %f y: %f",(*vi).P()[0],(*vi).P()[1]);
//        //(*vi).P()[0] = (*vi).P()[0] - (wld * halfw);
//        //(*vi).P()[1] = (*vi).P()[1] - (hld * halfh);
//        //  (*vi).P()[0] = (*vi).P()[0] - totalw/2;
//        //  (*vi).P()[1] = (*vi).P()[1] - totalh/2;
//         // qDebug("after pos x: %f y: %f",(*vi).P()[0],(*vi).P()[1]);
//      }
    }
    // update bounding box, normals
 //   Matrix44f rot; rot.SetRotateDeg(180,Point3f(0,1,0));
 //   tri::UpdatePosition<CMeshO>::Matrix(m.cm,rot,false);
    tri::UpdateNormal<CMeshO>::PerVertexNormalizedPerFace(m.cm);
    tri::UpdateNormal<CMeshO>::NormalizePerFace(m.cm);
    tri::UpdateBounding<CMeshO>::Box(m.cm);

    CMeshO::VertexIterator vi;

    
    return true;
}
	

MESHLAB_PLUGIN_NAME_EXPORTER(MeshGridPlugin)

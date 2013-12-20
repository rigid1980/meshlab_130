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
#include "algo_demo.h"
#include <QGLFramebufferObject>
#include <vcg/math/gen_normal.h>
#include <wrap/qt/checkGLError.h>
#include <iostream>
#include <meshlab/mainwindow.h>


using namespace std;
using namespace vcg;

AlgoDemoPlugin::AlgoDemoPlugin() 
{ 
	typeList 
	<< FP_SINGLE_ALGO_DEMO
    << FP_DUAL_ALGO_DEMO;
	
    foreach(FilterIDType tt , types())
        actionList << new QAction(filterName(tt), this);

}

AlgoDemoPlugin::~AlgoDemoPlugin()
{
}

QString AlgoDemoPlugin::filterName(FilterIDType algoId) const
{
	switch(algoId)
	{
    case FP_SINGLE_ALGO_DEMO :  return QString("Algorithm Demo - Single Mesh");
    case FP_DUAL_ALGO_DEMO  :  return QString("Algorithm Demo - Dual Mesh");
		default : assert(0); 
	}
	
	return QString("");
}

QString AlgoDemoPlugin::filterInfo(FilterIDType algoId) const
{
	switch(algoId)
	{
    case  FP_SINGLE_ALGO_DEMO: return QString("demo for single mesh");
    case FP_DUAL_ALGO_DEMO :  return QString("demo for dual mesh");
		default : assert(0); 
	}

	return QString("");
}

AlgoDemoPlugin::FilterClass AlgoDemoPlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_SINGLE_ALGO_DEMO :return MeshFilterInterface::Algorithm;
    case FP_DUAL_ALGO_DEMO : return MeshFilterInterface::Algorithm;
    default : assert(0);  
	}
}

int AlgoDemoPlugin::getRequirements(QAction */*action*/)
{
	//no requirements needed
	return 0;
}

void AlgoDemoPlugin::initParameterSet(QAction *action, MeshModel &m, RichParameterSet &parlst)
{

    MeshDocument *md = m.parent;
	
    if(md == NULL){
        qDebug()<<"meshDOC is NULL";
        return;
    }

    QStringList curvNameList = QStringList();
    for(int ii = 0; ii < md->meshList.size();++ii)
    {
         curvNameList.push_back(md->meshList[ii]->shortName());
    }
    int li = (md->meshList.size()>=1)?0:-1;
    int ri = (md->meshList.size()>=2)?1:-1;


								
    switch(ID(action))
    {
        case FP_DUAL_ALGO_DEMO:
            parlst.addParam(new RichEnum("originmesh", li, curvNameList, tr("Original Mesh:"),
                    QString("Choose mesh for left window")));
            parlst.addParam(new RichEnum("targetmesh", ri, curvNameList, tr("Target Right:"),
                    QString("Choose mesh for right window")));
            break;
        case FP_SINGLE_ALGO_DEMO:
            parlst.addParam(new RichEnum("originmesh", li, curvNameList, tr("Original Mesh:"),
                    QString("Choose mesh for left window")));
            break;
        default: break; // do not add any parameter for the other algos
    }
	/*
	parlst.addParam(new RichFloat("",0,"Directional Bias [0..1]","The balance between a uniform and a directionally biased set of lighting direction<br>:"
									" - 0 means light came only uniformly from any direction<br>"
									" - 1 means that all the light cames from the specified cone of directions <br>"
									" - other values mix the two set of lighting directions "));
	parlst.addParam(new RichInt ("loops",AMBOCC_DEFAULT_NUM_VIEWS,"Requested views", "Number of different views uniformly placed around the mesh. More views means better accuracy at the cost of increased calculation time"));
	parlst.addParam(new RichPoint3f("coneDir",Point3f(0,1,0),"Lighting Direction", "Number of different views placed around the mesh. More views means better accuracy at the cost of increased calculation time"));
	parlst.addParam(new RichFloat("coneAngle",30,"Cone amplitude", "Number of different views uniformly placed around the mesh. More views means better accuracy at the cost of increased calculation time"));
	parlst.addParam(new RichBool("useGPU",AMBOCC_USEGPU_BY_DEFAULT,"Use GPU acceleration","In order to use GPU-Mode, your hardware must support FBOs, FP32 Textures and Shaders. Normally increases the performance by a factor of 4x-5x"));
	parlst.addParam(new RichBool("useVBO",AMBOCC_USEVBO_BY_DEFAULT,"Use VBO if supported","By using VBO, Meshlab loads all the vertex structure in the VRam, greatly increasing rendering speed (for both CPU and GPU mode). Disable it if problem occurs"));
	parlst.addParam(new RichInt ("depthTexSize",AMBOCC_DEFAULT_TEXTURE_SIZE,"Depth texture size(should be 2^n)", "Defines the depth texture size used to compute occlusion from each point of view. Higher values means better accuracy usually with low impact on performance"));
	*/
	parlst.addParam(new RichInt ("loops",100,"loop times", "loop times"));
	parlst.addParam(new RichFloat("epsilon",0.01,"epsilon", "epsilon"));
}
bool AlgoDemoPlugin::applyFilter(QAction *algo, MeshDocument &md,
                             RichParameterSet & par, vcg::CallBackPos *cb)
{

   MeshModel* pm  = md.mm();
   if(pm == NULL) return false;
   
   /*
   MainWindow* mainwindow;
    foreach (QWidget *widget, QApplication::topLevelWidgets())
    {
        MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
        if (mainwindow)
        {
            break;
        }
    }
	if (mainwindow == NULL)
        {
            return false;
        }
		
    MeshModel* prm = mainwindow->newProjectAddMesh("resultant mesh","resultant mesh");
		

    GLArea* newGLA = mainwindow->newProject("resultant mesh");

    MeshModel* prm = newGLA->md()->addNewMesh("","resultant mesh",true);
	*/

   MeshModel* prm = md.addNewMesh("","resultant mesh",true);

    pm->cm.vert;         //vertics
    pm->cm.face;            //faces
    pm->cm.selVertVector;   //landmarks


    vcg::tri::Append<CMeshO,CMeshO>::MeshCopy(prm->cm,pm->cm);

    CMeshO::VertexIterator vi;
    for(vi = pm->cm.vert.begin(); vi != pm->cm.vert.end(); ++vi)
    {
        //prm->cm.addVertex (/*const aol::Vec3< RealType > &coords*/);
     // qDebug("pos x: %f y: %f",(*vi).P()[0],(*vi).P()[1]);
    //(*vi).P()[0] = (*vi).P()[0] - (wld * halfw);
    //(*vi).P()[1] = (*vi).P()[1] - (hld * halfh);
    //  (*vi).P()[0] = (*vi).P()[0] - totalw/2;
    //  (*vi).P()[1] = (*vi).P()[1] - totalh/2;
     // qDebug("after pos x: %f y: %f",(*vi).P()[0],(*vi).P()[1]);
    }
    CMeshO::FaceIterator vf;
    for(vf = pm->cm.face.begin(); vf!= pm->cm.face.end(); ++vf)
    {
				//(*vf).V()
    }

    
    return true;
}
	

MESHLAB_PLUGIN_NAME_EXPORTER(AlgoDemoPlugin)

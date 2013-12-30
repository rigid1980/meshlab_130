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
#include "map_cons.h"
#include <QGLFramebufferObject>
#include <vcg/math/gen_normal.h>
#include <wrap/qt/checkGLError.h>
#include <iostream>
#include <meshlab/mainwindow.h>


using namespace std;
using namespace vcg;

MapConsPlugin::MapConsPlugin() 
{ 
	typeList 
	<< FP_MAP_CONS
    << FP_ALGO_BHF;
	
    foreach(FilterIDType tt , types())
        actionList << new QAction(filterName(tt), this);

}

MapConsPlugin::~MapConsPlugin()
{
}

QString MapConsPlugin::filterName(FilterIDType algoId) const
{
	switch(algoId)
	{
    case FP_MAP_CONS :  return QString("Map Reconstruction");
    case FP_ALGO_BHF  :  return QString("Conformal Mapping");
		default : assert(0); 
	}
	
	return QString("");
}

QString MapConsPlugin::filterInfo(FilterIDType algoId) const
{
	switch(algoId)
	{
    case  FP_MAP_CONS: return QString("Map Reconstruction with given mu");
    case FP_ALGO_BHF :  return QString("BHF Conformal Mapping");
		default : assert(0); 
	}

	return QString("");
}

MapConsPlugin::FilterClass MapConsPlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_MAP_CONS :return MeshFilterInterface::Algorithm;
    case FP_ALGO_BHF : return MeshFilterInterface::Algorithm;
    default : assert(0);  
	}
}

int MapConsPlugin::getRequirements(QAction */*action*/)
{
	//no requirements needed
	return 0;
}

void MapConsPlugin::initParameterSet(QAction *action, MeshModel &m, RichParameterSet &parlst)
{

    MeshDocument *md = m.parent;
	
    if(md == NULL){
        qDebug()<<"meshDOC is NULL";
        return;
    }

    QStringList curvNameList = QStringList();
    for(int ii = 0; ii < md->meshList.size();++ii)
    {
         curvNameList.push_back(md->meshList[ii]->label());
    }
    int li = (md->meshList.size()>=1)?0:-1;
    int ri = (md->meshList.size()>=2)?1:-1;


	QString fileName = m.fullName();
	
	QFile file(fileName);
    if (fileName.isEmpty() || !file.exists()) {
        muFilePath = "./";
        muFileName = "mufile";
	} else {
	  // your method
	  QFileInfo const fileinfo(fileName);
      QString extension = QString("mu");
      muFilePath = fileinfo.absoluteFilePath () ;
      muFileName = fileinfo./*completeBaseName()*/fileName().append(".").append(extension);
	}
								
    switch(ID(action))
    {
        case FP_MAP_CONS:
			parlst.addParam(new RichString ("mufilename",muFileName,"mu file name"));
            break;
        case FP_ALGO_BHF:
            parlst.addParam(new RichEnum("conformaltype", li, QStringList()<<"One Hole"<<"Two Hole", tr("Holes:"),
		QString("Choose mesh for left window")));
            break;
        default: break; // do not add any parameter for the other algos
    }
	
	parlst.addParam(new RichEnum("originmesh", li, curvNameList, tr("Original Mesh:"),
		QString("Choose mesh for left window")));
	parlst.addParam(new RichInt ("loops",100,"loop times", "loop times"));
	parlst.addParam(new RichFloat("epsilon",0.01,"epsilon", "epsilon"));
}
bool MapConsPlugin::applyFilter(QAction *algo, MeshDocument &md,
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
    pm->cm.selVertVector;   //mus


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
	

MESHLAB_PLUGIN_NAME_EXPORTER(MapConsPlugin)

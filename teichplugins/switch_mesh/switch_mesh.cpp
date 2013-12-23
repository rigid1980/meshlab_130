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
#include "switch_mesh.h"
#include <QGLFramebufferObject>
#include <vcg/math/gen_normal.h>
#include <wrap/qt/checkGLError.h>
#include <iostream>
#include <meshlab/mainwindow.h>


using namespace std;
using namespace vcg;

SwitchMeshPlugin::SwitchMeshPlugin() 
{ 
	typeList 
	<< FP_SWITCH_MESH;
	
    foreach(FilterIDType tt , types())
        actionList << new QAction(filterName(tt), this);

}

SwitchMeshPlugin::~SwitchMeshPlugin()
{
}

QString SwitchMeshPlugin::filterName(FilterIDType algoId) const
{
	switch(algoId)
	{
    case FP_SWITCH_MESH :  return QString("Switch Current Mesh");
    	default : assert(0); 
	}
	
	return QString("");
}

QString SwitchMeshPlugin::filterInfo(FilterIDType algoId) const
{
	switch(algoId)
	{
    case  FP_SWITCH_MESH: return QString("switch mesh");
    
		default : assert(0); 
	}

	return QString("");
}

SwitchMeshPlugin::FilterClass SwitchMeshPlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_SWITCH_MESH :return MeshFilterInterface::Landmark;
    default : assert(0);  
	}
}

int SwitchMeshPlugin::getRequirements(QAction */*action*/)
{
	//no requirements needed
	return 0;
}

void SwitchMeshPlugin::initParameterSet(QAction *action, MeshModel &m, RichParameterSet &parlst)
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
    

								
    switch(ID(action))
    {
          case FP_SWITCH_MESH:
            parlst.addParam(new RichEnum("mesh", li, curvNameList, tr("Switch To:"),
                    QString("Choose mesh to current")));
            break;
        default: break; // do not add any parameter for the other algos
    }
	
}
bool SwitchMeshPlugin::applyFilter(QAction *algo, MeshDocument &md,
                             RichParameterSet & par, vcg::CallBackPos *cb)
{

    MainWindow* mw;
    foreach (QWidget *widget, QApplication::topLevelWidgets())
    {
        MainWindow* mw = dynamic_cast<MainWindow*>(widget);
        if (mw)
        {
            break;
        }
    }
    if (mw == NULL)
	{
		return false;
	}
		
    int idx = par.getInt("mesh");
	if(idx >= 0 && idx < md.meshList.size())
	{
		qDebug()<<"set current mesh "<<idx;
		md.setCurrentMesh(idx);
		
		
		int currSnapLayer = 0;
		foreach(MeshModel *mp,md.meshList) {
            //mw->GLA()->meshSetVisibility(mp,false);
			mp->visible=false;
			//mw->GLA()->meshVisibilityMap[mp->id()]=false;
		}
            //mw->GLA()->meshSetVisibility(md.meshList.at(idx),true);
            MeshModel *mp = md.getMesh(idx);
			mp->visible=true;
            //mw->GLA()->meshVisibilityMap[md.getMesh(idx)->id()]=true;
		
    //mw->GLA()->meshSetVisibility(md.getMesh(idx), true);
	//md.setCurrentRaster(idx);
		mw->GLA()->update();
	}
	
	
   //MeshModel* pm  = md.mm();
   //if(pm == NULL) return false;
  
    
    return true;
}
	

MESHLAB_PLUGIN_NAME_EXPORTER(SwitchMeshPlugin)

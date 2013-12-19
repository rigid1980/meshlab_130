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
* but WITHOUT ANY WARRANTY; without even the implied warrantydual_widget of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#include <GL/glew.h>
#include "dual_widget.h"
#include <QGLFramebufferObject>
#include <vcg/math/gen_normal.h>
#include <wrap/qt/checkGLError.h>
#include <iostream>

#include <meshlab/mainwindow.h>

using namespace std;
using namespace vcg;

DualWidgetPlugin::DualWidgetPlugin() 
{ 
	typeList 
	<< FP_DUAL_WIDGET;
	
    foreach(FilterIDType tt , types())
        actionList << new QAction(filterName(tt), this);

}

DualWidgetPlugin::~DualWidgetPlugin()
{
}

QString DualWidgetPlugin::filterName(FilterIDType algoId) const
{
	switch(algoId)
	{
    case FP_DUAL_WIDGET :  return QString("Show in Dual Widget");
    	default : assert(0); 
	}
	
	return QString("");
}

QString DualWidgetPlugin::filterInfo(FilterIDType algoId) const
{
	switch(algoId)
	{
    case  FP_DUAL_WIDGET: return QString("show dual mesh in one project window");
    	default : assert(0); 
	}

	return QString("");
}

DualWidgetPlugin::FilterClass DualWidgetPlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_DUAL_WIDGET :return MeshFilterInterface::Algorithm;
    default : assert(0);  
	}
}

int DualWidgetPlugin::getRequirements(QAction */*action*/)
{
	//no requirements needed
	return 0;
}

void DualWidgetPlugin::initParameterSet(QAction *action, MeshModel &m, RichParameterSet &parlst)
{
//	if(m == NULL){
//		qDebug()<<"...meshmodel NULL";
//		return;
//	}
	
    switch(ID(action))
	{
			case FP_DUAL_WIDGET:
            {
                MeshDocument *md = m.parent;
				
                if(md == NULL){
                    qDebug()<<"...meshDOC NULL";
                    return;
                }

                QStringList curvNameList = QStringList();
                for(int ii = 0; ii < md->meshList.size();++ii)
                {
                     curvNameList.push_back(md->meshList[ii]->shortName());
                }
                int li = (md->meshList.size()>=1)?0:-1;
                int ri = (md->meshList.size()>=2)?1:-1;
                parlst.addParam(new RichEnum("leftmesh", li, curvNameList, tr("Mesh in Left:"),
                                QString("Choose mesh for left window")));

                parlst.addParam(new RichEnum("rightmesh", ri, curvNameList, tr("Mesh in Right:"),
                                QString("Choose mesh for right window")));

            }
			break;
            default: break; // do not add any parameter for the other algos
  }
}
bool DualWidgetPlugin::applyFilter(QAction *algo, MeshDocument &md,
                             RichParameterSet & par, vcg::CallBackPos *cb)
{
    MainWindow* mainwindow;
    foreach (QWidget *widget, QApplication::topLevelWidgets())
    {
        MainWindow* mainwindow = dynamic_cast<MainWindow*>(widget);
        if (mainwindow)
        {
            break;
        }
    }
    
    int leftInd = par.getEnum("leftmesh");
    int rightInd = par.getEnum("rightmesh");

    qDebug()<<"..."<<leftInd<<"..."<<rightInd;

    MeshModel* m1 = NULL;
    MeshModel* m2 = NULL;

    if(md.meshList.size() >leftInd && leftInd > -1)
    {
          m1 = md.meshList[leftInd];
    }else{
		leftInd=-1;
	}
    if(md.meshList.size() >rightInd && rightInd > -1)
    {
          m2 = md.meshList[rightInd];
    }else{
		rightInd=-1;
	}

   //md.emit showInDual(m1,leftInd, m2, rightInd, QString("Dual Mesh Project Window"));
   // mainwindow->updateCustomSettings();
   // mainwindow->newProjectDualMesh2(m1,leftInd, m2, rightInd, QString("Dual Mesh Project Window"));
    md.newProjectDualMesh(m1,leftInd, m2, rightInd, QString("Dual Mesh Project Window"));

    return true;
}
	

MESHLAB_PLUGIN_NAME_EXPORTER(DualWidgetPlugin)

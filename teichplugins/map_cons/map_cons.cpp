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

#include "teichmuller.h"

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
    case  FP_MAP_CONS: return QString("Map Reconstruction with given mu. This algorithm works only for rectangle grid mesh.");
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
			parlst.addParam(new RichString ("mufilename","mufile","mu file name"));
            break;
        case FP_ALGO_BHF:
            parlst.addParam(new RichEnum("conformaltype", li, QStringList()<<"One Hole"<<"Two Hole", tr("Holes:"),
		QString("Choose mesh for left window")));
			parlst.addParam(new RichInt ("loops",100,"loop times", "loop times"));
	parlst.addParam(new RichFloat("epsilon",0.01,"epsilon", "epsilon"));
            break;
        default: break; // do not add any parameter for the other algos
    }
	
    parlst.addParam(new RichEnum("sourcemesh", li, curvNameList, tr("Source Mesh:"),
		QString("Choose source mesh ")));

}
bool MapConsPlugin::applyFilter(QAction *algo, MeshDocument &md,
                             RichParameterSet & parlst, vcg::CallBackPos *cb)
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

	int li = parlst.getEnum("sourcemesh");
    if(li<0 ) return false;
	   MeshModel* pm  = md.meshList[li];
	   if(pm == NULL) 
	   {
			QMessageBox::critical(mainwindow, tr("Parameter Error"), QString("Source mesh could not be empty!"));
			return false;
	   }
		
	/*
	double loops = parlst.getInt("loops");
	double epsilon = parlst.getFloat("epsilon");
	if(loops <= 0 ) 
   {
		QMessageBox::critical(mainwindow, tr("Parameter Error"), QString("loops must be greater than zero!"));
		return false;
   }
   if(epsilon <= 0 || epsilon >=1) 
   {
		QMessageBox::critical(mainwindow, tr("Parameter Error"), QString("0<epsilon<1 !"));
		return false;
   }
	*/
	
	
	muFileName = parlst.getString("mufilename");
	
	if(muFileName.isEmpty()){
        QMessageBox::critical(mainwindow, tr("Meshlab Opening Error"), QString("Filename is empty!"));
		return false;
	}
	
    muFileName = pm->pathName().append("/").append(muFileName);
	qWarning()<<"muFileName: "<<muFileName;
	 QFile mFile( muFileName );
    if(!mFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
            QMessageBox::critical(mainwindow, tr("Meshlab Opening Error"), QString("File does not exist or is unable to open!"));
			return false;
		}

	
	std::vector<cdouble> vmu;
	QTextStream in(&mFile);
	QString line ;
	while (!in.atEnd()) {
	   line = in.readLine();
	   QStringList l = line.trimmed().split(QRegExp("[ \\t+i,]+"));
	   if(l.size()<1) continue;
	   double muImage = 0;
	   if(l.size() == 1){
	   		muImage = 0;
			qDebug()<<l.at(0).toDouble()<<"  "<<0;
	   }

	   else {
			muImage = l.at(1).toDouble();
			qDebug()<<l.at(0).toDouble()<<"  "<<0;
	   }
		vmu.push_back(cdouble(l.at(0).toDouble(),muImage));
	}
		
		QString fileName = pm->label();
		fileName.remove(fileName.lastIndexOf('.'), fileName.length());
		QString extension = pm->label();
		extension.remove(0, pm->label().lastIndexOf('.')+1);
		qDebug()<<"extension  "<<extension;
			
	   QString label = fileName.append(".tmap");
	   if(!extension.isEmpty())
	    fileName.append(".").append(extension);
		/*
		QString fileName = pm->fullName();
		QString extension = QString("lmk");
		QString lmkfileName = fileName.append(".").append(extension);
		*/
	    QString fullName = pm->pathName().append("/").append(fileName);
	    MeshModel* prm = md.addNewMesh("","",true);
		prm->setFileName(fullName);
		vcg::tri::Append<CMeshO,CMeshO>::MeshCopy(prm->cm,pm->cm);
		prm->meshModified() = true;
		
	//output
   	VertexZ	fz;
	
    // input
    Eigen::ArrayX3i	face;
    VertexZ z;
	ArrayZ	mu;
	
	CMeshO::VertexIterator vi;
    z.resize(pm->cm.vert.size());
	qDebug()<<"original mesh size is "<<pm->cm.vert.size();
    int i = 0;
    for(vi = pm->cm.vert.begin(); vi != pm->cm.vert.end(); ++vi)
    {
       z(i++) = cdouble((*vi).P()[0], (*vi).P()[1]);
    }
	
	std::vector<int> vfi;
	std::vector<int> vfj;
	std::vector<int> vfk;	
	
	i=0;
    CMeshO::FaceIterator vf;
    for(vf = pm->cm.face.begin(); vf!= pm->cm.face.end(); ++vf)
    {
		//qDebug()<<"point 0 of face is "<<(unsigned int)((*vf).V(0) - &pm->cm.vert[0]);
		vfi.push_back((unsigned int)((*vf).V(0) - &pm->cm.vert[0]));
        vfj.push_back((unsigned int)((*vf).V(1) - &pm->cm.vert[0]));
		vfk.push_back((unsigned int)((*vf).V(2) - &pm->cm.vert[0]));
		i++;
    }

	face.resize(vfi.size(),3);
	face.col(0) = Map<ArrayXi>(vfi.data(),vfi.size());
	face.col(1) = Map<ArrayXi>(vfj.data(),vfj.size());
	face.col(2) = Map<ArrayXi>(vfk.data(),vfk.size());
	
	TriangleMesh mesh(face,z,TriangleMesh::COMPLEX_2D);
	
	qDebug()<<"face size"<<vmu.size();
	if(vfi.size() !=vmu.size()){
        QMessageBox::critical(mainwindow, tr("Algorithm Error"), 
			QString("Face size %1 != mu size %2").arg(vfi.size()).arg(vmu.size()));
		return false;
	}
    mu.resize(vmu.size());
    
	Index	corner;
	Index	landmark;
    ArrayZ	target;
	Scale	s(1,1,1);
    //Option	opt(0.001,0.95,100);
    //Option	opt(epsilon,mubound,loops);
	
	//corner = [1;(n-1)*m+1;m*n;m];
	std::vector<int> cornerVector;
	cornerVector.push_back(0);
    cornerVector.push_back(40*41);
    cornerVector.push_back(41*41-1);
    cornerVector.push_back(40);
	
	corner.resize(4);
	corner = Map<Index>(cornerVector.data(),cornerVector.size());
	
	int state = linear_beltrami_solver_rect(fz, mesh, mu, corner, landmark, target, s);
	qDebug()<<"result state is "<<state;
	
    //if(state == 0)
	{	

		
		i=0;
		for(vi = prm->cm.vert.begin(); vi != prm->cm.vert.end(); ++vi)
	    {
	       if(i<fz.size())
		   {
				(*vi).P()[0] = fz(i).real();
				(*vi).P()[1] = fz(i).imag();
	            (*vi).P()[2] = 0;
		   }
		   i++;
	    }	
	}
	 
    return true;
}
	

MESHLAB_PLUGIN_NAME_EXPORTER(MapConsPlugin)

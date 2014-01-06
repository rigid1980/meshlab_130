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
#include "algo_teich.h"
#include <QGLFramebufferObject>
//#include <vcg/math/gen_normal.h>
#include <wrap/qt/checkGLError.h>
//#include <iostream>
#include <meshlab/mainwindow.h>

#include "teichmuller.h"

using namespace std;
using namespace vcg;

AlgoTeichPlugin::AlgoTeichPlugin() 
{ 
	typeList 
	<< FP_ALGO_TEICH
    << FP_ALGO_QC;
	
    foreach(FilterIDType tt , types())
        actionList << new QAction(filterName(tt), this);

}

AlgoTeichPlugin::~AlgoTeichPlugin()
{
}

QString AlgoTeichPlugin::filterName(FilterIDType algoId) const
{
	switch(algoId)
	{
    case FP_ALGO_TEICH :  return QString("Teichmuller Mapping");
    case FP_ALGO_QC  :  return QString("QC Mapping");
		default : assert(0); 
	}
	
	return QString("");
}

QString AlgoTeichPlugin::filterInfo(FilterIDType algoId) const
{
	switch(algoId)
	{
    case  FP_ALGO_TEICH: return QString("Teichmuller extremal mapping between two surfaces with landmark matching");
    case FP_ALGO_QC :  return QString("QC mapping for 2 meshes");
		default : assert(0); 
	}

	return QString("");
}

AlgoTeichPlugin::FilterClass AlgoTeichPlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_ALGO_TEICH :return MeshFilterInterface::Algorithm;
    case FP_ALGO_QC : return MeshFilterInterface::Algorithm;
    default : assert(0);  
	}
}

int AlgoTeichPlugin::getRequirements(QAction */*action*/)
{
	//no requirements needed
	return 0;
}

void AlgoTeichPlugin::initParameterSet(QAction *action, MeshModel &m, RichParameterSet &parlst)
{

    MeshDocument *md = m.parent;
	
    if(md == NULL){
        qDebug()<<"meshDOC is NULL";
        return;
    }

    curvNameList = QStringList();
    for(int ii = 0; ii < md->meshList.size();++ii)
    {
         curvNameList.push_back(md->meshList[ii]->label());
    }
    int li = (md->meshList.size()>=1)?0:-1;
    int ri = (md->meshList.size()>=2)?1:-1;


								
    switch(ID(action))
    {
        case FP_ALGO_TEICH:
        case FP_ALGO_QC:
            parlst.addParam(new RichEnum("sourcemesh", li, curvNameList, tr("Source Mesh:"),
                    QString("Choose mesh for left window")));
            parlst.addParam(new RichEnum("targetmesh", ri, curvNameList, tr("Target Mesh:"),
                    QString("Choose mesh for right window")));
            break;
            parlst.addParam(new RichEnum("sourcemesh", li, curvNameList, tr("Source Mesh:"),
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
	parlst.addParam(new RichBool ("uselandmark",
				true,
				"use landmark",
				"If true, landmark will be used in calculation."));
			
    parlst.addParam(new RichFloat ("width",1.0,"target width", "target width  range (0,1]"));
    parlst.addParam(new RichFloat ("height",1.0,"target height", "target height  range (0,1]"));
    parlst.addParam(new RichFloat ("mubound",1.0,"mu bound", "mu bound"));
	parlst.addParam(new RichInt ("loops",100,"loop times", "loop times (integer)"));
	parlst.addParam(new RichFloat("epsilon",0.01,"epsilon", "epsilon"));
}
bool AlgoTeichPlugin::applyFilter(QAction *algo, MeshDocument &md,
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
	int ri = parlst.getEnum("targetmesh");
    if(li<0 ) return false;
   MeshModel* pm  = md.meshList[li];
   if(pm == NULL) 
   {
		QMessageBox::critical(mainwindow, tr("Parameter Error"), QString("Source mesh could not be empty!"));
		return false;
   }
    if(ri<0 ) return false;
   MeshModel* ptm  = md.meshList[ri];
   if(ptm == NULL) 
   {
		QMessageBox::critical(mainwindow, tr("Parameter Error"), QString("Target mesh could not be empty!"));
		return false;
   }
   
   double width = parlst.getFloat("width");
	double height = parlst.getFloat("height");
	double mubound = parlst.getFloat("mubound");
	if(width <= 0 ) 
   {
		QMessageBox::critical(mainwindow, tr("Parameter Error"), QString("Width must be greater than zero!"));
		return false;
   }
   if(height <= 0 ) 
   {
		QMessageBox::critical(mainwindow, tr("Parameter Error"), QString("height must be greater than zero!"));
		return false;
   }
   if(mubound  <= 0 ) 
   {
		QMessageBox::critical(mainwindow, tr("Parameter Error"), QString("Mu bound must be greater than zero!"));
		return false;
   }
	
	
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

   if(pm->cm.selVertVector.size() != ptm->cm.selVertVector.size())
   {
		QMessageBox::critical(mainwindow, tr("Parameter Error"), QString("Landmark size should be equal!"));
		return false;
   }
   
   bool uselandmark = parlst.getBool("uselandmark");
   
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
	prm->meshModified() = true;
	
    //output
   	VertexZ	fz;
	ArrayZ	mu;
	
    // input
    Eigen::ArrayX3i	face;
    VertexZ z;
	
    vcg::tri::Append<CMeshO,CMeshO>::MeshCopy(prm->cm,pm->cm);
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
	
	Index	landmark;
    ArrayZ	target;
	if(uselandmark)
	{
		landmark.resize(pm->cm.selVertVector.size());
	    qDebug()<<"landmark size "<<pm->cm.selVertVector.size();
		landmark = Map<Index>(pm->cm.selVertVector.data(),pm->cm.selVertVector.size());
		target.resize(ptm->cm.selVertVector.size());
		std::vector<cdouble> vt;
			
		i=0;
		for (std::vector<int>::iterator it = ptm->cm.selVertVector.begin() ; it != ptm->cm.selVertVector.end(); ++it)
		{
			int ii = *(it);
			CMeshO::VertexPointer vp = &ptm->cm.vert[ii];
			//qDebug()<<"sel "<<ii<<" "<<vp->P()[0]<<" "<<vp->P()[1];
			vt.push_back(cdouble(vp->P()[0],vp->P()[1]));
			
		}
		target.resize(vt.size());
		target = Map<ArrayZ>(vt.data(),vt.size());
	}
	
	
    Scale	s(1,1,1);
    //Option	opt(0.001,0.95,100);
    Option	opt(epsilon,mubound,loops);
	/*
	qDebug()<<"zzzzzzzzzz";
	for(i=0;i<z.size();i++){
		qDebug()<<z(i).real()<<" "<<z(i).imag();
	}
	qDebug()<<"landmark";
	for(i=0;i<landmark.size();i++){
		qDebug()<<landmark(i);
	}
	qDebug()<<"target";
	for(i=0;i<target.size();i++){
		qDebug()<<target(i).real()<<" "<<target(i).imag();
	}
	*/
    int state = techmuller_map_with_boundary(fz, mu, mesh, landmark, target, s, opt);
	qDebug()<<"result state is "<<state;
	/*
	const VertexZ & z	 = mesh.z();
	const Face	  & face = mesh.face();
	mesh.Clear();
	for(i=0;i<fz.size();i++)
	{
		double fu1 = fz(i).real();
		double fv1 = fz(i).imag();
		pm->cm.vert;
	}
	*/
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

	for (std::vector<int>::iterator it = pm->cm.selVertVector.begin() ; it != pm->cm.selVertVector.end(); ++it)
	{
		prm->cm.selVertVector.push_back(*(it));	
	}
	
	
	return true;
}
bool AlgoTeichPlugin::applyFilter2(QAction *algo, MeshDocument &md,
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
	int ri = parlst.getEnum("sourcemesh");
    if(li<0 ) return false;
   MeshModel* pm  = md.meshList[li];
   if(pm == NULL) return false;
    if(ri<0 ) return false;
   MeshModel* ptm  = md.meshList[ri];
   if(ptm == NULL) return false;
   if(pm->cm.selVertVector.size() != pm->cm.selVertVector.size())
   {
   }
   
   QString label = pm->label().append(".tmap");
   MeshModel* prm = md.addNewMesh("",label,true);

  
    //output
   	VertexZ	fz;
	ArrayZ	mu;
	
    // input
    Eigen::ArrayX3i	face;
    VertexZ z;
	
    vcg::tri::Append<CMeshO,CMeshO>::MeshCopy(prm->cm,pm->cm);

    CMeshO::VertexIterator vi;
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
	//indices.push_back((unsigned int)((*fi).V(0) - &(*m->vert.begin())));
		
        //vfi.push_back((unsigned int)((*vf).V(0) - (&(pm->cm.vert.begin()))));
		qDebug()<<"point 0 of face is "<<(unsigned int)((*vf).V(0) - &pm->cm.vert[0]);
		vfi.push_back((unsigned int)((*vf).V(0) - &pm->cm.vert[0]));
        vfj.push_back((unsigned int)((*vf).V(1) - &pm->cm.vert[0]));
		vfk.push_back((unsigned int)((*vf).V(2) - &pm->cm.vert[0]));
		i++;
    }
	
	qDebug()<<"mesh fase size is "<<i;
	
	face.resize(vfi.size(),3);
	face.col(0) = Map<ArrayXi>(vfi.data(),vfi.size());
	face.col(1) = Map<ArrayXi>(vfj.data(),vfj.size());
	face.col(2) = Map<ArrayXi>(vfk.data(),vfk.size());


//	for(vl = pm->cm.selVertVector.begin(); vl!= pm->cm.selVertVector.end(); ++vl)
//    {
//				//(*vf).V()
//    }

    


   // TriangleMesh mesh(face,z,TriangleMesh::COMPLEX_2D);

    Index	landmark;
    ArrayZ	target;

    Scale	s(1,1,1);
    Option	opt(0.001,0.95,100);


    //int state = techmuller_map_with_boundary(fz, mu, mesh, landmark, target, s, opt);

    return true;
}
	

MESHLAB_PLUGIN_NAME_EXPORTER(AlgoTeichPlugin)

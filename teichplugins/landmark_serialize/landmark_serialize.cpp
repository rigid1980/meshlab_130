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
#include "landmark_serialize.h"
#include <QGLFramebufferObject>
#include <vcg/math/gen_normal.h>
#include <wrap/qt/checkGLError.h>
#include <iostream>

#include <meshlab/mainwindow.h>

#include <QFileInfo>


using namespace std;
using namespace vcg;

LandmarkSerializePlugin::LandmarkSerializePlugin() 
{ 
	typeList 
	<< FP_LANDMARK_SERIALIZE_IMPORT
    << FP_LANDMARK_SERIALIZE_EXPORT;
	
    foreach(FilterIDType tt , types())
        actionList << new QAction(filterName(tt), this);

}

LandmarkSerializePlugin::~LandmarkSerializePlugin()
{
}

QString LandmarkSerializePlugin::filterName(FilterIDType algoId) const
{
	switch(algoId)
	{
    case FP_LANDMARK_SERIALIZE_IMPORT :  return QString("Import Landmark");
    case FP_LANDMARK_SERIALIZE_EXPORT :  return QString("Export Landmark");
		default : assert(0); 
	}
	
	return QString("");
}

QString LandmarkSerializePlugin::filterInfo(FilterIDType algoId) const
{
	switch(algoId)
	{
    case FP_LANDMARK_SERIALIZE_IMPORT: return QString("Import Landmark");
    case FP_LANDMARK_SERIALIZE_EXPORT :  return QString("Export Landmark");
		default : assert(0); 
	}

	return QString("");
}

LandmarkSerializePlugin::FilterClass LandmarkSerializePlugin::getClass(QAction *a)
{
  switch(ID(a))
  {
    case FP_LANDMARK_SERIALIZE_IMPORT :return MeshFilterInterface::Landmark;
    case FP_LANDMARK_SERIALIZE_EXPORT : return MeshFilterInterface::Landmark;
    default : assert(0);  
	}
}

int LandmarkSerializePlugin::getRequirements(QAction */*action*/)
{
	//no requirements needed
	return 0;
}

void LandmarkSerializePlugin::initParameterSet(QAction *action, MeshModel &m, RichParameterSet &parlst)
{

    QString fileName = m.fullName();
	
	QFile file(fileName);
    if (fileName.isEmpty() || !file.exists()) {
	  QMessageBox::warning(new QWidget(),"Error","You must import/save mesh file first!!");
      return ;
	} else {
	  // your method
	  QFileInfo const fileinfo(fileName);
      QString extension = QString("lmk");
      landmarkFilePath = fileinfo.absoluteFilePath () ;
      landmarkFileName = fileinfo./*completeBaseName()*/fileName().append(".").append(extension);
	}
	
	
	switch(ID(action))
	{
        case FP_LANDMARK_SERIALIZE_IMPORT:
        case FP_LANDMARK_SERIALIZE_EXPORT:
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
            parlst.addParam(new RichString ("landmarkfilename",landmarkFileName,"landmark file name"));
			break;
  default: break; // do not add any parameter for the other algos
  }
}
bool LandmarkSerializePlugin::applyFilter(QAction *action, MeshDocument &md,
                             RichParameterSet & par, vcg::CallBackPos *cb)
{
	MeshModel* mod  = md.mm();
	/*
	QString fileName = mod->fullName();
	QString extension = QString("lmk");
	QString lmkfileName = fileName.append(".").append(extension);
	*/
	landmarkFileName =par.getString("landmarkfilename");
	QFile mFile(landmarkFileName);

	switch(ID(action))
	{
	case FP_LANDMARK_SERIALIZE_IMPORT:
	{
		if(!mFile.open(QIODevice::ReadOnly | QIODevice::Text))
			return false;

		mod->cm.selVertVector.clear();
		QTextStream in(&mFile);
		//    QString line = in.readLine();
		//    line = in.readLine();
		//    int landmarks = line.toInt();
		QString line = in.readLine();
		line = in.readLine();
		while (!in.atEnd()) {
		   line = in.readLine();
		   QStringList l = line.split(QRegExp("[ \\t]+"));
			mod->cm.selVertVector.push_back(l.at(0).toInt());
		}
		break;
	}
	case FP_LANDMARK_SERIALIZE_EXPORT:
	{
		if (!mFile.open(QIODevice::WriteOnly | QIODevice::Text))
			return false;

		QTextStream stream(&mFile);

		stream << "LANDMARK"<< "\n";
		stream << mod->cm.selVertVector.size()<< "\n";
				
		CMeshO::VertexPointer  vp;
		for (std::vector<int>::iterator it = mod->cm.selVertVector.begin() ; it != mod->cm.selVertVector.end(); ++it)
		   {
			  if((*it)>=mod->cm.vert.size()) return false;
			  vp = &mod->cm.vert[*it];
			  stream << *it<<"\t("<<vp->P().X()<<","<<vp->P().Y()<<","<<vp->P().Z()<< ")\n";
			  //fprintf(fpout,"%d\t(%.4f,%.4f,%.4f)\n",*it,vp->P().X(),vp->P().Y(),vp->P().Z());
		  }
		mFile.flush();
		mFile.close();
		break;
	}
	default: return false; break; // do not add any parameter for the other algos
	}
	return true;
}
	

MESHLAB_PLUGIN_NAME_EXPORTER(LandmarkSerializePlugin)

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

#ifndef ALGO_TEICH_H
#define ALGO_TEICH_H

#include <QObject>

#include <common/interfaces.h>
#include <common/meshmodel.h>
#include <wrap/gl/gl_surface.h>
//class FILTERWidget;
class AlgoTeichPlugin : public QObject, public MeshFilterInterface
{
	Q_OBJECT
	MESHLAB_PLUGIN_IID_EXPORTER(MESH_FILTER_INTERFACE_IID)
	Q_INTERFACES(MeshFilterInterface)

// Attributes
protected:
	
public:


// Methods
public:
	enum { FP_ALGO_TEICH,
         FP_ALGO_QC } ;

	AlgoTeichPlugin();
	~AlgoTeichPlugin();
	
    virtual QString     filterName      (FilterIDType algoId) const;
    virtual QString		filterInfo(FilterIDType algoId) const;

    virtual FilterClass getClass(QAction *);
    virtual int         getRequirements (QAction *action);

//    virtual FilterClass getClass(QAction *filter)
//	{
//		if(ID(filter)==FP_ALGO_QC) return MeshAlgoInterface::FaceColoring;
//	   	else return MeshAlgoInterface::VertexColoring;
//	};
	
	virtual       void        initParameterSet(QAction *,
	                                           MeshModel &/*m*/,
	                                           RichParameterSet & /*parent*/);
    virtual       bool        applyFilter     (QAction *filter,
                                               MeshDocument &md,
                                               RichParameterSet & /*parent*/,
	                                           vcg::CallBackPos * cb) ;
	
private:
	bool init;

};

#endif

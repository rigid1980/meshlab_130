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

#ifndef SAMPLEEDITPLUGIN_H
#define SAMPLEEDITPLUGIN_H

#include <QObject>
#include <common/interfaces.h>


class LandmarkCapturePlugin : public QObject, public MeshEditInterface
{
	Q_OBJECT
	Q_INTERFACES(MeshEditInterface)
		
public:
    LandmarkCapturePlugin();
    virtual ~LandmarkCapturePlugin() {}

    static const QString Info();

    bool StartEdit(MeshModel &/*m*/, GLArea * /*parent*/);
    //void EndEdit(MeshModel &/*m*/, GLArea * /*parent*/){};
    void EndEdit(MeshModel &m, GLArea *gla );
    void Decorate(MeshModel &/*m*/, GLArea * /*parent*/, QPainter *p);
    void Decorate (MeshModel &/*m*/, GLArea * ){};
    void mousePressEvent(QMouseEvent *, MeshModel &, GLArea * ) {};
    void mouseMoveEvent(QMouseEvent *, MeshModel &, GLArea * ) {};
    void mouseReleaseEvent(QMouseEvent *event, MeshModel &/*m*/, GLArea * );
		
  void drawFace(CMeshO::FacePointer fp,MeshModel &m, GLArea *gla, QPainter *p);
  void drawPoint(CMeshO::VertexPointer vp, int i, MeshModel &m, GLArea *gla, QPainter *p);

    QPoint cur;
	QFont qFont;
    bool haveToPick;
    CMeshO::FacePointer curFacePtr;
    CMeshO::VertexPointer curVertPtr;

    void findNearest(CMeshO::FacePointer fp, int &fp_idx, MeshModel &m, int &mm_idx);
    int  transform(vcg::Matrix44f& m, int screenX, int screenY);
	int findScreenNearest(MeshModel &m,int screenX, int screenY);
    void drawSingleLandmark(MeshModel * mp, int ii, int j, QPainter *p,GLArea * gla);
};

#endif

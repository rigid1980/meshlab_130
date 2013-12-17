/****************************************************************************
* MeshLab                                                           o o     *
* An extendible mesh processor                                    o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005, 2006                                          \/)\/    *
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

#ifndef GLMAREA_H
#define GLMAREA_H
#include <GL/glew.h>

#include <vcg/space/plane3.h>
#include <vcg/space/line3.h>
#include <vcg/math/matrix44.h>
#include <wrap/gl/math.h>
#include <wrap/gl/trimesh.h>
#include <wrap/gui/trackball.h>
#include <vcg/math/shot.h>
#include <wrap/gl/shot.h>

#include <QTimer>
#include <QTime>

#include "../common/interfaces.h"
#include "glarea_setting.h"
#include "snapshotsetting.h"
#include "rendermodeactions.h"
#include "multiViewer_Container.h"

#include "glarea.h"
#include <QPoint>

#define SSHOT_BYTES_PER_PIXEL 4

//enum LightingModel{LDOUBLE,LFANCY};


class MeshModel;
class MainWindow;

class GLLMArea : public GLArea
{
	Q_OBJECT

  //typedef vcg::Shot<double> Shot;

public:
    GLLMArea(MultiViewer_Container *mvcont, RichParameterSet *current);
	~GLLMArea();


	int meshind;
    inline void setModelInd(int i) { meshind = i;}
protected:

  void paintEvent(QPaintEvent *event);



};


#endif

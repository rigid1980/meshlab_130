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

#include "../common/interfaces.h"
#include "gllmarea.h"
#include "mainwindow.h"

#include <QFileDialog>

#include <wrap/gl/picking.h>
#include <wrap/qt/trackball.h>
#include <wrap/qt/col_qt_convert.h>
#include <wrap/qt/shot_qt.h>
#include <wrap/qt/checkGLError.h>
#include <wrap/qt/gl_label.h>
#include <wrap/io_trimesh/export_ply.h>
#include <wrap/io_trimesh/import_ply.h>

#include <wrap/gl/pick.h>
#include<wrap/qt/gl_label.h>

#include <math.h>

using namespace std;
using namespace vcg;

GLLMArea::GLLMArea(MultiViewer_Container *mvcont, RichParameterSet *current)
    :    GLArea(mvcont, current)
{
//		menu = new QMenu(this);
	//connect(menu,SIGNAL(triggered(QAction*)),this,SIGNAL(selectedAction(QAction*)));
    meshind = -1 ;
}

GLLMArea::~GLLMArea()
{
	rendermodemap.clear();
}

void GLLMArea::mousePressEvent(QMouseEvent*e)
{
    GLArea::mousePressEvent(e);
	qDebug()<<"----------GLLMArea::mousePressEvent(QMouseEvent*e)";
/*
	if(meshind>-1 && meshind<this->md()->meshList.size())
		this->md()->setCurrentMesh(meshind);
*/
}

void GLLMArea::paintEvent(QPaintEvent */*event*/)
{
	if (mvc() == NULL) return;
	QPainter painter(this);
	painter.beginNativePainting();
	makeCurrent();

	if(!isValid() )return;
	QTime time;
	time.start();

	if(!this->md()->isBusy())
	{
	  initTexture(hasToUpdateTexture);
	  hasToUpdateTexture=false;
	}

	glClearColor(1.0,1.0,1.0,0.0);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	setView();  // Set Modelview and Projection matrix
	if((!takeSnapTile) || (takeSnapTile && !ss.transparentBackground) )
		drawGradient();  // draws the background

	drawLight();

	
	glPushMatrix();

	// Finally apply the Trackball for the model
	trackball.GetView();
	trackball.Apply();
	glPushMatrix();

	//glScale(d);
	//	glTranslate(-FullBBox.Center());
	//setLightModel();

	// Set proper colorMode
	/*if(rm.colorMode != GLW::CMNone)
	{
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	}
	else glColor(Color4b::LightGray);*/

	/*if(rm.backFaceCull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);*/

	if(!this->md()->isBusy())
	{
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		if (iRenderer)
			iRenderer->Render(currentShader, *this->md(), rendermodemap, this);
		else
		{
		  if(hasToSelect) // right mouse click you have to select a mesh
		  {
			int newId=RenderForSelection(pointToPick[0],pointToPick[1]);
			if(newId>=0)
			{
			  Logf(0,"Selected new Mesh %i",newId);
			  md()->setCurrentMesh(newId);
			  update();
			}
			hasToSelect=false;
		  }
		  //else
		  {
			//foreach(MeshModel * mp, this->md()->meshList)
            MeshModel * mp = NULL;
            if(meshind>-1 && meshind<this->md()->meshList.size())
				mp = this->md()->meshList.at(meshind);
			if(mp != NULL)	
			{
				QMap<int,RenderMode>::iterator it = rendermodemap.find(mp->id());
				if (it != rendermodemap.end())
				{
					RenderMode rm = it.value();
					setLightModel(rm);
					if(rm.colorMode != GLW::CMNone)
					{
						glEnable(GL_COLOR_MATERIAL);
						glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
					}
					else glColor(Color4b::LightGray);
					if(rm.backFaceCull) 
						glEnable(GL_CULL_FACE);
					else 
						glDisable(GL_CULL_FACE);
					//Mesh visibility is read from the viewer visibility map, not from the mesh
					mp->glw.SetHintParamf(GLW::HNPPointSize,glas.pointSize);
					mp->glw.SetHintParami(GLW::HNPPointDistanceAttenuation,glas.pointDistanceAttenuation?1:0);
					mp->glw.SetHintParami(GLW::HNPPointSmooth,glas.pointSmooth?1:0);
					if(meshVisibilityMap[mp->id()])
					{
						if (!md()->renderState().isEntityInRenderingState(id,MeshLabRenderState::MESH))
							mp->render(rm.drawMode,rm.colorMode,rm.textureMode);
					}
					QList<QAction *>& tmpset = iPerMeshDecoratorsListMap[mp->id()];
					for( QList<QAction *>::iterator it = tmpset.begin(); it != tmpset.end();++it)
					{
					  MeshDecorateInterface * decorInterface = qobject_cast<MeshDecorateInterface *>((*it)->parent());
					  decorInterface->decorateMesh(*it,*mp,this->glas.currentGlobalParamSet, this,&painter,md()->Log);
					}
					md()->renderState().render(mp->id(),rm.drawMode,rm.colorMode,rm.textureMode);
				}

                //mengbin
                int sens = (int)800/(int(sqrt((double)mp->cm.vert.size()))-1);
                int j=0;
                if(meshVisibilityMap[mp->id()])
                for (std::vector<int>::iterator it = mp->cm.selVertVector.begin() ; it != mp->cm.selVertVector.end(); ++it)
                {
                    j++;
                    int ii = *(it);
                    CMeshO::VertexPointer vp = &mp->cm.vert[ii];
                    if(vp != NULL){
                        int ps = 20<(sens/2)?20:sens/2;
                        glPointSize(ps);
                        //glPointSize(sens/2);
						glEnable( GL_POINT_SMOOTH );
						
                        glDisable(GL_DEPTH_TEST);
                        glDisable(GL_LIGHTING);
                        glDisable(GL_TEXTURE_2D);
                        glPushMatrix();
                        glBegin (GL_POINTS); 

                        //glNormal3f (0.0, 0.0, 1.0);
                        glColor4f(1, 0, 0, .3f);
                        //glColor3f(1.0,0.0,0.0);
                        glVertex(vp->P());

                        glEnd();
                        glPopMatrix();
						glDisable(GL_POINT_SMOOTH);
						

                        int i = tri::Index(mp->cm,vp);
                        //QString buf =QString("\nlm%1 v%2:(%3 %4 %5)").arg(j).arg(i).arg(vp->P()[0]).arg(vp->P()[1]).arg(vp->P()[2]);
						QString buf =QString("\nlm%1").arg(j);
                        vcg::glLabel::render(&painter,vp->P(),buf);
                    }
                }
			}
		  }
		}
		if(iEdit) {
		  iEdit->setLog(&md()->Log);
		  iEdit->Decorate(*mm(),this,&painter);
		}

		// Draw the selection
		if (mm() != NULL)
		{
			QMap<int,RenderMode>::iterator it = rendermodemap.find(mm()->id());
			if ((it != rendermodemap.end()) && it.value().selectedFace)  
				mm()->renderSelectedFace();
			if ((it != rendermodemap.end()) && it.value().selectedVert)  
				mm()->renderSelectedVert();
		}

		glPopAttrib();
	} ///end if busy

	glPopMatrix(); // We restore the state to immediately after the trackball (and before the bbox scaling/translating)

	if(trackBallVisible && !takeSnapTile && !(iEdit && !suspendedEditor))
		trackball.DrawPostApply();

	// The picking of the surface position has to be done in object space,
	// so after trackball transformation (and before the matrix associated to each mesh);
	if(hasToPick && hasToGetPickPos)
	{
		Point3f pp;
		hasToPick=false;
		if(Pick<Point3f>(pointToPick[0],pointToPick[1],pp))
		{
			emit transmitSurfacePos(nameToGetPickPos, pp);
			hasToGetPickPos=false;
		}
	}
	foreach(QAction * p , iPerDocDecoratorlist)
	{
	  MeshDecorateInterface * decorInterface = qobject_cast<MeshDecorateInterface *>(p->parent());
	  decorInterface->decorateDoc(p,*this->md(),this->glas.currentGlobalParamSet, this,&painter,md()->Log);
	}

	glPopMatrix(); // We restore the state to immediately before the trackball

	//If it is a raster viewer draw the image as a texture
	if(isRaster())
		drawTarget();

	// Double click move picked point to center
	// It has to be done in the before trackball space (we MOVE the trackball itself...)
	if(hasToPick && !hasToGetPickPos)
	{
		Point3f pp;
		hasToPick=false;
		if(Pick<Point3f>(pointToPick[0],pointToPick[1],pp))
		{
			trackball.Translate(-pp);
			trackball.Scale(1.25f);
			QCursor::setPos(mapToGlobal(QPoint(width()/2+2,height()/2+2)));
		}
	}

	// ...and take a snapshot
	if (takeSnapTile) pasteTile();

	// Finally display HELP if requested
	if (isHelpVisible()) displayHelp();

	// Draw the log area background
	// on the bottom of the glArea
	if(infoAreaVisible)
	{
		glPushAttrib(GL_ENABLE_BIT);
		glDisable(GL_DEPTH_TEST);
		displayInfo(&painter);
		displayRealTimeLog(&painter);
		updateFps(time.elapsed());
		glPopAttrib();
	}

	//Draw highlight if it is the current viewer
	if(mvc()->currentId==id)
		displayViewerHighlight();


	QString error = checkGLError::makeString("There are gl errors:");
	if(!error.isEmpty()) {
		Logf(GLLogStream::WARNING,qPrintable(error));
	}

	//check if viewers are linked
	MainWindow *window = qobject_cast<MainWindow *>(QApplication::activeWindow());
	if(window && window->linkViewersAct->isChecked() && mvc()->currentId==id)
		mvc()->updateTrackballInViewers();
	painter.endNativePainting();
}


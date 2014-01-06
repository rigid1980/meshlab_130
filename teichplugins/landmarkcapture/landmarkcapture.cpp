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
/****************************************************************************
  History
$Log: meshedit.cpp,v $
****************************************************************************/

#include <math.h>
#include <stdlib.h>
#include <meshlab/glarea.h>
#include "landmarkcapture.h"
#include <wrap/gl/pick.h>
#include<wrap/qt/gl_label.h>

#include <math.h>
// #include <vcg/complex/trimesh/update/topology.h>
//#include <vcg/complex/algorithms/halfedge_topology.h>
#include <common/meshmodel.h>
#include <vcg/complex/algorithms/update/topology.h>
#include <vcg/complex/algorithms/update/halfedge_topology.h>

using namespace std;
using namespace vcg;

LandmarkCapturePlugin::LandmarkCapturePlugin() {
	qFont.setFamily("Helvetica");
	qFont.setPixelSize(12);    
}

const QString LandmarkCapturePlugin::Info() 
{
    return tr("landmark picked from mesh.");
}
 
void LandmarkCapturePlugin::mouseReleaseEvent(QMouseEvent * event, MeshModel &/*m*/, GLArea * gla)
{
	gla->update();
	cur=event->pos();
	haveToPick=true;
}

//void LandmarkCapturePlugin::Decorate(MeshModel &m, GLArea * gla, QPainter *p)
//{
//    int sens = (int)500/(int(sqrt((double)m.cm.vert.size()))-1);

//    vector<CMeshO::VertexPointer> NewSel;
//    if(haveToPick)
//    {
//        glPushMatrix();
//        glMultMatrix(m.cm.Tr);

//        Log("point: %d %d size: %d sens:%d",cur.x(),gla->height() - cur.y(),m.cm.vert.size(),sens);
//        GLPickTri<CMeshO>::PickVert(cur.x(), gla->height() - cur.y(), m.cm, NewSel,sens,sens,true);
//        Log("pickvert size: %d",NewSel.size());


//        haveToPick=false;
//        glPopMatrix();

//    }

//    vector<CMeshO::VertexPointer>::iterator vpi;

//    if(NewSel.size()>0){
//        curVertPtr=NewSel.front();
//        int idx = tri::Index(m.cm,curVertPtr);

//        if(std::find(m.cm.selVertVector.begin(), m.cm.selVertVector.end(), idx)
//                == m.cm.selVertVector.end()) {
//            m.cm.selVertVector.push_back(idx);
//        } else {
//            /* v does not contain x */
//        }
//    }


////    for(vpi=NewSel.begin();vpi!=NewSel.end();++vpi)
////    {
////        Log("....%d %g %g %g " ,tri::Index(m.cm,*vpi),(*vpi)->P()[0],(*vpi)->P()[1],(*vpi)->P()[2]);
////   }

////        if(curFacePtr)
////        {
////            findNearest(curFacePtr,m);

////        glBegin(GL_POINTS);
////          glColor4f(1,0,0,.5f);
////        glVertex(curFacePtr->P(0));
////         glEnd();
////      static int i;
////		i++;
////		m.cm.selVertVector.push_back(i);

////     }


//}

void LandmarkCapturePlugin::Decorate(MeshModel &m, GLArea * gla, QPainter *p)
{

    if(haveToPick)
    {
        glPushMatrix();
        glMultMatrix(m.cm.Tr);
        vector<CMeshO::FacePointer> NewSel;
        Log("point: %d %d",cur.x(),gla->height() - cur.y());

        GLPickTri<CMeshO>::PickFace(cur.x(), gla->height() - cur.y(), m.cm, NewSel);
        Log("size: %d",NewSel.size());
        if(NewSel.size()>0)
        curFacePtr=NewSel.front();
        haveToPick=false;
        glPopMatrix();
    }
 if(curFacePtr)
 {
     int ret = findScreenNearest(m,cur.x(),gla->height() - cur.y());
         //int	ret = tri::Index(m.cm,vertics[idx]);
	if(std::find(m.cm.selVertVector.begin(), m.cm.selVertVector.end(), ret)
            ==m.cm.selVertVector.end()){
			    m.cm.selVertVector.push_back(ret);
				//        CMeshO::VertexPointer  vp = vertics[idx];
                //Log("Nearest point %d: %d",ret,m.cm.selVertVector.size());
				//gla->repaint();
                drawSingleLandmark(&m, ret,m.cm.selVertVector.size(),p,gla);
			}
 }
}

void LandmarkCapturePlugin::drawSingleLandmark(MeshModel * mp, int ii, int j, QPainter *p,GLArea * gla)
{
	int sens = (int)gla->width()/(int(sqrt((double)mp->cm.vert.size()))-1);
	int maxPs = (int)gla->width()/40;
                
    CMeshO::VertexPointer vp = &mp->cm.vert[ii];
	if(vp != NULL){
		int ps = maxPs<(sens/2)?maxPs:sens/2;
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
        vcg::glLabel::render(p,vp->P(),buf);
	}
}

int  LandmarkCapturePlugin::transform(vcg::Matrix44f& m, int screenX, int screenY)
{
	int ret = 0;
	
	Log(" Matrix:");
	Log(" %f %f %f %f",m[0][0],m[1][0],m[2][0],m[3][0]);
	Log(" %f %f %f %f",m[0][1],m[1][1],m[2][1],m[3][1]);
	Log(" %f %f %f %f",m[0][2],m[1][2],m[2][2],m[3][2]);
	Log(" *********Matrix");
	
	/*
    if(curFacePtr != NULL)
	{
		float tx[3];
        float ty[3];
        float tz[3];
		float t[3];
		
        float min_t =9999;
		for(int i=0;i<3;i++)
		{
		//    glVertex(curFacePtr->P(0));
//    glVertex(curFacePtr->P(1));
//    glVertex(curFacePtr->P(2));
            Point3<float>*  pp = &curFacePtr->P(i);
	
            tx[i] = pp->X()*m[0][0] + pp->Y() *m[1][0] + pp->Z() *m[2][0] + m[3][0];
            ty[i] = pp->X() *m[0][1] + pp->Y() *m[1][1] + pp->Z() *m[2][1] + m[3][1];
            tz[i] = pp->X() *m[0][2] + pp->Y() *m[1][2] + pp->Z() *m[2][2] + m[3][2];
			
            Log("trans cord: %f %f %f",tx[i],ty[i],tz[i]);
        
		}
	}
*/
	return ret;
                     
        //return Vector3F(tx, ty, tz);
}

int LandmarkCapturePlugin::findScreenNearest(MeshModel &m, int screenX, int screenY)
{
	int idx = 0;
	double min_dist = 9999;
        GLdouble resCoords[3];
        GLdouble mvMatrix_f[16];
        GLdouble prMatrix_f[16];
        GLint viewpSize[4];
      //  GLfloat *dFloat = new GLfloat[depthTexArea];

        glGetDoublev(GL_MODELVIEW_MATRIX, mvMatrix_f);
        glGetDoublev(GL_PROJECTION_MATRIX, prMatrix_f);
        glGetIntegerv(GL_VIEWPORT, viewpSize);

        //glReadPixels(0, 0, depthTexSize, depthTexSize, GL_DEPTH_COMPONENT, GL_FLOAT, dFloat);
//    vector< CMeshO::VertexPointer >  vertics = vcg::tri::HalfEdgeTopology<CMeshO> :: getVertices (curFacePtr);

//	for (int i=0; i<vertics.size(); ++i)
//	{
// //       CMeshO::VertexPointer  vp = vertics[i];
		
////            gluProject(vp->P()[0], vp->P()[1], vp->P()[2],
////						   (const GLdouble *) mvMatrix_f, (const GLdouble *) prMatrix_f, (const GLint *) viewpSize,
////						   &resCoords[0], &resCoords[1], &resCoords[2] );

////		int x = floor(resCoords[0]);
////		int y = floor(resCoords[1]);
		
////		double dist = sqrt(double((x-screenX)*(x-screenX)+(y-screenY)*(y-screenY)));
////		if(dist < min_dist){
////			min_dist=dist;
////			idx = i;
////		}
//	}
	
		//m.cm.selVertVector.push_back(vertics[ret]);
	
	

        for (int i=0; i<3; ++i)
        {
                //Point3<CMeshO::ScalarType> &vp = m.cm.vert[i].P();
				Point3<float>*  pp = &curFacePtr->P(i);

                gluProject(pp->X(), pp->Y(), pp->Z(),
                                   (const GLdouble *) mvMatrix_f, (const GLdouble *) prMatrix_f, (const GLint *) viewpSize,
                                   &resCoords[0], &resCoords[1], &resCoords[2] );

                int x = floor(resCoords[0]);
                int y = floor(resCoords[1]);
                
                double dist = sqrt(double((x-screenX)*(x-screenX)+(y-screenY)*(y-screenY)));
				if(dist < min_dist){
					min_dist=dist;
                    idx = i;
				}
        }

        int ret = tri::Index(m.cm,curFacePtr->V(idx));

    return ret;
}


/*
void LandmarkCapturePlugin:calcCoord(float x,float y,float z,vcg::Matrix44f,double *xr,double *yr,double *zr) {
	*xr=x*matrix[0]+y*matrix[4]+z*matrix[8]+matrix[12];
	*yr=x*matrix[1]+y*matrix[5]+z*matrix[9]+matrix[13];	
	*zr=x*matrix[2]+y*matrix[6]+z*matrix[10]+matrix[14];
	
	Matrix44f::spaceToSpace( tri, attrTri3 );
}
*/

//void LandmarkCapturePlugin::Decorate(MeshModel &m, GLArea * gla, QPainter *p)
//{
//    if(haveToPick)
//    {
//        glPushMatrix();
//        glMultMatrix(m.cm.Tr);
//        vector<CMeshO::VertexPointer> NewSel;
//        GLPickTri<CMeshO>::PickVert(cur.x(), gla->height() - cur.y(), m.cm, NewSel);
////        for(vpi=NewSel.begin();vpi!=NewSel.end();++vpi)
////                       (*vpi)->ClearS();

//        qDebug("LOG---: %.4f %.4f",cur.x(),gla->height() - cur.y());
//        Log("LOG: %.4f %.4f",cur.x(),gla->height() - cur.y());

//        if(NewSel.size() > 0) {
//            curVertPtr = NewSel.front();
//       //     qDebug("LOG: %.4f %.4f",curVertPtr->cP().X(),curVertPtr->cP().Y());
//         //   tri::ComponentFinder<CMeshO>::Dijkstra(m.cm, *curVertPtr, K, this->maxHop, this->NotReachableVector);
//        //    qDebug("LOG: %.4f %.4f",curVertPtr->cP().X(),curVertPtr->cP().Y());

//            ComponentVector.push_back(curVertPtr);
//        }

//        haveToPick=false;
//        glPopMatrix();
//    }

//    if(curVertPtr)
//    {
//        glPushMatrix();
//        glMultMatrix(m.cm.Tr);

//      //  qDebug("LOG: %.4f %.4f",curVertPtr->cP().X(),curVertPtr->cP().Y());

//        /* The actual selection is drawn in red (instead of the automatic drawing of selected vertex
//           of MeshLab) */
//        glBegin(GL_POINTS);
//        glColor4f(1,0,0,.5f);
////        Point3<double> const & p

//// glVertex(curVertPtr->cP());


//        glEnd();

//        glPopMatrix();

//    }
//}
void LandmarkCapturePlugin::findNearest(CMeshO::FacePointer fp, int &fp_idx, MeshModel &m, int &mm_idx)
{
    vector< CMeshO::VertexPointer >  vertics = vcg::tri::HalfEdgeTopology< CMeshO > :: getVertices (fp);
//	if(vertics.size() > 0)
	
////    for(int i=0;i<3;++i)
////    {
////      arg(fp->P(i)[0]).arg(fp->P(i)[1]).arg(fp->P(i)[2]);
////    }
////    std::vector<CMeshO::VertexPointer> vps = HalfEdgeTopology<MeshType>::getVertices(fp);
//    curVertPtr = vps[0];
}

void LandmarkCapturePlugin::drawFace(CMeshO::FacePointer fp, MeshModel &m, GLArea *gla, QPainter *p)
{

  //glDepthMask(GL_FALSE);
  //glDisable(GL_DEPTH_TEST);
  //p->endNativePainting();
  //p->save();
  //p->setRenderHint(QPainter::TextAntialiasing);
  //p->setPen(Qt::white);
  //QFont qFont;
  //qFont.setStyleStrategy(QFont::NoAntialias);
  //qFont.setFamily("Helvetica");
  //qFont.setPixelSize(12);
  //p->setFont(qFont);
  QString buf = QString("f%1\n (%3 %4 %5)").arg(tri::Index(m.cm,fp)).arg(tri::Index(m.cm,fp->V(0))).arg(tri::Index(m.cm,fp->V(1))).arg(tri::Index(m.cm,fp->V(2)));
  Point3f c=Barycenter(*fp);
  vcg::glLabel::render(p,c,buf);
  for(int i=0;i<3;++i)
    {
       buf =QString("\nv%1:%2 (%3 %4 %5)").arg(i).arg(fp->V(i) - &m.cm.vert[0]).arg(fp->P(i)[0]).arg(fp->P(i)[1]).arg(fp->P(i)[2]);
      if( m.hasDataMask(MeshModel::MM_VERTQUALITY) )
        buf +=QString(" - Q(%1)").arg(fp->V(i)->Q());
      if( m.hasDataMask(MeshModel::MM_WEDGTEXCOORD) )
          buf +=QString("- uv(%1 %2) id:%3").arg(fp->WT(i).U()).arg(fp->WT(i).V()).arg(fp->WT(i).N());
      if( m.hasDataMask(MeshModel::MM_VERTTEXCOORD) )
          buf +=QString("- uv(%1 %2) id:%3").arg(fp->V(i)->T().U()).arg(fp->V(i)->T().V()).arg(fp->V(i)->T().N());
    vcg::glLabel::render(p,fp->V(i)->P(),buf);
    }

  //p->drawText(QRect(0,0,gla->width(),gla->height()), Qt::AlignLeft | Qt::TextWordWrap, buf);
  //p->restore();
  //p->beginNativePainting();
}

void LandmarkCapturePlugin::drawPoint(CMeshO::VertexPointer vp, int i, MeshModel &m, GLArea *gla, QPainter *p)
{
    QString buf;
    {
        buf =QString("\nv%1: (%3 %4 %5)").arg(i).arg(vp->P()[0]).arg(vp->P()[1]).arg(vp->P()[2]);
		vcg::glLabel::render(p,vp->P(),buf);
    
		/*
       buf =QString("\nv%1:%2 (%3 %4 %5)").arg(i).arg(fp->V(i) - &m.cm.vert[0]).arg(fp->P(i)[0]).arg(fp->P(i)[1]).arg(fp->P(i)[2]);
      if( m.hasDataMask(MeshModel::MM_VERTQUALITY) )
        buf +=QString(" - Q(%1)").arg(fp->V(i)->Q());
      if( m.hasDataMask(MeshModel::MM_WEDGTEXCOORD) )
          buf +=QString("- uv(%1 %2) id:%3").arg(fp->WT(i).U()).arg(fp->WT(i).V()).arg(fp->WT(i).N());
      if( m.hasDataMask(MeshModel::MM_VERTTEXCOORD) )
          buf +=QString("- uv(%1 %2) id:%3").arg(fp->V(i)->T().U()).arg(fp->V(i)->T().V()).arg(fp->V(i)->T().N());
		  vcg::glLabel::render(p,fp->P(),buf);
    
		  */
    }

  //p->drawText(QRect(0,0,gla->width(),gla->height()), Qt::AlignLeft | Qt::TextWordWrap, buf);
  //p->restore();
  //p->beginNativePainting();
}

bool LandmarkCapturePlugin::StartEdit(MeshModel &m, GLArea *gla )
{
  curFacePtr=0;
  curVertPtr=0;
    gla->setCursor(Qt::PointingHandCursor);
	
	Log("point: %d %d",cur.x(),gla->height() - cur.y());
	
	MeshDocument *md = m.parent;
	for(int ii = 0; ii < md->meshList.size();++ii)
	{
		 MeshModel* mm = md->meshList[ii];
	}
	//md->setCurrentMesh(1);			
    Log("start on mesh : %s",m.shortName());

    m.cm.selVertVector.clear();
    m.cm.selFaceVector.clear();
    //ComponentVector.clear();
	return true;
}

void LandmarkCapturePlugin::EndEdit(MeshModel &m, GLArea *gla )
{
    Log("end on mesh : %s",m.shortName());
    //gla->md()->updateMeshSelected();
   //m.updateMeshSelected();
     Log("landmark size: %d",m.cm.selVertVector.size());

  //gla->repaint();
     //gla->updateMeshSelected();
}

/*
 * ============================================================================
 *                   The XyloComp Software License, Version 1.1
 * ============================================================================
 * 
 *    Copyright (C) 2016 XyloComp Inc. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modifica-
 * tion, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of  source code must  retain the above copyright  notice,
 *    this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. The end-user documentation included with the redistribution, if any, must
 *    include  the following  acknowledgment:  "This product includes  software
 *    developed  by  XyloComp Inc.  (http://www.xylocomp.com/)." Alternately, 
 *    this  acknowledgment may  appear in the software itself,  if
 *    and wherever such third-party acknowledgments normally appear.
 * 
 * 4. The name "XyloComp" must not be used to endorse  or promote  products 
 *    derived  from this  software without  prior written permission. 
 *    For written permission, please contact fred.patrick@xylocomp.com.
 * 
 * 5. Products  derived from this software may not  be called "XyloComp", 
 *    nor may "XyloComp" appear  in their name,  without prior written 
 *    permission  of Fred Patrick
 * 
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS  FOR A PARTICULAR  PURPOSE ARE  DISCLAIMED.  IN NO  EVENT SHALL
 * XYLOCOMP INC. OR ITS CONTRIBUTORS  BE LIABLE FOR  ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL  DAMAGES (INCLU-
 * DING, BUT NOT LIMITED TO, PROCUREMENT  OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR  PROFITS; OR BUSINESS  INTERRUPTION)  HOWEVER CAUSED AND ON
 * ANY  THEORY OF LIABILITY,  WHETHER  IN CONTRACT,  STRICT LIABILITY,  OR TORT
 * (INCLUDING  NEGLIGENCE OR  OTHERWISE) ARISING IN  ANY WAY OUT OF THE  USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include "slicepoints.h"
#include "ranges.h"

trk::SlicePoints::
SlicePoints(const std::string&  model_name)
{
    std::string path = "/Users/fredpatrick/wrk/cfg/" + model_name + ".vtx";
    std::ifstream vtxstrm(path.c_str() );
    char             sline[100];

    float xmin = 9999.;
    float xmax = -9999.;
    float ymin = 9999.;
    float ymax = -9999.;
    float zmin = 9999.;
    float zmax = -9999.;
    section_count_ = 0;
    int section_index  = 9999;
    vtxstrm.getline(sline, 100);
    while ( !vtxstrm.eof() ) {
        SlicePoint p;
        std::stringstream ss(sline);
        float x;
        float y;
        float z;
        ss >> p.zone_name >> p.section_index >> p.slice_index >> p.point_index >> x >> y >> z;
        p.point = QVector3D(x, y, -z);
        slice_points_ << p;

        if (x < xmin ) xmin = x;
        if (x > xmax ) xmax = x;
        if (y < ymin ) ymin = y;
        if (y > ymax ) ymax = y;
        if (z < zmin ) zmin = z;
        if (z > zmax ) zmax = z;

        if ( p.section_index != section_index ) {
            section_index = p.section_index;
            section_count_ += 1;
        }

        vtxstrm.getline(sline, 100);
    }
    std::cout << "SlicePoints.ctor, slice_points_.size = " << slice_points_.size() << std::endl;
    std::cout << "SlicePoints.ctor, xmin = " << xmin << std::endl;
    std::cout << "SlicePoints.ctor, xmax = " << xmax << std::endl;
    std::cout << "SlicePoints.ctor, ymin = " << ymin << std::endl;
    std::cout << "SlicePoints.ctor, ymax = " << ymax << std::endl;
    std::cout << "SlicePoints.ctor, zmin = " << zmin << std::endl;
    std::cout << "SlicePoints.ctor, zmax = " << zmax << std::endl;
    basicTriangles();
}

trk::SlicePoints::
~SlicePoints()
{
}

void
trk::SlicePoints::
basicTriangles()
{
    vertices_.clear();
    QVector3D p0[4];
    QVector3D p1[4];
    normals_.clear();
    QVector3D an[8];
    int np = slice_points_.size() - 5;
    int m = 0;
    for (int n = 0; n < np; n += 5) {
        makeSlice( p0, n);                                 //NOTE: p0 is returned
        makeSlice( p1, n+5);                               //NOTE: p1 is returned
        makeNormals(an, p0, p1);                           //NOTE: an is returned
        vertices_ << p0[0]; vertices_ << p1[0]; vertices_ << p0[3];
        normals_   << an[0]; normals_   << an[0]; normals_   << an[0];
        vertices_ << p0[3]; vertices_ << p1[0]; vertices_ << p1[3];
        normals_   << an[1]; normals_   << an[1]; normals_   << an[1];
        vertices_ << p0[3]; vertices_ << p1[3]; vertices_ << p0[2];
        normals_   << an[2]; normals_   << an[2]; normals_   << an[2];
        vertices_ << p0[2]; vertices_ << p1[3]; vertices_ << p1[2];
        normals_   << an[3]; normals_   << an[3]; normals_   << an[3];
        vertices_ << p0[2]; vertices_ << p1[2]; vertices_ << p0[1];
        normals_   << an[4]; normals_   << an[4]; normals_   << an[4];
        vertices_ << p0[1]; vertices_ << p1[2]; vertices_ << p1[1];
        normals_   << an[5]; normals_   << an[5]; normals_   << an[5];
        vertices_ << p0[1]; vertices_ << p1[1]; vertices_ << p0[0];
        normals_   << an[6]; normals_   << an[6]; normals_   << an[6];
        vertices_ << p0[0]; vertices_ << p1[1]; vertices_ << p1[0];
        normals_   << an[7]; normals_   << an[7]; normals_   << an[7];
        m = m + 1;
    }
    std::cout << "SlicePoints.basicTriangles, # of slices    = " << m << std::endl;
    std::cout << "SlicePoints.basicTriangles, # of triangles = " << m*8 << std::endl;
    std::cout << "SlicePoints.basicTriangles, # of vertices  = " << m*8*3 << std::endl;
    std::cout << "SlicePoints.basicTriangles, # of normals   = " << m*8*3 << std::endl;

}

void
trk::SlicePoints::
makeSlice( QVector3D* p, int n)
{
    p[0] = slice_points_[n+0].point;
    p[1] = slice_points_[n+2].point;
    p[2] = slice_points_[n+3].point;
    p[3] = slice_points_[n+4].point;
}

void
trk::SlicePoints::
makeNormals(QVector3D* an, QVector3D* p0, QVector3D* p1)       // Note: an is returned
{
    an[0] = QVector3D::normal(p1[0] - p0[0], p0[3] - p0[0] );
    an[1] = QVector3D::normal(p1[3] - p1[0], p0[3] - p1[0] );
    an[2] = QVector3D::normal(p1[3] - p0[3], p0[2] - p0[3] );
    an[3] = QVector3D::normal(p1[2] - p1[3], p0[2] - p1[3] );
    an[4] = QVector3D::normal(p1[2] - p0[2], p0[1] - p0[2] );
    an[5] = QVector3D::normal(p1[1] - p1[2], p0[1] - p1[2] );
    an[6] = QVector3D::normal(p1[1] - p0[1], p0[0] - p0[1] );
    an[7] = QVector3D::normal(p1[0] - p1[1], p0[0] - p1[1] );
}

QVector<QVector3D>&
trk::SlicePoints::
edgeVertices(int j)
{
    if ( j < 0 || j >= 5) return edge_vertices_;

    QVector3D delta;
    float     epsilon = .01;
    int       np      = slice_points_.size() / 5;
    edge_vertices_.resize(np);
    for (int n = 0; n < np; n++) {
        int l  = j + n * 5;
        int l1 = 1 + n * 5;
        delta = (slice_points_[l].point - slice_points_[l1].point) *epsilon / 1.656250;
        edge_vertices_[n] = slice_points_[l].point + delta;
    }
    return edge_vertices_;
}

QVector<QVector3D>&
trk::SlicePoints::
vertices()
{
    return vertices_;
}

QVector<QVector3D>&
trk::SlicePoints::
normals()
{
    return normals_;
}

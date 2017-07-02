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
#include <sstream>
#include <string>
#include <vector>
#include "shellgeometry.h"
#include "ranges.h"
#include "debugcntl.h"

trk::ShellGeometry::
ShellGeometry(std::ifstream&  vtxstrm, Ranges& r) : ranges_(r)
{
    dbg_ = DebugCntl::instance();
    while ( true) {
        std::string        tag;
        std::string        pname;
        vtxstrm >> tag >> pname;
        if ( tag == "shell" && pname != "end" ) {
            if        ( pname == "shell_type"  ) vtxstrm >> shell_type_;
            else if ( pname == "inline_length" ) vtxstrm >> inline_length_;
            else if ( pname == "slice_count"   ) vtxstrm >> slice_count_;
            else {
                std::cout << "ShellGeometry.ctor, unknown parameter name = " << 
                                         pname << std::endl;
            }
        } else {
            break;
        }
    }
    if ( dbg_->check(6) ) {
        *dbg_ << "ShellGeometry.ctor, shell_type         = " << shell_type_ << trk::endl;
        *dbg_ << "ShellGeometry.ctor, inline_length      = " << inline_length_ << trk::endl;
        *dbg_ << "ShellGeometry.ctor, slice_count        = " << slice_count_ << trk::endl;
    }

    std::string tag;
    SlicePoint  p;
    for ( int n = 0; n < slice_count_ * 5; n++ ) {
        if ( vtxstrm.eof() ) {
            std::cout << "ShellGeometry.ctor, reading point eof detected, n = " << 
                                                   n << std::endl;
            exit(0);
        }
        float x;
        float y;
        float z;
        vtxstrm >> tag >> p.slice_index >> p.point_index >> x >> y >> z;
        p.point = QVector3D(x, y, -z);
        slice_points_ << p;

    }
    if ( dbg_->check(6) ) {
        *dbg_ << "ShellGeometry.ctor, slice_points_.size = " << 
                                       slice_points_.size() << trk::endl;
    }
    basicTriangles(r);
}

trk::ShellGeometry::
ShellGeometry(const std::string& shell_type,
              float*             xf, 
              Ranges& r) : section_xform_(xf), ranges_(r)
{
    dbg_ = DebugCntl::instance();
    shell_type_ = shell_type;
    slice_count_ = 0;
    if ( dbg_->check(6)) {
        std::cout << "ShellGeometry.ctor, xform " << std::endl;
        for ( int i = 0; i < 4; i++) {
            QVector4D row = section_xform_.row(i);
            std::cout << std::setw(15) << row.x() << std::setw(15) << row.y() <<
                       std::setw(15) << row.z() << std::setw(15) << row.w() << std::endl;
        }
    }


}

trk::ShellGeometry::
~ShellGeometry()
{
}

void
trk::ShellGeometry::
add_slice(int slice_index, std::vector<Point>& pts, bool last_slice)
{
    SlicePoint slice_point;
    slice_point.slice_index = slice_index;
    for ( int i = 0; i < 5; i++) {
        QVector4D p = QVector4D(pts[i].x(), pts[i].y(), pts[i].z(), 1.0 );
        QVector4D q = section_xform_ * p;
        if ( (dbg_->check(5) && slice_index < 2) ||dbg_->check(6)) {
            std::cout << "ShellGeometry.add_slice, q   " << q.x() << ", " 
                             << q.y() << ", " << q.z() << ", " << q.w() << std::endl;
            std::cout << "ShellGeometry.add_slice, p   " << p.x() << ", " 
                             << p.y() << ", " << p.z() << std::endl;
        }
        slice_point.point_index = i;
        q.setZ( - q.z() );
        slice_point.point  = QVector3D(q);
        slice_points_ << slice_point;
    }
    if (last_slice) {
        basicTriangles(ranges_);
    }
}

void
trk::ShellGeometry::
basicTriangles(Ranges& r)
{
    vertexes_.clear();
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
        vertexes_ << p0[0]; vertexes_ << p1[0]; vertexes_ << p0[3];
        normals_   << an[0]; normals_   << an[0]; normals_   << an[0];
        vertexes_ << p0[3]; vertexes_ << p1[0]; vertexes_ << p1[3];
        normals_   << an[1]; normals_   << an[1]; normals_   << an[1];
        vertexes_ << p0[3]; vertexes_ << p1[3]; vertexes_ << p0[2];
        normals_   << an[2]; normals_   << an[2]; normals_   << an[2];
        vertexes_ << p0[2]; vertexes_ << p1[3]; vertexes_ << p1[2];
        normals_   << an[3]; normals_   << an[3]; normals_   << an[3];
        vertexes_ << p0[2]; vertexes_ << p1[2]; vertexes_ << p0[1];
        normals_   << an[4]; normals_   << an[4]; normals_   << an[4];
        vertexes_ << p0[1]; vertexes_ << p1[2]; vertexes_ << p1[1];
        normals_   << an[5]; normals_   << an[5]; normals_   << an[5];
        vertexes_ << p0[1]; vertexes_ << p1[1]; vertexes_ << p0[0];
        normals_   << an[6]; normals_   << an[6]; normals_   << an[6];
        vertexes_ << p0[0]; vertexes_ << p1[1]; vertexes_ << p1[0];
        normals_   << an[7]; normals_   << an[7]; normals_   << an[7];
        m = m + 1;
    }
    if ( dbg_->check(6) ) {
        *dbg_ << "ShellGeometry.basicTriangles, # of slices    = " << m << trk::endl;
        *dbg_ << "ShellGeometry.basicTriangles, # of triangles = " << m*8 << trk::endl;
        *dbg_ << "ShellGeometry.basicTriangles, # of vertexes  = " << m*8*3 << trk::endl;
        *dbg_ << "ShellGeometry.basicTriangles, # of normals   = " << m*8*3 << trk::endl;
    }
    r.update(vertexes_);
}

void
trk::ShellGeometry::
makeSlice( QVector3D* p, int n)
{
    p[0] = slice_points_[n+0].point;
    p[1] = slice_points_[n+2].point;
    p[2] = slice_points_[n+3].point;
    p[3] = slice_points_[n+4].point;
}

void
trk::ShellGeometry::
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

QVector<QVector3D>
trk::ShellGeometry::
edge_vertexes(int j)
{
    if ( j < 0 || j >= 5) return edge_vertexes_;

    QVector3D delta;
    float     epsilon = .01;
    int       np      = slice_points_.size() / 5;
    edge_vertexes_.resize(np);
    for (int n = 0; n < np; n++) {
        int l  = j + n * 5;
        int l1 = 1 + n * 5;
        delta = (slice_points_[l].point - slice_points_[l1].point) *epsilon / 1.656250;
        edge_vertexes_[n] = slice_points_[l].point + delta;
    }
    return edge_vertexes_;
}

QVector<QVector3D>
trk::ShellGeometry::
vertexes()
{
    return vertexes_;
}

QVector<QVector3D>
trk::ShellGeometry::
normals()
{
    return normals_;
}

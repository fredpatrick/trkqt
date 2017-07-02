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

#ifndef TRK_SHELLGEOMETRY_HH
#define TRK_SHELLGEOMETRY_HH

#include "point.h"
#include "ranges.h"

#include <string>
#include <fstream>
#include <QColor>
#include <QMatrix4x4>
#include <QVector>
#include <QVector3D>

namespace trk
{
    class DebugCntl;

    struct SlicePoint {
        std::string zonename;
        int         section_index;
        int         slice_index;
        int         point_index;
        QVector3D   point;
    };

    class ShellGeometry
    {
        public:
            ShellGeometry( std::ifstream& vtxstrm, Ranges& r);
            ShellGeometry( const std::string& shell_type, float* xf, Ranges& r);
            ~ShellGeometry();

            void                 add_slice(int                 slice_index, 
                                           std::vector<Point>& slice, 
                                           bool                last_slice = false);
            QVector<QVector3D>   vertexes();
            QVector<QVector3D>   normals();
            QVector<QVector3D>   edge_vertexes(int j);

            std::string          type()    { return shell_type_; }
            float                intline_length() { return inline_length_; }
            int                  size()           { return slice_count_; }
        private:
            void                 basicTriangles(Ranges& r);
            void                 makeSlice(QVector3D* p, int m);      //NOTE: p is returned
            void                 makeNormals(QVector3D* an,
                                             QVector3D* p0, 
                                             QVector3D* p1);          // NOTE: an is returned
            QMatrix4x4           section_xform_;
            Ranges&              ranges_;
            QVector<SlicePoint>  slice_points_;
            QVector<QVector3D>   vertexes_;
            QVector<QVector3D>   normals_;
            QVector<QVector3D>   edge_vertexes_;
            
            std::string          shell_type_;
            float                inline_length_;
            int                  slice_count_;
            DebugCntl*           dbg_;
    };
}

#endif

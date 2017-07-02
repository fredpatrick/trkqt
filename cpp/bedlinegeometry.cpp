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

#include "bedlinegeometry.h"
#include "debugcntl.h"
#include <iostream>
#include <sstream>

trk::BedlineGeometry::
BedlineGeometry(int line_index, std::vector<Point>& line,
                float* xf)                      : section_xform_(xf)
{
    dbg_ = DebugCntl::instance();
    line_index_ = line_index;
    int point_count = line.size();
    for ( int i = 0; i < point_count; i++ ) {
        QVector4D q = QVector4D(line[i].x(), line[i].y(),  line[i].z(), 1.0 );
        QVector3D p = QVector3D(section_xform_ * q );
        p.setZ( -p.z() );
        if ( (dbg_->check(5) && i < 2) )  {
            std:: cout << " i = " << i << "   " << print_point("q", q);
            std:: cout << " i = " << i << "   " << print_point("p", p);
        }
        vertexes_ << p;
    }
}

QVector<QVector3D>
trk::BedlineGeometry::
vertexes()
{
    return vertexes_;
}

std::string
trk::BedlineGeometry::
print_point(const std::string& tag, const QVector4D& p)
{
    std::ostringstream ss;
    ss << tag << std::setw(15) << p.x() << std::setw(15) << p.y() << std::setw(15) << p.z() 
                                        << std::setw(15) << p.w() << std::endl;
    return ss.str();
}

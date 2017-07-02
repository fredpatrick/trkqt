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

#include "ranges.h"
#include <iostream>
#include <sstream>
#include <string>

trk::Ranges::
Ranges()
{
}

trk::Ranges::
~Ranges()
{
}

void    
trk::Ranges::
init()
{
    xmin =  99999.;
    xmax = -99999.;
    ymin =  99999.;
    ymax = -99999.;
    zmin =  99999.;
    zmax = -99999.;
}

void    
trk::Ranges::
update(const QVector<QVector3D>& vertexes )
{
    for (int i = 0; i < vertexes.size(); i++ ) {
        if ( vertexes[i].x() < xmin ) xmin = vertexes[i].x();
        if ( vertexes[i].x() > xmax ) xmax = vertexes[i].x();
        if ( vertexes[i].y() < ymin ) ymin = vertexes[i].y();
        if ( vertexes[i].y() > ymax ) ymax = vertexes[i].y();
        if ( vertexes[i].z() < zmin ) zmin = vertexes[i].z();
        if ( vertexes[i].z() > zmax ) zmax = vertexes[i].z();
    }
}

std::string
trk::Ranges::
print(const std::string& tag)
{
    std::ostringstream ss;
    ss << std::endl << "Ranges.print, for " << tag << std::endl;
    ss << "Ranges, xmin = " << xmin << std::endl;
    ss << "Ranges, xmax = " << xmax << std::endl;
    ss << "Ranges, ymin = " << ymin << std::endl;
    ss << "Ranges, ymax = " << ymax << std::endl;
    ss << "Ranges, zmin = " << zmin << std::endl;
    ss << "Ranges, zmax = " << zmax << std::endl;
    return ss.str();
}

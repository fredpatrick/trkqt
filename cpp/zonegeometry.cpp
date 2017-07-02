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

#include "zonegeometry.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


trk::ZoneGeometry::
ZoneGeometry( std::ifstream& vtxstrm, Ranges& r)
{
    while ( true ) {
        std::string tag;
        std::string pname;
        vtxstrm >> tag >> pname;
        if (tag == "zone" && pname != "end" ) {
            if      ( pname == "zone_name"         ) vtxstrm >> zonename_;
            else if ( pname == "start_switch_name" ) vtxstrm >> start_switch_name_;
            else if ( pname == "start_switch_tag"  ) vtxstrm >> start_switch_tag_;
            else if ( pname == "zone_type"         ) vtxstrm >> zone_type_;
            else if ( pname == "end_switch_name"   ) vtxstrm >> end_switch_name_;
            else if ( pname == "end_switch_tag"    ) vtxstrm >> end_switch_tag_;
            else if ( pname == "section_count"     ) vtxstrm >> section_count_;
            else {
                std::cout << "ZoneGeometry.ctor, unknown parameter name = " << 
                                     pname << std::endl;
            }
        } else {
            break;
        }
    }

    section_geometries_.resize(section_count_);
    for ( int n = 0; n < section_count_; n++) {
            SectionGeometry* section_geometry = new SectionGeometry(vtxstrm, r);
            section_geometries_[n] = section_geometry;
    }
    return;
}

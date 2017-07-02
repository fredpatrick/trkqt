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

#include "pathnode.h"
#include "layoutconfig.h"

#include <iostream>

trk::PathNode::
PathNode()
{
}

trk::PathNode::
~PathNode()
{
}

trk::SwitchNode::
SwitchNode(std::ifstream& cfgstrm)
{
    type_ = "switch";
    cfgstrm >> name_ >> in_tag_ >> out_tag_ >> exit_tag_;
    if ( exit_tag_ != "N/A") cfgstrm >> exit_path_name_;
    if ( (in_tag_ == "A" && out_tag_ == "B" ) ||
         (in_tag_ == "B" && out_tag_ == "A" )    ) path_state_ = THRU;
    else                                           path_state_ = OUT;
}

trk::SwitchNode::
~SwitchNode()
{
}



trk::ZoneNode::
ZoneNode(std::ifstream& cfgstrm)
{
    type_ = "zone";
    cfgstrm >> name_ >> in_section_index_ >> in_section_tag_ >> 
                        out_section_index_ >> out_section_tag_ >>
                        begin_switch_name_ >> begin_switch_tag_ >>
                        end_switch_name_ >> end_switch_tag_;
    LayoutConfig* layout_config = LayoutConfig::instance();
    begin_switch_index_ = layout_config->switch_sensor_index(begin_switch_name_);
    end_switch_index_   = layout_config->switch_sensor_index(end_switch_name_);
}

trk::ZoneNode::
~ZoneNode()
{
}

std::ostream&
trk::operator<<( std::ostream& ostrm, trk::SwitchNode& swnd)
{
    ostrm << "\tSwitchNode - name           = " << swnd.name() << "\n";
    ostrm << "\t             type           = " << swnd.type() << "\n";
    ostrm << "\t             in_tag         = " << swnd.in_tag() << "\n";
    ostrm << "\t             out_tag        = " << swnd.out_tag() << "\n";
    ostrm << "\t             exit_tag       = " << swnd.exit_tag() << "\n";
    ostrm << "\t             exit_path_name = " << swnd.exit_path_name() << "\n";
    return ostrm;
}

std::ostream&
trk::operator<<( std::ostream& ostrm, trk::ZoneNode& znd)
{
    ostrm << "\tZoneNode   - name              = " << znd.name() << "\n";
    ostrm << "\t             type              = " << znd.type() << "\n";
    ostrm << "\t             in_section_tag    = " << znd.in_section_tag() << "\n";
    ostrm << "\t             out_section_tag   = " << znd.out_section_tag() << "\n";
    ostrm << "\t             begin_switch_name = " << znd.begin_switch_name() << "\n";
    ostrm << "\t             begin_switch_tag  = " << znd.begin_switch_tag() << "\n";
    ostrm << "\t             end_switch_name   = " << znd.end_switch_name() << "\n";
    ostrm << "\t             end_switch_tag    = " << znd.end_switch_tag() << "\n";
    return ostrm;
}

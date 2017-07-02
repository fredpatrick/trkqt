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

#ifndef TRK_PATHNODE_HH
#define TRK_PATHNODE_HH


#include "trkutl.h"

#include <string>
#include <fstream>

namespace trk
{
    class PathNode
    {
        public:
            PathNode();
            ~PathNode();
            
            std::string name() const { return name_;}
            std::string type() const { return type_;}

        protected:
            std::string     name_;
            std::string     type_;
    };

    class SwitchNode : public PathNode
    {
        public:
            SwitchNode(std::ifstream& cfgstrm);
            ~SwitchNode();

            std::string     in_tag() const         { return in_tag_; }
            std::string     out_tag() const        { return out_tag_; }
            std::string     exit_tag()const        { return exit_tag_;}
            std::string     exit_path_name() const { return exit_path_name_; }
            SW_DIRECTION    path_state()           { return path_state_; }
        private:
            std::string     in_tag_;
            std::string     out_tag_;
            std::string     exit_tag_;
            std::string     exit_path_name_;
            SW_DIRECTION    path_state_;
    };

    class ZoneNode : public PathNode
    {
        public:
            ZoneNode(std::ifstream& cfgstrm);
            ~ZoneNode();

            int             in_section_index()         { return in_section_index_; }
            std::string     in_section_tag()     const { return in_section_tag_; }
            int             out_section_index()        { return out_section_index_; }
            std::string     out_section_tag()    const { return out_section_tag_; }
            std::string     begin_switch_name()  const { return begin_switch_name_; }
            std::string     begin_switch_tag()   const { return begin_switch_tag_; }
            int             begin_switch_index() const { return begin_switch_index_; }
            std::string     end_switch_name()    const { return end_switch_name_; }
            std::string     end_switch_tag()     const { return end_switch_tag_; }
            int             end_switch_index()   const { return end_switch_index_; }

        private:
            int             in_section_index_;
            std::string     in_section_tag_;
            int             out_section_index_;
            std::string     out_section_tag_;
            std::string     begin_switch_name_;
            std::string     begin_switch_tag_;
            int             begin_switch_index_;
            std::string     end_switch_name_;
            std::string     end_switch_tag_;
            int             end_switch_index_;
    };

    std::ostream&
    operator<<( std::ostream& ostrm, trk::SwitchNode& swnd);

    std::ostream&
    operator<<( std::ostream& ostrm, trk::ZoneNode& znd);
}
            
#endif

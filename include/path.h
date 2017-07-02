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

#ifndef TRK_PATH_HH
#define TRK_PATH_HH

#include "trkutl.h"

#include <fstream>
#include <string>
#include <map>
#include <vector>

namespace trk
{
    class PathNode;
    class SwitchNode;
    class ZoneNode;
    class DebugCntl;
    
    class Path
    {
        public:
            Path(std::ifstream& cfgstrm);
            ~Path();

            SwitchNode*              add_switch_node(std::ifstream& cfgstrm);
            ZoneNode*                add_zone_node(std::ifstream& cfgstrm);
            std::string              path_name()        { return path_name_; }
            std::string              path_type()        { return path_type_; }
            std::string              first_zonename()   { return first_zonename_; }
            std::string              in_switch_name()   { return in_switch_name_; }
            int                      exit_switch_index(){ return exit_switch_index_;}
            std::string              exit_path_name()   { return exit_path_name_; }
            std::vector<int>         zone_indexes()     { return zone_indexes_; }
            std::vector<int>         switch_indexes()   { return switch_indexes_; }
            int                      switch_count()     { return switch_count_; }
            int                      zone_count()       { return zone_count_; }
            int                      first_zone()       { return zone_indexes_[0]; }

            bool                     is_path            ( const std::string& current_zonename,
                                                          const std::string& startup_zonename);
            std::string              next_zonename      ( const std::string& zonename);
            SW_DIRECTION             path_state         ( int switch_index);
            std::string              switch_entry_tag   ( int switch_index);

            std::vector<std::string> pathnodes();
            void                     assign_node_indexes();

            bool                     has_zone     (int zone_index);
            int                      begin_switch_index(int zone_index = 99999);
            std::string              begin_switch_tag  (int zone_index = 99999);
            int                      end_switch_index  (int zone_index = 99999);
            std::string              end_switch_tag    (int zone_index = 99999);
            bool                     has_switch   (int switch_index);

        private:
            DebugCntl*               dbg_;
            std::string              in_switch_name_;
            int                      exit_switch_index_;
            std::string              exit_path_name_;
            std::string              path_name_;
            std::string              path_type_;
            std::vector<PathNode*>   pathnodes_;
            int                      pathnode_count_;
            std::string              first_zonename_;
            std::vector<int>         switch_indexes_;
            std::vector<int>         zone_indexes_;
            std::vector<int>         sw_node_indexes_;
            std::vector<int>         zn_node_indexes_;
            std::vector<SwitchNode*> switch_nodes_;
            std::vector<ZoneNode*>   zone_nodes_;;
            int                      switch_count_;
            int                      zone_count_;

    };

    std::ostream& operator<<( std::ostream& ostrm, trk::Path& p);
}

#endif

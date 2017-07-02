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

#include "path.h"
#include "pathnode.h"
#include "debugcntl.h"
#include "layoutconfig.h"

#include <iostream>
#include <sstream>

trk::Path::
Path(std::ifstream& cfgstrm)
{
    dbg_ = DebugCntl::instance();
    cfgstrm >> path_name_ >> path_type_;
    first_zonename_ = "";
}

trk::Path::
~Path()
{
}

trk::SwitchNode*
trk::Path::
add_switch_node(std::ifstream& cfgstrm)
{
    SwitchNode* switch_node = new SwitchNode(cfgstrm);
    if ( DebugCntl::instance()->check(5) ) {
        std::ostringstream ss;
        ss << *switch_node;
        *dbg_ << ss.str();
    }
    pathnodes_.push_back(switch_node);
    pathnode_count_ = pathnodes_.size();
    if ( switch_node->exit_tag() != "N/A" ) {
        exit_path_name_   = switch_node->exit_path_name();
        std::string exnm = switch_node->name();
        exit_switch_index_ =  LayoutConfig::instance()->switch_sensor_index(exnm);
    }
    return switch_node;
}

trk::ZoneNode*
trk::Path::
add_zone_node(std::ifstream& cfgstrm)
{
    ZoneNode* zone_node = new ZoneNode(cfgstrm);
    if ( DebugCntl::instance()->check(5) ) {
        std::ostringstream ss;
        ss << *zone_node;
        *dbg_ << ss.str();
    }
    pathnodes_.push_back(zone_node);
    pathnode_count_ = pathnodes_.size();
    if ( first_zonename_ == "" ) {
        first_zonename_ = zone_node->name();
        in_switch_name_  = pathnodes_[0]->name();
    }
    return zone_node;
}

void
trk::Path::
assign_node_indexes()
{
    LayoutConfig* layout_config = LayoutConfig::instance();
    int node_count = pathnodes_.size();
    zone_nodes_.clear();
    zone_indexes_.clear();
    switch_nodes_.clear();
    switch_indexes_.clear();
    int switch_count = layout_config->switch_count();
    sw_node_indexes_.resize(switch_count);
    for ( int i = 0; i < switch_count; i++) sw_node_indexes_[i] = 999;
    int zone_count = layout_config->track_sensor_count();
    zn_node_indexes_.resize(zone_count);
    for ( int i = 0; i < zone_count; i++) zn_node_indexes_[i] = 999;

    for ( int i = 0; i < node_count; i++) {
        PathNode* node = pathnodes_[i];
        std::string name = node->name();
        std::string type = node->type();
        if ( type == "switch" ) {
            switch_nodes_.push_back( (SwitchNode*)node );
            int n     = switch_nodes_.size() - 1;
            int index = layout_config->switch_sensor_index(name);
            sw_node_indexes_[index] = n;
            switch_indexes_.push_back(index);
        } else if ( type == "zone" ) {
            zone_nodes_.push_back( (ZoneNode*)node );
            int n = zone_nodes_.size() - 1;
            int index = layout_config->track_sensor_index(name);
            zn_node_indexes_[index] = n;
            zone_indexes_.push_back(index);
        }
    }
    if ( dbg_->check(2) ) {
        *dbg_ << "Path.assign_node, path name = " << path_name_ << trk::endl;
        for ( int j = 0; j < zone_count; j++ ) {
            *dbg_ << "               j = " << j << ", pathnode_index = " << zn_node_indexes_[j]
                                       << trk::endl;
        }
    }

}

trk::SW_DIRECTION
trk::Path::
path_state(int switch_index)
{
    int         n   = sw_node_indexes_.at(switch_index);
    SwitchNode* swn = switch_nodes_[n];
    return swn->path_state();
}

std::string
trk::Path::
switch_entry_tag(int switch_index)
{
    int         n   = sw_node_indexes_.at(switch_index);
    SwitchNode* swn = switch_nodes_[n];
    return swn->in_tag();
}

int
trk::Path::
begin_switch_index(int zone_index)
{
    std::cout << "Path.begin_switch_index, zone_index = " << zone_index << std::endl;
    ZoneNode* zn;
    if ( zone_index == 99999 ) {
        zn = zone_nodes_[0];
    } else {
        int n = zn_node_indexes_.at( zone_index );
        std::cout << "Path.begin_switch_index, n = " << n << std::endl;
        zn    = zone_nodes_[n];
    }
    return zn->begin_switch_index();
}

std::string
trk::Path::
begin_switch_tag(int zone_index)
{
    ZoneNode* zn;
    if ( zone_index == 99999 ) {
        zn = zone_nodes_[0];
    } else {
        int n = zn_node_indexes_.at( zone_index );
        zn    = zone_nodes_[n];
    }
    return zn->begin_switch_tag();
}

int
trk::Path::
end_switch_index(int zone_index)
{
    std::cout << "Path.end, zone_index = " << zone_index << std::endl;
    ZoneNode* zn;
    if ( zone_index == 99999 ) {
        int n = zone_nodes_.size() - 1;
        std::cout << "Path.end_switch_index, n = " << n << std::endl;
        zn = zone_nodes_[n];
    } else {
        int n = zn_node_indexes_.at( zone_index );
        std::cout << "Path.end_switch_index, n = " << n << std::endl;
        zn    = zone_nodes_[n];
    }
    return zn->end_switch_index();
}

std::string
trk::Path::
end_switch_tag(int zone_index)
{
    ZoneNode* zn;
    if ( zone_index == 99999 ) {
        int n = zone_nodes_.size() - 1;
        zn = zone_nodes_[n];
    } else {
        int n = zn_node_indexes_.at( zone_index );
        zn    = zone_nodes_[n];
    }
    return zn->end_switch_tag();
}

bool
trk::Path::
is_path(const std::string& current_zonename, const std::string& startup_zonename)
{
    if ( dbg_->check(3) ) {
        *dbg_ << "Path.is_path, name = " << path_name_ << 
                                 ", current_zonename = " << current_zonename << 
                                 ", startup_zonename = " << startup_zonename << "\n";
    }
    bool found = false;
    for ( int i = 0; i < pathnode_count_; i++) {
        if ( pathnodes_[i]->name() == startup_zonename ) {
            *dbg_ << "Path.is_path, found path containing startup_zone, " <<
                     startup_zonename <<  ", " << this->path_name() << "\n";
            int ii = (i + 2)%pathnode_count_;
            if ( pathnodes_[ii]->name() == current_zonename) {
                found = true;
                if (dbg_->check(3) ) {
                    *dbg_ << "Path.is_path, foundit\n";
                }
                break;
            }
        }
    }
    return found;
}

bool
trk::Path::
has_zone(int zone_index)
{
    if ( zn_node_indexes_.at(zone_index) != 999) return true;
    return false;
}
bool
trk::Path::
has_switch( int switch_index)
{
    if (sw_node_indexes_.at(switch_index) != 999) return true;
    return false;
}

std::string
trk::Path::
next_zonename(const std::string& zonename)
{
    int node_count = pathnodes_.size();
    for ( int i = 0; i < node_count; i++ ) {
        std::string node_name = pathnodes_[i]->name();
        if ( node_name == zonename ) {
            int inext = (i+2)%node_count;
            return pathnodes_[inext]->name();
        }
    }
    return "";
}

std::ostream&
trk::operator<<( std::ostream& ostrm, trk::Path& p)
{
    ostrm << "\tPath       - path_name        = " << p.path_name() << "\n";
    ostrm << "\t             path_type        = " << p.path_type() << "\n";
    ostrm << "\t             in_switch_name   = " << p.in_switch_name() << "\n";
    ostrm << "\t             exit_switch_index= " << p.exit_switch_index() << "\n";
    ostrm << "\t             exit_path_name   = " << p.exit_path_name() << "\n";
    ostrm << "\t             first_zonename   = " << p.first_zonename() << "\n";
    return ostrm;
}

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

#include "layoutstate.h"
#include "cmdpacket.h"
#include "debugcntl.h"
#include "path.h"
#include "pathnode.h"
#include "paths.h"
#include "layoutgeometry.h"
#include "layoutconfig.h"
#include "socketclient.h"
#include "blockevent.h"
#include "switchevent.h"
#include "trackevent.h"
#include "zonegeometry.h"
#include "switch.h"
#include "switcher.h"

#include <iostream>
#include <sstream>
using namespace trk;

trk::LayoutState::
LayoutState(SocketClient* socket_client)
{
    socket_client_   = socket_client;
    dbg_             = DebugCntl::instance();
    if ( dbg_->check(4) ) {
        *dbg_ << trk::jbtime << "-LayoutState.ctor, begin" << trk::endl;
    }
    layout_config_   = LayoutConfig::instance();
    paths_           = Paths::instance();
    layout_geometry_ = LayoutGeometry::instance();

    std::vector<std::string> path_names = paths_->path_names();

    zonenames_ = layout_geometry_->zonenames();
    int zone_count = zonenames_.size();
    for (int i = 0; i < zone_count; i++) {
        ZoneGeometry* zone_geometry = layout_geometry_->zone_geometry(zonenames_[i]);
        zone_geometries_[zonenames_[i] ] = zone_geometry;
    }
    track_sensor_names_ = layout_config_->track_sensor_names();

    track_sensor_count_ = layout_config_->track_sensor_count();
    track_states_.resize(track_sensor_count_);
    active_zones_.resize(track_sensor_count_);
    for ( int i = 0; i < track_sensor_count_; i++) {
        track_states_[i] = IDLE;
        active_zones_[i] = false;
    }
    int n = layout_config_->block_sensor_count();
    block_states_.resize(n);
    for ( int i = 0; i < n; i++) {
        block_states_[i] = STOP;
    }
    switch_count_ = layout_config_->switch_count();
    switches_.resize(switch_count_);
    switch_states_.resize(switch_count_);
    for ( int i = 0; i < switch_count_; i++) {
        switches_[i]      = 0;
        switch_states_[i] = NOVAL;
    }
    switch_names_ = layout_config_->switch_names();
    initialized_   = false;
    in_transition_ = false;

        
    if ( dbg_->check(1) ) {
        *dbg_<< trk::jbtime << "LayoutState.ctor, finished" << trk::endl;;
    }
    //address_dump();
}

trk::LayoutState::
~LayoutState()
{
}

void
trk::LayoutState::
initialize()
{
    std::cout << "LayoutState.initialize, start" << std::endl;
    layout_initialized_ = false;
    for ( int i = 0; i < switch_count_; i++) {
        int swn = layout_config_->switch_sensor_index(switch_names_[i] );
        switches_[swn] = new Switch( switch_names_[swn], socket_client_);
    }
    connect( socket_client_, SIGNAL(track_cmd(CmdPacket*) ),
             this,           SLOT(track_cmd_response(CmdPacket*) ) );
    track_scan();
    switcher_ = new Switcher(socket_client_, THRU);
    connect( switcher_, SIGNAL(switcher_complete(bool) ),
             this,      SLOT(switcher_complete(bool) ) );
}

void
trk::LayoutState::
finish_initialization()
{
    std::cout << "LayoutState.finish_initialization" << std::endl;
    if ( dbg_->check(4) ) {
        *dbg_ << trk::jbtime << "LayoutState.finish_initialization, " <<
                  ", active_zone_count = " << active_zone_count_ << trk::endl;
    }
    previous_path_name_ = "";
    previous_zonename_  = "";
    current_zonename_   = "";
    next_exit_zonename_ = "";
    next_path_zonename_ = "";
    exit_path_expected_ = false;

    startup_           = true;
    track_event_count_ = 0;
    for ( int i = 0; i < 16; i++) switch_bfr_[i] = 0;
    connect( socket_client_, SIGNAL(track_event(TrackEvent*) ),
             this,           SLOT(new_track_event(TrackEvent*) ) );
    connect( socket_client_, SIGNAL(switch_event(SwitchEvent*) ),
             this,           SLOT(new_switch_event(SwitchEvent*) ) );
    connect( socket_client_, SIGNAL(block_event(BlockEvent*) ),
             this,           SLOT(new_block_event(BlockEvent*) ) );
    initialized_ = true;
    emit layoutstate_initialized();
    return;
}

void
trk::LayoutState::
new_block_event(BlockEvent* event)
{
    std::cout << "LayoutState, new_block_event, name = " << event->block_name() << std::endl;
}

void
trk::LayoutState::
new_track_event(TrackEvent* event)
{
    if ( !initialized_ ) return;
    //address_dump();

    std::string zonename     = event->zonename();
    int         zone_index   = event->sensor_index();
    TRK_STATE   track_state  = event->track_state();
    tm_event_                = event->tm_event();
    if ( dbg_->check(2) ) {
        *dbg_<< tm_event_ << "LayoutState.new_track_event-0, ##############################" 
                         << " new zonename = " << zonename << "-" << track_state << trk::endl;
    }

    if ( track_state == IDLE && !current_path_ ) return;

    spurious_event_ = false;
    if ( track_state == BUSY && startup_) {
        startup(zonename);
    } else if ( track_state == BUSY && !startup_  && current_path_) {
        zone_is_busy(zonename, zone_index );
    } else if ( track_state == IDLE  && current_path_) {
        zone_is_idle(zonename, zone_index);
    }

    if ( dbg_->check(2) && !spurious_event_ ) {
        for ( int i = 0; i < switch_count_; i++) {
            *dbg_ << trk::jbtime  << "LayoutState.new_track_event, " << i
                   << " queued =  " <<  dbg_->bools(switches_[i]->queued()) 
                   << ", available = " << dbg_->bools(switches_[i]->available())
                   << ", state = " << switch_states_[i] << trk::endl;
        }
    }
    emit layoutstate_changed(event, active_zones_, spurious_event_);
}

void
trk::LayoutState::
startup(const std::string& zonename)
{
    int zone_index = layout_config_->track_sensor_index(zonename);
    current_path_ = 0;
    std::string startup_zonename = "";
    for ( int i = 0; i < track_sensor_count_; i++) {        // this code won't work if 2 trains
        if ( track_states_[i] == BUSY ) {
            startup_zonename = track_sensor_names_[i];
            current_path_       = paths_->find_startup_path( zonename, startup_zonename ); 
            if ( current_path_ ) break;
        }
    }
    if ( current_path_ != 0 ) {
        active_zones_[zone_index] = true;
        active_zone_count_  += 1;
        previous_zonename_  = startup_zonename;
        current_zonename_   = zonename;
        previous_path_      = 0;
        exit_path_          = paths_->path( current_path_->exit_path_name() );
        next_exit_zonename_ = exit_path_->first_zonename();
        next_path_zonename_ = current_path_->next_zonename(zonename);

        if ( dbg_->check(2) ) {
            *dbg_ << print_state("start_up");
        }
        emit new_path();
        emit zone_busy(zone_index);
        startup_   = false;
    }
}

void
trk::LayoutState::
zone_is_busy(const std::string& zonename, int zone_index)
{
    if (dbg_->check(2) ) {
        *dbg_ << "LayoutState.zone_is_busy,zonename = " << zonename << 
                            ", zone_index = " << zone_index << trk::endl;
    }
    if ( zonename == next_path_zonename_  || zonename == next_exit_zonename_) {
        previous_zonename_  = current_zonename_;
        current_zonename_   = zonename;
        active_zones_[zone_index] = true;
        active_zone_count_  += 1;
        if ( zonename == next_exit_zonename_) {
            if (dbg_->check(2) ) {
                *dbg_ << "LayoutState.zone_is_busy,new path " << trk::endl;
            }
            in_transition_      = true;
            previous_path_      = current_path_;
            current_path_       = exit_path_;
            exit_path_          = paths_->path( current_path_->exit_path_name() );
            next_exit_zonename_ = exit_path_->first_zonename();
            next_path_zonename_ = current_path_->next_zonename(zonename);
            if ( current_path_->path_type() == "transition" ) exit_path_expected_ = true;
            emit new_path();
            if ( dbg_->check(2) ) {
                *dbg_ << print_state("zone_is_busy path");
            }
        }
        next_path_zonename_ = current_path_->next_zonename(zonename);
        set_switches(zone_index);
        emit zone_busy(zone_index);
    } else {
        if ( dbg_->check(3) ) {
            *dbg_ << tm_event_ << "LayoutState, new_track_event, ##########################"
                       << " unexpected zonename = " << zonename << trk::endl;
            *dbg_<< print_state("spurious event");
        }
        spurious_event_ = true;
    }
}

void
trk::LayoutState::
zone_is_idle(const std::string& zonename, int zone_index)
{
    std::cout << "LayoutState.zone_is_idle,zonename = " << zonename << 
                      ",current_path_ = " << current_path_ << std::endl;
    if ( dbg_->check(2) ) {
        *dbg_ << print_state("zone_is_idle");
    }
    if ( active_zones_[zone_index] ) {
        active_zones_[zone_index] = false;
        active_zone_count_  -= 1;
        in_transition_ = false;
        emit zone_idle(zone_index);
        if ( !previous_path_ ) return;

        if ( current_path_->path_type() == "loop" ) {
            std::cout << "LayoutState.zone_is_idle, loop 0" << 
                       ", current_path_ = " << current_path_ << std::endl;
            int begin_switch_index = current_path_->begin_switch_index();
            std::cout << "LayoutState.zone_is_idle, loop 1" << std::endl;
            SW_DIRECTION path_state = current_path_->path_state(begin_switch_index);
            std::cout << "LayoutState.zone_is_idle, loop 2" << std::endl;
            SW_DIRECTION begin_switch_state = switch_states_[begin_switch_index];
            std::cout << "LayoutState.zone_is_idle, loop 3" << std::endl;
            Switch* sw = switches_[begin_switch_index];
            std::cout << "LayoutState.zone_is_idle, loop 4" << std::endl;
            if ( begin_switch_state != path_state ) {
                sw->set_queued(path_state);
                emit switch_queued(begin_switch_index, false);
            }
        } else if ( current_path_->path_type() == "transition" ) {
            std::cout << "LayoutState.zone_is_idle, transition 0" << std::endl;
            int end_switch_index = previous_path_->end_switch_index();
            std::cout << "LayoutState.zone_is_idle, transition 1" << std::endl;
            SW_DIRECTION path_state = previous_path_->path_state(end_switch_index);
            std::cout << "LayoutState.zone_is_idle, transition 2" << std::endl;
            SW_DIRECTION end_switch_state = switch_states_[end_switch_index];
            std::cout << "LayoutState.zone_is_idle, transition 3" << std::endl;
            Switch* sw = switches_[end_switch_index];
            std::cout << "LayoutState.zone_is_idle, transition 4" << std::endl;
            if ( end_switch_state != path_state ) {
                sw->set_queued( path_state );
                emit switch_queued(end_switch_index, false);
            }
        }
    } else {
        if ( dbg_->check(2) ) {
            *dbg_ << tm_event_ << "LayoutState, new_track_event, ##########################"
                       << " unexpected zonename = " << zonename << trk::endl;
            *dbg_<<print_state("spurious event");
        }
        spurious_event_ = true;
    }
}

void
trk::LayoutState::
set_switches(int zone_index)
{
    int end_switch_index    = current_path_->end_switch_index(zone_index);

    Switch*     end_switch         = switches_[end_switch_index];
    int         l                  = track_event_count_ % 16;
    switch_bfr_[l]                 = end_switch;
    track_event_count_ += 1;
    if ( dbg_->check(3) ) {
        *dbg_ << trk::jbtime << "LayoutState::new_track_event, end_switch_index = " 
                      << end_switch_index << ", switch_bfr index = " << l << trk::endl;
    }
    if ( track_event_count_ > 2 ) {
        switch_bfr_ [ (track_event_count_-1)%16 ]->set_available(false);
        switch_bfr_ [ (track_event_count_-3)%16 ]->set_available(true);
        std::string  end_switch_tag  = current_path_->end_switch_tag(zone_index);
        SW_DIRECTION end_swstate     = end_switch->switch_state();
        if ( end_switch_tag == "B" && end_swstate == OUT ) {
                      end_switch->auto_event_coming( THRU);
        } else if ( end_switch_tag == "C" && end_swstate == THRU) {
                      end_switch->auto_event_coming( OUT);
        }
    }
}

void
trk::LayoutState::
leaving_zone()
{
}

void
trk::LayoutState::
track_scan()
{
    std::cout << "LayoutState.track_scan, begin" << std::endl;
    int track_sensor_count = zonenames_.size();
    CmdPacket* cmd_packet = new CmdPacket("scan", "track", track_sensor_count);
    track_seqno_ = cmd_packet->cmd_seqno();
    for (int i = 0; i < track_sensor_count; i++) {
        std::pair<int, int> item;
        item.first = i;
        item.second = -1;
        cmd_packet->item(i, item);
    }
    track_scan_complete_ = false;
    cmd_packet->write(socket_client_);
}

void
trk::LayoutState::
track_cmd_response(CmdPacket* cmnd)
{
    std::cout << "LayoutState.track_cmd_response" << std:: endl;
    std::string command = cmnd->command();
    int seqno = cmnd->cmd_seqno();
    if ( seqno != track_seqno_ ) return ;

    int n_item = cmnd->n_item();
    for ( int i = 0; i < n_item; i++) {
        std::pair<int, int> item = cmnd->item(i);
        int ii = item.first;
        track_states_[ii] = (TRK_STATE)(item.second);
        if ( track_states_[ii] == BUSY ) active_zones_[ii] = true;
    }
    track_scan_complete_ = true;
    if ( track_scan_complete_ && switch_scan_complete_ ) {
        finish_initialization();
    }
}

SW_DIRECTION
trk::LayoutState::
switch_state(int switch_index)
{
    if ( !initialized_ ) return THRU;

    return switch_states_[switch_index];
}

void
trk::LayoutState::
new_switch_event(SwitchEvent* event)
{
    if ( startup_ ) return;
    if ( dbg_->check(2) ) {
        *dbg_<< tm_event_ << "LayoutState.new_switch_event, ##############################" 
                         << " name = " << event->switch_name() << "-" << 
                         event->state() << trk::endl;
    }
    int swn = event->sw_num();
    std::cout << "LayoutState, new_switch_event, swn = " << swn 
               << ", current_path_ = " << current_path_ << std::endl;
    int exit_switch_index = current_path_->exit_switch_index();
    std::cout << "LayoutState, new_switch_event, exit_switch_index = " 
                                          << exit_switch_index << std::endl;
    bool is_exit = false;
    switches_[swn]->switch_event(event);
    switch_states_[swn] = event->state();

    if ( swn == exit_switch_index) is_exit = true;
    emit switch_thrown(swn, is_exit);
}

void
trk::LayoutState::
switcher_complete(bool status)
{
    std::cout << "LayoutState.switcher_complete, status = " << 
                                               std::boolalpha << status << std::endl;
    switch_states_ = switcher_->switch_states();
    switch_scan_complete_ = true;
    delete switcher_;
    if ( track_scan_complete_ && switch_scan_complete_ ) {
        finish_initialization();
    }
}

std::vector<int>
trk::LayoutState::
active_zone_indexes()
{
    std::vector<int> azi;
    for ( int i = 0; i < track_sensor_count_; i++) {
        if ( active_zones_[i] ) azi.push_back(i);
    }
    return azi;
}

void
trk::LayoutState::
next_path()
{
    int switch_index = current_path_->exit_switch_index();
    if (dbg_->check(3) ) {
        *dbg_ << trk::jbtime << "LayoutState::next_path, button pushed, exit_switch is " 
                                                     << switch_index << trk::endl;
    }
    next_exit_zonename_ = exit_path_->first_zonename();
    Switch* exit_switch = switches_[switch_index];
    exit_switch->set_queued(OUT);
    emit switch_queued( switch_index, true);
}

std::string
trk::LayoutState::
print_state(const std::string& msg)
{
    std::ostringstream ss;
    ss << tm_event_ << "LayoutState::new_track_event, ############## new path ############## " 
                                                  << msg << std::endl;
    ss << "   current_path name  = " << std::setw(16) << current_path_->path_name()  << 
           ", next_path_zonename = " << std::setw(6)  << next_path_zonename_ << std::endl;
    ss << "   exit_path name     = " << std::setw(16) << exit_path_->path_name() <<
           ", next_exit_zonename = " << std::setw(6)  << next_exit_zonename_ << std::endl; 
    ss << "   previous_zonename  = " << std::setw(6)  << previous_zonename_ << std::endl;
    return ss.str();
}

bool
trk::LayoutState::
on_path(Path* path, int switch_index)
{
    SW_DIRECTION current_state = switch_states_[switch_index];
    SW_DIRECTION path_state    = path->path_state(switch_index);
    std::cout << "LayoutState.on_path, " << path->path_name() << ", current_state = " << 
                    current_state << ", path_state = " << path_state << std::endl;
    if ( current_state == path_state) return true;
    else                              return false;
}

bool
trk::LayoutState::
entry_tag_is_A(Path* path, int switch_index)
{
    std::string tag = path->switch_entry_tag(switch_index);
    std::cout << "LayoutState.entry_tag_is_A, = " << tag << std::endl;
    if ( tag == "A") return true;
    else             return false;
}

void
trk::LayoutState::
address_dump()
{
    std::cout << "LayoutState.ctor,&zone_geometries_   = " << &zone_geometries_ << std::endl;
    std::cout << "LayoutState.ctor,&zonenames_         = " << &zonenames_      
                                                    << zonenames_[0] << std::endl;
    std::cout << "LayoutState.ctor,&track_states_      = " << &track_states_ 
                                                    << track_states_[0] << std::endl;
    std::cout << "LayoutState.ctor,&track_sensor_names_= " << &track_sensor_names_
                                                    << track_sensor_names_[0] << std::endl;
    std::cout << "LayoutState.ctor,&switch_states_     = " << &switch_states_    
                                                    << switch_states_[0] << std::endl;
    std::cout << "LayoutState.ctor,&block_states_      = " << &block_states_   
                                                    << block_states_[0] << std::endl;
    std::cout << "LayoutState.ctor,&switch_bfr_        = " << &switch_bfr_     
                                                    << switch_bfr_[0] << std::endl;
    std::cout << "LayoutState.ctor,&switches_          = " << &switches_      
                                                    << switches_[0] << std::endl;
    std::cout << "LayoutState.ctor,&switch_names_      = " << &switch_names_ 
                                                    << switch_names_[0] << std::endl;
    std::cout << "LayoutState.ctor,&active_zones_      = " << &active_zones_
                                                    << active_zones_[0] << std::endl;
}
/*
bool
trk::LayoutState::
switch_queued(int switch_index)
{
    Switch* sw = switches_[switch_index];
    std::cout << "LayoutState.switch_queued, " <<  std::boolalpha << sw->queued() << std::endl;
    return sw->queued();
}
*/

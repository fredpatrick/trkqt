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

#include "switch.h"
#include "switchevent.h"
#include "cmdpacket.h"
#include "debugcntl.h"
#include "layoutconfig.h"
#include "socketclient.h"
#include <iomanip>
#include <sstream>

trk::Switch::
Switch(const std::string& switch_name,
       SocketClient*      socket_client)
{
    switch_name_    = switch_name;
    layout_config_  = LayoutConfig::instance();
    switch_index_   = layout_config_->switch_sensor_index(switch_name_);
    socket_client_  = socket_client;
    target_state_   = THRU;
    reset();
    dbg_            = DebugCntl::instance();
}

void
trk::Switch::
reset()
{
    available_          =   false;
    queued_             =   false;
    auto_event_         =   false;
    switch_cmd_sent_    =   false;
}

void
trk::Switch::
set_available( bool available)
{
    available_  =   available;
    if ( dbg_->check(3) ) {
        *dbg_ << trk::jbtime  << "Switch.set_available to " 
                 << dbg_->bools(available) << " for " << switch_name_ << trk::endl;
    }
    if( queued_ && available_ ) throw_switch( target_state_);
}

void
trk::Switch::
set_queued(SW_DIRECTION target_state)
{
    queued_     =   true;
    if ( dbg_->check(3) ) {
        *dbg_ << trk::jbtime << "Switch.set_queued to "  << dbg_->bools(queued_) << 
             " for " << switch_name_ << ", target_state = " << target_state << trk::endl;
    }
    target_state_ = target_state;
    if( queued_ && available_ ) throw_switch( target_state_);
}

void
trk::Switch::
auto_event_coming(SW_DIRECTION target_state)
{
    if ( dbg_->check(2) ) {
        *dbg_ << trk::jbtime << "Switch.auto_event_coming, " << switch_name_ <<
                              ", target_state = " << target_state << trk::endl;
    }
    auto_event_ =   true;
    target_state_ = target_state;
}

void
trk::Switch::
triggered_event_coming(SW_DIRECTION target_state)
{
    if ( dbg_->check(2) ) {
        *dbg_ << trk::jbtime << "Switch.triggered_event_coming, " << switch_name_ << 
                                ", target_state = " << target_state << trk::endl;
    }
    target_state_ = target_state;
}

void
trk::Switch::
throw_switch( SW_DIRECTION target_state)
{
    if ( dbg_->check(3) ) {
        *dbg_ << trk::jbtime << "Switch.throw_switch " << switch_name_ << " to " 
                         << target_state << trk::endl;;
    }
    CmdPacket* cmd_packet = new CmdPacket("set", "switch", 1);
    std::pair<int, int> item;
    item.first  =   switch_index_;
    item.second =   target_state;
    cmd_packet->item(0, item);
    cmd_packet->write(socket_client_);
    reset();
    switch_cmd_sent_    = true;
    expecting_response_ = false;
    event_count_        = 0;
}

void
trk::Switch::
switch_event(SwitchEvent* event)
{
    int             swn =   event->sw_num();
}
                
void
trk::Switch::
scan_switch()
{
    std::cout << "Switch::scan" << std::endl;
    CmdPacket* cmd_packet = new CmdPacket("scan", "switch", 1);
    std::pair<int, int> entry;
    entry.first = switch_index_;
    entry.second = -1;
    cmd_packet->item(0, entry);

    if ( dbg_->check(1) ) {
        *dbg_ << trk::jbtime << "Switch::scan, cmd_seqno = " << 
                              cmd_packet->cmd_seqno() << trk::endl;
    }
    cmd_packet->write(socket_client_);
}

void
trk::Switch::
cmd_response(CmdPacket* cmnd) 
{
    std::string command = cmnd->command();
    int seqno = cmnd->cmd_seqno();
    if ( dbg_->check(4) ) {
        *dbg_ << trk::jbtime << "Switch::cmd_response, seq_no = " << seqno << trk::endl;
    }
    std::pair<int, int> entry = cmnd->item(0);
    int swn = entry.first;
    SW_DIRECTION swd = (SW_DIRECTION)entry.second;
    if ( dbg_->check(1) ) {
        *dbg_ << trk::jbtime << "Switch.cmd_response, swn = " << swn << 
                                          ", swd = " << swd << trk::endl;
    }
    emit cmd_response_received(swn, swd);
}



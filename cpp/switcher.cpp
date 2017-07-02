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

#include "switcher.h"
#include "cmdpacket.h"
#include "debugcntl.h"
#include "jobclock.h"
#include "layoutconfig.h"
#include "socketclient.h"
#include "switchevent.h"

#include <iostream>
#include <iomanip>
#include <sstream>

trk::Switcher::
Switcher(SocketClient* socket_client, SW_DIRECTION target_state)
{
    socket_client_  = socket_client;
    target_state_   = target_state;

    dbg_            = DebugCntl::instance();
    job_clock_      = JobClock::instance();
    layout_config_  = LayoutConfig::instance();

    connect(socket_client_, SIGNAL(switch_cmd(CmdPacket*) ),
            this,           SLOT(scan_response(CmdPacket*) ) );
    connect(socket_client_, SIGNAL(switch_event(SwitchEvent*) ),
            this,           SLOT(switch_event(SwitchEvent*) ) );
    ss_.setf(std::ios_base::fixed, std::ios_base::floatfield);
    ss_ << " sw " << "   begin time " << "    scan time   " << " delta " 
                 << "   event time " << " delta " << " tm delta " << std::endl;
    switch_count_   = layout_config_->switch_count();
    switch_states_.resize(switch_count_);
    for ( int i = 0; i < switch_count_; i++ ) switch_states_[i] = NOVAL;
    current_switch_ = 0;
    next_switch( current_switch_);
    return;
}

trk::Switcher::
~Switcher() 
{
    disconnect(this, SIGNAL(switcher_complete(bool)), 0, 0);
    disconnect(socket_client_, SIGNAL(switch_cmd(CmdPacket*)), 0, 0);
    disconnect(socket_client_, SIGNAL(switch_event(SwitchEvent*)), 0, 0);
}

void
trk::Switcher::
next_switch(int swn)
{
    CmdPacket* cmd_packet = new CmdPacket("scan", "switch", 1);
    scan_seqno_ = cmd_packet->cmd_seqno();
    std::pair<int, int> entry;
    entry.first = swn;
    entry.second = -1;
    cmd_packet->item(0, entry);
    cmd_packet->write(socket_client_);
    tm_begin_ = job_clock_->job_time();
    ss_ << std::setw(4) << current_switch_ << std::setw(12) << tm_begin_;
}

void
trk::Switcher::
scan_response(CmdPacket* cmnd)
{
    int seqno = cmnd->cmd_seqno();
    if ( seqno != scan_seqno_ ) return ;

    std::pair<int, int> entry = cmnd->item(0);
    int swn = entry.first;
    SW_DIRECTION swd = (SW_DIRECTION)entry.second;
    switch_states_[swn] = swd;
    if ( dbg_->check(4) ) {
        *dbg_ <<  "Switcher.scan_response, seq_no = " << seqno <<
                             ",swn = " << swn << ", swd = " << swd << trk::endl;
    }
    delete cmnd;
    tm_scan_  = job_clock_->job_time();
    double dt = tm_scan_ -tm_begin_;
    ss_ << std::setw(12) << tm_scan_ << std::setw(10) << dt;
    if ( swd == target_state_ ) {
        complete(false);
        return;
    }

    CmdPacket* cmd_packet = new CmdPacket("set", "switch", 1);
    std::pair<int, int> item;
    item.first  =   swn;
    item.second =   target_state_;
    cmd_packet->item(0, item);
    cmd_packet->write(socket_client_);
    std::cout << "Switcher.scan_response, set packet written" << std::endl;
    return;
}

void
trk::Switcher::
switch_event(SwitchEvent* event)
{
    std::cout << "Switcher.switch_event" << std::endl;
    int swn = event->sw_num();
    if ( swn != current_switch_ ) return;

    SW_DIRECTION swd    = event->state();
    if ( dbg_->check(4) ) {
        *dbg_ <<  "Switcher.switch_event, swn = " << swn << ", swd = " << swd << trk::endl;
    }
    switch_states_[swn] = swd; 
    tm_event_           = event->tm_event();
    double tm_local_    = job_clock_->job_time();
    dt_event_           = tm_local_ - tm_event_;
    double dt           = tm_local_ - tm_scan_;
    ss_ << std::setw(12) << tm_local_ << std::setw(10) << dt 
                                         << std::setw(10) << dt_event_;
    if ( swd == target_state_ ) {
        complete(true);
    } else {
        std::cout << "Switcher.switch_event, PROBLEM- " << swn << " but wrong state" 
                                           << std::endl;
        emit switcher_complete(false);
    }
    return;
}

void 
trk::Switcher::
complete(bool sw_thrown)
{
    ss_ << std::endl;
    if ( sw_thrown ) {
    }

    current_switch_ +=1;
    if ( current_switch_ < switch_count_ ) {
        next_switch( current_switch_);
    } else {
        *dbg_ << ss_.str() <<  trk::endl;
        emit switcher_complete(true);
    }
    return;
}


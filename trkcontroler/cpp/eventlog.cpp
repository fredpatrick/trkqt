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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include "eventlog.h"
#include <QGroupBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextCursor>
#include <QTextCharFormat>
#include "debugcntl.h"
#include "layoutconfig.h"
#include "cmdpacket.h"
#include "socketclient.h"
#include "layoutstate.h"
#include "layoutgeometry.h"
#include "trackevent.h"
#include "trkutl.h"

trk::EventLog::
EventLog( LayoutState*       layout_state,
          SocketClient*      socket_client,
          const std::string& evtnam,
          QWidget*           parent) : QFrame(parent), evtstrm_(evtnam.c_str())
{
    dbg_ = DebugCntl::instance();
    if ( dbg_->check(4) ) {
        std::cout << "EventLog.ctor, begin" << std::endl;
    }
    layout_config_ = LayoutConfig::instance();
    layout_state_   = layout_state;
    socket_client_  = socket_client;
    setFrameStyle(QFrame::Box | QFrame::Plain);
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(0);
    QGroupBox*   event_log_group = new QGroupBox("Event Log", this);
    QVBoxLayout* event_log_layout = new QVBoxLayout;
    event_log_layout->setSpacing(0);

    text_edit_ = new QPlainTextEdit(this);
    event_log_layout->addWidget(text_edit_);
    QGroupBox* evcm_box = new QGroupBox;
    QHBoxLayout* evcm_layout = new QHBoxLayout;
    eventlog_scan_ = new QPushButton(tr("Scan") );
    eventlog_scan_->setEnabled(false);
    connect (eventlog_scan_, SIGNAL(clicked()), this, SLOT(scan() ) );
    evcm_layout->addWidget(eventlog_scan_, 0);
    evcm_layout->addStretch(10);
    evcm_box->setLayout(evcm_layout);
    event_log_layout->addWidget(evcm_box);
    event_log_group->setLayout(event_log_layout);
    layout->addWidget(event_log_group);
    setLayout(layout);
    resize(1200,800);
    
    text_edit_->setMaximumBlockCount(50);
    cursor_        = text_edit_->textCursor();
    cursor_.movePosition(QTextCursor::End);

    QTextCharFormat format;
    format.setFontFamily("Courier");
    format.setFontFixedPitch(true);
    cursor_.setCharFormat(format);

    std::string line = print_log_header();
    qhdr_  = line.c_str();
    cursor_.insertText(qhdr_);
    evtstrm_ << line;
    line_count_ = 0;
    if ( dbg_->check(4) ) {
        std::cout << "EventLog.ctor, complete" << std::endl;
    }
}

trk::EventLog::
~EventLog()
{
}

void
trk::EventLog::
initialize()
{
    std::cout << "EventLog.initialize" << std::endl;
    eventlog_scan_->setEnabled(true);
    connect( layout_state_, SIGNAL(layoutstate_changed(TrackEvent*,
                                                      const std::vector<bool>&,
                                                      bool) ),
             this,          SLOT(layoutstate_changed(TrackEvent*, 
                                                      const std::vector<bool>&,
                                                      bool) ) );
    connect( socket_client_, SIGNAL( track_cmd(CmdPacket*) ),
             this,           SLOT( cmd_response(CmdPacket*) ) );
    std::string sscan = print_scan_line();
    evtstrm_ << sscan << std::endl;
    cursor_.insertText(sscan.c_str());

}

void
trk::EventLog::
layoutstate_changed(TrackEvent* event,
                    const std::vector<bool>& active_zones,
                    bool spurious_event)
{
    if ( dbg_->check(4) ) {
        std::cout << "EventLog.layoutstate_changed" << std::endl;
    }
    std::string path_name           = layout_state_->current_path_name();
    std::string line = print_log_entry(event, path_name, active_zones, spurious_event);
    evtstrm_ << line << std::endl;
    text_edit_->appendPlainText(line.c_str() );
    line_count_++;
    if ( line_count_ > 50) {
        cursor_.insertText(qhdr_);
        line_count_ = 0;
    }
}

std::string
trk::EventLog::
print_log_header()
{
    LayoutGeometry* layout_geometry = LayoutGeometry::instance();
    std::vector<std::string> zonenames = layout_geometry->zonenames();
    std::ostringstream ans;
    ans << "  Time  ";
    int zone_count = zonenames.size();
    for (int i = 0; i < zone_count; i++) {
        ans << "| " << zonenames[i] << "  ";
    }
    ans << std::endl;
    return ans.str();
}

std::string
trk::EventLog::
print_scan_line()
{
    std::vector<TRK_STATE> track_states = layout_state_->track_states();
    std::cout << "EventLog.print_scan_line, track_states" << std::endl;
    int track_state_count = track_states.size();

    std::ostringstream ans;
    ans << "        ";
    for ( int i = 0; i < track_state_count; i++) {
        ans << "| " << track_states[i]  << "  ";
    }
    return ans.str();
}

std::string
trk::EventLog::
print_log_entry(TrackEvent* event,
                const std::string&       current_path_name,
                const std::vector<bool>& active_zones,
                bool spurious_event)
{
    std::ostringstream ans;

    ans.setf(std::ios_base::fixed, std::ios_base::floatfield);
    ans.precision(4);
    double      tm_event   = event->tm_event();
    TRK_STATE   trkstate  = event->track_state();
    int track_sensor_index = event->sensor_index();
    ans << std::setw(8) << tm_event;
    int n = layout_config_->track_sensor_count();
    for (int i = 0; i < n; i++ ) {
        if ( track_sensor_index == i ) {
            if ( !spurious_event ) {
                if ( trkstate == BUSY ) {
                    ans << "| BUSY  ";
                } else if ( trkstate == IDLE ) {
                    ans << "| IDLE  ";
                } else {
                    ans << "|XXXXXXX";
                }
            } else {
                ans << "spurious";
            }
        } else if ( active_zones[i] ) {
            ans << "|   *   ";
        } else {
            ans << "|       ";
        }
    }
    
    ans << "| " << current_path_name;
    return ans.str();
}

void
trk::EventLog::
scan()
{
    std::cout << "EventLOg.initialize" << std::endl;
    CmdPacket* cmd_packet = new CmdPacket("scan", "track", 9);
    for (int i = 0; i < 9; i++) {
        std::pair<int, int> item;
        item.first = i;
        item.second = -1;
        cmd_packet->item(i, item);
    }
    std::cout << "EventLog.initialize, ready to write cmd_pack" << std::endl;
    cmd_packet->write(socket_client_);
}


void
trk::EventLog::
cmd_response(CmdPacket* cmnd) 
{
    std::cout << "EventLog.cmd_response" << std::endl;
    std::string command = cmnd->command();
    int seqno = cmnd->cmd_seqno();
    std::cout << "EventLog::cmd_response, seq_no = " << seqno << std::endl;
    int n_item = cmnd->n_item();

    std::ostringstream ans;
    ans << "\n        ";
    for ( int i = 0; i < n_item; i++) {
        std::pair<int, int> item = cmnd->item(i);

        TRK_STATE trk_state = (TRK_STATE)item.second;
        ans << "| " << trk_state  << "  ";
    }
    evtstrm_ << ans;
    cursor_.insertText(ans.str().c_str());
}

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

#include "switchclient.h"
#include "switchbox.h"
#include "socketclient.h"
#include "cmdpacket.h"
#include "debugcntl.h"
#include "layoutconfig.h"
#include "eventdevice.h"
#include "switchevent.h"
#include "trkutl.h"

#include <QPushButton>
#include <QButtonGroup>
#include <QRadioButton>
#include <QLabel>
#include <QVBoxLayout>

#include <string>
#include <vector>
#include <iostream>
#include <utility>

trk::SwitchClient::
SwitchClient(QWidget* parent) : QFrame(parent)
{
    dbg_ = trk::DebugCntl::instance();

    LayoutConfig* layout_config = LayoutConfig::instance();
    socket_client_ = 0;
    setFrameStyle(QFrame::Box | QFrame::Plain);
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(0);
    QGroupBox* switch_group = new QGroupBox("Switch Controls", this);
    switch_group->setFlat(true);
    QVBoxLayout* switch_layout = new QVBoxLayout;
    switch_layout->setSpacing(0);
    std::vector<std::string> switch_names = layout_config->switch_names();
    int switch_count = switch_names.size();
    for ( int i = 0; i < switch_count; i++) {
        sw_box_[i] = new SwitchBox(this, switch_names[i]);
        switch_layout->addWidget(sw_box_[i]);
    }

    state_       = STARTUP;
    QGroupBox* cmnd_box = new QGroupBox;
    cmnd_box->setFlat(true);
    QHBoxLayout* cmnd_layout = new QHBoxLayout;
    cmnd_layout->setSpacing(0);
    scan_button_ = new QPushButton(tr("Scan") );
    cmnd_layout->addWidget(scan_button_, 0);
    cmnd_layout->addStretch(10);
    cmnd_box->setLayout(cmnd_layout);
    switch_layout->addWidget(cmnd_box);
    switch_group->setLayout(switch_layout);
    layout->addWidget(switch_group);

    scan_button_->setEnabled(false);
    connect(scan_button_, SIGNAL(clicked()), this, SLOT(scan() ) );
    setLayout(layout);
}

trk::SwitchClient::
~SwitchClient()
{
}

void
trk::SwitchClient::
initialize(SocketClient* sc)
{
    socket_client_ = sc;
    if ( socket_client_ == 0 ) {
        std::cout << "SwitchClient,initialize" << std::endl;
    }
    connect( socket_client_, SIGNAL(switch_cmd(CmdPacket*) ),
             this,           SLOT(cmd_response(CmdPacket*) ) );
    connect( socket_client_, SIGNAL(switch_event(SwitchEvent*) ),
             this,           SLOT(next_switch_event(SwitchEvent*) ) );
    scan_button_->setEnabled(true);
}

void
trk::SwitchClient::
scan()
{
    CmdPacket* cmd_packet = new CmdPacket("scan", "switch", 6);
    for (int i = 0; i < 6; i++) {
        std::pair<int, int> entry;
        entry.first = i;
        entry.second = -1;
        cmd_packet->item(i, entry);
    }
    if ( dbg_->check(5) ) {
        *dbg_ << "SwitchClient::initialize, cmd_seqno = " << 
                                  cmd_packet->cmd_seqno() << trk::endl;
    }
    cmd_packet->write(socket_client_);
}

void
trk::SwitchClient::
cmd_response(CmdPacket* cmnd) 
{
    std::string command = cmnd->command();
    int seqno = cmnd->cmd_seqno();
    if ( dbg_->check(5) ) {
        *dbg_ << "SwitchClient::cmd_response, seq_no = " << seqno << trk::endl;
    }
    int nitem = cmnd->n_item();
    for ( int i = 0; i < nitem; i++) {
        std::pair<int, int> entry = cmnd->item(i);
        int swn = entry.first;
        SW_DIRECTION swd = (SW_DIRECTION)entry.second;
        if ( dbg_->check(5) ) {
            *dbg_ << "SwitchClient.cmd_response, swn = " << swn << 
                                              ", swd = " << swd << trk::endl; 
        }
        sw_box_[swn]->set(swd);
    }
}

void 
trk::SwitchClient::
switch_changed(int id)
{
    int sw_number = id / 10;
    int sw_direc  = id - sw_number * 10;
    if  ( dbg_->check(5) ) {
        *dbg_ << "SwitchClient.switch_changed, sw_number = " << sw_number <<
                               ", sw_direc = " << (SW_DIRECTION)sw_direc << trk::endl;
    }
    CmdPacket* cmd_packet = new CmdPacket("set", "switch", 1);
    std::pair<int, int> item;
    item.first = sw_number;
    item.second = sw_direc;
    cmd_packet->item(0, item);
    cmd_packet->write(socket_client_);
}

void
trk::SwitchClient::
next_switch_event(SwitchEvent* event)
{
    int          swn = event->sw_num();
    SW_DIRECTION sw_state  = event->state();
    
    if ( dbg_->check(5) ) {
        *dbg_ << "SwitchClient.next_switch_event, swn = " << swn <<
                                                     ",state = " << sw_state << trk::endl;
    }
    sw_box_[swn]->next_event(sw_state);
    if ( dbg_->check(5) ) {
        *dbg_ << "SwitchClient.next_switch_event, complete" << trk::endl;
    }
}

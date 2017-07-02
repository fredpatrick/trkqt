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

#include "blockclient.h"
#include "blockbox.h"
#include "socketclient.h"
#include "CmdPacket.h"
#include "DebugCntl.h"
#include "EventDevice.h"
#include "BlockEvent.h"
#include "LayoutConfig.h"
#include "trkutl.h"

#include <QPushButton>
#include <QButtonGroup>
#include <QRadioButton>
#include <QLabel>
#include <QVBoxLayout>

#include <string>
#include <iostream>
#include <utility>

trk::BlockClient::
BlockClient(QWidget* parent) : QFrame(parent)
{
    dbg_ = trk::DebugCntl::instance();
    socket_client_ = 0;
    layout_config_     = LayoutConfig::instance();
    block_names_       = layout_config_->block_sensor_names();
    setFrameStyle(QFrame::Box | QFrame::Plain);
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(0);
    QGroupBox* block_group = new QGroupBox("Block Controls", this);
    QVBoxLayout* block_layout = new QVBoxLayout;
    block_layout->setSpacing(0);
    for ( int i = 0; i < (int)block_names_.size(); i++) {
        blk_box_[i] = new BlockBox(this, i, block_names_[i]);
        block_layout->addWidget(blk_box_[i]);
    }

    state_       = STARTUP;
    QGroupBox* cmnd_box = new QGroupBox;
    QHBoxLayout* cmnd_layout = new QHBoxLayout;
    cmnd_layout->setSpacing(0);
    scan_button_ = new QPushButton(tr("Scan") );
    cmnd_layout->addWidget(scan_button_, 0);
    cmnd_layout->addStretch(10);
    cmnd_box->setLayout(cmnd_layout);
    block_layout->addWidget(cmnd_box);
    block_group->setLayout(block_layout);
    layout->addWidget(block_group);

    scan_button_->setEnabled(false);
    connect(scan_button_, SIGNAL(clicked()), this, SLOT(scan() ) );
    setLayout(layout);
}

trk::BlockClient::
~BlockClient()
{
}

void
trk::BlockClient::
initialize(SocketClient* sc)
{
    socket_client_ = sc;
    if ( socket_client_ == 0 ) {
        std::cout << "BlockClient,socked_client, socket_client_ = 0" << std::endl;
    }
    connect( socket_client_, SIGNAL(block_cmd(CmdPacket*) ),
             this,           SLOT(cmd_response(CmdPacket*) ) );
    connect( socket_client_, SIGNAL(block_event(BlockEvent*) ),
             this,           SLOT(next_block_event(BlockEvent*) ) );
    scan_button_->setEnabled(true);
    state_ = INITIALIZING;
    scan();
}

void
trk::BlockClient::
scan()
{
    CmdPacket* cmd_packet = new CmdPacket("scan", "block", 2);
    for (int i = 0; i < (int)block_names_.size(); i++) {
        std::pair<int, int> entry;
        entry.first = i;
        entry.second = -1;
        cmd_packet->item(i, entry);
    }
    if ( dbg_->check(4) ) {
        std::cout << "BlockClient::initialize, cmd_seqno = " << 
                              cmd_packet->cmd_seqno() << std::endl;
    }
    cmd_packet->write(socket_client_);
}

void
trk::BlockClient::
cmd_response(CmdPacket* cmnd) 
{
    std::string command = cmnd->command();
    int seqno = cmnd->cmd_seqno();
    if ( dbg_->check(4) ) {
        std::cout << "BlockClient::cmd_response, seq_no = " << seqno << std::endl;
    }
    int nitem = cmnd->n_item();
    for ( int i = 0; i < nitem; i++) {
        std::pair<int, int> entry = cmnd->item(i);
        int blk_number            = entry.first;
        BLK_STATE blk_state       = (BLK_STATE)entry.second;
        if ( dbg_->check(4) ) {
            std::cout << "BlockClient.cmd_response, blk_number = " << blk_number << 
                                              ", blk_state = " << blk_state << std::endl; 
        }
        blk_box_[blk_number]->set(blk_state);
    }
}

void 
trk::BlockClient::
block_changed(int id)
{
    int blk_number = id / 10;
    int blk_state  = id - blk_number * 10;
    if  ( dbg_->check(4) ) {
        std::cout << "BlockClient.block_changed, blk_number = " << blk_number <<
                               ", blk_state = " << (BLK_STATE)blk_state << std::endl;
    }
    CmdPacket* cmd_packet = new CmdPacket("set", "block", 1);
    std::pair<int, int> item;
    item.first = blk_number;
    item.second = blk_state;
    cmd_packet->item(0, item);
    cmd_packet->write(socket_client_);
}

void
trk::BlockClient::
next_block_event(BlockEvent* event)
{
    std::string  block_name = event->block_name();
    int          blkn       = layout_config_->block_sensor_index(block_name);
    BLK_STATE    blk_state  = event->block_state();
    
    if ( dbg_->check(4) ) {
        std::cout << "BlockClient.next_block_event, blk_number = " << blkn <<
                                                 ", block_name = " << block_names_[blkn] <<
                                                 ",blk_state = " << blk_state << std::endl;
    }
    blk_box_[blkn]->next_event(blk_state);
    if ( dbg_->check(4) ) {
        std::cout << "BlockClient.next_block_event, complete" << std::endl;
    }
}

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
#include "trkcontroler.h"
#include "eventlog.h"
#include "trkviewer.h"
#include "switchclient.h"
#include "blockclient.h"
#include "filestore.h"
#include "inipacket.h"
#include "jobclock.h"
#include "layoutstate.h"
#include "socketclient.h"
#include "cmdpacket.h"

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QLayout>
#include <QPlainTextEdit>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QColor>
#include <QStringList>
#include <QResizeEvent>
#include <QFrame>

trk::TrkControler::
TrkControler(QWidget *parent) : QDialog(parent)
{
    std::cout << "################################################# TrkControler" << std::endl;
    std::cout << "##############################################################" << std::endl;
    setWindowTitle("Track Controler");
    eventlog_ = 0;
    dbg_ = DebugCntl::instance();

    QVBoxLayout* dialog_layout = new QVBoxLayout;
    dialog_layout->setSpacing(0);

    QGroupBox* main_group = new QGroupBox;
    main_group->setFlat(true);
    QHBoxLayout* main_layout = new QHBoxLayout;
    main_layout->setSpacing(0);

    QGroupBox* cmndbox = new QGroupBox;
    cmndbox->setFlat(true);
    QVBoxLayout* cmnd_layout = new QVBoxLayout;
    cmnd_layout->setSpacing(0);
    switch_client_ = new SwitchClient(this);
    cmnd_layout->addWidget(switch_client_, 0);
    block_client_  = new BlockClient(this);
    cmnd_layout->addWidget(block_client_,0);

    QFrame* configuration = new QFrame(this);
    configuration->setFrameStyle(QFrame::Box | QFrame::Plain);
    QGroupBox* cfggroup = new QGroupBox("Configuration", configuration);
    cfggroup->setFlat(true);
    QVBoxLayout* cfglayout = new QVBoxLayout;
    QGroupBox* cfgbtn_group = new QGroupBox;
    QHBoxLayout* cfgbtn_layout = new QHBoxLayout;
    eventlog_button_   = new QPushButton(tr("EventLog") );
    eventlog_button_->setEnabled(false);
    layoutview_button_ = new QPushButton(tr("LayoutView") );
    layoutview_button_->setEnabled(false);
    cfgbtn_layout->addWidget(eventlog_button_, 0);
    cfgbtn_layout->setSpacing(5);
    cfgbtn_layout->addWidget(layoutview_button_, 0);
    cfgbtn_layout->addStretch(10);
    cfgbtn_group->setLayout(cfgbtn_layout);
    cfglayout->addWidget(cfgbtn_group, 0);
    configuration->setLayout(cfglayout);
    cmnd_layout->addWidget(configuration);
    //connect(eventlog_button_, SIGNAL(clicked()), this, SLOT(eventlog()) );

    QFrame* controls = new QFrame(this);
    controls->setFrameStyle(QFrame::Box | QFrame::Plain);
    QGroupBox* ctrlgroup = new QGroupBox("Controls", controls);
    ctrlgroup->setFlat(true);
    QVBoxLayout* ctrllayout = new QVBoxLayout;
    QGroupBox* ctrlbtn_group = new QGroupBox;
    QHBoxLayout* ctrlbtn_layout = new QHBoxLayout;
    nextpath_button_   = new QPushButton(tr("Next Path") );
    nextpath_button_->setEnabled(false);
    autorun_button_ = new QPushButton(tr("Auto Run") );
    autorun_button_->setEnabled(false);
    ctrlbtn_layout->addWidget(nextpath_button_, 0);
    ctrlbtn_layout->setSpacing(5);
    ctrlbtn_layout->addWidget(autorun_button_, 0);
    ctrlbtn_layout->addStretch(10);
    ctrlbtn_group->setLayout(ctrlbtn_layout);
    ctrllayout->addWidget(ctrlbtn_group, 0);
    controls->setLayout(ctrlbtn_layout);
    cmnd_layout->addWidget(controls);
    cmnd_layout->addStretch(10);
    cmndbox->setLayout(cmnd_layout);


    main_layout->addWidget(cmndbox,0);
    main_group->setLayout(main_layout);

    connect_button_ = new QPushButton(tr("Connect") );
    disconnect_button_ = new QPushButton(tr("Disconnect") );
    quit_button_ = new QPushButton(tr("Quit") );
    shutdown_button_ = new QPushButton(tr("Shutdown") );
    disconnect_button_->setEnabled(false);
    dialog_bb_ = new QDialogButtonBox;
    dialog_bb_->addButton(connect_button_, QDialogButtonBox::ActionRole);
    dialog_bb_->addButton(disconnect_button_, QDialogButtonBox::ActionRole);
    dialog_bb_->addButton(quit_button_, QDialogButtonBox::RejectRole);
    dialog_bb_->addButton(shutdown_button_, QDialogButtonBox::RejectRole);
    connect(connect_button_, SIGNAL(clicked()), this, SLOT(connect_to_BBB()) );
    connect(disconnect_button_, SIGNAL(clicked()), this, SLOT(disconnect_from_BBB()) );
    connect(quit_button_, SIGNAL(clicked()), this, SLOT(disconnect_from_BBB()) );
    connect(shutdown_button_, SIGNAL(clicked()), this, SLOT(shutdown_BBB_client()) );
    dialog_layout->addWidget(main_group, 0);
    dialog_layout->addWidget(dialog_bb_, 0);
    setLayout(dialog_layout);
    resize(600,800);
    move(100,0);
    socket_client_ = new SocketClient(this);
    layoutstate_   = new LayoutState(socket_client_);
    connect(layoutstate_,     SIGNAL(layoutstate_initialized() ),
            this,             SLOT(enable_layoutstate_buttons() ) );
    connect(nextpath_button_, SIGNAL(clicked() ), 
            layoutstate_,     SLOT(next_path() ) );
    trkviewer_ = new TrkViewer(layoutstate_,
                               0);
    eventlog_  = new EventLog(layoutstate_,
                              socket_client_,
                              FileStore::instance()->evtfil(),
                              0);
    connect(layoutstate_, SIGNAL(layoutstate_initialized() ),
            eventlog_,    SLOT(initialize() ) );
    eventlog_->show();
    trkviewer_->show();
}

trk::TrkControler::
~TrkControler()
{
}

void
trk::TrkControler::
connect_to_BBB()
{
    std::cout << "#####################################################################";
    std::cout << "Connect to BeagleBone" << std::endl;
    std::cout << "#####################################################################";
    std::cout << std::endl;
    connect ( socket_client_->socket(), SIGNAL(connected() ), 
              this,                     SLOT(socket_connected() ) );
    connect ( socket_client_, SIGNAL(ini_packet(IniPacket*) ),
              this,           SLOT(ini_packet_received(IniPacket*) ) );
    socket_client_->connect_to_server("192.168.1.167", 17303);
}

void 
trk::TrkControler::
socket_connected()
{
    IniPacket* ip = new IniPacket("tod");
    std::string todts = JobClock::instance()->tod_timestamp();
    ip->tod_timestamp( todts);
    ip->write(socket_client_);
    delete ip;

}

void 
trk::TrkControler::
ini_packet_received(IniPacket* ip)
{
    std::string type = ip->type();

    if ( type != "btm" ) return;
    double t0 = ip->t0();
    double t1 = ip->t1();
    JobClock* job_clock = JobClock::instance();
    job_clock->base_time(t0);
    double tt = job_clock->job_time();
    *dbg_ << "Trkcontroler::ini_packet_received, t0 = " << t0 << ", t1 = " << t1 
                                           << ", tt = " << tt << trk::endl; 
    CmdPacket* cmd_packet = new CmdPacket("finish_startup", "", 0);
    cmd_packet->write(socket_client_);
    delete cmd_packet;

    switch_client_->initialize(socket_client_);
    block_client_->initialize(socket_client_);
    
    layoutstate_->initialize();
    eventlog_button_->setEnabled(false);
    disconnect_button_->setEnabled(true);
    connect_button_->setEnabled(false);
}

void
trk::TrkControler::
enable_layoutstate_buttons()
{
    std::cout << "Trkcontroler.enable_layoutstate_buttons" << std::endl;
    nextpath_button_->setEnabled(true);
}

void
trk::TrkControler::
eventlog()
{
    eventlog_button_->setEnabled(false);
}

void
trk::TrkControler::
disconnect_from_BBB()
{
    if ( socket_client_ ) {
        CmdPacket* cmd = new CmdPacket("break", "", 0);
        cmd->write(socket_client_);
    }
    delete socket_client_;
    if ( eventlog_ ) eventlog_->close();
    if ( trkviewer_) trkviewer_->close();
    close();
}

void
trk::TrkControler::
shutdown_BBB_client()
{
    if ( socket_client_ ) {
        CmdPacket* cmd = new CmdPacket("shutdown", "", 0);
        cmd->write(socket_client_);
    }
    delete socket_client_;
    if ( eventlog_ ) eventlog_->close();
    if ( trkviewer_) trkviewer_->close();
    close();
}

void::
trk::TrkControler::
resizeEvent(QResizeEvent* event)
{
   QSize osz = event->oldSize();
    QSize nsz = event->size();

}                                         

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

#include <QTcpSocket>
#include "socketclient.h"
#include "packetbuffer.h"
#include "blockevent.h"
#include "breakevent.h"
#include "switchevent.h"
#include "trackevent.h"
#include "cmdpacket.h"
#include "inipacket.h"
#include "msgpacket.h"
#include "debugcntl.h"
#include "jobclock.h"

trk::SocketClient::
SocketClient(QObject* parent)
{
    dbg_      = DebugCntl::instance();
    jobclock_ = JobClock::instance();
    socket_   = new QTcpSocket(parent);
    blklen_   = 0;
}

void
trk::SocketClient::
connect_to_server(const QString& ipaddr, int portno)
{
    connect( socket_, SIGNAL(readyRead() ),
             this,    SLOT(read_packet() ) );
    socket_->connectToHost(ipaddr, portno);
}

trk::SocketClient::
~SocketClient()
{
}

QTcpSocket*
trk::SocketClient::
socket()
{
    return socket_;
}

void
trk::SocketClient::
read_packet()
{
    int n = socket_->bytesAvailable();
    if ( dbg_->check(5) ) {
        *dbg_ << "SocketClient.read_packet,bytesAvailable = " << n << 
                                         ", blklen_ = " << blklen_ << trk::endl;
    }
    //QDataStream in(socket_);
    bool done = false;
    while ( !done ) {
        if ( blklen_ == 0 ) {
            int nc = socket_->bytesAvailable();
            if ( nc < (int) sizeof(quint32) ) return;
//          int nb = in.readRawData((char*)&blklen_, sizeof(qint32) );
            int nb = socket_->read( (char*)&blklen_, sizeof(qint32) );
            if ( dbg_->check(5) ) {
                *dbg_ << "SocketClient.read_packet, blklen_ = " << blklen_ << 
                                                ", nb = " << nb << trk::endl;
            }
        } else {
            if ( socket_->bytesAvailable() < blklen_ ) return;
            char* bfr = new char[blklen_];
//          int nb = in.readRawData(bfr, blklen_);
            int nb = socket_->read(bfr, blklen_);
            if ( dbg_->check(5) ) {
                *dbg_ << "SocketClient.read_packet, blklen_ = " << blklen_ << 
                                                ", nb = " << nb << trk::endl;
            }
            PacketBuffer* pckt = new PacketBuffer(blklen_, bfr);
            delete[]  bfr;

            distribute_packet(pckt); 

            blklen_ = 0;
        }
    }
}

int
trk::SocketClient::
write(PacketBuffer* pckt)
{
    QDataStream out(socket_);
    int    bfrlen = pckt->bfrlen();
    char*  bfr    = pckt->bfr();
    int nb = out.writeRawData((char*)&bfrlen, sizeof(qint32) );
    int nr = out.writeRawData(bfr, bfrlen);
    socket_->flush(); 
    return nr;
}

trk::PacketBuffer*
trk::SocketClient::
read()
{
    std::cout << "SocketClient.rerad not implemented" << std::endl;
    return 0;
}
            

void
trk::SocketClient::
distribute_packet(PacketBuffer* pbfr)
{
    std::string tag = pbfr->tag();
    if ( dbg_->check(2) ) {
        *dbg_ << trk::jbtime  << "SocketClient.distribute, tag = " << tag << trk::endl;
    }

    if ( tag == "INI" ) {
        IniPacket* ip = new IniPacket(pbfr);
        emit ini_packet(ip);
    } else if ( tag == "BLK" ) {
        BlockEvent* event = new BlockEvent(pbfr);
        emit block_event(event);
    } else if (tag == "BRK" ) {
        BreakEvent* event = new BreakEvent(pbfr);
        emit break_event(event);
    } else if (tag == "SW ") {
        SwitchEvent* event = new SwitchEvent(pbfr);
        emit switch_event(event);
    } else if (tag == "TRK") {
        TrackEvent* event = new TrackEvent(pbfr);
        emit track_event(event);
    } else if (tag == "CMD") {
        CmdPacket* cmnd = new CmdPacket(pbfr);
        std::string type = cmnd->type();
        if        (type == "switch" ) {
            emit switch_cmd(cmnd);
        } else if (type == "track" ) {
            emit track_cmd(cmnd);
        } else if (type == "block" ) {
            emit block_cmd(cmnd);
        } else {
            std::cout << "SocketClient::distribute_packet, type = " << type << 
                                              " unknown" << std::endl;
        }
    } else if (tag == "MSG") {
        MsgPacket* message = new MsgPacket(pbfr);
        emit msg_packet(message);
    }
}

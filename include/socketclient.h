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

#ifndef TRK_SOCKETCLIENT_HH
#define TRK_SOCKETCLIENT_HH

#include <QString>
#include <QTcpSocket>
#include "EventDevice.h"

namespace trk 
{
    class PacketBuffer;
    class BlockEvent;
    class BreakEvent;
    class SwitchEvent;
    class TrackEvent;
    class CmdPacket;
    class MsgPacket;
    class IniPacket;
    class DebugCntl;
    class JobClock;

    class SocketClient : public QObject, public EventDevice
    {
        Q_OBJECT

        public:
            SocketClient(QObject* parent);
            ~SocketClient();

            QTcpSocket*   socket();
            void          connect_to_server( const QString& ipaddr,
                                             int            portno);
            int           write(PacketBuffer* packet);
            PacketBuffer* read();
        protected:
            void          distribute_packet(PacketBuffer* pbfr);
        private slots:
            void read_packet();

        signals:
            void break_event(BreakEvent* event);
            void block_event(BlockEvent* event);
            void switch_event(SwitchEvent* event);
            void track_event(TrackEvent* event);
            void switch_cmd(CmdPacket* cmnd);
            void track_cmd(CmdPacket* cmnd);
            void block_cmd(CmdPacket* cmnd);
            void msg_packet(MsgPacket* message);
            void ini_packet(IniPacket* ip);

        private:
            int         blklen_;
            QTcpSocket* socket_;
            DebugCntl*  dbg_;
            JobClock*   jobclock_;
    };
}
#endif

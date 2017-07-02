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

#ifndef TRK_EVENTLOG_HH
#define TRK_EVENTLOG_HH
#include <string>
#include <vector>
#include <fstream>
#include <QString>
#include <QPlainTextEdit>
#include <QFrame>

class QPushButton;
class QTextCursor;


namespace trk
{
    class DebugCntl;
    class CmdPacket;
    class SocketClient;
    class TrackEvent;
    class LayoutState;
    class LayoutConfig;

    class EventLog : public QFrame
    {
        Q_OBJECT

        public:
            EventLog(LayoutState*       layout_state,
                     SocketClient*      socket_client,
                     const std::string& ev_lognam,
                     QWidget*           parent=0);
            ~EventLog();
            
            void        socket_client(SocketClient* sc);

        public slots:
            void        initialize();
            void        layoutstate_changed(TrackEvent*, const std::vector<bool>&, bool);
        private slots:
            void        scan();
            void        cmd_response(CmdPacket*);

        private:
            std::string     print_log_header();
            std::string     print_scan_line();
            std::string     print_log_entry(TrackEvent*              event, 
                                            const std::string&       path_name,
                                            const std::vector<bool>& active_zones,
                                            bool                     spurious_event);

            QPushButton*    eventlog_scan_;
            QPlainTextEdit* text_edit_;
            QTextCursor     cursor_;
            int             line_count_;
            QString         qhdr_;
            std::ofstream   evtstrm_;
            LayoutState*    layout_state_;
            SocketClient*   socket_client_;
            DebugCntl*      dbg_;
            LayoutConfig*   layout_config_;
    };
}
#endif

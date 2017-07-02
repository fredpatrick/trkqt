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

#ifndef TRK_SWITCH_HH
#define TRK_SWITCH_HH

#include <QObject>
#include <string>
#include "trkutl.h"

namespace trk
{
    class CmdPacket;
    class DebugCntl;
    class LayoutConfig;
    class SocketClient;
    class SwitchEvent;

    class Switch : public QObject
    {
        Q_OBJECT

        public:
            Switch(const std::string& switch_name,
                   SocketClient*      socket_client);
            ~Switch() {}

            std::string     switch_name()  { return switch_name_; }
            int             switch_index() { return switch_index_; }
            SW_DIRECTION    switch_state() { return switch_state_; }
            bool            available()    { return available_; }
            bool            queued()       { return queued_; }
            void            switch_state(SW_DIRECTION swd) { switch_state_ = swd; }
            void            set_queued(SW_DIRECTION);

        public slots:
            void            reset();
            void            set_available(bool available);
            void            auto_event_coming(SW_DIRECTION event_state);
            void            triggered_event_coming(SW_DIRECTION target_state);

            void            switch_event(SwitchEvent* event);
            void            cmd_response(CmdPacket* cmd_packet);
            void            scan_switch();
            void            throw_switch(SW_DIRECTION target_state);

        signals:
            void            switch_thrown(std::string);
            void            cmd_response_received(int, int);

        private:
            std::string     switch_name_;
            int             switch_index_;

            bool            available_;
            bool            queued_;
            bool            auto_event_;
            bool            switch_cmd_sent_;
            bool            expecting_response_;
            SW_DIRECTION    switch_state_;
            SW_DIRECTION    target_state_;
            SW_DIRECTION    thrown_state_;
            SocketClient*   socket_client_;
            LayoutConfig*   layout_config_;
            int             event_count_;
            
            std::vector<std::pair<int, int> >   event_items_;
            DebugCntl*      dbg_;
    };
}
#endif

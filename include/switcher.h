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

#ifndef TRK_SWITCHER_HH
#define TRK_SWITCHER_HH

#include "trkutl.h"
#include <QObject>

#include <sstream>
#include <vector>

namespace trk
{
    class CmdPacket;
    class DebugCntl;
    class JobClock;
    class LayoutConfig;
    class SocketClient;
    class SwitchEvent;

    class Switcher : public QObject
    {
        Q_OBJECT

        public:
            Switcher(SocketClient* socket_client, SW_DIRECTION target_state);
            ~Switcher();

            std::vector<SW_DIRECTION> switch_states(){ return switch_states_; }

        signals:
            void                      switcher_complete(bool status);
        private slots:
            void                      scan_response(CmdPacket*);
            void                      switch_event(SwitchEvent*);

        private:
            void                      next_switch(int switch_index);
            void                      complete(bool sw_thrown);
            LayoutConfig*             layout_config_;
            DebugCntl*                dbg_;
            JobClock*                 job_clock_;
            SocketClient*             socket_client_;
            SW_DIRECTION              target_state_;
            std::vector<SW_DIRECTION> switch_states_;
            int                       switch_count_;
            int                       current_switch_;
            int                       scan_seqno_;
            double                    tm_begin_;
            double                    tm_scan_;
            double                    tm_local_;
            double                    tm_event_;
            double                    dt_event_;
            std::ostringstream        ss_;
    };
}
#endif

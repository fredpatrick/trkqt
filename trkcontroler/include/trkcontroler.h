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

#ifndef TRK_TRKCONTROLER_HH
#define TRK_TRKCONTROLER_HH

#include <QDialog>

class QGroupBox;
class QButtonGroup;
class QDialogButtonBox;
class QRadioButton;
class QResizeEvent;

namespace trk {

    class SocketClient;
    class SwitchClient;
    class BlockClient;
    class EventLog;
    class CmdPacket;
    class IniPacket;
    class LayoutState;
    class TrkViewer;
    class DebugCntl;

    class TrkControler : public QDialog
    {
        Q_OBJECT

        public:
            explicit TrkControler(QWidget *parent = 0);
            ~TrkControler();

            void              resizeEvent(QResizeEvent* event);
        public slots:
            void              connect_to_BBB();
            void              disconnect_from_BBB();
            void              socket_connected();
            void              ini_packet_received(IniPacket* ip);
        private slots:
            void              eventlog();
            void              enable_layoutstate_buttons();
        private:
            QButtonGroup*     bg_[6];
            QRadioButton*     rb_thru_[6];
            QRadioButton*     rb_out_[6];
            QDialogButtonBox* dialog_bb_;
            SocketClient*     socket_client_;
            SwitchClient*     switch_client_;
            BlockClient*      block_client_;
            EventLog*         eventlog_;
            TrkViewer*        trkviewer_;
            LayoutState*      layoutstate_;
            DebugCntl*        dbg_;

            QPushButton*      eventlog_button_;
            QPushButton*      layoutview_button_;
            QPushButton*      nextpath_button_;
            QPushButton*      autorun_button_;
            QPushButton*      connect_button_;
            QPushButton*      disconnect_button_;
            QPushButton*      quit_button_;
    };

}

#endif // MAINWINDOW_H
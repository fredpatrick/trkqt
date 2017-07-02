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

#ifndef TRK_LAYOUTSTATE_HH
#define TRK_LAYOUTSTATE_HH

#include <map>
#include <string>
#include <vector>
#include "path.h"
#include "trkutl.h"
#include "zonegeometry.h"
#include <QObject>

namespace trk
{
    class CmdPacket;
    class DebugCntl;
    class ZoneNode;
    class SwitchNode;
    class Paths;
    class Switch;
    class Switcher;
    class LayoutConfig;
    class LayoutGeometry;
    class ZoneGeometry;
    class BlockEvent;
    class SwitchEvent;
    class TrackEvent;
    class SocketClient;
    
    class LayoutState : public QObject
    {
        Q_OBJECT

        public:
            LayoutState(SocketClient* socket_client);
            ~LayoutState();

            void                   initialize();
            std::vector<TRK_STATE> track_states()     { return track_states_; }
            Path*                  current_path()     { return current_path_; }
            Path*                  exit_path()        { return exit_path_; }
            int                    exit_switch_index()
                                                { return current_path_->exit_switch_index(); }
            std::string            current_path_name(){ return current_path_->path_name(); }
            SW_DIRECTION           switch_state(int i);
            std::vector<int>       active_zone_indexes();
            bool                   in_transition()    { return in_transition_; }

            bool                   on_path       (Path* path, int switch_index);
            bool                   entry_tag_is_A(Path* path, int switch_index);

        private slots:
            void                next_path();
            void                new_track_event(TrackEvent* event);
            void                new_block_event(BlockEvent* event);
            void                new_switch_event(SwitchEvent* event);
            void                track_cmd_response(CmdPacket* cmdr);
            void                switcher_complete(bool);


        signals:
            void                layoutstate_changed(TrackEvent*, 
                                                    const std::vector<bool>&, 
                                                    bool);
            void                layoutstate_initialized();
            void                zone_busy(int);
            void                zone_idle(int);
            void                new_path();
            void                switch_queued(int, bool);
            void                switch_thrown(int, bool);

        private:
            void                track_scan();
            Switcher*           switcher_;
            void                finish_initialization();
            std::string         print_state(const std::string& msg);
            void                startup(const std::string& zonename);
            void                zone_is_busy(const std::string& zonename,
                                             int                zone_index);
            void                zone_is_idle(const std::string& zonename,
                                             int                zone_index);
            void                set_switches(int zone_index);
            void                leaving_zone();

            bool                initialized_;
            bool                startup_;
            bool                in_transition_;
            bool                track_scan_complete_;
            bool                switch_scan_complete_;
            bool                layout_initialized_;
            bool                spurious_event_;
            bool                exit_path_expected_;
            
            double              tm_event_;
            int                 active_zone_count_;
            std::string         previous_path_name_;
            std::string         next_path_zonename_;
            std::string         next_exit_zonename_;
            Path*               previous_path_;
            Path*               current_path_;
            Path*               exit_path_;
            std::string         startup_zonename_;
            std::string         current_zonename_;
            ZoneNode*           current_zonenode_;
            std::string         previous_zonename_;
            ZoneNode*           previous_zonenode_;

            Paths*                               paths_;
            int                                  switch_seqno_;
            int                                  track_seqno_;
            int                                  track_event_count_;
            int                                  track_sensor_count_;
            int                                  switch_count_;
            LayoutConfig*                        layout_config_;
            LayoutGeometry*                      layout_geometry_;
            SocketClient*                        socket_client_;
            DebugCntl*                           dbg_;
            void                                 address_dump();
            std::map<std::string, ZoneGeometry*> zone_geometries_;
            std::vector<std::string>             zonenames_;
            std::vector<TRK_STATE>               track_states_;
            std::vector<std::string>             track_sensor_names_;
            std::vector<SW_DIRECTION>            switch_states_;
            std::vector<BLK_STATE>               block_states_;
            Switch*                              switch_bfr_[16];
            std::vector<Switch*>                 switches_;
            std::vector<std::string>             switch_names_;
            std::vector<bool>                    active_zones_;
    };
                        
}
#endif

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

 #ifndef TRK_LAYOUTGEOMETRY_HH
 #define TRK_LAYOUTGEOMETRY_HH

#include "zonegeometry.h"
#include "switchgeometry.h"
#include "filestore.h"
#include "ranges.h"

#include <string>
#include <fstream>
#include <sstream>

namespace trk 
{
    class LayoutConfig;
    class DebugCntl;

    class LayoutGeometry
    {
        public:
            static LayoutGeometry* instance();

            ~LayoutGeometry();

            void        read_vtx_data();
            std::vector<std::string> zonenames();
            ZoneGeometry*            zone_geometry( const std::string& zonename)
                                         { return zone_geometries_[zonename]; }
            ZoneGeometry*            zone_geometry(int zone_index)
                                         { return zone_geometries_by_index_[zone_index]; }
            std::vector<std::string> switch_names();
            SwitchGeometry*          switch_geometry( const std::string& switch_name)
                                         { return switch_geometries_[switch_name]; }
            SwitchGeometry*          switch_geometry( int switch_index)
                                         {return switch_geometries_by_index_[switch_index]; }
            Ranges&                  ranges()       { return ranges_; }
            double                   bedw()        { return bed_w_; }
            double                   bedh()        { return bed_h_; }
            double                   bedtw()       { return bed_tw_; }
            int                      zone_count()   { return zone_count_; }
            int                      switch_count() { return switch_count_; }
            int                      shell_count()  { return shell_count_; }
        protected:
            LayoutGeometry();
        private:
            std::map<std::string, ZoneGeometry*>     zone_geometries_;
            std::map<std::string, SwitchGeometry*>   switch_geometries_;
            std::vector<ZoneGeometry*>               zone_geometries_by_index_;
            std::vector<SwitchGeometry*>             switch_geometries_by_index_;
            Ranges                                   ranges_;
            double                           bed_w_;
            double                           bed_h_;
            double                           bed_tw_;
            int                              zone_count_;
            int                              switch_count_;
            int                              shell_count_;
            LayoutConfig*                    layout_config_;
            DebugCntl*                       dbg_;

            static LayoutGeometry*                  instance_;
    };
}
#endif

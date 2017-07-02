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

#ifndef TRK_SWITCHGEOMETRY_HH
#define TRK_SWITCHGEOMETRY_HH

#include "shellgeometry.h"
#include "point.h"
#include "ranges.h"
#include <string>

#include <vector>


namespace trk 
{
    enum SW_SHELL_TYPE { THRUB=2, THRUC, OUTB, OUTC };
    class DebugCntl;
    class BedlineGeometry;
    class LayoutGeometry;

    class SwitchGeometry
    {
        public:
            SwitchGeometry( LayoutGeometry* layout_geometry, 
                            std::ifstream&  vtxstrm,  
                            Ranges&         r);
            ~SwitchGeometry() {}

            std::string  switch_name()   { return switch_name_; }
            std::string  zonename_A()    { return zonename_A_; }
            std::string  zonename_B()    { return zonename_B_; }
            std::string  zonename_C()    { return zonename_C_; }
            int          shell_count()   { return shell_count_; }
            int          bedline_count() { return bedline_count_; }
            int          shell_index(const std::string& shell_type);

            ShellGeometry*       shell_geometry(int i)   { return shell_geometries_[i]; }
            BedlineGeometry*     bedline_geometry(int i) { return bedline_geometries_[i]; }
        private:
            std::string print_xform();
            std::string print_origin();
            std::string switch_name_;
            std::string direction_;
            void        flipx(std::vector<Point>& slice);
            void        adjust_line(int line_index, std::vector<Point>& line);
            float       slope_;
            float       arc_radius_;
            float       arc_degrees_;
            float       arc_origin_[3];
            int         arc_count_;
            float       ab_length_;
            float       ac_length_;
            float       xf_[16];
            std::string zonename_A_;
            std::string zonename_B_;
            std::string zonename_C_;
            int         bedline_count_;
            int         shell_count_;
            int         thru_shell_index_;
            int         out_shell_index_;

            float           bedw_;
            float           bedh_;
            float           bedtw_;
            float           r0_;
            float           r1_;
            float           r2_;
            float           r3_;
            float           thetaa_;
            float           thetab_;
            float           thetac_;
            float           thetad_;
            float           thetat_;
            float           dtheta_;
            Point           delta_[8];
            ShellGeometry*  build_thrub(Ranges& r);
            ShellGeometry*  build_thruc(Ranges& r);
            ShellGeometry*  build_outb(Ranges& r);
            ShellGeometry*  build_outc(Ranges& r);
            void            build_thru_bedlines();
            void            build_out_bedlines();

            std::vector<ShellGeometry*>   shell_geometries_;
            std::vector<BedlineGeometry*> bedline_geometries_;
            DebugCntl*                  dbg_;
    };
}

#endif

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

#include "switchgeometry.h"
#include "bedlinegeometry.h"
#include "debugcntl.h"
#include "layoutgeometry.h"
#include "point.h"
#include "trkutl.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


trk::SwitchGeometry::
SwitchGeometry( LayoutGeometry* layout_geometry, std::ifstream& vtxstrm, Ranges& r)
{
    dbg_ = DebugCntl::instance();

    while ( true ) {
        std::string tag;
        std::string pname;
        vtxstrm >> tag >> pname;
        if (tag == "switch" && pname != "end" ) {
            if      ( pname == "switch_name"    ) vtxstrm >> switch_name_;
            else if ( pname == "direction"      ) vtxstrm >> direction_;
            else if ( pname == "slope"          ) vtxstrm >> slope_;
            else if ( pname == "arc_radius"     ) vtxstrm >> arc_radius_;
            else if ( pname == "arc_degrees"    ) vtxstrm >> arc_degrees_;
            else if ( pname == "arc_origin"     ) vtxstrm >> arc_origin_[0] >> arc_origin_[1] >>
                                                        arc_origin_[2];
            else if ( pname == "arc_count"      ) vtxstrm >> arc_count_;
            else if ( pname == "ab_length"      ) vtxstrm >> ab_length_;
            else if ( pname == "ac_length"      ) vtxstrm >> ac_length_;
            else if ( pname == "xform"          ) {
                for ( int i = 0; i < 16; i++) vtxstrm >> xf_[i];
            }
            else if ( pname == "zone_name_A"    ) vtxstrm >> zonename_A_;
            else if ( pname == "zone_name_B"    ) vtxstrm >> zonename_B_;
            else if ( pname == "zone_name_C"    ) vtxstrm >> zonename_C_;
            else if ( pname == "shell_count"    ) vtxstrm >> shell_count_;
            else {
                std::cout << "SwitchGeometry.ctor, unknown parameter name = " << 
                                   pname << std::endl;
            }
        } else {
            break;
        }
    }
    if ( dbg_->check(5) ) {
        *dbg_ << "SwitchGeometry.ctor, switch_name   = " << switch_name_ << trk::endl;
        *dbg_ << "SwitchGeometry.ctor, direction     = " << direction_ << trk::endl;
        *dbg_ << "SwitchGeometry.ctor, slope         = " << slope_ << trk::endl;
        *dbg_ << "SwitchGeometry.ctor, arc_radius    = " << arc_radius_ << trk::endl;
        *dbg_ << "SwitchGeometry.ctor, arc_degrees   = " << arc_degrees_ << trk::endl;
        *dbg_ << print_origin();
        *dbg_ << "SwitchGeometry.ctor, arc_count     = " << arc_count_ << trk::endl;
        *dbg_ << "SwitchGeometry.ctor, ab_length     = " << ab_length_ << trk::endl;
        *dbg_ << "SwitchGeometry.ctor, ac_length     = " << ac_length_ << trk::endl;
        *dbg_ << print_xform();
        *dbg_ << "SwitchGeometry.ctor, zonename_A    = " << zonename_A_  << trk::endl;
        *dbg_ << "SwitchGeometry.ctor, zonename_B    = " << zonename_B_  << trk::endl;
        *dbg_ << "SwitchGeometry.ctor, zonename_C    = " << zonename_C_  << trk::endl;
        *dbg_ << "SwitchGeometry.ctor, shell_count   = " << shell_count_ << trk::endl;
    }
    shell_geometries_.resize(shell_count_);
    for ( int n = 0; n < shell_count_; n++) {
            ShellGeometry* sg = new ShellGeometry(vtxstrm, r);
            if      ( sg->type() == "thru" ) shell_geometries_[THRU] = sg;
            else if ( sg->type() == "out")   shell_geometries_[OUT]  = sg;
    }
    shell_count_ = 6;

    float pi = 3.14159265;
    bedw_       = layout_geometry->bedw();
    bedh_       = layout_geometry->bedh();
    bedtw_      = layout_geometry->bedtw();
    r0_         = arc_radius_ - 0.5 * bedw_;
    r1_         = arc_radius_ + 0.5 * bedw_;
    r2_         = arc_radius_ + 0.5 * bedtw_;
    r3_         = arc_radius_ - 0.5 * bedtw_;
    thetaa_     = acos( r2_ / r1_ );
    thetab_     = acos( r0_ / r3_ );
    thetac_     = acos( r0_ / r1_ );
    thetad_     = acos( r3_ / r2_ );
    thetat_     = arc_degrees_ * pi / 180.0;
    dtheta_     = arc_degrees_ * pi / 180.0 / arc_count_;
    shell_geometries_.resize(6);
    shell_geometries_[THRUB] = build_thrub(r);
    shell_geometries_[THRUC] = build_thruc(r);
    shell_geometries_[OUTB]  = build_outb (r);
    shell_geometries_[OUTC]  = build_outc (r);
    delta_[0] = Point( -0.010,  0.000);
    delta_[1] = Point( -0.007,  0.007);
    delta_[2] = Point(  0.000,  0.010);
    delta_[3] = Point(  0.000,  0.010);
    delta_[4] = Point(  0.000,  0.010);
    delta_[5] = Point(  0.000,  0.010);
    delta_[6] = Point(  0.007,  0.007);
    delta_[7] = Point(  0.010,  0.010);

    build_thru_bedlines();
    build_out_bedlines();
    bedline_count_ = bedline_geometries_.size();
    return;
}

std::string
trk::SwitchGeometry::
print_xform()
{
    std::ostringstream ss;
    ss.setf(std::ios_base::fixed, std::ios_base::floatfield);
    ss.precision(6);
    for ( int j = 0; j < 16; j += 4 )  {
        if ( j == 0 )  ss << "SwitchGeometry.ctor, xform         = ";
        else           ss << std::setw(37) << " ";
        for ( int i = 0; i < 4; i++ ) ss << std::setw(10) << xf_[j + i ];
        ss << std::endl;
    }    
    return ss.str();
}

std::string
trk::SwitchGeometry::
print_origin()
{
    std::ostringstream ss;
    ss.setf(std::ios_base::fixed, std::ios_base::floatfield);
    ss.precision(6);
    ss << "SwitchGeometry.ctor, arc_origin    = ";
    ss << std::setw(10) << arc_origin_[0] << std::setw(10) << arc_origin_[1] << 
                                             std::setw(10) << arc_origin_[2] << std::endl;
    return ss.str();
}

trk::ShellGeometry*
trk::SwitchGeometry::
build_thrub(Ranges& r)
{
    ShellGeometry* thrub = new ShellGeometry("thrub", xf_, r);

    std::vector<Point> slice;
    slice.push_back( Point( 0.5*bedw_,  0.0,        0.0) );
    slice.push_back( Point( 0.0,        0.0,        0.0) );
    slice.push_back( Point(-0.5*bedw_,  0.0,        0.0) );
    slice.push_back( Point(-0.5*bedtw_, 0.0,        bedh_) );
    slice.push_back( Point( 0.5*bedtw_, 0.0,        bedh_) );
    if ( direction_ == "Left") flipx(slice);
    thrub->add_slice(0, slice);
    slice.clear();
    slice.push_back( Point( 0.5*bedw_,  ab_length_, 0.0) );
    slice.push_back( Point( 0.0,        ab_length_, 0.0) );
    slice.push_back( Point(-0.5*bedw_,  ab_length_, 0.0) );
    slice.push_back( Point(-0.5*bedtw_, ab_length_, bedh_) );
    slice.push_back( Point( 0.5*bedtw_, ab_length_, bedh_) );
    if ( direction_ == "Left") flipx(slice);
    thrub->add_slice(1, slice, true);
    return thrub;
}

trk::ShellGeometry*
trk::SwitchGeometry::
build_thruc(Ranges& r)
{
    ShellGeometry* thruc = new ShellGeometry("thruc", xf_, r);

    float           theta;
    bool            last_slice = false;
    int             i          = 0;
    while ( i < arc_count_ + 1) {
        theta = i * dtheta_;
        std::vector<Point> slice;
        if ( theta < thetab_ ) {
            slice.push_back(Point(arc_radius_ - r0_*cos(theta), r0_*sin(theta), 0.0));
            slice.push_back(Point(0.5*bedw_                   , r0_*tan(theta), 0.0));
            slice.push_back(Point(0.5*bedw_                   , r0_*tan(theta), 0.0));
            slice.push_back(Point(0.5*bedw_                   , r0_*tan(theta), bedh_));
            slice.push_back(Point(0.5*bedw_                   , r0_*tan(theta), bedh_));
        } else if ( (theta >= thetab_) && (theta < thetac_)) {
            slice.push_back(Point(arc_radius_ - r0_*cos(theta), r0_*sin(theta), 0.0));
            slice.push_back(Point(0.5*bedw_,                    r0_*tan(theta), 0.0));
            slice.push_back(Point(0.5*bedw_,                    r0_*tan(theta), 0.0));
            slice.push_back(Point(0.5*bedw_,                    r0_*tan(theta), bedh_));
            slice.push_back(Point(arc_radius_ - r3_*cos(theta), r3_*sin(theta), bedh_));
        } else if ( theta >= thetac_ ) {
            slice.push_back(Point(arc_radius_ - r0_*cos(theta), r0_*sin(theta), 0.0));
            slice.push_back(Point(arc_radius_ - r1_*cos(theta), r1_*sin(theta), 0.0));
            slice.push_back(Point(arc_radius_ - r1_*cos(theta), r1_*sin(theta), 0.0));
            slice.push_back(Point(arc_radius_ - r1_*cos(theta), r1_*sin(theta), bedh_));
            slice.push_back(Point(arc_radius_ - r3_*cos(theta), r3_*sin(theta), bedh_));
        }
        if ( direction_ == "Left") flipx(slice);
        if ( i == arc_count_ ) last_slice = true;
        thruc->add_slice(i, slice,last_slice);
        i += 1;
    }
    return thruc;
}

trk::ShellGeometry*
trk::SwitchGeometry::
build_outb(Ranges& r)
{
    ShellGeometry* outb = new ShellGeometry("outb", xf_, r);

    float           theta;
    bool            last_slice = false;
    int             i          = 0;
    while ( i < arc_count_ + 1) {
        theta = i * dtheta_;
        std::vector<Point> slice;
        if ( theta < thetaa_ ) {
            slice.push_back(Point(arc_radius_ - r1_*cos(theta), r1_*sin(theta), 0.0));
            slice.push_back( Point( 0.0,                        r1_*sin(theta), 0.0));
            slice.push_back(Point(-0.5*bedw_,                   r1_*sin(theta), 0.0));
            slice.push_back(Point(arc_radius_ - r1_*cos(theta), r1_*sin(theta), bedh_));
            slice.push_back(Point(arc_radius_ - r1_*cos(theta), r1_*sin(theta), bedh_));
        } else if ( (theta >= thetaa_) && ( theta < thetac_) ) {
            slice.push_back(Point(arc_radius_ - r1_*cos(theta), r1_*sin(theta), 0.0));
            slice.push_back( Point( 0.0,                        r1_*sin(theta), 0.0));
            slice.push_back(Point(-0.5*bedw_,                   r1_*sin(theta), 0.0));
            slice.push_back(Point(-0.5*bedtw_,                  r1_*sin(theta), 0.4375));
            slice.push_back(Point(arc_radius_ - r1_*cos(theta), r1_*sin(theta), 0.4375));
        } else if ( (theta >= thetac_ ) ) {
            slice.push_back(Point( 0.5*bedw_,  r1_*sin(thetac_), 0.0));
            slice.push_back( Point( 0.0,       r1_*sin(thetac_), 0.0) );
            slice.push_back(Point(-0.5*bedw_,  r1_*sin(thetac_), 0.0));
            slice.push_back(Point(-0.5*bedtw_, r1_*sin(thetac_), 0.4375));
            slice.push_back(Point( 0.5*bedw_,  r1_*sin(thetac_), 0.4375));
            if ( direction_ == "Left") flipx(slice);
            outb->add_slice(0, slice, last_slice);
            slice.clear();
            slice.push_back( Point( 0.5*bedw_,  ab_length_, 0.0) );
            slice.push_back( Point( 0.0,        ab_length_, 0.0) );
            slice.push_back( Point(-0.5*bedw_,  ab_length_, 0.0) );
            slice.push_back( Point(-0.5*bedtw_, ab_length_, bedh_) );
            slice.push_back( Point( 0.5*bedtw_, ab_length_, bedh_) );
            if ( direction_ == "Left") flipx(slice);
            outb->add_slice(i+1, slice, true);
            break;
        }
        if ( direction_ == "Left") flipx(slice);
        outb->add_slice(i, slice, true);
        i += 1;
    }
    return outb;
}

trk::ShellGeometry*
trk::SwitchGeometry::
build_outc(Ranges& r)
{
    ShellGeometry* outc = new ShellGeometry("outc", xf_, r);

    float           theta;
    bool            last_slice = false;
    int             i          = 0;
    float           rc         = arc_radius_;
    while ( i < arc_count_ + 1) {
        theta = i * dtheta_;
        std::vector<Point> slice;
        slice.push_back(Point(arc_radius_ - r0_*cos(theta), r0_*sin(theta), 0.0));
        slice.push_back(Point(arc_radius_ - rc*cos(theta),  rc*sin(theta),  0.0));
        slice.push_back(Point(arc_radius_ - r1_*cos(theta), r1_*sin(theta), 0.0));
        slice.push_back(Point(arc_radius_ - r1_*cos(theta), r1_*sin(theta), bedh_));
        slice.push_back(Point(arc_radius_ - r3_*cos(theta), r3_*sin(theta), bedh_));
        if ( direction_ == "Left") flipx(slice);
        if ( i == arc_count_) last_slice = true;
        outc->add_slice(i, slice, last_slice);
        i += 1;
    }
    return outc;
}

void
trk::SwitchGeometry::
flipx(std::vector<Point>& slice)
{
    int np = slice.size();
    for ( int i = 0; i < np; i++) {
        slice[i].x( -slice[i].x() );
    }
}

void
trk::SwitchGeometry::
build_thru_bedlines()
{
    int line_index = 0;
    std::vector<Point> line;
    line.push_back(Point(-0.5*bedw_,  0.0,        0.0));
    line.push_back(Point(-0.5*bedw_,  ab_length_, 0.0));
    adjust_line(line_index, line);
    line_index += 1;
    bedline_geometries_.push_back(new BedlineGeometry(line_index, line, xf_));
    line.clear();
    line.push_back(Point(-0.5*bedtw_, 0.0,        0.4375));
    line.push_back(Point(-0.5*bedtw_, ab_length_, 0.4375));
    adjust_line(line_index, line);
    line_index += 1;
    bedline_geometries_.push_back(new BedlineGeometry(line_index, line, xf_));
    line.clear();
    line.push_back(Point( 0.5*bedtw_, r2_*sin(thetad_), 0.4375));
    line.push_back(Point( 0.5*bedtw_, ab_length_,       0.4375));
    adjust_line(line_index, line);
    line_index += 1;
    bedline_geometries_.push_back(new BedlineGeometry(line_index, line, xf_));
    line.clear();
    line.push_back(Point( 0.5*bedw_,  r1_*sin(thetac_), 0.4375));
    line.push_back(Point( 0.5*bedw_,  ab_length_,       0.4375));
    adjust_line(line_index, line);
    line_index += 1;
    bedline_geometries_.push_back(new BedlineGeometry(line_index, line, xf_));
    return;
}

void
trk::SwitchGeometry::
build_out_bedlines()
{
    int line_index = 4;
    std::vector<Point> line;
    std::vector<Point> center_line;
    float theta = thetac_;
    line.clear();
    while ( theta < thetat_) {
        line.push_back(Point(arc_radius_ - r1_*cos(theta), r1_*sin(theta), 0.0));
        theta += dtheta_;
    }
    line.push_back(Point(arc_radius_ - r1_*cos(thetat_), r1_*sin(thetat_), 0.0));
    adjust_line(line_index, line);
    line_index += 1;
    bedline_geometries_.push_back(new BedlineGeometry(line_index, line, xf_));

    theta = thetad_;
    line.clear();
    while ( theta < thetat_) {
        line.push_back(Point(arc_radius_ - r2_*cos(theta), r2_*sin(theta), 0.4375));
        theta += dtheta_;
    }
    line.push_back(Point(arc_radius_ - r2_*cos(thetat_), r2_*sin(thetat_), 0.4375));
    adjust_line(line_index, line);
    line_index += 1;
    bedline_geometries_.push_back(new BedlineGeometry(line_index, line, xf_));

    line.clear();
    for ( int i = 0; i < arc_count_+1; i++) {
        theta = i * dtheta_;
        line.push_back(Point(arc_radius_ - r3_*cos(theta), r3_*sin(theta), 0.4375));
    }
    adjust_line(line_index, line);
    line_index += 1;
    bedline_geometries_.push_back(new BedlineGeometry(line_index, line, xf_));

    line.clear();
    for ( int i = 0; i < arc_count_+1; i++) {
        theta = i * dtheta_;
        line.push_back(Point(arc_radius_ - r0_*cos(theta), r0_*sin(theta), 0.0));
    }
    adjust_line(line_index, line);
    line_index += 1;
    bedline_geometries_.push_back(new BedlineGeometry(line_index, line, xf_));
}

void
trk::SwitchGeometry::
adjust_line(int line_index, std::vector<Point>& line)
{
    int np = line.size();
    for ( int i = 0; i < np; i++) {
        line[i] =  line[i] + delta_[line_index];
        if ( direction_ == "Left" ) line[i].x( -line[i].x() );
    }
}


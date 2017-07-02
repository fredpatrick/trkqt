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

#include "layoutgeometry.h"
#include "layoutconfig.h"
#include "filestore.h"
#include "debugcntl.h"

trk::LayoutGeometry* trk::LayoutGeometry::instance_;

trk::LayoutGeometry*
trk::LayoutGeometry::
instance()
{
    if ( !instance_ ) {
        instance_ = new LayoutGeometry();
    }
    return instance_;
}

trk::LayoutGeometry::
LayoutGeometry()
{
    dbg_ = DebugCntl::instance(); 
    layout_config_ = LayoutConfig::instance();
    FileStore* fs = FileStore::instance();
    std::ifstream vtxstrm(fs->vtxfil().c_str() );
    ranges_.init();
    int zone_count = layout_config_->track_sensor_count();
    zone_geometries_by_index_.resize(zone_count);
    int switch_count = layout_config_->switch_count();
    switch_geometries_by_index_.resize(switch_count);
    while ( true ) {
        std::string tag;
        std::string pname;
        vtxstrm >> tag >> pname;
        if (tag == "layout" && pname != "end" ) {
            if      ( pname == "bed_w"       ) vtxstrm >> bed_w_;
            else if ( pname == "bed_h"       ) vtxstrm >> bed_h_;
            else if ( pname == "bed_tw"      ) vtxstrm >> bed_tw_;
            else if ( pname == "zone_count"  ) vtxstrm >> zone_count_;
            else if ( pname == "switch_count") vtxstrm >> switch_count_;
            else {
                std::cout << "LayoutGeometry.ctor, unknown parameter name = " << 
                                     pname << std::endl;
            }
        } else {
            break;
        }
    }
    shell_count_ = 6;
    std::cout << "LayoutGeometry.ctor, zone_count     = " << zone_count_ << std::endl;
    std::cout << "LayoutGeometry.ctor, switch_count   = " << switch_count_ << std::endl;
    std::cout << "LayoutGeometry.ctor, shell_count    = " << shell_count_ << std::endl;
    for ( int i = 0; i < zone_count_; i++ ) {
        ZoneGeometry* zg = new ZoneGeometry(vtxstrm, ranges_);
        zone_geometries_[zg->zonename() ] = zg;
        int zone_index = layout_config_->track_sensor_index(zg->zonename());
        zone_geometries_by_index_[zone_index] = zg;
    } 
    for ( int i = 0; i < switch_count_; i++) {
        SwitchGeometry* sg = new SwitchGeometry(this, vtxstrm, ranges_);
        switch_geometries_[sg->switch_name() ] = sg;
        int switch_index = layout_config_->switch_sensor_index(sg->switch_name());
        switch_geometries_by_index_[switch_index] = sg;
    }
    if ( dbg_->check(4) ) {
        *dbg_ << "LayoutGeometry.ctor, geometry data loaded" << trk::endl;
        *dbg_ << ranges_.print("LayoutGeometry");
    }
}

trk::LayoutGeometry::
~LayoutGeometry()
{
    std::cout << "LayoutGeometry.dtor" << std::endl;
}

std::vector<std::string>
trk::LayoutGeometry::
zonenames()
{
    std::vector<std::string> znms;
    znms.resize(zone_geometries_.size() );
    typedef std::map<std::string, ZoneGeometry*>::iterator CI;
    for ( CI p = zone_geometries_.begin(); p != zone_geometries_.end(); p++ ) {
        std::string zn = p->second->zonename();
        int         zi = layout_config_->track_sensor_index(zn);
        znms[zi]        = p->first;
    }
    return znms;
}

std::vector<std::string>
trk::LayoutGeometry::
switch_names()
{
    std::vector<std::string> swnms;
    swnms.resize(switch_geometries_.size() );
    typedef std::map<std::string, SwitchGeometry*>::iterator CI;
    for ( CI p = switch_geometries_.begin(); p != switch_geometries_.end(); p++ ) {
        std::string swn = p->second->switch_name();
        int         swi = layout_config_->switch_sensor_index(swn);
        swnms[swi] = p->first;
    }
    return swnms;
}

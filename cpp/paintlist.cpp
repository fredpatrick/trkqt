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

#include "paintlist.h"
#include "debugcntl.h"

#include "iostream"

trk::PaintList::
PaintList(int zone_count, int switch_count, int shell_count)
{
    dbg_ = DebugCntl::instance();
    zone_count_   = zone_count;
    switch_count_ = switch_count;
    shell_count_  = shell_count;

    paint_list_.resize(zone_count + switch_count * shell_count_);
}

void 
trk::PaintList::
item_by_zone(int zone_index, const PaintItem& pl)
{
    paint_list_[zone_index] = pl;
}

void 
trk::PaintList::
color_by_zone(int zone_index, const QColor& color)
{
    if ( dbg_->check(3) ) {
        *dbg_ << "PaintList.color_by_zone, zone _index = " << zone_index << ", color = " << 
                 color.red() << ", " << color.green() << ", " << color.blue() << trk::endl;
    }
    paint_list_[zone_index].vtx_color = color;
}

trk::PaintItem&
trk::PaintList::
item_by_zone(int zone_index)
{
    return paint_list_[zone_index];
}

void 
trk::PaintList::
item_by_switch(int switch_index, int shell_index, const PaintItem& ps)
{
    int i = zone_count_ + switch_index * shell_count_ + shell_index;
    paint_list_[i] = ps;
}

void
trk::PaintList::
color_by_switch(int switch_index, int shell_index, const QColor& color)
{
    if ( dbg_->check(3) ) {
        *dbg_ << "PaintList.color_by_switch, switch_index = " << switch_index <<
                    ", shell_index = " << shell_index << ", color = " <<
                 color.red() << ", " << color.green() << ", " << color.blue() << trk::endl;
    }
    int i = zone_count_ + switch_index * shell_count_ + shell_index;
    paint_list_[i].vtx_color = color;
}

trk::PaintItem&
trk::PaintList::
item_by_switch(int switch_index, int shell_index)
{
    int i = zone_count_ + switch_index * shell_count_ + shell_index;
    return paint_list_[i];
}

void
trk::PaintList::
set_default_color(const QColor& color)
{
    int n = paint_list_.size();
    for ( int i = 0; i < n; i++) {
        paint_list_[i].vtx_color = color;
    }
}

trk::PaintItem&
trk::PaintList::
operator[](int n)
{
    return paint_list_[n];
}

int
trk::PaintList::
size()
{   
    return paint_list_.size();
}

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

#include "switchbox.h"
#include "switchclient.h"
#include "debugcntl.h"
#include "layoutconfig.h"

#include <QButtonGroup>
#include <QRadioButton>
#include <QLabel>
#include <QHBoxLayout>

trk::SwitchBox::
SwitchBox(SwitchClient* parent, const std::string& switch_name) : QGroupBox(parent)
{
    dbg_ = DebugCntl::instance();
    setFlat(true);
    setContentsMargins(0, 0, 0, 0);
    QHBoxLayout* layout = new QHBoxLayout;
    QLabel* sw_label    = new QLabel(tr(switch_name.c_str()) );
    rb_thru_            = new QRadioButton(tr("THRU"));
    rb_out_             = new QRadioButton(tr("OUT"));
    label_              = new QLabel(tr("    "));
    layout->addWidget(sw_label);

    layout->addWidget(sw_label);
    layout->addWidget(rb_thru_);
    layout->addWidget(rb_out_);
    layout->addWidget(label_);
    setLayout(layout);
    button_group_ = new QButtonGroup;
    int sw_number = LayoutConfig::instance()->switch_sensor_index(switch_name);
    button_group_->addButton(rb_thru_, sw_number * 10 + 0);
    button_group_->addButton(rb_out_,  sw_number * 10 + 1);
    connect(button_group_,static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonPressed),
            parent,       &trk::SwitchClient::switch_changed);

    rb_thru_->setEnabled(false);
    rb_out_->setEnabled(false);
    state_ = VERIFIED;
    swd_   = NOVAL;
    sw_number_ = sw_number;
}

trk::SwitchBox::
~SwitchBox()
{
}

void
trk::SwitchBox::
next_event(SW_DIRECTION sw_state)
{
    if ( dbg_->check(4) ) {
        *dbg_ << "SwitchBox.next_event, sw_number = " << sw_number_ << 
                           ", state = " << state_  << trk::endl;
    }
/*
    if (state_ == VERIFIED ) {
        swd0_ = swd;
        val0_ = value;
        state_ = NOTVERIFIED;
        rb_thru_->setEnabled(false);
        rb_out_->setEnabled(false);
        label_->setText(tr("NOVAL"));
        return;
    }
    if ( dbg_->check(4) ) {
        *dbg_ << "SwitchBox.next_event, sw_number = " << sw_number_ << trk::endl;
        *dbg_ << "SwitchBox.next_event" <<
                             ", swd0 = " << swd0_ << ", val0 = " << val0_ << trk::endl;
        *dbg_ << "SwitchBox.next_event" <<
                             ", swd  = " << swd   << ", value = " << value << trk::endl;
    }
    if (swd0_ == swd || val0_ == value ) {
        std::cout << "SwitchBox.next_event, sw_number = " << sw_number_ <<
                                    ", inconsistent events" << std::endl;
        std::cout << "SwitchBox,next_event, THIS SHOULD NOT HAPPEN" << std::endl;
        label_->setText(tr("NOVAL"));
    } else if (swd0_ == THRU ) {
        if ( val0_ == true ) rb_thru_->setChecked(true);
        else                 rb_out_->setChecked(true);
    } else if (swd0_ == OUT  ) {
        if ( val0_ == true ) rb_out_->setChecked(true);
        else                 rb_thru_->setChecked(true);
    }
    swd_   = swd;
*/
    state_ = VERIFIED;
    if ( sw_state == THRU ) {
        rb_thru_->setChecked(true);
        rb_out_->setChecked(false);
    } else if ( sw_state == OUT) {
        rb_thru_->setChecked(false);
        rb_out_->setChecked(true);
    }
    rb_thru_->setEnabled(true);
    rb_out_->setEnabled(true);
    label_->setText("    ");
}

void
trk::SwitchBox::
set(SW_DIRECTION swd)
{
    if ( swd == THRU ) {
        rb_thru_->setChecked(true);
    } else if (swd == OUT ) {
        rb_out_->setChecked(true);
    }
    rb_thru_->setEnabled(true);
    rb_out_->setEnabled(true);
}

QButtonGroup*
trk::SwitchBox::
button_group()
{
    return button_group_;
}

trk::SW_DIRECTION
trk::SwitchBox::
sw_direc()
{
    return swd_;
}

std::ostream&
trk::operator<<( std::ostream& ostrm, trk::SwitchBox::STATE& s)
{
    if (      s == trk::SwitchBox::VERIFIED )    ostrm << "VERFIED";
    else if ( s == trk::SwitchBox::NOTVERIFIED ) ostrm << "NOTVERFIED";
    return ostrm;
}

        



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

#include "blockbox.h"
#include "blockclient.h"
#include "DebugCntl.h"

#include <QButtonGroup>
#include <QRadioButton>
#include <QLabel>
#include <QHBoxLayout>

trk::BlockBox::
BlockBox(BlockClient*       parent, 
         int                blk_number, 
         const std::string& blk_name) : QGroupBox(parent)
{
    dbg_        = DebugCntl::instance();
    blk_number_ = blk_number;
    blk_name_   = blk_name_;

    setContentsMargins(0, 0, 0, 0);
    QHBoxLayout* layout = new QHBoxLayout;
    QLabel* blk_label   = new QLabel(tr(blk_name.c_str() ) );
    rb_go_              = new QRadioButton(tr("GO"));
    rb_stop_            = new QRadioButton(tr("STOP"));
    layout->addWidget(blk_label);

    layout->addWidget(blk_label);
    layout->addWidget(rb_go_);
    layout->addWidget(rb_stop_);
    setLayout(layout);
    button_group_ = new QButtonGroup;
    button_group_->addButton(rb_go_, blk_number * 10 + 0);
    button_group_->addButton(rb_stop_,  blk_number * 10 + 1);
    connect(button_group_,static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonPressed),
            parent,       &trk::BlockClient::block_changed);

    rb_go_->setEnabled(false);
    rb_stop_->setEnabled(false);
}

trk::BlockBox::
~BlockBox()
{
}

void
trk::BlockBox::
next_event(BLK_STATE blk_state)
{
    if ( dbg_->check(1) ) {
        std::cout << "BlockBox.next_event, blk_number = " << blk_number_ << 
                           ", blk_name = " << blk_name_  << std::endl;
    }
    if      (blk_state == GO)   rb_go_->setChecked(true);
    else if (blk_state == STOP) rb_stop_->setChecked(true);
}

void
trk::BlockBox::
set(BLK_STATE blk_state)
{
    if ( blk_state == GO ) {
        rb_go_->setChecked(true);
    } else if (blk_state == STOP ) {
        rb_stop_->setChecked(true);
    }
    rb_go_->setEnabled(true);
    rb_stop_->setEnabled(true);
}

QButtonGroup*
trk::BlockBox::
button_group()
{
    return button_group_;
}

trk::BLK_STATE
trk::BlockBox::
blk_state()
{
    return blk_state_;
}

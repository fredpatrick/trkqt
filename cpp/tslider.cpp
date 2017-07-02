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

#include <QSlider>
#include <QGridLayout>
#include <QLabel>
#include <QTextEdit>
#include <QString>
#include <string>
#include <iostream>
#include "tslider.h"

trk::TSlider::
TSlider(QWidget*           parent,
        const std::string& name,
        int                minval,
        int                maxval,
        int value) : QWidget(parent)
{
    slider_ = new QSlider(Qt::Vertical, this);
    slider_->setRange(minval, maxval);
    slider_->setValue(value);

    QGridLayout* layout = new QGridLayout;
    QString      str;
    QLabel*      namelbl = new QLabel(name.c_str(), this);
    QLabel*      minvlbl = new QLabel(str.setNum(minval), this);
    QLabel*      maxvlbl = new QLabel(str.setNum(maxval), this);
                 valtxt_ = new QLabel(str.setNum(value), this);

    layout->addWidget(slider_, 1, 0, 8, 1);
    layout->addWidget(maxvlbl, 0, 0, 1, 1);
    layout->addWidget(minvlbl, 9, 0, 1, 1);
    layout->addWidget(valtxt_, 10, 0, 1, 1);
    layout->addWidget(namelbl, 11, 0, 2, 1);
    setLayout(layout);

    connect(slider_, SIGNAL(valueChanged(int) ),
            this,    SLOT(setValue(int) ));

    connect(slider_, SIGNAL(sliderReleased() ),
            this,    SLOT(sliderFinished() ));
}

trk::TSlider::
~TSlider()
{
    delete slider_;
}

void
trk::TSlider::
setValue(int value)
{
    value_ = value;
    QString str;
    valtxt_->setText(str.setNum(value_) );


    //emit valueChanged(value_);
}

int
trk::TSlider::
value()
{
    return slider_->value();
}

void 
trk::TSlider::
sliderFinished()
{
    value_ = slider_->value();
    std::cout << "TSlider.sliderFinished, value = " << value_ << std::endl;
    emit valueChanged(value_);
}
    

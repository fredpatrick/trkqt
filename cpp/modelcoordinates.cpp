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

#include "modelcoordinates.h"
#include <iostream>
#include <iomanip>
#include <QHBoxLayout>
#include <QVector3D>

trk::ModelCoordinates::
ModelCoordinates(QWidget* parent) : QGroupBox(parent)
{
    polar_angle_slider_     = new TSlider(this, "Polar\nAngle", 0, 360,  30);
    azimuth_slider_         = new TSlider(this, "Azimuth\n"   , 0, 180,  60);
    scale_slider_           = new TSlider(this, "Scale (%)"   , 50,  200, 100);


    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(polar_angle_slider_);
    layout->addWidget(azimuth_slider_);
    layout->addWidget(scale_slider_);
    setLayout(layout);

    connect(polar_angle_slider_    , SIGNAL(valueChanged(int)), 
            this                   , SLOT(polarAngleChanged(int) ));
    connect(azimuth_slider_        , SIGNAL(valueChanged(int)), 
            this                   , SLOT(azimuthChanged(int) ));
    connect(scale_slider_          ,SIGNAL(valueChanged(int)), 
            this,                    SLOT(scaleChanged(int) ));
    makeXform();
}

trk::ModelCoordinates::
~ModelCoordinates()
{
    delete polar_angle_slider_;
    delete azimuth_slider_;
    delete scale_slider_;
}

void
trk::ModelCoordinates::
makeXform()
{
    std::cout << "ModelCoordinates.makeXform" << std::endl;
    xform_.setToIdentity();
    polar_angle_ = polar_angle_slider_->value();
    azimuth_     = azimuth_slider_->value();
    scale_       = scale_slider_->value() / 100.;

    xform_.rotate(  azimuth_, 1.0f, 0.0f, 0.0f);
    xform_.rotate(polar_angle_, 0.0f, 0.0f, 1.0f);
    xform_.scale(scale_);

    print();
}

QMatrix4x4
trk::ModelCoordinates::
modelXform()
{
    return xform_;
}

void 
trk::ModelCoordinates::
azimuthChanged(int value)
{
    azimuth_ = value;
    makeXform();
    std::cout << "ModelCoordinates.azimuth_changed, value = " << value << std::endl;
    print();
    emit model_coordinates_changed( xform_);
}

void 
trk::ModelCoordinates::
polarAngleChanged(int value)
{
    polar_angle_ = value;
    makeXform();
    emit model_coordinates_changed( xform_);
}

void 
trk::ModelCoordinates::
scaleChanged(int value)
{
    float f = value / 100.0;
    scale_ =  f;
    makeXform();
    emit model_coordinates_changed( xform_);
}

void 
trk::ModelCoordinates::
print()
{
    std::cout << "\n" << std::endl;
    std::cout << std::setprecision(6);
    for ( int i = 0; i < 4; i++) {
        QVector4D r = xform_.row(i);
        std::cout << "ModelCoordinates,xform    = ";
        for ( int j = 0; j < 4; j++) std::cout << std::setw(10) << r[j];
        std::cout << std::endl;
    }
    QVector4D vp = QVector4D(0.0, 0.0, 10.0, 1.0);
    QVector4D v  = xform_ * vp;
    std::cout << "ModelCoordinates.print, v = ";
    for ( int i = 0; i < 4; i++ ) std::cout << std::setw(10) << v[i];
    std::cout << std::endl;
}

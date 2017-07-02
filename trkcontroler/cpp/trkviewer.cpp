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

#include "trkviewer.h"
#include "trkutl.h"
#include "modelcoordinates.h"
#include "gllayoutwidget.h"
#include "layoutstate.h"

#include <iostream>
#include <string>
#include <iomanip>
#include <QGroupBox>
#include <QGridLayout>

trk::TrkViewer::
TrkViewer( LayoutState* layoutstate, 
           QWidget* parent) : QFrame(parent)
{
    model_coordinates_ = new ModelCoordinates(this);
    GLLayoutWidget* gllayoutwidget = new GLLayoutWidget(model_coordinates_, 
                                                        layoutstate);

    //std::cout << "TrkViewer.ctor, gllayoutwidget" << std::endl;
    setFrameStyle(QFrame::Box | QFrame::Plain);
    QVBoxLayout* layout = new QVBoxLayout;

    QGroupBox* viewerbox = new QGroupBox(this);
    viewerbox->setTitle("TrkViewer");
    QGridLayout* viewerlayout = new QGridLayout;
    viewerlayout->addWidget(gllayoutwidget, 1, 0, 1, 8);
    viewerlayout->addWidget(model_coordinates_, 1, 9, 1, 1);
    viewerbox->setLayout(viewerlayout);

    layout->addWidget(viewerbox);
    setLayout(layout);

    resize(1200, 800);
}

trk::TrkViewer::
~TrkViewer()
{
    delete model_coordinates_;
}

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

#ifndef TRK_GLLAYOUTWIDGET_HH
#define TRK_GLLAYOUTWIDGET_HH

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QColor>
#include <QMatrix4x4>


#include "trkutl.h"
#include <string>
#include <utility>

namespace trk
{
    class GLAxesShader;
    class GLBedLineShader;
    class GLLayoutShader;
    class DebugCntl;
    class LayoutConfig;
    class LayoutGeometry;
    class LayoutState;
    class ModelCoordinates;
    class PaintList;
    class Path;
    class Ranges;

    class GLLayoutWidget : public QOpenGLWidget, protected QOpenGLFunctions
    {
        Q_OBJECT

        public:
            GLLayoutWidget(ModelCoordinates* mode_coordinates,
                           LayoutState*      layout_state);
            ~GLLayoutWidget();

        public slots:
            void        layoutstate_initialized();
            void        model_coordinates_changed(QMatrix4x4&);
            void        zone_busy(int);
            void        zone_idle(int);
            void        new_path();
            void        switch_queued(int, bool);
            void        switch_thrown(int, bool);

        protected:
            void        initializeGL();
            void        paintGL();
            void        resizeGL(int w, int h);

        private:
            void                      set_path_colors(Path*, const QColor&);
            void                      set_exitpath_colors(const QColor&);
            std::pair<int, int>       get_shell_indexes(int switch_index);
            std::string               print_xform(const QMatrix4x4&  xform,
                                                  const std::string& tag);
            std::string               qcolor_to_s(const QColor& color);
            std::string               icolor_to_s(int icolor);
            DebugCntl*                dbg_;
            LayoutConfig*             layout_config_;
            LayoutGeometry*           layout_geometry_;
            LayoutState*              layout_state_;
            QMatrix4x4                make_ortho_xform(Ranges& r);
            QMatrix4x4                ortho_xform_;
            GLLayoutShader*           layout_shader_;
            GLAxesShader*             axes_shader_;
            GLBedLineShader*          bedline_shader_;
            ModelCoordinates*         model_coordinates_;
            QColor                    background_;
            QMatrix4x4                model_xform_;
            enum COLORCODE {WHITE, BLACK, RED, GREEN, BLUE,
                            LIGHTSTEELBLUE, LIGHTYELLOW, YELLOW, LIME, LIMEGREEN, 
                            ORANGE, DARKGREEN};
            std::vector<QColor>       colors_;
            QColor                    default_zone_clr_;
            QColor                    active_zone_clr_;
            QColor                    exit_zone_clr_;
            QColor                    current_path_clr_;
            QColor                    interdit_clr_;
            std::vector<QColor>       default_switch_clrs_;
            std::vector<QColor>       queued_switch_clrs_;
            PaintList*                paint_list_;
            int                       zone_count_;
            int                       switch_count_;
            int                       shell_count_;
    };
}
#endif

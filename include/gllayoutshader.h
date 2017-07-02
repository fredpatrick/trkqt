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

#ifndef TRK_GLLAYOUTSHADER_HH
#define TRK_GLLAYOUTSHADER_HH

#include <QOpenGLBuffer>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QColor>
#include <QMatrix4x4>
#include <QVector>
#include <QVector3D>

#include "gllayoutwidget.h"
#include "modelcoordinates.h"
#include "ranges.h"
#include <string>
#include <vector>

namespace trk
{
    class DebugCntl;
    class LayoutConfig;
    class LayoutGeometry;
    class PaintList;

    class GLLayoutShader
    {
        public:
            GLLayoutShader(PaintList* paint_list, const QColor& color);
            ~GLLayoutShader();

            void                      initialize();
            void                      paint(int vtx_i0, 
                                            int vtx_count, 
                                            const QColor& color,
                                            const QMatrix4x4& ortho_xform);
            void                      update_vertexes(const QMatrix4x4& xform, Ranges& r);

        private:
            void                                load_buffer();
            DebugCntl*                          dbg_;
            LayoutGeometry*                     layout_geometry_;
            LayoutConfig*                       layout_config_;

            QOpenGLShader*                      vshader_;
            QOpenGLShader*                      fshader_;
            QOpenGLShaderProgram*               program_;
            QOpenGLBuffer                       vbo_;

            int                                 vertex_attr_;
            int                                 normal_attr_;
            int                                 ortho_uniform_;
            int                                 ucolor_uniform_;

            QVector<QVector3D>                  rw_vertexes_;
            QVector<QVector3D>                  rw_normals_;
            QVector<QVector3D>                  vertexes_;
            QVector<QVector3D>                  normals_;
            QColor                              color_;
            std::vector<QColor>                 colors_;
            QVector3D                           ucolor_;
            int                                 zone_count_;
            int                                 switch_count_;
    };
}
#endif

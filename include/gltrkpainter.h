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

#ifndef TRK_GLTRKPAINTER_HH
#define TRK_GLTRKPAINTER_HH

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QColor>
#include <QMatrix4x4>
#include <QVector>
#include <QVector3D>

#include "modelxform.h"


namespace trk
{
    class LayoutGeometry;

    class GLTrkPainter : public QOpenGLWidget, protected QOpenGLFunctions
    {
        Q_OBJECT

        public:
            GLTrkPainter(const QColor&             background, 
                         ModelXform*               model_xform);
            ~GLTrkPainter();

        public slots:
            void newModelXform(QMatrix4x4&);
            
        protected:
            void initializeGL()         Q_DECL_OVERRIDE;
            void paintGL()              Q_DECL_OVERRIDE;
            void resizeGL(int w, int h) Q_DECL_OVERRIDE;

        private:

            LayoutGeometry*             layout_geometry_;
            QMatrix4x4                  makeOrthoXform();
            void                        loadBedVertexBuffer();
            void                        loadAxesVertexBuffer();
            void                        loadBedLinesVertexBuffer();
            void                        initializeBed();
            void                        initializeBedLines();
            void                        initializeAxes();
            void                        paintBed();
            void                        paintBedLines();
            void                        paintAxes();
            void                        printAxes();
            void                        printXform(const QMatrix4x4& xform,
                                                   const std::string& tag);

            QOpenGLShader*              vshader_;
            QOpenGLShader*              fshader_;
            QOpenGLShaderProgram*       program_;
            QOpenGLBuffer               vbo_;

            int                         vertex_attr_;
            int                         normal_attr_;
            int                         matrix_uniform_;
            int                         bcolor_uniform_;

            QOpenGLShader*              vshader_axes_;
            QOpenGLShader*              fshader_axes_;
            QOpenGLShaderProgram*       program_axes_;
            QOpenGLBuffer               vbo_axes_;

            int                         axes_vertex_attr_;
            int                         axes_color_attr_;
            int                         axes_matrix_uniform_;
            QOpenGLShader*              vshader_bed_lines_;
            QOpenGLShader*              fshader_bed_lines_;
            QOpenGLShaderProgram*       program_bed_lines_;
            QOpenGLBuffer               vbo_bed_lines_;

            int                         bed_lines_vertex_attr_;
            int                         bed_lines_matrix_uniform_;

            QVector<QVector3D>          rw_vertices_;
            QVector<QVector3D>          rw_normals_;
            QVector<QVector3D>          rw_axes_vertices_;
            QVector<QVector3D>          rw_edge_vertices_;
            QVector<QVector3D>          vertices_;
            QVector<QVector3D>          normals_;
            QVector<QVector3D>          axes_vertices_;
            QVector<QVector3D>          edge_vertices_;

            QColor                      background_;
            QColor                      bed_color_;
            QMatrix4x4                  model_xform_;
            QMatrix4x4                  ortho_xform_;
            QVector3D                   bcolor_;
    };
}

#endif

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

#include "glaxesshader.h"
#include <iostream>

trk::GLAxesShader::
GLAxesShader()
{
    dbg_ = DebugCntl::instance();
    layout_geometry_ = LayoutGeometry::instance();
    Ranges r = layout_geometry_->ranges();;
    std::cout << r.print("AxesShader") << std::endl;
    rw_vertexes_.resize(12);
    rw_vertexes_[0]   = QVector3D(    0.0,     0.0,      0.0);
    rw_vertexes_[1]   = QVector3D( r.xmax,     0.0,      0.0);
    rw_vertexes_[2]   = QVector3D(    0.0,     0.0,      0.0);
    rw_vertexes_[3]   = QVector3D(    0.0,  r.ymax,      0.0);
    rw_vertexes_[4]   = QVector3D(    0.0,     0.0,      0.0);
    rw_vertexes_[5]   = QVector3D(    0.0,     0.0,   r.zmin);
    rw_vertexes_[6]   = QVector3D(    0.0,     0.0,      0.0);
    rw_vertexes_[7]   = QVector3D(-r.xmax,     0.0,      0.0);
    rw_vertexes_[8]   = QVector3D(    0.0,     0.0,      0.0);
    rw_vertexes_[9]   = QVector3D(    0.0, -r.ymax,      0.0);
    rw_vertexes_[10]  = QVector3D(    0.0,     0.0,      0.0);
    rw_vertexes_[11]  = QVector3D(    0.0,     0.0,      0.0);
}

void 
trk::GLAxesShader::
initialize()
{
    vshader_ = new QOpenGLShader(QOpenGLShader::Vertex);
    const char* vsrc_axes =
    "attribute highp vec4 vertex;\n"
    "attribute mediump vec4 vcolor;\n"
    "uniform mediump mat4 ortho;\n"
    "varying mediump vec4 color;\n"
    "void main()\n"
    "{\n"
    "   color = clamp(vcolor, 0.0, 1.0);\n"
    "   gl_Position = ortho * vertex;\n"
    "}\n";
    if ( !vshader_->compileSourceCode(vsrc_axes) ) {
        std::cout << "GLAxesShader.initializeGL, " <<
                         "axes vertex shader failed to compile" << std::endl;
        return;
    }

    fshader_ = new QOpenGLShader(QOpenGLShader::Fragment);
    const char* fsrc_axes =
    "varying mediump vec4 color\n;"
    "void main()\n"
    "{\n"
    "   gl_FragColor = color;\n"
    "}\n";
    if ( !fshader_->compileSourceCode(fsrc_axes) ) {
        std::cout << "GLAxesShader.initializeGL, " <<
                         "axes fragment shader failed to compile" << std::endl;
        return;
    }

    program_ = new QOpenGLShaderProgram;
    program_->addShader(vshader_);
    program_->addShader(fshader_);
    program_->link();

    vertex_attr_ = program_->attributeLocation("vertex");
    color_attr_  = program_->attributeLocation("vcolor");
    ortho_uniform_ = program_->uniformLocation("ortho");

    if ( !vbo_.create() ) {
        std::cout << "GLAxesShader,initializeAxes, vbo_.create() failed" << std::endl;
    }
    //load_buffer();
}

void
trk::GLAxesShader::
paint(const QMatrix4x4& ortho_xform)
{
    program_->bind();
    glVertexAttrib4f(color_attr_, 1.0, 0.0, 0.0, 1.0);
    
    program_->setUniformValue(ortho_uniform_, ortho_xform);
    program_->enableAttributeArray(vertex_attr_);
    program_->enableAttributeArray(color_attr_);
    vbo_.bind();
    program_->setAttributeBuffer(vertex_attr_,
                                      GL_FLOAT,
                                      0,
                                      3,
                                      7 * sizeof(GLfloat) );
    program_->setAttributeBuffer(color_attr_,
                                      GL_FLOAT,
                                      3 * sizeof(GLfloat),
                                      4,
                                      7 * sizeof(GLfloat));
    vbo_.release();
    glLineWidth(3.0);

    glDrawArrays(GL_LINES, 
                 0,
                 6 );
    glLineWidth(1.0);
    glDrawArrays(GL_LINES, 
                 6 , 
                 6 );
    program_->disableAttributeArray(vertex_attr_);
    program_->disableAttributeArray(color_attr_);
    program_->release();
}

void 
trk::GLAxesShader::
load_buffer()
{
    if ( !vbo_.bind() ) {
        std::cout << "GLAxesShader,initializeAxes, vbo_.bind() failed" << std::endl;
    }
    QVector<QVector4D> colors;
    colors.resize(vertexes_.size() );
    colors[0]  = QVector4D(1.0, 0.0, 0.0, 1.0);
    colors[1]  = QVector4D(1.0, 0.0, 0.0, 1.0);
    colors[2]  = QVector4D(0.0, 1.0, 0.0, 1.0);
    colors[3]  = QVector4D(0.0, 1.0, 0.0, 1.0);
    colors[4]  = QVector4D(0.0, 0.0, 1.0, 1.0);
    colors[5]  = QVector4D(0.0, 0.0, 1.0, 1.0);
    colors[6]  = QVector4D(1.0, 0.0, 0.0, 1.0);
    colors[7]  = QVector4D(1.0, 0.0, 0.0, 1.0);
    colors[8]  = QVector4D(0.0, 1.0, 0.0, 1.0);
    colors[9]  = QVector4D(0.0, 1.0, 0.0, 1.0);
    colors[10] = QVector4D(0.0, 0.0, 1.0, 1.0);
    colors[11] = QVector4D(0.0, 0.0, 1.0, 1.0);
    QVector<GLfloat> axes_buf;
    axes_buf.resize(vertexes_.size() * 7);
    GLfloat* q = axes_buf.data();
    for ( int i = 0; i < vertexes_.size(); i++) {
        *q++ =vertexes_[i].x();
        *q++ =vertexes_[i].y();
        *q++ =vertexes_[i].z();
        *q++ = colors[i].x();
        *q++ = colors[i].y();
        *q++ = colors[i].z();
        *q++ = colors[i].w();
    }
    vbo_.allocate(axes_buf.constData(), axes_buf.count() * sizeof(GLfloat) );
    vbo_.release();
}

void
trk::GLAxesShader::
update_vertexes(const QMatrix4x4& xform,
                Ranges&           r)
{
    vertexes_.resize(rw_vertexes_.size() );
    for ( int i = 0; i < rw_vertexes_.size(); i++ ) {
        vertexes_[i] = xform * rw_vertexes_[i];
    }
    load_buffer();
    r.update(vertexes_);
}

void 
trk::GLAxesShader::
print_axes(const QMatrix4x4& ortho_xform)
{
    std::ostringstream ss;
    ss << "GLAxesShader.print_axes, " << std::endl;
    std::string tag[3];
    tag[0] = "x"; tag[1] = "y"; tag[2] = "z";
    int n = vertexes_.size();
    for ( int i = 0; i < n; i += 2) {
        ss << std::setprecision(4);
        for ( int k = 0; k < 2; k++) {
            QVector3D p = ortho_xform * vertexes_[i+k];
            for (int j = 0; j < 3; j++) ss << std::setw(8) << p[j];
        }
        ss << std::endl;
    }
    *dbg_ << ss.str();
}

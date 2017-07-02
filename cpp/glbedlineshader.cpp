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

#include "glbedlineshader.h"
#include "bedlinegeometry.h"
#include "debugcntl.h"
#include "layoutconfig.h"
#include "layoutgeometry.h"
#include "paintlist.h"

#include <iomanip>
#include <iostream>
#include <sstream>

trk::GLBedLineShader::
GLBedLineShader()
{
    dbg_             = DebugCntl::instance();
    layout_config_   = LayoutConfig::instance();
    layout_geometry_ = LayoutGeometry::instance();
    rw_vertexes_.clear();
    int i0 = 0;
    int                count;
    std::vector<std::string> zonenames = layout_config_->track_sensor_names();
    zone_count_ = layout_config_->track_sensor_count();
    for ( int i = 0; i < zone_count_; i++) {
        ZoneGeometry* zone_geometry = layout_geometry_->zone_geometry( i);
        for ( int k = 0; k < 5; k++ ) {
            int count  = 0;
            int section_count = zone_geometry->section_count();
            for ( int j = 0; j < section_count; j++ ) {
                SectionGeometry* section_geometry = zone_geometry->section_geometry(j);
                ShellGeometry* shell_geometry = section_geometry->shell_geometry(0);
                if ( k == 1 ) continue;
                QVector<QVector3D> vertexes = shell_geometry->edge_vertexes(k);
                rw_vertexes_ += vertexes;
                count += vertexes.size();
            }
            std::pair<int,int> item;
            item.first = i0;
            item.second = count;
            i0 += count;
            vtx_list_.push_back(item);
        }
    }
    switch_count_ = layout_config_->switch_count();
    std::vector<std::string> switch_names = layout_config_->switch_names();
    for ( int i = 0; i < switch_count_; i++) {
        SwitchGeometry* switch_geometry = layout_geometry_->switch_geometry(i);
        int bedline_count = switch_geometry->bedline_count();
        for ( int j = 0; j < bedline_count; j++) {
            BedlineGeometry* bedline_geometry = switch_geometry->bedline_geometry(j);
            QVector<QVector3D> vertexes = bedline_geometry->vertexes();
            rw_vertexes_ += vertexes;
            count = vertexes.size();
            std::pair<int,int> item;
            item.first  = i0;
            item.second = count;
            vtx_list_.push_back(item);
            i0 += count;
        }
    }
}

void
trk::GLBedLineShader::
initialize()
{
    vshader_ = new QOpenGLShader(QOpenGLShader::Vertex);
    const char* vsrc =
        "attribute highp vec4 vertex;\n"
        "uniform mediump mat4 ortho;\n"
        "varying mediump vec4 fcolor;\n"
        "void main(void)\n"
        "{\n"
        "   fcolor = vec4(0.0, 0.0, 0.0, 1.0);\n"
        "   gl_Position = ortho * vertex;\n"
        "}\n";
    if ( !vshader_->compileSourceCode(vsrc) ) {
        std::cout << "GLBedLineShader.initialize Compile failed for vertex shader" << std::endl;
        return;
    }

    fshader_ = new QOpenGLShader(QOpenGLShader::Fragment);
    const char* fsrc = 
        "varying   mediump vec4 fcolor;\n"
        "void main(void)\n"
        "{\n"
        "   gl_FragColor = fcolor;\n"
        "}\n";
    if ( !fshader_->compileSourceCode(fsrc) ) {
        std::cout << "GLBedLineShader::initialize fshader failed to compile" << std::endl;
        return;
    }

    program_ = new QOpenGLShaderProgram;
    program_->addShader(vshader_);
    program_->addShader(fshader_);
    program_->link();

    vertex_attr_    = program_->attributeLocation("vertex");
    ortho_uniform_  = program_->uniformLocation("ortho");
//  std::cout << "GLBedLineShader.initialize.vertex_attr = " << vertex_attr_ << std::endl;
//  std::cout << "GLBedLineShader.initialize.matrix_uniform_ = " << ortho_uniform_ << std::endl;

    if ( !vbo_.create() ) {
        std::cout << "GLBedLineShader,initialize, vbo_.create() failed" << std::endl;
    }
}

void 
trk::GLBedLineShader::
load_buffer()
{
    if ( !vbo_.bind() ) {
        std::cout << "GLBedLineShader,load_buffer, vbo_.bind() failed" << std::endl;
    }
    const int vertex_count =  vertexes_.size();
    QVector<GLfloat> buf;
    buf.resize(vertex_count * 3);
    GLfloat* p = buf.data();
    int n = 0;
    for ( int i = 0; i < vertex_count; i++) {        
        *p++ = vertexes_[i].x();
        *p++ = vertexes_[i].y();
        *p++ = vertexes_[i].z();
        n+=3;
    }
    vbo_.allocate(buf.constData(), buf.count() * sizeof(GLfloat) );
    vbo_.release();
}

void
trk::GLBedLineShader::
paint( const QMatrix4x4& ortho_xform)
{
    program_->bind();
    program_->setUniformValue(ortho_uniform_, ortho_xform);
    program_->enableAttributeArray(vertex_attr_);

    vbo_.bind();
    program_->setAttributeBuffer(vertex_attr_,
                                 GL_FLOAT,
                                  0, 
                                  3,
                                  0);
    vbo_.release();
    glLineWidth(3.0);
    int item_count = vtx_list_.size();
    for ( int l = 0; l < item_count; l++ ) {
        glDrawArrays(GL_LINE_STRIP, 
                     vtx_list_[l].first,
                     vtx_list_[l].second);
    }

    program_->disableAttributeArray(vertex_attr_);
    program_->release();
}

void
trk::GLBedLineShader::
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

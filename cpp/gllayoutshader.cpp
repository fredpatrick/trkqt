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

#include "gllayoutshader.h"
#include "debugcntl.h"
#include "layoutconfig.h"
#include "layoutgeometry.h"
#include "paintlist.h"

#include <iomanip>
#include <iostream>
#include <sstream>

trk::GLLayoutShader::
GLLayoutShader(PaintList* paint_list, const QColor& color)
{
    std::cout << "GLLayoutShader.ctor" << std::endl;
    dbg_             = DebugCntl::instance();
    layout_config_   = LayoutConfig::instance();
    layout_geometry_ = LayoutGeometry::instance();
    rw_vertexes_.clear();
    rw_normals_.clear();
    PaintItem item;
    int i0 = 0;
    std::vector<std::string> zonenames = layout_config_->track_sensor_names();
    zone_count_ = layout_config_->track_sensor_count();
    for ( int i = 0; i < zone_count_; i++) {
        ZoneGeometry* zone_geometry = layout_geometry_->zone_geometry( i);
        item.vtx_i0 = i0;
        int ic = 0; 
        int section_count = zone_geometry->section_count();
        for ( int j = 0; j < section_count; j++ ) {
            SectionGeometry* section_geometry = zone_geometry->section_geometry(j);
            ShellGeometry* shell_geometry = section_geometry->shell_geometry(0);
            rw_vertexes_ += shell_geometry->vertexes();
            rw_normals_  += shell_geometry->normals();
            int count = shell_geometry->vertexes().size();
            i0 += count;
            ic += count;
        }
        item.vtx_count = ic;
        item.vtx_color = color;
        paint_list->item_by_zone(i, item);
    }
    switch_count_ = layout_config_->switch_count();
    std::vector<std::string> switch_names = layout_config_->switch_names();
    for ( int i = 0; i < switch_count_; i++) {
        SwitchGeometry* switch_geometry = layout_geometry_->switch_geometry(i);
        int shell_count = switch_geometry->shell_count();
        for ( int j = 0; j < shell_count; j++) {
            ShellGeometry* shell_geometry = switch_geometry->shell_geometry(j);
            rw_vertexes_ += shell_geometry->vertexes();
            rw_normals_  += shell_geometry->normals();
            int count = shell_geometry->vertexes().size();
            item.vtx_i0    = i0;
            item.vtx_count = count;
            item.vtx_color = color;
            paint_list->item_by_switch(i, j, item);
            i0 += count;

        }
    }
}

void
trk::GLLayoutShader::
initialize()
{
    vshader_ = new QOpenGLShader(QOpenGLShader::Vertex);
    const char* vsrc =
        "attribute highp vec4 vertex;\n"
        "attribute mediump vec3 normal;\n"
        "uniform mediump mat4 ortho;\n"
        "uniform mediump vec3 ucolor;\n"
        "varying mediump vec4 fcolor;\n"
        "void main(void)\n"
        "{\n"
        "   vec3 toLight = normalize(vec3(0.0, 0.3, 1.0));\n"
        "   float angle = max(dot(normal, toLight), 0.0);\n"
        "   vec3 col = ucolor;\n"
        "   fcolor = vec4(ucolor, 1.0);\n"
        "   gl_Position = ortho * vertex;\n"
        "}\n";
    if ( !vshader_->compileSourceCode(vsrc) ) {
        std::cout << "GLLayoutShader.initialize Compile failed for vertex shader" << std::endl;
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
        std::cout << "GLLayoutShader::initialize fshader failed to compile" << std::endl;
        return;
    }

    program_ = new QOpenGLShaderProgram;
    program_->addShader(vshader_);
    program_->addShader(fshader_);
    program_->link();

    vertex_attr_    = program_->attributeLocation("vertex");
    normal_attr_    = program_->attributeLocation("normal");
    ortho_uniform_ = program_->uniformLocation("ortho");
    ucolor_uniform_ = program_->uniformLocation("ucolor");
    //std::cout << "GLLayoutShader.initialize.vertex_attr = " << vertex_attr_ << std::endl;
    //std::cout << "GLLayoutShader.initialize.normal_attr = " << normal_attr_ << std::endl;
    //std::cout << "GLLayoutShader.initialize.matrix_uniform_ = " << ortho_uniform_ << std::endl;
    //std::cout << "GLLayoutShader.initialize.ucolor_uniform_ = " << ucolor_uniform_ << std::endl;

    if ( !vbo_.create() ) {
        std::cout << "GLLayoutShader,initialize, vbo_.create() failed" << std::endl;
    }
}

void 
trk::GLLayoutShader::
load_buffer()
{
    if ( !vbo_.bind() ) {
        std::cout << "GLLayoutShader,load_buffer, vbo_.bind() failed" << std::endl;
    }
    const int vertex_count =  vertexes_.size();
    QVector<GLfloat> buf;
    buf.resize(vertex_count * 6);
    GLfloat* p = buf.data();
    int n = 0;
    for ( int i = 0; i < vertex_count; i++) {        
        *p++ = vertexes_[i].x();
        *p++ = vertexes_[i].y();
        *p++ = vertexes_[i].z();
        *p++ = normals_[i].x();
        *p++ = normals_[i].y();
        *p++ = normals_[i].z();
        n+=6;
    }
    vbo_.allocate(buf.constData(), buf.count() * sizeof(GLfloat) );
    vbo_.release();
}

void
trk::GLLayoutShader::
paint(int vtx_i0, int vtx_count, const QColor& color, const QMatrix4x4& ortho_xform)
{
    if ( dbg_->check(4) ) {
        std::ostringstream ss;
        ss << "GLLayoutShader.paint, vtx_i0 = " << std::setw(6) << vtx_i0 << ", vtx_count = "
               << std::setw(5) << vtx_count << ", color = " 
               << color.red() << ", " << color.green() << ", " << color.blue() << std::endl;
        *dbg_ << ss.str();
    }
    QVector3D ucolor = QVector3D(color.redF(), color.greenF(), color.blueF() );
    program_->bind();
    program_->setUniformValue(ortho_uniform_, ortho_xform);
    program_->setUniformValue(ucolor_uniform_, ucolor);
    program_->enableAttributeArray(vertex_attr_);
    program_->enableAttributeArray(normal_attr_);

    vbo_.bind();
    program_->setAttributeBuffer(vertex_attr_,
                                  GL_FLOAT,
                                  0, 
                                  3,
                                  6 * sizeof(GLfloat));
    program_->setAttributeBuffer(normal_attr_,
                                  GL_FLOAT,
                                  3, 
                                  3,
                                  6 * sizeof(GLfloat));
    vbo_.release();
    glDrawArrays(GL_TRIANGLES, vtx_i0, vtx_count);

    program_->disableAttributeArray(vertex_attr_);
    program_->disableAttributeArray(normal_attr_);
    program_->release();
}

void
trk::GLLayoutShader::
update_vertexes(const QMatrix4x4& xform,
                Ranges&           r)
{
    vertexes_.resize(rw_vertexes_.size() );
    normals_.resize(rw_vertexes_.size() );
    for ( int i = 0; i < rw_vertexes_.size(); i++ ) {
        vertexes_[i] = xform * rw_vertexes_[i];
        normals_[i]  = xform * rw_normals_[i];
    }
    load_buffer();
    r.update(vertexes_);
}

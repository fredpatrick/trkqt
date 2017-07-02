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

#include "gltrkpainter.h"
#include "ranges.h"
#include "layoutgeometry.h"
#include "zonegeometry.h"
#include "sectiongeometry.h"
#include "shellgeometry.h"
#include "switchgeometry.h"
#include <string.h>
#include <iostream>
#include <iomanip>
#include <math.h>
#include <QPainter>


trk::GLTrkPainter::
GLTrkPainter(const QColor&             background, 
             ModelXform*               mx)
{                                                  
    std::cout << "GLTrkPainter::ctor" << std::endl;
    layout_geometry_ = LayoutGeometry::instance();
    background_      = background;
    rw_vertices_.clear();
    rw_normals_.clear();
    rw_edge_vertices_.clear();
    std::vector<std::string> zonenames = layout_geometry_->zonenames();
    int zone_count = zonenames.size();
    for ( int i = 0; i < zone_count; i++) {
        ZoneGeometry* zone_geometry = layout_geometry_->zone_geometry( zonenames[i] );
        int section_count = zone_geometry->section_count();
        for ( int j = 0; j < section_count; j++ ) {
            SectionGeometry* section_geometry = zone_geometry->section_geometry(j);
            ShellGeometry* shell_geometry = section_geometry->shell_geometry(0);
            rw_vertices_ += shell_geometry->vertices();
            rw_normals_  += shell_geometry->normals();
            for ( int k = 0; k < 5; k++ ) {
                if ( k == 1 ) continue;
                rw_edge_vertices_ += shell_geometry->edge_vertices(k);
            }
        }
    }
    Ranges r;
    r.init();
    r.update(rw_vertices_, "rw_vertices");
    rw_axes_vertices_.resize(12);
    rw_axes_vertices_[0]   = QVector3D(    0.0,     0.0,      0.0);
    rw_axes_vertices_[1]   = QVector3D( r.xmax,     0.0,      0.0);
    rw_axes_vertices_[2]   = QVector3D(    0.0,     0.0,      0.0);
    rw_axes_vertices_[3]   = QVector3D(    0.0,  r.ymax,      0.0);
    rw_axes_vertices_[4]   = QVector3D(    0.0,     0.0,      0.0);
    rw_axes_vertices_[5]   = QVector3D(    0.0,     0.0,   r.zmin);
    rw_axes_vertices_[6]   = QVector3D(    0.0,     0.0,      0.0);
    rw_axes_vertices_[7]   = QVector3D(-r.xmax,     0.0,      0.0);
    rw_axes_vertices_[8]   = QVector3D(    0.0,     0.0,      0.0);
    rw_axes_vertices_[9]   = QVector3D(    0.0, -r.ymax,      0.0);
    rw_axes_vertices_[10]  = QVector3D(    0.0,     0.0,      0.0);
    rw_axes_vertices_[11]  = QVector3D(    0.0,     0.0,      0.0);
    model_xform_ = mx->modelXform();                             // this triggers update
    ortho_xform_.setToIdentity();
    printAxes();
    setMinimumSize(500, 400);
    resize(1500,1200);
    std::cout << "GLTrkPainter::ctor-1" << std::endl;
}

trk::GLTrkPainter::
~GLTrkPainter()
{
    makeCurrent();
    delete program_;
    delete vshader_;
    delete fshader_;
    vbo_.destroy();
    doneCurrent();
}

void
trk::GLTrkPainter::
initializeGL()
{
    std::cout << "GLTrkPainter::initializeGL" << std::endl;
    initializeOpenGLFunctions();

    const unsigned char* sbuf;
    sbuf = glGetString(GL_VERSION);
    std::cout << strlen((char*)sbuf) << std::endl;
    std::string str = (char*)sbuf;
    std::cout << str << std::endl;

    initializeBed();
    initializeBedLines();

    initializeAxes();
    newModelXform(model_xform_);

    std::cout << "GLTrkPainter::initializeGL conplete" << std::endl;
}

void
trk::GLTrkPainter::
paintGL()
{
    std::cout << "GLTrkPainter.paintGL" << std::endl;
    QPainter painter;
    painter.begin(this);
    painter.beginNativePainting();
  
    background_.setRgb(255,255,255);
    glClearColor(background_.redF(),
                 background_.greenF(),
                 background_.blueF(),
                 true);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    bed_color_.setRgb(176,196,222);                         // LightSteelBlue
    bcolor_ = QVector3D(bed_color_.redF(),
                        bed_color_.greenF(),
                        bed_color_.blueF());
/*
    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
*/
    glEnable(GL_DEPTH_TEST);
    glDepthRangef(0.0,1.0);
    glDepthMask(true);
    glDepthFunc(GL_LESS);
    std::cout << "GLTrkPainter.paintGl. vertices_.size      = " << 
                                         vertices_.size() << std::endl;
    std::cout << "GLTrkPainter.paintGl. axes_vertices_.size = " << 
                                  axes_vertices_.size() << std::endl;

    paintAxes();
    paintBed();
    paintBedLines();


    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    painter.endNativePainting();
    painter.end();
}

void
trk::GLTrkPainter::
resizeGL(int w, int h)
{
    std::cout << "GLTrkPainter.resize, w = " << w << ", h = " << h << std::endl;
}

//############################################################Begin bed specific opengl code
//############################################################
void
trk::GLTrkPainter::
initializeBed()
{
    vshader_ = new QOpenGLShader(QOpenGLShader::Vertex);
    const char* vsrc =
        "attribute highp vec4 vertex;\n"
        "attribute mediump vec3 normal;\n"
        "uniform mediump mat4 matrix;\n"
        "uniform mediump vec3 bcolor;\n"
        "varying mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "   vec3 toLight = normalize(vec3(0.0, 0.3, 1.0));\n"
        "   float angle = max(dot(normal, toLight), 0.0);\n"
        "   vec3 col = bcolor;\n"
        "   color = vec4(bcolor, 1.0);\n"
        "   gl_Position = matrix * vertex;\n"
        "}\n";
    if ( !vshader_->compileSourceCode(vsrc) ) {
        std::cout << "GLTrkPainter.initializeGL Compile failed for vertex shader" << std::endl;
        return;
    }

    fshader_ = new QOpenGLShader(QOpenGLShader::Fragment);
    const char* fsrc = 
        "varying   mediump vec4 color;\n"
        "void main(void)\n"
        "{\n"
        "   gl_FragColor = color;\n"
        "}\n";
    if ( !fshader_->compileSourceCode(fsrc) ) {
        std::cout << "GLTrkPainter::initializeGL fshader failed to compile" << std::endl;
        return;
    }


    program_ = new QOpenGLShaderProgram;
    program_->addShader(vshader_);
    program_->addShader(fshader_);
    program_->link();

    vertex_attr_    = program_->attributeLocation("vertex");
    normal_attr_    = program_->attributeLocation("normal");
    matrix_uniform_ = program_->uniformLocation("matrix");
    bcolor_uniform_ = program_->uniformLocation("bcolor");
    std::cout << "GLTrkPainter.initializeGL.vertex_attr = " << vertex_attr_ << std::endl;
    std::cout << "GLTrkPainter.initializeGL.normal_attr = " << normal_attr_ << std::endl;
    std::cout << "GLTrkPainter.initializeGL.matrix_uniform_ = " << matrix_uniform_ << std::endl;
    std::cout << "GLTrkPainter.initializeGL.bcolor_uniform_ = " << bcolor_uniform_ << std::endl;

    if ( !vbo_.create() ) {
        std::cout << "GLTrkPainter,initializeGL, vbo_.create() failed" << std::endl;
    }
}

void
trk::GLTrkPainter::
paintBed()
{
    std::cout << "GLTrkPainter.paintBeds" << std::endl;
    loadBedVertexBuffer();
    ortho_xform_ = makeOrthoXform();
    program_->bind();
    program_->setUniformValue(matrix_uniform_, ortho_xform_);
    program_->setUniformValue(bcolor_uniform_, bcolor_);
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
    glDrawArrays(GL_TRIANGLES, 0, vertices_.size());

    program_->disableAttributeArray(vertex_attr_);
    program_->disableAttributeArray(normal_attr_);
    program_->release();
}

void
trk::GLTrkPainter::
loadBedVertexBuffer()
{
    if ( !vbo_.bind() ) {
        std::cout << "GLTrkPainter,initializeGL, vbo_.bind() failed" << std::endl;
    }
    const int vertex_count =  vertices_.size();
    QVector<GLfloat> buf;
    buf.resize(vertex_count * 6);
    GLfloat* p = buf.data();
    int n = 0;
    for ( int i = 0; i < vertex_count; i++) {        
        *p++ = vertices_[i].x();
        *p++ = vertices_[i].y();
        *p++ = vertices_[i].z();
        *p++ = normals_[i].x();
        *p++ = normals_[i].y();
        *p++ = normals_[i].z();
        n+=6;
    }
    vbo_.allocate(buf.constData(), buf.count() * sizeof(GLfloat) );
    vbo_.release();
}

//############################################################Begin axes specific opengl code
//############################################################
void
trk::GLTrkPainter::
initializeAxes()
{
    std::cout << "GLTrkPainter.initializeAXes" << std::endl;
    vshader_axes_ = new QOpenGLShader(QOpenGLShader::Vertex);
    const char* vsrc_axes =
    "attribute highp vec4 vertex;\n"
    "attribute mediump vec4 vcolor;\n"
    "uniform mediump mat4 matrix;\n"
    "varying mediump vec4 color;\n"
    "void main()\n"
    "{\n"
    "   color = clamp(vcolor, 0.0, 1.0);\n"
    "   gl_Position = matrix * vertex;\n"
    "}\n";
    if ( !vshader_axes_->compileSourceCode(vsrc_axes) ) {
        std::cout << "GLTrkPainter.initializeGL, " <<
                         "axes vertex shader failed to compile" << std::endl;
        return;
    }

    fshader_axes_ = new QOpenGLShader(QOpenGLShader::Fragment);
    const char* fsrc_axes =
    "varying mediump vec4 color\n;"
    "void main()\n"
    "{\n"
    "   gl_FragColor = color;\n"
    "}\n";
    if ( !fshader_axes_->compileSourceCode(fsrc_axes) ) {
        std::cout << "GLTrkPainter.initializeGL, " <<
                         "axes fragment shader failed to compile" << std::endl;
        return;
    }

    program_axes_ = new QOpenGLShaderProgram;
    program_axes_->addShader(vshader_axes_);
    program_axes_->addShader(fshader_axes_);
    program_axes_->link();

    axes_vertex_attr_ = program_axes_->attributeLocation("vertex");
    axes_color_attr_  = program_axes_->attributeLocation("vcolor");
    axes_matrix_uniform_ = program_axes_->uniformLocation("matrix");
    std::cout << "GLTrkPainter.initializeAxes.axes_vertex_attr = " << 
                                            axes_vertex_attr_ << std::endl;
    std::cout << "GLTrkPainter.initializeAxes.axes_color_attr = " << 
                                            axes_color_attr_ << std::endl;
    std::cout << "GLTrkPainter.initializeAxes.axes_matrix_uniform_ = " << 
                                            axes_matrix_uniform_ << std::endl;

    if ( !vbo_axes_.create() ) {
        std::cout << "GLTrkPainter,initializeAxes, vbo_.create() failed" << std::endl;
    }
}

void
trk::GLTrkPainter::
paintAxes()
{
    std::cout << "GLTrkPainter.paintAxes" << std::endl;
    loadAxesVertexBuffer();
    std::cout << "GLTrkPainter.paintAxes, after loadAxesVertexBuffer" << std::endl;
    ortho_xform_ = makeOrthoXform();
    printAxes();
    program_axes_->bind();
    glVertexAttrib4f(axes_color_attr_, 1.0, 0.0, 0.0, 1.0);
    program_axes_->setUniformValue(axes_matrix_uniform_, ortho_xform_);
    std::cout << "GLTrkPainter.paintAxes ortho_xform set" << std::endl;
    program_axes_->enableAttributeArray(axes_vertex_attr_);
    program_axes_->enableAttributeArray(axes_color_attr_);
    vbo_axes_.bind();
    program_axes_->setAttributeBuffer(axes_vertex_attr_,
                                      GL_FLOAT,
                                      0,
                                      3,
                                      7 * sizeof(GLfloat) );
    program_axes_->setAttributeBuffer(axes_color_attr_,
                                      GL_FLOAT,
                                      3 * sizeof(GLfloat),
                                      4,
                                      7 * sizeof(GLfloat));
    vbo_axes_.release();
    glLineWidth(3.0);

    std::cout << "GLTrkPainter.paintAxes before glDrawArrays" << std::endl;
    glDrawArrays(GL_LINES, 
                 0,
                 6 );
    std::cout << "GLTrkPainter.paintAxes after glDrawArrays" << std::endl;
    glLineWidth(1.0);
    glDrawArrays(GL_LINES, 
                 6 , 
                 6 );
    program_axes_->disableAttributeArray(axes_vertex_attr_);
    program_axes_->disableAttributeArray(axes_color_attr_);
    program_axes_->release();
    std::cout << "GLTrkPainter.paintAxes complete" << std::endl;
}

void 
trk::GLTrkPainter::
loadAxesVertexBuffer()
{
    std::cout << "GLTrkPainter.loadAxesVertexBuffer, axes_vertices_.size = " <<
                           axes_vertices_.size() << std::endl;
    if ( !vbo_axes_.bind() ) {
        std::cout << "GLTrkPainter,initializeAxes, vbo_.bind() failed" << std::endl;
    }
    QVector<QVector4D> colors;
    colors.resize(axes_vertices_.size() );
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
    std::cout << "GLTrkPainter.loadAxesVertexBuffer, colors assigned" << std::endl;
    std::cout.flush();
    QVector<GLfloat> axes_buf;
    axes_buf.resize(axes_vertices_.size() * 7);
    GLfloat* q = axes_buf.data();
    for ( int i = 0; i < axes_vertices_.size(); i++) {
        *q++ =axes_vertices_[i].x();
        *q++ =axes_vertices_[i].y();
        *q++ =axes_vertices_[i].z();
        *q++ = colors[i].x();
        *q++ = colors[i].y();
        *q++ = colors[i].z();
        *q++ = colors[i].w();
    }
    vbo_axes_.allocate(axes_buf.constData(), axes_buf.count() * sizeof(GLfloat) );
    vbo_axes_.release();
}

//############################################################Begin bedLine specific opengl code
//############################################################
void
trk::GLTrkPainter::
initializeBedLines()
{
    std::cout << "GLTrkPainter.initializeBedLines" << std::endl;
    vshader_bed_lines_ = new QOpenGLShader(QOpenGLShader::Vertex);
    const char* vsrc_axes =
    "attribute highp vec4 vertex;\n"
    "uniform mediump mat4 matrix;\n"
    "varying mediump vec4 color;\n"
    "void main()\n"
    "{\n"
    "   color = vec4(0.0, 0.0, 0.0, 1.0);\n"
    "   gl_Position = matrix * vertex;\n"
    "}\n";
    if ( !vshader_bed_lines_->compileSourceCode(vsrc_axes) ) {
        std::cout << "GLTrkPainter.initializeBedLines, " <<
                         "bed_lines vertex shader failed to compile" << std::endl;
        return;
    }
    std::cout << "GLTrkPainter.initializeBedLines, vshader compiled" << std::endl;

    fshader_bed_lines_ = new QOpenGLShader(QOpenGLShader::Fragment);
    const char* fsrc_axes =
    "varying mediump vec4 color\n;"
    "void main()\n"
    "{\n"
    "   gl_FragColor = color;\n"
    "}\n";
    if ( !fshader_bed_lines_->compileSourceCode(fsrc_axes) ) {
        std::cout << "GLTrkPainter.initializeBedLines, " <<
                         "bed lines fragment shader failed to compile" << std::endl;
        return;
    }
    std::cout << "GLTrkPainter.initializeBedLines, fshader compiled" << std::endl;

    program_bed_lines_ = new QOpenGLShaderProgram;
    program_bed_lines_->addShader(vshader_bed_lines_);
    program_bed_lines_->addShader(fshader_bed_lines_);
    program_bed_lines_->link();

    bed_lines_vertex_attr_ = program_bed_lines_->attributeLocation("vertex");
    bed_lines_matrix_uniform_ = program_bed_lines_->uniformLocation("matrix");
    std::cout << "GLTrkPainter.initializeBedLines.bed_lines_vertex_attr = " << 
                                            bed_lines_vertex_attr_ << std::endl;
    std::cout << "GLTrkPainter.initializeAxes.bed_lines_matrix_uniform_ = " << 
                                            bed_lines_matrix_uniform_ << std::endl;

    if ( !vbo_bed_lines_.create() ) {
        std::cout << "GLTrkPainter,initializeBedLines, vbo_.create() failed" << std::endl;
    }
}

void
trk::GLTrkPainter::
paintBedLines()
{
    std::cout << "GLTrkPainter.paintBedLines" << std::endl;
    loadBedLinesVertexBuffer();
    std::cout << "GLTrkPainter.paintBedLines, after oadBedLinesVertexBuffer" << std::endl;
    ortho_xform_ = makeOrthoXform();
    program_bed_lines_->bind();
    program_axes_->setUniformValue(bed_lines_matrix_uniform_, ortho_xform_);
    std::cout << "GLTrkPainter.paintBedLines ortho_xform set" << std::endl;
    program_bed_lines_->enableAttributeArray(bed_lines_vertex_attr_);
    vbo_bed_lines_.bind();
    program_bed_lines_->setAttributeBuffer(bed_lines_vertex_attr_,
                                      GL_FLOAT,
                                      0,
                                      3,
                                      0);
    vbo_bed_lines_.release();
    glLineWidth(3.0);

    std::cout << "GLTrkPainter.paintBedLines before glDrawArrays, edge_vertices_.size = " << 
                                edge_vertices_.size() << std::endl;

    int np = edge_vertices_.size() / 4;
    for ( int k = 0; k < 4; k++ ) {
        glDrawArrays(GL_LINE_STRIP, 
                     k * np,
                     np );
    }
    std::cout << "GLTrkPainter.paintBedLines after glDrawArrays" << std::endl;
    program_bed_lines_->disableAttributeArray(bed_lines_vertex_attr_);
    program_bed_lines_->release();
    std::cout << "GLTrkPainter.paintBedLines complete" << std::endl;
}

void 
trk::GLTrkPainter::
loadBedLinesVertexBuffer()
{
    std::cout << "GLTrkPainter.loadBedLinesVertexBuffer, edge_vertices_.size = " <<
                           edge_vertices_.size() << std::endl;
    if ( !vbo_bed_lines_.bind() ) {
        std::cout << "GLTrkPainter,loadBedLinesVertexBuffer, vbo_.bind() failed" << std::endl;
    }
    Ranges r;
    r.init();
    r.update(edge_vertices_, "edge_vertices_" );
    QVector<GLfloat> edge_vertices_buf;
    edge_vertices_buf.resize(edge_vertices_.size() * 3);
    GLfloat* q = edge_vertices_buf.data();
    for ( int i = 0; i < edge_vertices_.size(); i++) {
        *q++ =edge_vertices_[i].x();
        *q++ =edge_vertices_[i].y();
        *q++ =edge_vertices_[i].z();
    }
    vbo_bed_lines_.allocate(edge_vertices_buf.constData(), 
                            edge_vertices_buf.count() * sizeof(GLfloat) );
    vbo_bed_lines_.release();
}


//#################################################################### Begin general utilities
//####################################################################
void
trk::GLTrkPainter::
newModelXform(QMatrix4x4& xform)
{
    model_xform_ = xform;
    std::cout << "GLTrkPainter.newModelXform, rw_vertex count = " << 
                                  rw_vertices_.size() << std::endl;
    vertices_.resize(rw_vertices_.size() );
    normals_.resize(rw_vertices_.size() );
    for ( int i = 0; i < rw_vertices_.size(); i++ ) {
        vertices_[i] = xform * rw_vertices_[i];
        normals_[i]  = xform * rw_normals_[i];
    }
    axes_vertices_.resize( rw_axes_vertices_.size() );
    for ( int i = 0; i < rw_axes_vertices_.size(); i ++) {
        axes_vertices_[i] = xform * rw_axes_vertices_[i];
    }
    edge_vertices_.resize( rw_edge_vertices_.size() );
    for ( int i = 0; i < rw_edge_vertices_.size(); i ++) {
        edge_vertices_[i] = xform * rw_edge_vertices_[i];
    }
    printAxes();
    //printCenterLine();
    update();
}

QMatrix4x4
trk::GLTrkPainter::
makeOrthoXform()
{
    std::cout << "GLTrkPainter.makeOrthoXform, verices_.size = " <<
                          vertices_.size() << std::endl;
    Ranges r;
    r.init();
    r.update(vertices_, "vertices_" );
    r.update(axes_vertices_, "axes_vertices_" );

    float w = 0.0;
    if ( fabs(r.xmin) > w ) w = fabs(r.xmin);
    if ( fabs(r.xmax) > w ) w = fabs(r.xmax);
    if ( fabs(r.ymin) > w)  w = fabs(r.ymin);
    if ( fabs(r.ymax) > w ) w = fabs(r.ymax);

    float zn = r.zmin - .001;
    float zf = r.zmax + .001;
    float c11 = 1.0 / w;
    float c22 = 1.0 / w;
    float c33 =  1.0/(zf - zn);
    float c34 = -zn/(zf - zn);
    QMatrix4x4 xform;
    xform.setToIdentity();
    xform.setColumn(0, QVector4D(c11, 0.0, 0.0, 0.0) );
    xform.setColumn(1, QVector4D(0.0, c22, 0.0, 0.0) );
    xform.setColumn(2, QVector4D(0.0, 0.0, c33, 0.0) );
    xform.setColumn(3, QVector4D(0.0, 0.0, c34, 1.0) );
    
    printXform(xform, "OrthoXform");
    return xform;
}

void 
trk::GLTrkPainter::
printAxes()
{
    std::cout << "GLTrkPainter.print_axes, " << std::endl;
    std::string tag[3];
    tag[0] = "x"; tag[1] = "y"; tag[2] = "z";
    int n = axes_vertices_.size();
    for ( int i = 0; i < n; i += 2) {
        std::cout << std::setprecision(4);
        for ( int k = 0; k < 2; k++) {
            QVector3D p = ortho_xform_ * axes_vertices_[i+k];
            for (int j = 0; j < 3; j++) std::cout << std::setw(8) << p[j];
        }
        std::cout << std::endl;
    }
}

void 
trk::GLTrkPainter::
printXform(const QMatrix4x4& xform,
           const std::string& tag)
{
    std::cout << "GLTstWidget,printXform, " << tag << std::endl;
    std::cout << std::setprecision(4);
    for ( int i = 0; i < 4; i++) {
        QVector4D r = xform.row(i);
        std::cout << std::setw(15);
        for ( int j = 0; j < 4; j++) std::cout << std::setw(10) << r[j];
        std::cout << std::endl;
    }
}

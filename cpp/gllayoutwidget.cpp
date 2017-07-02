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

#include "gllayoutwidget.h"
#include "glaxesshader.h"
#include "glbedlineshader.h"
#include "gllayoutshader.h"
#include "debugcntl.h"
#include "layoutconfig.h"
#include "layoutgeometry.h"
#include "layoutstate.h"
#include "modelcoordinates.h"
#include "path.h"
#include "ranges.h"
#include "paintlist.h"

#include <QPainter>

#include <math.h>

trk::GLLayoutWidget::
GLLayoutWidget(ModelCoordinates* model_coordinates, LayoutState* layout_state)
{
    std::cout << "GLLayoutWidget.ctor" << std::endl;
    dbg_               = DebugCntl::instance();
    layout_config_     = LayoutConfig::instance();
    layout_geometry_   = LayoutGeometry::instance();
    zone_count_        = layout_config_->track_sensor_count();
    switch_count_      = layout_config_->switch_count();
    shell_count_       = layout_geometry_->shell_count();
    layout_state_      = layout_state;

    colors_.resize(12);
    colors_[WHITE]          = QColor(   0,   0,   0);
    colors_[BLACK]          = QColor( 255, 255, 255);
    colors_[RED]            = QColor( 255,   0,   0);
    colors_[GREEN]          = QColor(   0, 128,   0);
    colors_[BLUE]           = QColor(   0,   0, 255);
    colors_[LIGHTSTEELBLUE] = QColor( 176, 196, 222);
    colors_[LIGHTYELLOW]    = QColor( 255, 255, 224);
    colors_[LIME]           = QColor(   0, 255,   0);
    colors_[LIMEGREEN]      = QColor(  50, 205,  50);
    colors_[ORANGE]         = QColor( 255, 165,   0);
    colors_[DARKGREEN]      = QColor(   0, 100,   0);

    default_zone_clr_       = colors_[LIGHTSTEELBLUE];
    active_zone_clr_        = colors_[LIMEGREEN];
    exit_zone_clr_          = colors_[LIGHTYELLOW];
    current_path_clr_       = colors_[LIME];
    interdit_clr_           = colors_[RED];
    default_switch_clrs_.push_back(current_path_clr_);
    default_switch_clrs_.push_back(default_zone_clr_);
    queued_switch_clrs_.push_back(current_path_clr_);
    queued_switch_clrs_.push_back(colors_[ORANGE]);

    if ( dbg_->check(4) ) {
        *dbg_ << "GLLayoutWidget.ctor, default_switch_clrs = " << 
                        qcolor_to_s(default_switch_clrs_[0]) << ", " <<
                        qcolor_to_s(default_switch_clrs_[1]) << trk::endl;
        *dbg_ << "GLLayoutWidget.ctor, queued_switch_clrs = " << 
                        qcolor_to_s(queued_switch_clrs_[0]) << ", " <<
                        qcolor_to_s(queued_switch_clrs_[1]) << trk::endl;
        *dbg_ << "GLLayoutWidget.ctor, default_zone_clr = " << 
                        qcolor_to_s(default_zone_clr_) << trk::endl;
        *dbg_ << "GLLayoutWidget.ctor, active_zone_clr = " << 
                        qcolor_to_s(active_zone_clr_) << trk::endl;
        *dbg_ << "GLLayoutWidget.ctor, exit_zone_clr = " << 
                        qcolor_to_s(exit_zone_clr_) << trk::endl;
        *dbg_ << "GLLayoutWidget.ctor, current_path_clr = " << 
                        qcolor_to_s(current_path_clr_) << trk::endl;
    }

    paint_list_        = new PaintList(zone_count_, switch_count_, shell_count_);

    model_coordinates_ = model_coordinates;
    connect(layout_state_,      SIGNAL(layoutstate_initialized() ),
            this,               SLOT  (layoutstate_initialized() ) );


    model_xform_       = model_coordinates_->modelXform();
    layout_shader_     = new GLLayoutShader(paint_list_, default_zone_clr_);
    axes_shader_       = new GLAxesShader();
    bedline_shader_    = new GLBedLineShader();
    std::cout << "GLLayoutWidget.ctor, complete" << std::endl;
    //setMinimumSize(500, 400);
    //resize(1500,1200);
}

trk::GLLayoutWidget::
~GLLayoutWidget()
{
}

void
trk::GLLayoutWidget::
layoutstate_initialized()
{
    std::cout << "GLLayoutWidget.layoutstate_initialized" << std:: endl;
    connect(model_coordinates_, SIGNAL(model_coordinates_changed(QMatrix4x4&) ),
            this,               SLOT(model_coordinates_changed(QMatrix4x4&) ) );
    connect(layout_state_,      SIGNAL(zone_busy(int) ),
            this,               SLOT  (zone_busy(int) ) ) ;
    connect(layout_state_,      SIGNAL(zone_idle(int) ),
            this,               SLOT  (zone_idle(int) ) ) ;
    connect(layout_state_,      SIGNAL(new_path() ),
            this,               SLOT  (new_path() ) ) ;
    connect(layout_state_,      SIGNAL(switch_queued(int, bool) ),
            this,               SLOT  (switch_queued(int, bool) ) ) ;
    connect(layout_state_,      SIGNAL(switch_thrown(int, bool) ),
            this,               SLOT  (switch_thrown(int, bool) ) ) ;
    std::vector<int> active_zone_indexes = layout_state_->active_zone_indexes();
    paint_list_->set_default_color( colors_[LIGHTSTEELBLUE] );
    int azi_count = active_zone_indexes.size();
    std::cout << "GLLayoutWidget.layoutstate_initialized, azi_count = " << 
                                                  azi_count << std::endl;
    for ( int i = 0; i < azi_count; i++ ) {
        paint_list_->color_by_zone( active_zone_indexes[i], active_zone_clr_ );
    }
    update();
}

void
trk::GLLayoutWidget::
initializeGL()
{
    initializeOpenGLFunctions();
    layout_shader_->initialize();
    axes_shader_->initialize();
    bedline_shader_->initialize();
    model_coordinates_changed(model_xform_);
    if ( dbg_->check(4)) {
        *dbg_ << print_xform(model_xform_, "InitializeGL, model_xform");
        *dbg_ << print_xform(ortho_xform_, "InitializeGL, ortho_xform");
    }
    
}

void
trk::GLLayoutWidget::paintGL()
{
    if ( dbg_->check(4)) {
        *dbg_ << print_xform(ortho_xform_, "paintGL" );
        *dbg_ << "GLLayoutWidget.paintGL, zone_count_ = " << zone_count_ << trk::endl;
        *dbg_ << "GLLayoutWidget.paintGL, switch_count_ = " << switch_count_ << trk::endl;
    }
    QPainter painter;
    painter.begin(this);
    painter.beginNativePainting();
  
    background_.setRgb(245,245,245);
    glClearColor(background_.redF(),
                 background_.greenF(),
                 background_.blueF(),
                 true);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
/*
    glFrontFace(GL_CW);
    glCullFace(GL_FRONT);
    glEnable(GL_CULL_FACE);
*/
    glEnable(GL_DEPTH_TEST);
    glDepthRangef(0.0,1.0);
    glDepthMask(true);
    glDepthFunc(GL_LESS);

    //int n = paint_list_->size();
    for (int i = 0; i < zone_count_; i++) {
        PaintItem item = paint_list_->item_by_zone(i);
        layout_shader_->paint(item.vtx_i0, item.vtx_count, item.vtx_color, ortho_xform_);
    }
    for ( int i = 0; i < switch_count_; i++) {
        std::pair<int, int> shells = get_shell_indexes(i);
        PaintItem item = paint_list_->item_by_switch(i, shells.first);
        layout_shader_->paint(item.vtx_i0, item.vtx_count, item.vtx_color, ortho_xform_);
        item = paint_list_->item_by_switch(i, shells.second);
        layout_shader_->paint(item.vtx_i0, item.vtx_count, item.vtx_color, ortho_xform_);
    }
    axes_shader_->paint(ortho_xform_);
    bedline_shader_->paint(ortho_xform_);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    painter.endNativePainting();
    painter.end();
}

void
trk::GLLayoutWidget::
resizeGL(int w, int h)
{
    std::cout << "GLLayoutWidget.resizeGL, w = " << w << ",h = " << h << std::endl;
}

void
trk::GLLayoutWidget::
model_coordinates_changed(QMatrix4x4& xform)
{
    model_xform_ = xform;
    Ranges r;
    r.init();
    layout_shader_->update_vertexes(model_xform_ , r); // updates vertexes and r
    axes_shader_->update_vertexes(model_xform_ , r); // updates vertexes and r
    bedline_shader_->update_vertexes(model_xform_ , r); // updates vertexes and r

    if ( dbg_->check(4)) {
        *dbg_ << r.print("model_coordinates_changed");
        std::cout << r.print("model_coordinates_changed, layout") << std::endl;
        std::cout << r.print("model_coordinates_changed, axes") << std::endl;
        std::cout << r.print("model_coordinates_changed-bedline") << std::endl;
    }
    ortho_xform_ = make_ortho_xform(r);
    update();
}

void
trk::GLLayoutWidget::
zone_busy(int zone_index)
{
    std::cout << "GLLayoutWidget.zone_busy, zone_index = " << zone_index << std::endl;
    paint_list_->color_by_zone( zone_index, active_zone_clr_ );
    update();
}

void
trk::GLLayoutWidget::
zone_idle(int zone_index)
{
    Path* path = layout_state_->current_path();
    if ( path->has_zone(zone_index) ) {
        if ( layout_state_->in_transition() ) {
            paint_list_->color_by_zone(zone_index, default_zone_clr_);
        } else {
            paint_list_->color_by_zone(zone_index, current_path_clr_);
        }
        update();
    }
}

void
trk::GLLayoutWidget::
new_path()
{
    Path* current_path = layout_state_->current_path();

    if ( dbg_->check(2) ) {
        *dbg_ << "GLLayoutWidget.new_path, = " << current_path->path_name() << trk::endl;
    }
    paint_list_->set_default_color( default_zone_clr_ );

    std::vector<int> zi = current_path->zone_indexes();
    int nz = zi.size();
    for ( int i = 0; i < nz; i++) {
        paint_list_->color_by_zone(zi[i], current_path_clr_ );
    }
    std::vector<int> swi = current_path->switch_indexes();
    int ns = swi.size();
    for ( int i = 0; i < ns; i++ ) {
        int switch_index = swi[i];
        std::pair<int, int> shells = get_shell_indexes(switch_index);
        paint_list_->color_by_switch(switch_index, shells.first,  default_switch_clrs_[0]);
        paint_list_->color_by_switch(switch_index, shells.second, default_switch_clrs_[1]);
    }

    Path* exit_path = layout_state_->exit_path();
    int exit_swi    = exit_path->begin_switch_index();
    std::vector<int> ezi = exit_path->zone_indexes();
    paint_list_->color_by_zone( ezi[0], exit_zone_clr_);
    std::pair<int, int> shells = get_shell_indexes(exit_swi);
    paint_list_->color_by_switch(exit_swi,shells.first  ,current_path_clr_);
    paint_list_->color_by_switch(exit_swi,shells.second ,exit_zone_clr_);
}

void
trk::GLLayoutWidget::
set_path_colors(Path* path, const QColor& path_color)
{
    std::vector<int> zi = path->zone_indexes();
    int nz = zi.size();
    for ( int i = 0; i < nz; i++) {
        paint_list_->color_by_zone(zi[i], path_color );
    }
    std::vector<int> swi = path->switch_indexes();
    int ns = swi.size();
    for ( int i = 0; i < ns; i++ ) {
        int switch_index = swi[i];
        std::pair<int, int> shells = get_shell_indexes(switch_index);
        paint_list_->color_by_switch(switch_index, shells.first,  default_switch_clrs_[0]);
        paint_list_->color_by_switch(switch_index, shells.second, default_switch_clrs_[1]);
    }
}

void
trk::GLLayoutWidget::
set_exitpath_colors(const QColor& path_color)
{
    Path* exit_path = layout_state_->exit_path();
    int exit_switch_index = layout_state_->exit_switch_index();
    std::vector<int> zi = exit_path->zone_indexes();
    paint_list_->color_by_zone( zi[0], current_path_clr_);
    std::pair<int, int> shells = get_shell_indexes(exit_switch_index);
    paint_list_->color_by_switch(exit_switch_index,shells.first  ,current_path_clr_);
    paint_list_->color_by_switch(exit_switch_index,shells.second ,path_color);
}

void
trk::GLLayoutWidget::
switch_queued(int switch_index, bool is_exit)
{
    if ( dbg_->check(2) ) {
        std::ostringstream ss;
        ss << "GLLayoutWidget.switch_queued, switch_index = " << switch_index 
                      << ", is_exit = " << std::boolalpha << is_exit << std::endl;
        *dbg_ << ss.str();
    }

    std::pair<int, int> shells = get_shell_indexes(switch_index);
    if ( is_exit ) {
        paint_list_->color_by_switch(switch_index, shells.first, current_path_clr_);
        paint_list_->color_by_switch(switch_index, shells.second, colors_[ORANGE]);
        Path* exit_path = layout_state_->exit_path();
        paint_list_->color_by_zone(exit_path->first_zone(), colors_[ORANGE]); 
    } else {
        paint_list_->color_by_switch(switch_index, shells.first, queued_switch_clrs_[0]);
        paint_list_->color_by_switch(switch_index, shells.second, queued_switch_clrs_[1]);
    }
    update();
}

void
trk::GLLayoutWidget::
switch_thrown(int switch_index, bool is_exit)
{
    if ( dbg_->check(2) ) {
        std::ostringstream ss;
        ss << "GLLayoutWidget.switch_thrown, switch_index = " << switch_index 
                      << ", is_exit = " << std::boolalpha << is_exit << std::endl;
        *dbg_ << ss.str();
    }

    std::pair<int, int> shells = get_shell_indexes(switch_index);
    if ( is_exit ) {
        Path* exit_path = layout_state_->exit_path();
        std::vector<int> zi = exit_path->zone_indexes();
        paint_list_->color_by_zone( zi[0], current_path_clr_);
        paint_list_->color_by_switch(switch_index,shells.first  ,current_path_clr_);
        paint_list_->color_by_switch(switch_index,shells.second ,interdit_clr_);
    } else {
        paint_list_->color_by_switch(switch_index, shells.first, default_switch_clrs_[0]);
        paint_list_->color_by_switch(switch_index, shells.second, default_switch_clrs_[1]);
    }
    update();
}

std::pair<int, int>
trk::GLLayoutWidget::
get_shell_indexes(int switch_index)
{
    int shell0;
    int shell1;
    SW_DIRECTION current_state = layout_state_->switch_state(switch_index);
    if ( current_state == THRU) {
        shell0 = THRUB;
        shell1 = THRUC;
    } else {
        shell0 = OUTC;
        shell1 = OUTB;
    }
    std::pair<int, int> shells(shell0, shell1);
    return shells;
}

QMatrix4x4
trk::GLLayoutWidget::
make_ortho_xform(Ranges& r)
{
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
    
    //printXform(xform, "OrthoXform");
    return xform;
}

std::string
trk::GLLayoutWidget::
print_xform(const QMatrix4x4& xform,
            const std::string& tag)
{
    std::ostringstream ss;
    ss << std::endl << "   " << tag << std::endl;
    ss << std::setprecision(6);
    for ( int i = 0; i < 4; i++) {
        QVector4D r = xform.row(i);
        ss << std::setw(17);
        for ( int j = 0; j < 4; j++) ss << std::setw(12) << r[j];
        ss << std::endl;
    }
    return ss.str();
}

std::string 
trk::GLLayoutWidget::
qcolor_to_s(const QColor& color)
{
    int icolor = 99;
    int ncolor = colors_.size();
    for ( int i = 0; i < ncolor; i++ ) {
        if ( color == colors_[i] ) {
            icolor = i;
            break;
        }
    }
    return icolor_to_s(icolor);
}

std::string
trk::GLLayoutWidget::
icolor_to_s(int i)
{
    if      ( i == WHITE)           return "WHITE";
    else if ( i == BLACK )          return "BLACK";
    else if ( i == LIGHTSTEELBLUE ) return "LIGHTSTEELBLUE";
    else if ( i == RED )            return "RED";
    else if ( i == GREEN )          return "GREEN";
    else if ( i == BLUE )           return "BLUE";
    else if ( i == LIGHTYELLOW )    return "LIGHTYELLOW";
    else if ( i == YELLOW  )        return "YELLOW";
    else if ( i == LIME )           return "LIME";
    else if ( i == LIMEGREEN )      return "LIMEGREEN";
    else if ( i == ORANGE )         return "ORANGE";
    else if ( i == DARKGREEN )      return "DARKGREEEN";
    else                            return "UNKNOWN";
}

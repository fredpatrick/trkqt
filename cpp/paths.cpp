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

#include <iostream>
#include <sstream>

#include "paths.h"
#include "path.h"
#include "pathnode.h"
#include "debugcntl.h"
#include "filestore.h"

trk::Paths* trk::Paths::instance_ = 0;

trk::Paths*
trk::Paths::
instance()
{
    if ( !instance_ ) {
        instance_ = new Paths();
    }
    return instance_;
}

trk::Paths::
Paths()
{
    dbg_ = DebugCntl::instance();
    if ( dbg_->check(4) ) {
        *dbg_ << "Paths.ctor begin\n";
    }

    FileStore* fs = FileStore::instance();
    std::ifstream pthstrm(fs->pthfil().c_str() );
    Path*         current_path = 0;
    std::string   tag;
    int           path_index = 0;
    while ( true )  {
        pthstrm >> tag;
        if ( pthstrm.eof() ) break;

        if ( tag == "path" ) {
            current_path = new Path(pthstrm);
            paths_[current_path->path_name()] = current_path;
            if ( dbg_->check(5) ) {
                *dbg_ << "Paths.ctor, new path,                  name = " << 
                                  current_path->path_name() << "\n";
            }
            path_index += 1;
        } else if ( tag == "switch" ) {
            SwitchNode* swn = current_path->add_switch_node(pthstrm);

        } else if ( tag == "zone"   ) {
            ZoneNode* zn = current_path->add_zone_node(pthstrm);
        } else {
            std::cout << "Paths.ctor, error reading path definition file, tag = " 
                               << tag << std::endl;
        }
    }

    if ( dbg_->check(5) ) {
        typedef std::map<std::string, Path*>::iterator CI;
        for ( CI p = paths_.begin(); p != paths_.end(); p++) {
            std::ostringstream ss;
            ss << *p->second << std::endl;
            *dbg_ << ss.str();
        }
    }

    typedef std::map<std::string, Path*>::iterator CI;
    for ( CI p = paths_.begin(); p != paths_.end(); p++) {
        p->second->assign_node_indexes();
    }


}

trk::Paths::
~Paths()
{
}

std::vector<std::string> 
trk::Paths::
path_names()
{
    typedef std::map<std::string, Path*>::const_iterator CI;
    std::vector<std::string> nms;
    for ( CI p = paths_.begin(); p != paths_.end(); p++ ) {
        nms.push_back(p->second->path_name() ) ;
    }
    return nms;
}

trk::Path*
trk::Paths::
path(const std::string& path_name)
{
    return paths_[path_name];
}

trk::Path*
trk::Paths::
find_startup_path(const std::string& current_zonename,
                  const std::string& startup_zonename)
{
    if ( dbg_->check(3) ) {
        *dbg_ << "Paths.find_starrup_path, current_zonename = " <<
                   current_zonename << ", startup_zonename = " << startup_zonename << "\n";
    }
    typedef std::map<std::string, Path*>::iterator CI;
    for ( CI p = paths_.begin(); p != paths_.end(); p++) {
        if ( p->second->is_path(current_zonename, startup_zonename) ) {
            return p->second;
        }
    }
    std::cout << "Paths.find_startup_path, no paths found" << std::endl;
    return  0;
}

std::string
trk::Paths::
exit_path_zonename(const std::string& path_name)
{
    std::string exit_path_name = paths_[path_name]->exit_path_name();
    return paths_[exit_path_name]->first_zonename();
}


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

#include "exitswitchstate.h"

trk::ExitSwitchState::
ExitSwitchState(SocketClient*       socket_client,
                const std::string&  switch_name,
                int                 switch_index,
                const std::string&  exit_tag))
{
    socket_client_ = socket_client;
    switch_name_   = switch_name;
    sw_num_        = switch_index;
    if (exit_tag == "C" ) sw_direc_ = OUT;
    else                  sw_direc_ = THRU;

    queued_             = false;
    available_          = false;
    switch_cmnd_sent_   = false;
    state_              = CONTINUE;
}

trk::ExitSwitchState::
~ExitSwitchState()
{
}

void
trk::ExitSwitchState::
set_queued()
{
    if ( state_ == EXIT ) return;
    queued_ = true;
    if ( queued_ && available_ ) throw_exit_switch();
}

void 
trk::ExitSwitchState::
set_available(bool available)
{
    if ( state_ == EXIT ) return;
    available_ = available;
    if ( queued_ && available_ ) throw_exit_switch();
}

void
trk::ExitSwitchState::
throw_exit_switch()
{

    CmdPacket* cmd_packet = new CmdPacket("set", "switch", 1);
    std::pair<int, int> item;
    item.first = sw_num_;
    item.second = sw_direc_;
    cmd_packet->item(0, item);
    cmd_packet->write(socket_client_);
    switch_cmd_sent_ = true;
}

void
trk::ExitSwitchState::
switch_thrown()
{
    state_ = EXIT;
}

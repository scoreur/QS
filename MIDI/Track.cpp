/*
  Copyright (C) 2010 Karol Krizka <kkrizka@gmail.com>
  
  URL: http://www.krizka.net/projects/libmidi/
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

  $Id$
*/

#include "Track.h"

#include "ChannelControllerEvent.h"
#include "MetaGenericEvent.h"
#include "MetaTextEvent.h"
#include "MetaNumberEvent.h"
#include "Defines.h"

#include <iostream>
#include <iomanip>
using namespace std;

namespace MIDI
{
  Track::Track()
  { }
  
  Track::Track(byte* data,dword size)
    : _data(data),_size(size),_pos(0)
  {
    byte lastMIDICommand;
    int lastChannel;
    
    while(_pos<_size)
      {
	dword deltaTime=readNextVariableLength();
	byte eventType=readNextByte();
	
	switch(eventType)
	  {
	  case 0xFF: //Meta event
	    {
	      byte type=readNextByte();
	      dword length=readNextVariableLength();
	      byte metaData[length];
	      for(int i=0;i<length;i++)
		{
		  metaData[i]=readNextByte();
		}
	      
	      MetaEvent *event;
	      switch(type)
		{
		case MIDI_METAEVENT_TEXT:
		case MIDI_METAEVENT_COPYRIGHT:
		case MIDI_METAEVENT_TRACKNAME:
		case MIDI_METAEVENT_MAKER:
		  event=new MetaTextEvent(deltaTime,type,metaData,length);
		  break;
		case MIDI_METAEVENT_SETTEMPO:
		  event=new MetaNumberEvent(deltaTime,type,metaData,length);
		  break;
		default:
		  event=new MetaGenericEvent(deltaTime,type,metaData,length);
		  break;
		}
	      _events.push_back(event);
	    }
	    break;
	  case 0xF0: // System Exclusive Events
	  case 0xF7:
	    {
	      dword length=readNextVariableLength();
	      byte sysData[length];
	      for(int i=0;i<length;i++)
		{
		  sysData[i]=readNextByte();
		}
            //#warning "System Exclusive events are unimplemented"
	      // IGNORE
	    }
	    break;
	  default:
	    { // Everything else is a channel event
	      byte command;
	      int channel;
	      byte param1;
	      byte param2;
	      
	      if((eventType>>7)==1) // MSB of 1 indicates that the first 4 bits are the channel event type
		{
		  /*
		   * 4 bits:event type
		   * 4 bits:channel
		   * 1 byte:param1
		   * 1 byte:param2
		   */
		  command=(0xF0&eventType)>>4;
		  channel=(int)(0x0F & eventType);
		  param1=readNextByte();
		  param2=readNextByte();
		}
	      else // This is a running status, since MSB==0
		{
		  /*
		   * Event type/channel are from previous event
		   * 1 byte:param1
		   * 1 byte:param2
		   */
		  command=lastMIDICommand;
		  channel=lastChannel;
		  param1=eventType;
		  param2=readNextByte();
		}
	      
	      // Store the event
	      ChannelEvent *event=0;
	      if(command==MIDI_CHEVENT_CONTROLLER)
		event=new ChannelControllerEvent(deltaTime,channel,param1,param2);
	      else
		event=new ChannelEvent(deltaTime,command,channel,param1,param2);
	      
	      _events.push_back(event);
	      
	      lastMIDICommand=command;
	      lastChannel=channel;
	    }
	    break;
	  }
      }
  }
  
  unsigned int Track::numEvents()
  {
    return _events.size();
  }
  
  Event* Track::event(unsigned int id)
  {
    return _events[id];
  }
  
  void Track::addEvent(Event* event)
  {
    _events.push_back(event);
  }
  
  dword Track::readNextVariableLength()
  {
    dword result=0;
    bool flag=0;
    do
      {
	byte i=_data[_pos];
	dword contribution=(dword)(0x7F & i); // Ignore first bit, it is a flag
	// Shift down to make space for new bit
	result=(result<<7);
	result+=contribution;
	
	flag=(0x80 & i); // The first bit is 0 if this is the final contribution
	
	_pos++;
      } while(flag);
    
    return result;
  }
  
  byte Track::readNextByte()
  {
    byte result=_data[_pos];
    _pos++;
    return result;
  }
  
  DataBuffer Track::data()
  {
    DataBuffer data(1000000);
    
    byte lastCommand=MIDI_CHEVENT_INVALID;
    
    for(int i=0;i<_events.size();i++)
      {
	DataBuffer eventData;
	if(_events[i]->type()==MIDI_CHEVENT)
	  {
	    ChannelEvent *chevent=(ChannelEvent*)_events[i];
	    if(chevent->command()==lastCommand)
	      eventData=chevent->data(true);
	    else
	      eventData=chevent->data(false);
	    
	    lastCommand=chevent->command();
	  }
	else
	  eventData=_events[i]->data();
	
	data.write(eventData);
      }
    
    return data;
  }
}

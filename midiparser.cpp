#include "midiparser.h"
#include "MIDI.h"
#include <QDebug>

MidiParser::MidiParser()
{
}

void MidiParser::test(std::string fileName){
    MIDI::File mid(fileName.c_str());
    mid.open();

     fstream tmp;
     tmp.open(fileName.append(".txt"), std::ios::out);

   MIDI::Header* header=mid.header();

   tmp << "Header" <<'\n';
   tmp << "\tFormat Type: " << header->formatType() <<'\n';
   tmp << "\tNumber of Tracks: " << header->numTracks() <<'\n';

   if(header->framesPerSecond())
     {
       tmp << "\tFrames Per Second: " << header->framesPerSecond() <<'\n';
       tmp << "\tTicks Per Frame: " << header->ticksPerFrame() <<'\n';
     }
   else
     tmp << "\tTicks Per Beat: " << header->ticksPerBeat() <<'\n';

   for(int i=0;i<mid.numTracks();i++)
     {
       MIDI::Track* track=mid.track(i);
       tmp << "Track #" << i+1 <<'\n';

       for(int j=0;j<track->numEvents();j++)
     {
       MIDI::ChannelEvent* event = (MIDI::ChannelEvent*)track->event(j);
       tmp<< event->type() << event->command() << event->param1() << event->param2();
    qDebug()<<"test!";
     }
     }
     tmp.close();
}

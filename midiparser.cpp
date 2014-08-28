#include "midiparser.h"
#include "MIDI.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonDocument>
#include <map>
using namespace std;

MidiParser::MidiParser()
{
}

void MidiParser::test(std::string fileName){
    MIDI::File mid(fileName.c_str());
    mid.open();

    fstream tmp;
    tmp.open(fileName.append(".txt"), std::ios::out);

    MIDI::Header* header=mid.header();

//    tmp << "Header" <<'\n';
//    tmp << "\tFormat Type: " << header->formatType() <<'\n';
//    tmp << "\tNumber of Tracks: " << header->numTracks() <<'\n';
    tmp << header->formatType() << ' ' << header->numTracks() << endl;
    if(header->framesPerSecond())
    {
        tmp << "\tFrames Per Second: " << header->framesPerSecond() <<'\n';
        tmp << "\tTicks Per Frame: " << header->ticksPerFrame() <<'\n';
    }
    else
        tmp << "\tTicks Per Beat: " << header->ticksPerBeat() <<'\n';

    QJsonArray cxq;

    for(int i=0;i<mid.numTracks();i++)
    {
        map<int, pair<int, int> >g;
        QJsonArray wyj;
        MIDI::Track* track=mid.track(i);
        tmp << "Track #" << i+1 <<'\n';
        int tim = 0;
        for(int j=0;j<track->numEvents();j++)
        {
            MIDI::ChannelEvent* event = (MIDI::ChannelEvent*)track->event(j);
            tim += event->deltaTime();
            if ((int)event->command() == 8 || (int)event->command() == 9) {
                if ((int)event->command() == 8) {
                    qDebug() << (int)event->param1() << endl;
                    if (g.count((int)event->param1())) {
                        QJsonArray wkn;
                        wkn.append(g[(int)event->param1()]);
                        wkn.append(tim);
                        wkn.append((int)event->param1());
                        wyj.append(wkn);
                        g.erase((int)event->param1());
                    }
                } else {
                    g[(int)event->param1()] = tim;
                }
            }
            tmp << (int)tim << ' ' << (int)event->type() << ' ' << (int)event->command() << ' ' << (int)event->param1() << ' ' << (int)event->param2() << endl;
        }
        cxq.append(wyj);
    }
    qDebug() << cxq << endl;
    tmp.close();
}

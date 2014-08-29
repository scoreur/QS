/// @file MIDI.h
///
/// @author WYJ
///
///////////////////

#ifndef __MIDI_FILE_
#define __MIDI_FILE_
#include <QDebug>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#define debug(XXX) //qDebug()<<XXX;
using std::vector;
using std::string;


quint32 B2bigEndian(const quint8 *length, quint16 size = 4){
    quint32 tmp = (quint32)length[0];
    for(int i=1;i<size;++i){
        tmp = (tmp<<8)+(quint32)length[i];
    }
    return tmp;
}

quint32 B2varyNum(char* &length){
    quint32 tmp = 0x7F & (quint32)*(length);
    while(0x80 & (quint32)*length)
        tmp = (tmp << 7) + (0x7F & (quint32)*(++length));
    ++length;
    return tmp;
}

struct Chunk{
    char type[4];
    quint8 length[4];
    char *content;
    
    Chunk():type{0,0,0,0},length{0,0,0,0},
    content(0){}                //for writing
    Chunk(char *in){            //for reading
        memcpy(type, in, 4);
        memcpy(length, (in+4), 4);
        content = in+8;         //shallow copy
    }
    quint32 len() const{            //big endian
        return B2bigEndian(length);
    }
    
};

struct ControlChunk{
    quint32 deltaTime;
    char type;// <=0x7F, or 0xF0/0xF7
    quint32 length;
    char *content;
    ControlChunk(quint32 delta, char* &in):
    deltaTime(delta), type(*(in++)),
    content(0){
        
        length = B2varyNum(in);
        if(length>0){
            content = new char[length];
            memcpy(content, in, length);
            in += length;
        }
    }
    ~ControlChunk(){
        if(length > 0)
            delete[]content;
    }

};

struct MidiEvent{
    quint32 deltaTime;
    char type;
    quint8 param1, param2;
    MidiEvent(quint32 delta, char *&in):
    deltaTime(delta){
        quint8 tmp = *(quint8*)in;
        if(0x80 & tmp)
            type = *(in++);
        else
            type = 0;
        param1 = *(quint8*)(in++);
        if(tmp>>4 == 0xC || tmp>>4 == 0xD)
            param2 = 0;
        else
            param2 = *(quint8*)(in++);
    }
    ~MidiEvent(){}
    };
/// @brief Midi Header chunck
class MidiHeader:public Chunk{
public:
    MidiHeader(){
        strcpy(type, "MThd");
        
    }
    MidiHeader(char *in):Chunk(in){
        debug("Header created!")
        fmt = (Format)B2bigEndian((quint8*)content,2);
        nTracks = B2bigEndian((quint8*)content+2,2);
        division = B2bigEndian((quint8*)content+4,2);
    }
    ~MidiHeader(){}
    
    enum Format{
        single = 0,
        multi_sync,
        multi_independent
    } fmt;
    friend class MidiFileIn;
private:
    quint32 nTracks;
    quint32 division;
    
    
};

/// @brief Midi Track Chunk
class MidiTrack:public Chunk{
public:
    MidiTrack(){
        strcpy(type, "MTrk");

    }
    MidiTrack(char *in):Chunk(in){
        debug("Track created!")
        in += 8;
        char *beg = in;
        quint32 l = len();
        while(in - beg < l){
            quint32 delta = B2varyNum(in);
            switch(*(quint8*)in){
                case 0xFF:
                    ++in;
                    cEvts.push_back(new ControlChunk(delta, in));
                    break;
                case 0xF0: case 0xF7:
                    cEvts.push_back(new ControlChunk(delta, in));
                    break;
                default:
                    mEvts.push_back(new MidiEvent(delta, in));
                    break;
            }
        }
        //debug("Track: "<< mEvts.size())
    }
    ~MidiTrack(){
        for(vector<MidiEvent*>::iterator iter=mEvts.begin();
            iter != mEvts.end(); ++iter)
            delete *iter;
        for(vector<ControlChunk*>::iterator iter=cEvts.begin();
             iter != cEvts.end(); ++iter)
            delete *iter;
        debug("delete Tracks!")
    }
    
private:
    vector<MidiEvent*> mEvts;//midi events
    vector<ControlChunk*> cEvts;//control events
    
};

/// @brief Raw Midi data
class RawFile{
public:
    RawFile() : buffer(0), length(0){

    }

    RawFile(char *fileName):RawFile(){
        std::ifstream in(fileName);
        if(!in){
            return;
        }
        in.seekg(0, std::ios::beg);
        quint32 beg = in.tellg();
        in.seekg(0, std::ios::end);
        length = in.tellg();
        length -= beg;
        buffer = new char[length];
        in.seekg(0, std::ios::beg);
        in.read(buffer, length);
    }
    RawFile(const RawFile& _raw):length(_raw.length){
        buffer = new char[length];
        memcpy(buffer, _raw.buffer, length);
    }
    ~RawFile(){
        if(length>0)
            delete[]buffer;
        debug("delete raw file!")
        
    }
    quint32 len() const{
        return length;
    }
    char *operator()(quint32 i=0)const{
        return buffer+i;
    }
    void write(char *fileName){
        std::ofstream out(fileName);
        out.write(buffer, length);
    }

private:
    char *buffer;
    quint32 length;
    
};

/// @brief Input Midi File
class MidiFileIn{
public:

    MidiFileIn(char *fileName):
    raw(fileName){
        construct();
    }
    MidiFileIn(const RawFile &_raw):raw(_raw){
        construct();
    }
    ~MidiFileIn(){
        delete Header;
        for(vector<MidiTrack*>::iterator iter = tracks.begin();
            iter != tracks.end(); ++iter)
            delete *iter;
        debug("delete chunks!")
    }
    
    MidiHeader & header() const{
        return *Header;
    }
    RawFile raw;
private:
    void construct(){
        quint32 ptr = 0;
        Header = new MidiHeader(raw(ptr));
        ptr += (Header->len()+8);
        
        for(quint32 i=0;i<Header->nTracks;++i){
            tracks.push_back(new MidiTrack(raw(ptr)));
            ptr += (tracks[i]->len()+8);
        }
        
        
    }
    
    MidiHeader *Header;
    vector<MidiTrack*>tracks;

};

class MIDIFileOut{
public:
    MIDIFileOut(quint32 _nTracks){

    }
    ~MIDIFileOut(){}

    RawFile toRawFile(){

        RawFile tmp;
        return tmp;
    }

private:
    MidiHeader *Header;
    vector<MidiTrack*>tracks;


};

//test
/*
int main(int argc, char *argv[]){
    
    if(argc==1)
        return 1;
    MidiFileIn midiIn(argv[1]);
    debug("raw file length: "<<midiIn.raw.len())
    debug("header length: "<<(midiIn.header().len()))
    return 0;
}
*/

#endif /* __MIDI_ */

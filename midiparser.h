#ifndef MIDIPARSER_H
#define MIDIPARSER_H
#include <string>
#include <fstream>
using namespace std;
/**
 * @brief The MidiParser class, for parsing midi into json array
 */
class MidiParser
{
public:
    MidiParser();
    static void test(std::string fileName);
};

#endif // MIDIPARSER_H

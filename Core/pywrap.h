#ifndef PYWRAP_H
#define PYWRAP_H

/**
 * @brief The Pywrap class, for wrapping python module
 */
class Pywrap
{
public:
    Pywrap();
    ~Pywrap();

    void load(char ** &argv);
};


#endif // PYWRAP_H

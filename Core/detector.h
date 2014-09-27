#ifndef DETECTOR_H
#define DETECTOR_H
#include <iostream>
#include <cstring>
#include <vector>
#include "svm.h"
#include "Wave/wavfile.h"

class Detector{
public:
    Detector();
    ~Detector(){}

    //svm part
    static struct svm_node *x;
    static int max_nr_attr;
    static struct svm_model* model;
    static char *line;
    static int max_line_len;
    static int predict_probability;


    static void predict(FILE *input, FILE *output);
    static void loadNodeFromFile(FILE *input);
    static char * readline(FILE *input);
    static void exit_input_error(int line_num);
    static void test();
    static void analysis(std::string insrc, std::string modelsrc, std::string outsrc);

    //onset part
    static void maxNormalize(std::vector<double> &data);
    static void preprocess(std::vector<double> &data);
    static std::vector<qreal> preprocess(const WavFile &wavin, quint32 len);
    static double Hanning(int i,int N);
    static double winsecs;
    static std::vector<qreal> compute_f(const char *inputFileName, int len);
    static std::vector<qreal> compute_f(const WavFile &wavin, int len);
    static std::vector<qreal> compute_f(std::vector<qreal>&signal, int h);
    static void compute_f(const char *inputFileName, const char *outputFileName, int len);
    static void compute_onset(const WavFile &wavin, std::vector<quint8>&pitch, std::vector<quint16>&dura);
    static void computeOnset(const WavFile &wavin);
    static qreal quantize(std::vector<quint32>data);


};


#endif // DETECTOR_H

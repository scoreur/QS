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
    static char * readline(FILE *input);
    static void exit_input_error(int line_num);
    static void test();
    static void analysis(std::string insrc, std::string modelsrc, std::string outsrc);

    //onset part
    static void maxNormalize(std::vector<double> &data);
    static void preprocess(std::vector<double> &data);
    static double Hanning(int i,int N);
    static double winsecs;
    static std::vector<double> compute_f(const char *inputFileName, int len);
    static std::vector<double> compute_f(const WavFile &wavin, int len);
    static void compute_f(const char *inputFileName, const char *outputFileName, int len);
    static void compute_onset(const WavFile &wavin, std::vector<quint8>&pitch, std::vector<quint16>&dura);
    static qreal quantize(std::vector<quint32>data);


};


#endif // DETECTOR_H

#ifndef DETECTOR_H
#define DETECTOR_H
#include <iostream>
#include <cstring>
#include "svm.h"

class Detector{
public:
    Detector();
    ~Detector(){}

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
};


#endif // DETECTOR_H

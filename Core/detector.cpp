#include "detector.h"
#include <cstdlib>
#include <errno.h>
#include <fstream>
#include <thread>

#include <QDebug>

/** Modified from libsvm file svm-predict.c
 * @brief Detector::Detector
 */

Detector::Detector(){

}
void Detector::test(){
    qDebug()<<"begin test svm\n";
    std::thread th((Detector::analysis), std::string("/Users/user/test.txt"), std::string("/Users/user/model.dat"), std::string("/Users/user/output.txt"));
    th.detach();

}
void Detector::analysis(std::string insrc, std::string modelsrc, std::string outsrc){
    FILE *input = fopen(insrc.c_str(), "r");
    if(!input){
        qDebug()<<"not exist!";
        return;
    }
    FILE *output = fopen(outsrc.c_str(), "w");
    model = svm_load_model(modelsrc.c_str());
    if(model == 0){
        qDebug()<<"failed to load model!";
    }
    qDebug()<<"model loaded";
    x = (struct svm_node *) malloc(max_nr_attr*sizeof(struct svm_node));


    predict(input, output);
    qDebug()<<"predict finished";
    svm_free_and_destroy_model(&model);
    free(x);
    free(line);

    fclose(input);
    fclose(output);

}
struct svm_node * Detector::x = NULL;
int Detector::max_nr_attr = 64;
struct svm_model* Detector::model = NULL;
char * Detector::line = NULL;
int Detector::max_line_len = 0;
int Detector::predict_probability=0;


//error handling
void Detector::exit_input_error(int line_num)
{
    fprintf(stderr,"Wrong input format at line %d\n", line_num);
}

//important part!!
void Detector::predict(FILE *input, FILE *output)
{
    int correct = 0;
    int total = 0;
    double error = 0;
    double sump = 0, sumt = 0, sumpp = 0, sumtt = 0, sumpt = 0;

    int svm_type=svm_get_svm_type(model);
    int nr_class=svm_get_nr_class(model);
    double *prob_estimates=NULL;
    int j;

    qDebug()<<"begin predict";
    if(predict_probability)
    {
        if (svm_type==NU_SVR || svm_type==EPSILON_SVR)
        printf("Prob. model for test data: target value = predicted value + z,\nz: Laplace distribution e^(-|z|/sigma)/(2sigma),sigma=%g\n",svm_get_svr_probability(model));
        else
        {
            int *labels=(int *) malloc(nr_class*sizeof(int));
            svm_get_labels(model,labels);
            prob_estimates = (double *) malloc(nr_class*sizeof(double));
            fprintf(output,"labels");
            for(j=0;j<nr_class;j++)
            fprintf(output," %d",labels[j]);
            fprintf(output,"\n");
            free(labels);
        }
    }


    max_line_len = 1024;
    line = (char *)malloc(max_line_len*sizeof(char));
    while(readline(input) != NULL)
    {
        int i = 0;
        double target_label, predict_label;
        char *val, *label, *endptr;
        int inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0

        label = strtok(line," \t\n");
        if(label == NULL) {// empty line
            exit_input_error(total+1);
            return;
        }

        //string to double
        target_label = strtod(label,&endptr);
        if(endptr == label || *endptr != '\0'){
            exit_input_error(total+1);
        }

        qDebug()<<"read xi";
        while(1)
        {
            if(i>=max_nr_attr-1)	// need one more for index = -1
            {
                max_nr_attr *= 2;
                x = (struct svm_node *) realloc(x,max_nr_attr*sizeof(struct svm_node));
            }


            val = strtok(NULL," ");

            if(val == NULL)
                break;//here break the loop
            errno = 0;
            x[i].index = i;//IMPORTANT
            inst_max_index = x[i].index;
            x[i].value = strtod(val,&endptr);//IMPORTANT

            errno = 0;
            qDebug()<<++i;
        }
        x[i].index = -1;
        qDebug()<<"end reading xi";

        if (predict_probability && (svm_type==C_SVC || svm_type==NU_SVC))
        {
            predict_label = svm_predict_probability(model,x,prob_estimates);
            fprintf(output,"%g",predict_label);
            for(j=0;j<nr_class;j++)
            fprintf(output," %g",prob_estimates[j]);
            fprintf(output,"\n");
        }
        else
        {
            predict_label = svm_predict(model,x);
            fprintf(output,"%g\n",predict_label);//modify here
        }

        //calculate accuracy
        if(predict_label == target_label)
        ++correct;
        error += (predict_label-target_label)*(predict_label-target_label);
        sump += predict_label;
        sumt += target_label;
        sumpp += predict_label*predict_label;
        sumtt += target_label*target_label;
        sumpt += predict_label*target_label;
        ++total;
    }
    if (svm_type==NU_SVR || svm_type==EPSILON_SVR)
    {
        printf("Mean squared error = %g (regression)\n",error/total);
        printf("Squared correlation coefficient = %g (regression)\n",
             ((total*sumpt-sump*sumt)*(total*sumpt-sump*sumt))/
             ((total*sumpp-sump*sump)*(total*sumtt-sumt*sumt))
             );
    }
    else
    printf("Accuracy = %g%% (%d/%d) (classification)\n",
         (double)correct/total*100,correct,total);
    if(predict_probability)
    free(prob_estimates);
}

char* Detector::readline(FILE *input)
{
    int len;

    if(fgets(line,max_line_len,input) == NULL)
    return NULL;

    while(strrchr(line,'\n') == NULL)
    {
        max_line_len *= 2;
        line = (char *) realloc(line,max_line_len);
        len = (int) strlen(line);
        if(fgets(line+len,max_line_len-len,input) == NULL)
        break;
    }
    return line;
}


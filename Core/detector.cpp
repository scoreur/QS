#include "detector.h"
#include "spectrum.h"
#include <cstdlib>
#include <errno.h>
#include <fstream>
using std::vector;
#include <thread>
#include <cmath>
#include <QDebug>

/** Modified from libsvm file svm-predict.c
 * @brief Detector::Detector
 */

Detector::Detector(){

}
void Detector::test(){
    qDebug()<<"begin test svm\n";
    std::thread th((Detector::analysis), std::string("/Users/user/test.txt"),
                   std::string("/Users/user/model.dat"), std::string("/Users/user/output.txt"));
    th.detach();

}
void Detector::maxNormalize(vector<qreal> &data){
    qreal max = 0;
    for (vector<qreal>::iterator iter = data.begin();
         iter != data.end();++iter)
            if (*iter>max)
                max = *iter;
    if(max==0) return;
    for (vector<qreal>::iterator iter = data.begin();
         iter != data.end();++iter)
        (*iter) /= max;

}
void Detector::preprocess(vector<qreal> &data){
    vector<qreal>::iterator iter = data.begin();
    qreal pre_thres = 0.1;
    while(iter != data.end() && (*iter)<pre_thres){//0.1?
        (*iter) = 0;
        ++iter;
    }
    maxNormalize(data);
}
vector<qreal> Detector::preprocess(const WavFile& wavin, quint32 len){
    quint16 step = wavin.nChannels();
    if(len<100) len = wavin.length/step;
    vector<qreal> signal;
    signal.reserve(len);
    for(int i=0;i<len;++i){
        signal.push_back((qreal)wavin.data[step*i]);
    }
    //only process left channel
    preprocess(signal);

    return signal;
}

double Detector::Hanning(int i,int num)
{
    return 0.5*(1-::cos(2*M_PI*i/(num-1)));
}
std::vector<qreal> Detector::compute_f(vector<qreal>&signal, int h){
    int N = 2*h; //window size
    int len = signal.size();
    int M = int(len/h+0.5) -1;// window number, rounded by 0.5  WYJ

    std::vector<qreal> f;
    f.assign(M, 0.0);
    for (int m=0;m<M;m++)
    {

        vector<qreal> out = Spectrum::realFFT(signal.data()+m*h, N);
        qreal *iter = f.data()+m;
        for (int i=int(f.size()/2);i>=0;i--)//size N -> f.size()
            (*iter) += out.at(i);
        //TODO: add more advanced discrimination
        //qDebug()<<m<<'\t'<<*iter<<'\n';
    }
    maxNormalize(f);
    return f;
}

std::vector<qreal> Detector::compute_f(const WavFile &wavin, int len){ 

    vector<qreal> signal = preprocess(wavin, len);
    int h = ::pow(2,::ceil(::log(wavin.sampleps()*winsecs)/::log(2))); //hopsize
    return compute_f(signal, h);


}

vector<double> Detector::compute_f(const char *inputFileName, int len){

    WavFile wavin(inputFileName);
    return compute_f(wavin, len);

}

void Detector::compute_f(const char *inputFileName, const char *outputFileName, int len){

    vector<qreal> f = Detector::compute_f(inputFileName, len);
    std::ofstream fout(outputFileName);
    for (vector<qreal>::iterator iter = f.begin(); iter != f.end(); ++iter)
    {
        fout<<*iter<<' ';
    }


}

qreal abs(const cmplx &c){
    return ::sqrt(c.real()*c.real()+c.imag()*c.imag());
}

qreal quantizeQuality(vector<qreal> data, qreal beat){
    qreal error = 0;
    quint32 num = data.size();
    for(quint32 i=0;i<num;++i){
        error += ::abs(data[i]-(int)(data[i]/beat+0.5));
    }
    qDebug()<<beat<<"error "<<error;
    return error/num;
}
qreal Detector::quantize(std::vector<quint32> data){//order reversed
    quint32 num = data.size();
    vector<qreal> quan;
    quan.reserve(num);
    qreal min = (qreal)data.at(0) - (qreal)data.at(1);
    for(quint32 i=1;i<num;++i){
        qreal tmp = (qreal)data.at(i-1) - (qreal)data.at(i);
        if(tmp < 2.0) continue;
        quan.push_back(tmp);
        if(min>quan.back()) min = quan.back();
    }
    qDebug()<<"quantize: "<<min*winsecs;
    qreal scalemin = 0.8 * min;
    qreal scalemax = 1.2 * min;
    qreal step = 0.05 * min;
    qreal minerror = 1;
    qreal bestbeat = scalemin;
    for(qreal beat = bestbeat; beat < scalemax; beat += step){
        qreal tmp = quantizeQuality(quan, beat);
        qDebug()<<"tmp "<<tmp;
        if(tmp<minerror){
            minerror = tmp;
            bestbeat = beat;
        }
    }
    return bestbeat;
}


void Detector::compute_onset(const WavFile &wavin, std::vector<quint8> &pitch, std::vector<quint16> &dura){
    vector<quint32> onset;
    vector<qreal>signal = preprocess(wavin, 0);
    int h = ::pow(2,::ceil(::log(wavin.sampleps()*winsecs)/::log(2))); //hopsize
    vector<qreal> f = compute_f(signal,h);
    qreal *iter = f.data()+f.size()-1;
    for(quint32 m = f.size()-1;m>0;--m, --iter){
        if(*iter >-1e-6 && *iter<1e-6)
            *iter = 0;
        else{
            *iter = 1.0-*(iter-1)/(*iter);
        }
    }
    *iter = 0;//f[0] == 0;
    int b_hop = 7;
    model = svm_load_model("/Users/user/model.dat");
    qreal threshhold1 = 0.5; //standard
    qreal threshhold2 = 0.4; //the lowest requirement for f_onset
    qreal threshhold3 = 0.6; //to compensate for the decline of the requirement for f_onset, the local maximium must be greater than threshhold3
    for(quint32 m = f.size()-2; m>0; --m){
        quint32 max = m;
        if(f.at(max)<f.at(m-1)) max = m-1;
        if(f.at(max)<f.at(m+1)) max = m+1;
        if ((max == m && f.at(m)>threshhold1) || (f.at(m)>threshhold2 && f.at(max)>threshhold3)){
            onset.push_back(m);
            qDebug()<<"compute "<< m;

            vector<qreal>feature;
            if(m+4<=f.size()){
                //qDebug()<<"cqt";
                vector<cmplx> cq = Spectrum::CQT(signal.data()+(m+2)*h, 2*h, 1);

                //qDebug()<<"cqt "<<cq.size();
                for(int k=1;k<83;++k){
                    qreal tmpsum = 0;
                    for(int j=k*b_hop-int(b_hop/2); j<=k*b_hop+int(b_hop/2);++j)
                        tmpsum += abs(cq.at(j));
                    feature.push_back(tmpsum/b_hop);
                }
                maxNormalize(feature);

            }else{
                feature.assign(82, 0.0);
            }
            //qDebug()<<m*winsecs;
            //qDebug()<<feature.front()<<","<<feature.back();

            max_nr_attr = 128;
            x = (struct svm_node *) malloc(max_nr_attr*sizeof(struct svm_node));
            for(int i=0;i<feature.size();++i){
                x[i].index = i;
                x[i].value = feature.at(i);
            }
            x[feature.size()].index = -1;
            double predict_label = svm_predict(model,x);
            qDebug()<<"predict pitch"<<predict_label;//modify here
        }
    }
    //doing cqt and svm to compute pitch






    //quantize onset to make dura;
    qreal bestbeat = quantize(onset);
     qDebug()<<"best beat: "<<bestbeat;

     dura.push_back(0);
     for(quint32 i=onset.size()-1;i>0;--i){
         qreal tmp = onset[i-1]-onset[i];
         dura.push_back(3*(quint16)(tmp/bestbeat+0.5));
         //qDebug()<<dura.back();
     }





}

void Detector::computeOnset(const WavFile &wavin){
    vector<quint8> pitch;
    vector<quint16> dura;
    compute_onset(wavin, pitch, dura);
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
//static members
struct svm_node * Detector::x = NULL;
int Detector::max_nr_attr = 64;
struct svm_model* Detector::model = NULL;
char * Detector::line = NULL;
int Detector::max_line_len = 0;
int Detector::predict_probability=0;

double Detector::winsecs = 0.03;

//error handling
void Detector::exit_input_error(int line_num)
{
    fprintf(stderr,"Wrong input format at line %d\n", line_num);
}

void Detector::loadNodeFromFile(FILE *input){

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
            ++i;
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


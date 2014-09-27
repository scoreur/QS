#include "qsrecorder.h"
#include <QDebug>
#include <QTimer>
#include "wavfile.h"


QSRecorder::QSRecorder(QObject *parent):
    QObject(parent),
    mode(QAudio::AudioInput),
    state(QAudio::StoppedState),
    availableAudioInputDevices
                         (QAudioDeviceInfo::availableDevices(QAudio::AudioInput)),
    audioInputDevice(QAudioDeviceInfo::defaultInputDevice()),
    availableAudioOutputDevices
                (QAudioDeviceInfo::availableDevices(QAudio::AudioOutput)),
    audioOutputDevice(QAudioDeviceInfo::defaultOutputDevice())
{

    init();
    recordButton = new QPushButton(QString("Record"), 0);
    recordButton->resize(75,20);
    recordButton->show();

}

QSRecorder::~QSRecorder(){

}

void QSRecorder::init(){
    qDebug()<<"init:"<<audioInputDevice.deviceName();
    format.setSampleRate(44100);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);
    audioInput = new QAudioInput(audioInputDevice, format, this);

}

void QSRecorder::record(QString fileName, quint32 milisecs){
    recFile = new QFile(fileName);   // Class member
    recFile->open( QIODevice::WriteOnly | QIODevice::Truncate );
    recFile->write((char*)WavFile::default_wavHeader, 44);

        if (!audioInputDevice.isFormatSupported(format)) {
            qDebug() << "Default format not supported, trying to use the nearest.";
            format = audioInputDevice.nearestFormat(format);
        }


        connect(audioInput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
        QTimer::singleShot(milisecs, this, SLOT(stopRecording()));
        audioInput->start(recFile);

}

void QSRecorder::playback(){
    {

        recFile->open(QIODevice::ReadOnly);

        audioOutput = new QAudioOutput(audioOutputDevice, format, this);

        if (!audioOutputDevice.isFormatSupported(format)) {
            qDebug() << "Raw audio format not supported by backend, cannot play audio.";
            return;
        }

        connect(audioOutput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
        audioOutput->start(recFile);
    }
}

void QSRecorder::handleStateChanged(QAudio::State newState)
{
    switch (newState) {
        case QAudio::StoppedState:
            if (audioInput->error() != QAudio::NoError) {
                // Error handling
            } else {
                // Finished recording
            }
            break;

        case QAudio::ActiveState:
            // Started recording - read from IO device
            break;

        default:
            // ... other cases as appropriate
            break;
    }
}

void QSRecorder::stopRecording()
{
    audioInput->stop();
    quint32 len = recFile->size();
    len -= 8;
    recFile->seek(4); recFile->write((char*)&len, 4);
    len -= 36;
    recFile->seek(40); recFile->write((char*)&len, 4);
    recFile->close();
    //playback();

}

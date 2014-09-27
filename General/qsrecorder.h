#ifndef QSRECORDER_H
#define QSRECORDER_H
#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QBuffer>
#include <QByteArray>
#include <QDir>
#include <QObject>
#include <QVector>
#include <QAudioInput>
#include <QAudioOutput>
#include <QPushButton>

/**
 * @brief The QSRecorder class, for recording microphone inputs
 */
class QSRecorder : public QObject
{

    Q_OBJECT
public:
    explicit QSRecorder(QObject *parent = 0);
    ~QSRecorder();

    QAudioFormat        format;
    QAudio::Mode        mode;
    QAudio::State       state;

    const QList<QAudioDeviceInfo> availableAudioInputDevices;
    QAudioDeviceInfo    audioInputDevice;
    QAudioInput*        audioInput;
    QIODevice*          recFile;

    const QList<QAudioDeviceInfo> availableAudioOutputDevices;
    QAudioDeviceInfo    audioOutputDevice;
    QAudioOutput*       audioOutput;

    QPushButton* recordButton;

    void init();
    void record(QString fileName, quint32 milisecs = 1000);
    void playback();

public slots:
    void handleStateChanged(QAudio::State newState);
    void stopRecording();
private:


};


#endif // QSRECORDER_H

#ifndef SPECTRUMGRAPH_H
#define SPECTRUMGRAPH_H
#include <QVector>
#include <QGraphicsRectItem>
#include <QPicture>

class Spectrum;
/**
 * @brief The SpectrumGraph class, for drawing spectrum
 */
class SpectrumGraph: public QGraphicsRectItem{
public:
    SpectrumGraph(Spectrum *spectrum, QGraphicsScene *scene, QGraphicsItem *parent = 0);
    SpectrumGraph(QGraphicsScene *scene);
    ~SpectrumGraph(){}
    void fresh(Spectrum *spectrum);
    void fresh(qint16 data[], quint32 num, quint8 mode=0, quint8 step=1);

    friend class WavScene;
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QVector<QPointF> points;
    QPicture pic;

};

#endif // SPECTRUMGRAPH_H

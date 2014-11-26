#include "spectrumgraph.h"
#include "Core/spectrum.h"
#include "qspreset.h"
#include "wavscene.h"
#include <QDebug>

//return the norm of complex number with accuracy 1e-6
qreal cmplx_norm(const cmplx & c){
    qreal rl = c.real();
    if(rl>-1e-6 && rl<1e-6) rl = 0;
    qreal im = c.imag();
    if(im>-1e-6 && im<1e-6) im = 0;
    return ::sqrt(rl*rl+im*im);
}

//constructors of SpectrumGraph
SpectrumGraph::SpectrumGraph(Spectrum *spectrum, QGraphicsScene *scene, QGraphicsItem *parent)
    : QGraphicsRectItem(parent){
    scene->addItem(this);
    setRect(QSPreset::wavViewRect);
    fresh(spectrum);

}
SpectrumGraph::SpectrumGraph(QGraphicsScene *scene): QGraphicsRectItem(0){
    scene->addItem(this);
    setRect(QSPreset::wavViewRect);
}

//update the graph with new spectrum
void SpectrumGraph::fresh(Spectrum *spectrum){
     qDebug()<<"start fresh "<<spectrum->vallen;
    points.clear();
    qreal temp_x = 0, temp_y = 180, step_x = rect().width()/spectrum->vallen;

    for(std::vector<cmplx>::iterator iter=spectrum->val.begin();
        iter != spectrum->val.end(); ++iter, temp_x+=step_x){
        qreal normtmp = cmplx_norm(*iter)/(1<<15);
        points<<QPointF(temp_x, temp_y - normtmp*750);
    }
    QPainter pt;
    pt.begin(&pic);
    pt.setPen(QPen(Qt::red));
    pt.drawPolyline(&points[0], points.size());
    pt.end();
    scene()->update(sceneBoundingRect());
}
void SpectrumGraph::fresh(qint16 data[], quint32 num, quint8 mode, quint8 step){
    Spectrum sp(data, num, mode, step);
    fresh(&sp);
}

void SpectrumGraph::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget){
    Q_UNUSED(option)
    Q_UNUSED(widget)
    if(((WavScene*)scene())->showSpect)
        painter->drawPicture(10,10,pic);


}


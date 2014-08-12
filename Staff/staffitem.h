#ifndef STAFFITEM_H
#define STAFFITEM_H

#include <QGraphicsObject>



class StaffItem : public QGraphicsObject
{
    Q_OBJECT
public:
    explicit StaffItem(QGraphicsItem *parent = 0);
    ~StaffItem(){}
signals:

public slots:

private:
    quint8 pitch;
    qint8 mark;

};

#endif // STAFFITEM_H

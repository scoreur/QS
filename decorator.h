#ifndef DECORATOR_H
#define DECORATOR_H

class Decorator{

public:
    virtual void paint(QPainter *painter) = 0;
protected:
    Decorator(Decorator *_next = 0) : next(_next){}
    virtual ~Decorator(){
        delete next;
    }
    void setNext(Decorator *_next){next = _next;}

    Decorator *next;
};

#endif // DECORATOR_H

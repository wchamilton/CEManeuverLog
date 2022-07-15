#ifndef SCROLLEATER_H
#define SCROLLEATER_H

#include <QObject>
#include <QEvent>
#include <QDebug>

class ScrollEater : public QObject
{
    Q_OBJECT
public:
    ScrollEater(QObject* parent = nullptr) : QObject(parent) {}

protected:
    bool eventFilter(QObject *watched, QEvent *event) override {
        qDebug() << event->type();
        if (event->type() != QEvent::Wheel) {
            qDebug() << "let slide";
            return QObject::eventFilter(watched, event);
        }
        qDebug() << "OMNOMNOM";
        return true;
    }
};

#endif // SCROLLEATER_H

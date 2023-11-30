#include "receivedrawingarea.h"
#include <QDebug>

ReceiveDrawingArea::ReceiveDrawingArea(QWidget *parent) :
    QWidget(parent)
{
    setAutoFillBackground(true);
    setPalette(QPalette(Qt::white));
}

void ReceiveDrawingArea::drawLine(int x1, int y1, int x2, int y2)
{
    QVector<QPointF> line;
    line.push_back(QPointF(x1, y1));
    line.push_back(QPointF(x2, y2));
    receivedLines.push_back(line);
    update();
}


void ReceiveDrawingArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap));


    for (const QVector<QPointF>& line : receivedLines)
    {
        for (int i = 0; i < line.size() - 1; i++)
        {
            painter.drawLine(line[i], line[i+1]);
        }
    }

}


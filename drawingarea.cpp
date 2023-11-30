#include "drawingarea.h"
#include "communication.h"
#include <QPoint>
#include <QDebug>
#include <thread>

DrawingArea::DrawingArea(QWidget *parent) :
    QWidget(parent),
    isDrawing(false)
{
    setAutoFillBackground(true);
    setPalette(QPalette(Qt::white));
}

void DrawingArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap));

    for (const QVector<QPointF>& line : lines)
    {
        for (int i = 0; i < line.size() - 1; ++i)
        {
            painter.drawLine(line[i], line[i + 1]);
        }
    }
}

void DrawingArea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        lines.append(QVector<QPointF>());
        lines.last().append(event->pos());
        isDrawing = true;
    }
}

void DrawingArea::mouseMoveEvent(QMouseEvent *event)
{
    if (isDrawing && (event->buttons() & Qt::LeftButton))
    {
        lines.last().append(event->pos());
        movePositions.append(event->pos());
        update();

        QPoint mousePos = mapFromGlobal(QCursor::pos());
        qDebug() << mousePos.x() << "\n" << mousePos.y();
        loadIntoSendXQueue(mousePos.x());
        loadIntoSendYQueue(mousePos.y());

    }
}

void DrawingArea::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        lines.last().append(event->pos());
        isDrawing = false;
        update();

        testView();
    }
}


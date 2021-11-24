#include "qlinkmap.h"

QLinkMap::QLinkMap() {

}

QUuid QLinkMap::addLines(const QList<QLine> &lines, const QColor &color)
{
    QUuid uuid = QUuid::createUuid();
    this->linesMap.insert(uuid, QColoredLineList(color, lines));
    return uuid;
}

void QLinkMap::removeLines(const QUuid &uuid)
{
    assert(linesMap.contains(uuid));
    this->linesMap.remove(uuid);
}

void QLinkMap::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    for (auto &coloredLines: linesMap) {
        QPen pen = QPen(coloredLines.color);
        pen.setWidth(5);
        painter.setPen(pen);
        painter.drawLines(coloredLines.linesList);
    }
}

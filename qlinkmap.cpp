#include "qlinkmap.h"

QLinkMap::QLinkMap() {

}

QUuid QLinkMap::addLines(const QList<QLine> &lines)
{
    QUuid uuid = QUuid::createUuid();
    this->linesMap.insert(uuid, lines);
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
    painter.setPen(QPen(Qt::red, 5));
    for (auto &lines: linesMap) {
        painter.drawLines(lines);
    }
}

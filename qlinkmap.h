#ifndef QLINKMAP_H
#define QLINKMAP_H

#include "includes.h"

// Extends QFrame to override paintEvent and draw connections between blocks.
class QLinkMap : public QFrame {
private:
    // Each connection is represented with a list of QLine, associated with a
    // uuid. The uuid is used to remove a connection.
    QMap<QUuid, QList<QLine>> linesMap;

public:
    QLinkMap();

    // Add a connection.
    QUuid addLines(const QList<QLine> &lines);

    // Remove a connection.
    void removeLines(const QUuid &uuid);
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // QLINKMAP_H

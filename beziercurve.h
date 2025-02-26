#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H
#include <QVector3D>


class BezierCurve
{
public:
    BezierCurve();
    int d=3;

    QVector3D evaluateBezier(float t);
};

#endif // BEZIERCLASS_H

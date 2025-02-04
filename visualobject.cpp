#include "visualobject.h"

VisualObject::VisualObject() {}

void VisualObject::move(float x, float y, float z) {
    mMatrix.translate(x, y, z);
}
void VisualObject::scale(float s) {
    mMatrix.scale(s);
}
void VisualObject::rotate(float t, float x, float y, float z) {
    mMatrix.rotate(t, x, y, z);
}


#ifndef OBJ_H
#define OBJ_H

#include "vector.h"
#include <QVector>

// A simple parser that reads and writes Wavefront .obj files
class OBJ
{
public:
    struct Index
    {
        int vertex;
        int coord;
        int normal;

        Index() : vertex(-1), coord(-1), normal(-1) {}
        Index(int vertex, int coord, int normal) : vertex(vertex), coord(coord), normal(normal) {}
    };

    struct Triangle
    {
        Index a, b, c;

        Triangle() {}
        Triangle(const Index &a, const Index &b, const Index &c) : a(a), b(b), c(c) {}
    };

    QVector<Vector3> vertices;
    QVector<Vector2> coords;
    QVector<Vector3> normals;
    QVector<Triangle> triangles;

    void draw() const;
    bool read(const QString &path);
    bool write(const QString &path) const;

private:
    Index getIndex(const QString &str) const;
    void drawIndex(const Index &index) const;
};

#endif // OBJ_H

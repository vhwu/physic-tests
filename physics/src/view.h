#ifndef VIEW_H
#define VIEW_H

#include <qgl.h>
#include <QTime>
#include <QTimer>

class MyApplication;

class View : public QGLWidget
{
    Q_OBJECT

public:
    View(QWidget *parent);
    ~View();

private:
    MyApplication* _app;
    QTime time;
    QTimer timer;
    float fps;

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void wheelEvent(QWheelEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    //int mouseX,mouseY;

private slots:
    void tick();
};

#endif // VIEW_H


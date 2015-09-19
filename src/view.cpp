#include "view.h"
#include "game/myapplication.h"
#include <QApplication>
#include <QKeyEvent>
#include "vector.h"
#include "engine/common/raytracer.h"

#if defined(__APPLE__) || defined(MACOSX)
    #include <OpenGL/glu.h>
    //#include <GLUT/glut.h>
#else
    #include <GL/glu.h>
#endif

View::View(QWidget *parent) : QGLWidget(parent)
{
    // View needs all mouse move events, not just mouse drag events
    setMouseTracking(true);

    // Hide the cursor since this is a fullscreen app
    //setCursor(Qt::BlankCursor);

    // View needs keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    // The game loop is implemented using a timer
    connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));

    //Creates application
    _app = new MyApplication(this);
}

View::~View()
{
    delete _app;
}

void View::initializeGL()
{
    // Start a timer that will try to get 60 frames per second (the actual
    // frame rate depends on the operating system and other running programs)
    time.start();
    timer.start(1000 / 60);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glEnable( GL_BLEND );
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0,0.0,0.0,0.0);
    glShadeModel(GL_SMOOTH);  //to blend colors

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHT0);

    _app->initializeGL();
}

void View::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(1,1,1);
    renderText(700, 15, "FPS: " + QString::number((int) (fps)), this->font());


    _app->storeMatrix();
    _app->onDraw();

    //Store model and projection information
    float proj[16];
    glGetFloatv(GL_PROJECTION_MATRIX, proj);
    float model[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, model);
    int view[4];
    glGetIntegerv(GL_VIEWPORT, view);
    const double w = view[2];
    const double h = view[3];

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,w,0,h,-1,1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    _app->onUI();

    //Restore matrices
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(proj);
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(model);
}

void View::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    _app->onResize(Vector2(w,h));
}

void View::mousePressEvent(QMouseEvent *event)
{
    _app->mousePressEvent(event);
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    _app->mouseMoveEvent(event);
}

void View::mouseReleaseEvent(QMouseEvent *event)
{
    _app->mouseReleaseEvent(event);
}

void View::wheelEvent(QWheelEvent *event)
{
    _app->wheelEvent(event);
}

void View::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) QApplication::quit();
    _app->keyPressEvent(event);
}

void View::keyReleaseEvent(QKeyEvent *event)
{
    _app->keyReleaseEvent(event);
}

void View::tick()
{
    // Get the number of seconds since the last tick (variable update rate)
    float seconds = time.restart() * 0.001f;
    fps = .02f / seconds + .98f * fps;

    _app->tick(seconds);

    // Flag this view for repainting (Qt will call paintGL() soon after)
    update();
}

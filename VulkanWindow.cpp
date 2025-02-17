#include "VulkanWindow.h"
#include <QKeyEvent>
#include <QDebug>

VulkanWindow::VulkanWindow()
{  }

QVulkanWindowRenderer* VulkanWindow::createRenderer()
{
    qDebug() << "createRenderer()";

    //Makes a new instance of the RenderWindow (our Renderer) class
    return mRenderWindow= new RenderWindow(this, true); // last true == try MSAA
}

void VulkanWindow::keyPressEvent(QKeyEvent *event)
{


    if(event->key()==Qt::Key_0){
        qDebug("Object 1 chosen");
        mIndex=0;
    }

    if(event->key()==Qt::Key_1){
        qDebug("Object 2 chosen");
        mIndex=1;
    }

    if (event->key() == Qt::Key_A)
    {
        qDebug("moving left or right");
        dynamic_cast<RenderWindow*>(mRenderWindow)->mObjects.at(mIndex)->move(-0.1f);

    }
    if (event->key() == Qt::Key_S)
    {
        qDebug("scale");
        dynamic_cast<RenderWindow*>(mRenderWindow)->mObjects.at(mIndex)->scale(1.0f);

    }
    if(event->key() == Qt::Key_C)
    {
        qDebug("move camera");
        dynamic_cast<RenderWindow*>(mRenderWindow)->mCamera.translate(.0f, 0.0f, -1.0f);

    }
    if(event->key() == Qt::Key_R)
    {
        qDebug("rotate camera");
        dynamic_cast<RenderWindow*>(mRenderWindow)->mCamera.rotate(45.0f,.0f, 1.0f, 0.0f);

    }
    if (event->key() == Qt::Key_Escape)
    {
        QCoreApplication::quit();       //Shuts down the whole program
    }
}

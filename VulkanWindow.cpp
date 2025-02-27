#include "VulkanWindow.h"
#include "Renderer.h"
#include <QKeyEvent>

VulkanWindow::VulkanWindow()
{
    mSelectedObject = nullptr;
}

QVulkanWindowRenderer* VulkanWindow::createRenderer()
{
    //Makes a new instance of the Renderer (our Renderer) class
    mRenderer = new Renderer(this, true); // last true == try MSAA
    return mRenderer;
}

void VulkanWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_W)
    {
        dynamic_cast<Renderer*>(mRenderer)->mObjects.at(mIndex)->move(0.0f, 1.0f, 0.0f);
        // if(mSelectedObject)
        // {
        //     qDebug("Move forward");
        //     mSelectedObject->move(0.0f,10.0f, 0.0f);
        // }

    }

    if (event->key() == Qt::Key_S)
    {
        if(mSelectedObject)
        {
            qDebug("Move backwards");
            mSelectedObject->move(-10.1f,0.0f, 0.0f);
        }
        dynamic_cast<Renderer*>(mRenderer)->mObjects.at(mIndex)->move(0.0f, -1.0f, 0.0f);
    }

    if (event->key() == Qt::Key_A)
    {
        if(mSelectedObject)
        {
            qDebug("Move left");
            mSelectedObject->move(0.0f,0.0f, 10.1f);
        }
        dynamic_cast<Renderer*>(mRenderer)->mObjects.at(mIndex)->move(0.0f,0.0f, 1.1f);
    }

    if (event->key() == Qt::Key_D)
    {
        if(mSelectedObject)
        {
            qDebug("Move right");
            mSelectedObject->move(0.0f,0.0f, -10.1f);
        }
        dynamic_cast<Renderer*>(mRenderer)->mObjects.at(mIndex)->move(0.0f,0.0f, -1.1f);
    }

    if(event->key() == Qt::Key_M)
    {
        qDebug("Scaling object");
        dynamic_cast<Renderer*>(mRenderer)->mObjects.at(mIndex)->scale(0.9f);
    }
    if (event->key() == Qt::Key_Escape)
    {
        QCoreApplication::quit();       //Shuts down the whole program
    }

    if (event->key() == Qt::Key_0){
        qDebug("triangle object");
        mIndex = 9;
    }
    if (event->key() == Qt::Key_1){
        qDebug("rectangle object");
        mIndex = 1;
    }

    //****** Camera control ******** //
    if(event->key() == Qt::Key_T)
    {
        dynamic_cast<Renderer*>(mRenderer)->mCamera.translate(.0f, 0.0f, 0.2f);
    }
    if(event->key() == Qt::Key_G)
    {
        dynamic_cast<Renderer*>(mRenderer)->mCamera.translate(.0f, 0.0f, -0.2f);
    }

    if(event->key() == Qt::Key_A)
    {
        // dynamic_cast<Renderer*>(mRenderer)->mCamera.translate(.0f, 0.0f, -0.2f);
    }
    if(event->key() == Qt::Key_Y)
    {
        dynamic_cast<Renderer*>(mRenderer)->mCamera.rotate(45, 0.0f, 1.0f, 0.0f);
    }
    if(event->key() == Qt::Key_X)
    {
        dynamic_cast<Renderer*>(mRenderer)->mCamera.rotate(45, 1.0f, 0.0f, 0.0f);
    }
    if(event->key() == Qt::Key_Z)
    {
        dynamic_cast<Renderer*>(mRenderer)->mCamera.rotate(45, 0.0f, 0.0f, 1.0f);
    }
}

// void VulkanWindow::mouseMoveEvent(QMouseEvent *event)
// {
//     // qDebug("X: %d, mouselast: %d" , event->pos().x(), mMouseXlast);
//     if(event->pos().x() - mMouseXlast > 0)
//         dynamic_cast<Renderer*>(mRenderer)->mCamera.rotate(0.5f, 0.0f, 1.0f, 0.0f);

//     if(event->pos().x() - mMouseXlast < 0)
//         dynamic_cast<Renderer*>(mRenderer)->mCamera.rotate(-0.5f, 0.0f, 1.0f, 0.0f);

//     mMouseXlast = event->pos().x();
// }

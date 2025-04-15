#include "VulkanWindow.h"
#include "Renderer.h"
#include <QKeyEvent>
#include <vector>
VulkanWindow::VulkanWindow()
{
    //mSelectedObject = nullptr;
}

QVulkanWindowRenderer* VulkanWindow::createRenderer()
{
    //Makes a new instance of the Renderer (our Renderer) class
    mRenderer = new Renderer(this, true); // last true == try MSAA
    mCamera = &dynamic_cast<Renderer*>(mRenderer)->mCamera;
    mCamera = &dynamic_cast<Renderer*>(mRenderer)->insideCamera;
    return mRenderer;
}

void VulkanWindow::setCameraSpeed(float value)
{
    mCameraSpeed += value;

    //Keep within some min and max values
    if(mCameraSpeed < 0.01f)
        mCameraSpeed = 0.01f;
    if (mCameraSpeed > 0.3f)
        mCameraSpeed = 0.3f;
}


void VulkanWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_W)
    {
        if (dynamic_cast<Renderer*>(mRenderer)->CanMove){
            float rate=-0.5f;
            dynamic_cast<Renderer*>(mRenderer)->mObjects.at(1)->move(0.0f, 0.0f, rate);
            qDebug("Move forward");
            dynamic_cast<Renderer*>(mRenderer)->mObjects.at(1)->updateMiddlePoints(0,0.0f,0.0f,rate);

            // if(mSelectedObject)
            // {
            //     qDebug("Move forward");
            //     mSelectedObject->move(0.0f,10.0f, 0.0f);
            // }
        }

        else{
            qDebug("You lost");
        }
    }


    if (event->key() == Qt::Key_S )
    {
        if (dynamic_cast<Renderer*>(mRenderer)->CanMove){
            // if(mSelectedObject)
            // {
            //     qDebug("Move backwards");
            //     mSelectedObject->move(-10.1f,0.0f, 0.0f);
            // }
            //mIndex
            float rate=0.5f;
            dynamic_cast<Renderer*>(mRenderer)->mObjects.at(1)->move(0.0f, 0.0f,rate);
            dynamic_cast<Renderer*>(mRenderer)->mObjects.at(1)->updateMiddlePoints(0,0.0f,0.0f,rate);


            qDebug("Move backwards");
        }

        else{
            qDebug("You lost");
        }

    }



    if (event->key() == Qt::Key_A )
    {
        if (dynamic_cast<Renderer*>(mRenderer)->CanMove){
            // if(mSelectedObject)
            // {
            //     qDebug("Move left");
            //     mSelectedObject->move(0.0f,0.0f, 10.1f);
            // }
            float rate=-0.5f;
            qDebug("Move left");
            dynamic_cast<Renderer*>(mRenderer)->mObjects.at(1)->move(rate,0.0f, 0.0f);
            dynamic_cast<Renderer*>(mRenderer)->mObjects.at(1)->updateMiddlePoints(0,rate,0.0f,0.0f);

        }

        else{
            qDebug("You lost");
        }
    }



    if (event->key() == Qt::Key_D )
    {
         if (dynamic_cast<Renderer*>(mRenderer)->CanMove){
             // if(mSelectedObject)
             // {
             //     qDebug("Move right");
             //     mSelectedObject->move(0.0f,0.0f, -10.1f);
             // }
             float rate=0.5f;
             qDebug("Move right");
             dynamic_cast<Renderer*>(mRenderer)->mObjects.at(1)->move(rate,0.0f, 0.0f);
             dynamic_cast<Renderer*>(mRenderer)->mObjects.at(1)->updateMiddlePoints(0,rate,0.0f,0.0f);
         }
         else{
             qDebug("You lost");
         }


    }



    // if(event->key() == Qt::Key_M)
    // {
    //     qDebug("Scaling object");
    //     dynamic_cast<Renderer*>(mRenderer)->mObjects.at(mIndex)->scale(0.9f);
    // }
    if (event->key() == Qt::Key_Escape)
    {
        QCoreApplication::quit();       //Shuts down the whole program
    }

    // if (event->key() == Qt::Key_0){
    //     qDebug("plane");
    //     mIndex = 0;
    // }
    // if (event->key() == Qt::Key_1){
    //     qDebug("Player");
    //     mIndex = 1;
    // }
    // if (event->key() == Qt::Key_2){
    //     qDebug("rectangle object");
    //     mIndex = 2;
    // }

    //****** Camera control ******** //
    // if(event->key() == Qt::Key_T)
    // {
    //     dynamic_cast<Renderer*>(mRenderer)->mCamera.translate(.0f, 0.0f, 0.2f);
    // }
    // if(event->key() == Qt::Key_G)
    // {
    //     dynamic_cast<Renderer*>(mRenderer)->mCamera.translate(.0f, 0.0f, -0.2f);
    // }

    // if(event->key() == Qt::Key_I)
    // {
    //     // dynamic_cast<Renderer*>(mRenderer)->mCamera.translate(.0f, 0.0f, -0.2f);
    //     grabKeyboard();
    // }
    if(event->key() == Qt::Key_Y)
    {
        dynamic_cast<Renderer*>(mRenderer)->mCamera.rotate(45, 0.0f, 1.0f, 0.0f);

    }
    // if(event->key() == Qt::Key_X)
    // {
    //     dynamic_cast<Renderer*>(mRenderer)->mCamera.rotate(45, 1.0f, 0.0f, 0.0f);
    // }
    // if(event->key() == Qt::Key_Z)
    // {
    //     //dynamic_cast<Renderer*>(mRenderer)->mCamera.rotate(45, 0.0f, 0.0f, 1.0f);
    // }

    //////////////////////collect pickup
    if(event->key() == Qt::Key_K)
    {
        VisualObject* player{dynamic_cast<Renderer*>(mRenderer)->mObjects.at(1)};
        std::vector<VisualObject*> temp=dynamic_cast<Renderer*>(mRenderer)->mPickups;


       //check if player collides with any pickups, iterating through every pickup....
        for (auto it=temp.begin(); it!=temp.end(); it++){
            pickup=dynamic_cast<Renderer*>(mRenderer)->checkCollision(player, *it);
            if (dynamic_cast<Renderer*>(mRenderer)->IsColliding){
                approved=pickup;

                count+=1;
                qDebug("Picked up");
                qDebug()<<count;
                qDebug(" out of ");
                qDebug()<<6;

                //deletion could have been done with map, but i'm lazy to redo vector to map right now...
                auto it=std::find(temp.begin(), temp.end(), approved);
                auto itr2=temp.begin();
                int index=it-itr2;
                //temp.pop_back(); doesnt affect renderer, while dynamic cast does
                dynamic_cast<Renderer*>(mRenderer)->mPickups.erase(dynamic_cast<Renderer*>(mRenderer)->mPickups.begin()+index);

                if (count==6){
                    qDebug("You won");

                }
                break;
            }
            else{
                qDebug("You are too far away");
            }
        }
        pickup=nullptr;
    }




    //    You get the keyboard input like this
    if(event->key() == Qt::Key_W)
    {
        mInput.W = true;
    }
    if(event->key() == Qt::Key_S)
    {
        mInput.S = true;
    }
    if(event->key() == Qt::Key_D)
    {
        mInput.D = true;
    }
    if(event->key() == Qt::Key_A)
    {
        mInput.A = true;
    }
    if(event->key() == Qt::Key_Q)
    {
        mInput.Q = true;
    }
    if(event->key() == Qt::Key_E)
    {
        mInput.E = true;
    }
    if(event->key() == Qt::Key_Z)
    {
    }
    if(event->key() == Qt::Key_X)
    {
    }
    if(event->key() == Qt::Key_Up)
    {
        mInput.UP = true;
    }
    if(event->key() == Qt::Key_Down)
    {
        mInput.DOWN = true;
    }
    if(event->key() == Qt::Key_Left)
    {
        mInput.LEFT = true;
    }
    if(event->key() == Qt::Key_Right)
    {
        mInput.RIGHT = true;
    }

}


void VulkanWindow::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_W)
    {
        mInput.W = false;
    }
    if(event->key() == Qt::Key_S)
    {
        mInput.S = false;
    }
    if(event->key() == Qt::Key_D)
    {
        mInput.D = false;
    }
    if(event->key() == Qt::Key_A)
    {
        mInput.A = false;
    }
    if(event->key() == Qt::Key_Q)
    {
        mInput.Q = false;
    }
    if(event->key() == Qt::Key_E)
    {
        mInput.E = false;
    }
    if(event->key() == Qt::Key_Z)
    {
    }
    if(event->key() == Qt::Key_X)
    {
    }
    if(event->key() == Qt::Key_Up)
    {
        mInput.UP = false;
    }
    if(event->key() == Qt::Key_Down)
    {
        mInput.DOWN = false;
    }
    if(event->key() == Qt::Key_Left)
    {
        mInput.LEFT = false;
    }
    if(event->key() == Qt::Key_Right)
    {
        mInput.RIGHT = false;
    }
}

void VulkanWindow::wheelEvent(QWheelEvent *event)
{
    //QWheelEvent gives an x and y value in a QPoint
    //Y == Vertical scroll, X = Horizontal scroll
    QPoint numDegrees = event->angleDelta();

    //if RMB, change the speed of the camera
    if (mInput.RMB)
    {
        if (numDegrees.y() < 1)
            setCameraSpeed(-0.002f);
        if (numDegrees.y() > 1)
            setCameraSpeed(0.002f);
    }
    qDebug("CameraSpeed: %f", mCameraSpeed);
}

void VulkanWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        mInput.RMB = true;
    if (event->button() == Qt::LeftButton)
        mInput.LMB = true;
    if (event->button() == Qt::MiddleButton)
        mInput.MMB = true;
}

void VulkanWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        mInput.RMB = false;
    if (event->button() == Qt::LeftButton)
        mInput.LMB = false;
    if (event->button() == Qt::MiddleButton)
        mInput.MMB = false;
}

void VulkanWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (mInput.RMB)
    {
        //Using mMouseXYlast as deltaXY so we don't need extra variables
        mMouseXlast = event->pos().x() - mMouseXlast;
        mMouseYlast = event->pos().y() - mMouseYlast;

        if (mMouseXlast != 0)
            dynamic_cast<Renderer*>(mRenderer)->mCamera.yaw(-mCameraRotateSpeed * mMouseXlast);
        if (mMouseYlast != 0)
            dynamic_cast<Renderer*>(mRenderer)->mCamera.pitch(-mCameraRotateSpeed * mMouseYlast);
    }
    mMouseXlast = event->pos().x();
    mMouseYlast = event->pos().y();
}

void VulkanWindow::handleInput()
{
    //Camera
    mCamera->setSpeed(0.f);  //cancel last frame movement
    if (mInput.RMB)
    {
        if (mInput.W)
            mCamera->setSpeed(mCameraSpeed);
        if (mInput.S)
            mCamera->setSpeed(-mCameraSpeed);
        if (mInput.D)
            mCamera->moveRight(-mCameraSpeed);
        if (mInput.A)
            mCamera->moveRight(mCameraSpeed);
        if (mInput.Q)
            mCamera->updateHeigth(mCameraSpeed);
        if (mInput.E)
            mCamera->updateHeigth(-mCameraSpeed);
    }
}

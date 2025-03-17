#include "VulkanWindow.h"
#include "Renderer.h"
#include <QKeyEvent>
#include <vector>
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

void VulkanWindow::handleInput()
{


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

#ifndef VULKANWINDOW_H
#define VULKANWINDOW_H

#include <QVulkanWindow>
#include "VisualObject.h"
#include "input.h"
#include <QtAlgorithms>

/*The QVulkanWindow subclass reimplements the factory function QVulkanWindow::createRenderer().
This returns a new instance of the QVulkanWindowRenderer subclass.
A QVulkanWindow is a Vulkan-capable QWindow that manages a Vulkan device, a graphics queue,
a command pool and buffer, a depth-stencil image and a double-buffered FIFO swapchain, etc.
In order to be able to access various Vulkan resources via the window object,
a pointer to the window is passed and stored via the constructor.*/
class VulkanWindow : public QVulkanWindow
{
    Q_OBJECT

public:
    VulkanWindow();

    QVulkanWindowRenderer* createRenderer() override;
    QVulkanWindowRenderer* getRenderer() const { return mRenderer; }
    void setSelectedObject(VisualObject* object) { mSelectedObject = object; }
    void handleInput();


signals:
    void frameQueued(int colorValue);

protected:
    //The QVulkanWindow is a QWindow that we inherit from and have these functions
    // to capture mouse and keyboard.
    // Uncomment to use (you also have to make the definitions of
    // these functions in the cpp-file to use them of course!)
    //
    //    void mousePressEvent(QMouseEvent *event) override{}
    //void mouseMoveEvent(QMouseEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;              //the only one we use now
    void keyReleaseEvent(QKeyEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;



    QVulkanWindowRenderer* mRenderer{ nullptr };
    VisualObject* mSelectedObject{ nullptr };
    int mIndex{0};

    int count{0};
    VisualObject* approved{nullptr};
    VisualObject* pickup {nullptr};
 private:
    void setCameraSpeed(float value);
    Input mInput;
    float mCameraSpeed{0.005f};
    float mCameraRotateSpeed{ -0.1f };
    int mMouseXlast{0}; //for mouse rotate input
    int mMouseYlast{0};

    class Camera* mCamera{ nullptr };
//     int mMouseXlast{0}; //for mouse rotate input
};
#endif // VULKANWINDOW_H

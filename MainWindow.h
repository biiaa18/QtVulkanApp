#ifndef HELLOVULKANWIDGET_H
#define HELLOVULKANWIDGET_H

#include <QWidget>
using namespace std;
class QMenu;
class QMenuBar;
class QAction;
class QDialogButtonBox;

QT_FORWARD_DECLARE_CLASS(QTabWidget)
QT_FORWARD_DECLARE_CLASS(QPlainTextEdit)

//Forward declaration
class VulkanWindow;
<<<<<<< Updated upstream
=======
// class QMenu;
// class QMenuBar;
// class QAction;
// class QDialogButtonBox;
>>>>>>> Stashed changes

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(VulkanWindow *vw, QPlainTextEdit *logWidget);

public slots:
    void onScreenGrabRequested();

private:
    VulkanWindow *mVulkanWindow{ nullptr };
    QTabWidget *mInfoTab{ nullptr };
    QPlainTextEdit *mLogWidget{ nullptr };

<<<<<<< Updated upstream
    QMenuBar* menuBar;
    QMenu* fileMenu;
    QAction* openFileAction;
    QAction* exitAction;
    QMenuBar* createMenu();
    string mSelectedName;

private slots:
    void openFile();
    void selectName();

=======
//     QMenuBar* createMenu();

//     QMenuBar* menuBar{ nullptr };
//     QMenu* fileMenu{ nullptr };
//     QAction* openFileAction{ nullptr };
//     QAction* exitAction{ nullptr };
//     std::string mSelectedName;

// private slots:
//     void openFile();
//     void selectName();
>>>>>>> Stashed changes
};

#endif // HELLOVULKANWIDGET_H

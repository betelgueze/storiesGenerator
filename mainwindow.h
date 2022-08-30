#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QImage>
#include <QPixmap>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QColor>
#include <QListWidgetItem>

#include "stories_generator.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum myState{FILES_READ};

    myState m_state;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    size_t GetTypeListOccurencesAt(size_t i);

private slots:
    void on_StoriesList_itemClicked(QListWidgetItem *item);

    void on_SaveStoryButton_clicked();

    void on_FlushButton_clicked();

    void on_GenerateButton_clicked();

    void on_IntepretationsList_itemDoubleClicked(QListWidgetItem *item);

    void on_actionAbout_triggered();

    void on_actionLicense_triggered();

    void on_actionHelp_triggered();

    void on_actionWhite_triggered();

    void on_actionDark_attempt_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionOpen_Stories_Directory_triggered();

    void on_actionOpen_Vsuvky_Directory_triggered();

    void on_actionSave_Story_triggered();

    void on_actionFlush_Stories_to_HDD_triggered();

    void on_actionExit_triggered();

private:
    Ui::MainWindow *ui;

    StoriesList * m_sl;
    StoryTypesList * m_stl;
    VsuvkyList * m_vl;
};

#endif // MAINWINDOW_H

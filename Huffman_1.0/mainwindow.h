#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfo>
#include "function.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected slots:
    void select_source();
    void select_target();
    void encoding();
    void decoding();
    //void opeation();
private:
    Ui::MainWindow *ui;
    QFileInfo *source_file_info;
    Function *func;
    QString target_file_path;
    bool defalut_path;
    QString the_other_suffix(QString suffix);
    void coding_abled(QString suffix);
    void show_result(QString res);
    bool is_true_file();
};
#endif // MAINWINDOW_H

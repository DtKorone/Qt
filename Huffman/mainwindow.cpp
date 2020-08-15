#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    source_file_info = new QFileInfo;
    func=new Function;
    defalut_path=false;
    coding_abled("");
    connect(ui->select_sorce_file,SIGNAL(clicked()),this,SLOT(select_source()));
    connect(ui->select_target_file,SIGNAL(clicked()),this,SLOT(select_target()));
    connect(ui->encoding_button,SIGNAL(clicked()),this,SLOT(encoding()));
    connect(ui->decoding_button,SIGNAL(clicked()),this,SLOT(decoding()));
}

MainWindow::~MainWindow()
{
    delete source_file_info;
    delete func;
    delete ui;
}
void MainWindow::encoding()
{
    if(!is_true_file()) return;
    bool flag=false;
    QString f1,f2;
    f1=source_file_info->filePath();
    f2=target_file_path;
    flag=func->operatefile(f1,f2,0);
    if(flag)
        show_result("成功");
    else
        show_result("失败");
}
void MainWindow::decoding()
{
    if(!is_true_file()) return;
    bool flag=false;
    QString f1,f2;
    f1=source_file_info->filePath();
    f2=target_file_path;
    flag=func->operatefile(f1,f2,1);
    if(flag)
        show_result("成功");
    else
        show_result("失败");
}
void MainWindow::select_source()
{
    show_result("");
    QFileDialog *fileDialog = new QFileDialog(this);
    //设置文件窗口标题
    fileDialog->setWindowTitle(tr("选择文件"));
    //设置文件窗口路径
   // fileDialog->setDirectory(".");
    //设置文件过滤器
    QStringList nameFilters;
    nameFilters.append(tr("All(*.txt *.hm)"));
    nameFilters.append(tr("text(*.txt)"));
    nameFilters.append(tr("huffman(*.hm)"));
    fileDialog->setNameFilters(nameFilters);
    //设置可以选择多个文件,默认为只能选择一个文件QFileDialog::ExistingFiles
    //fileDialog->setFileMode(QFileDialog::ExistingFiles);
    //设置文件窗口显示模式
    fileDialog->setViewMode(QFileDialog::Detail);
    QStringList fileName;
     if(fileDialog->exec())
     {
         fileName = fileDialog->selectedFiles();
     }
     if(!fileName.isEmpty())
     {
         source_file_info->setFile(fileName.front());
         ui->source_file->setText(source_file_info->absoluteFilePath());
         ui->target_file->setText(source_file_info->baseName()+the_other_suffix(source_file_info->suffix()));
         coding_abled(source_file_info->suffix());
         target_file_path=source_file_info->path()+"/"+ui->target_file->text();
         defalut_path=true;
     }
     delete fileDialog;
}
void MainWindow::select_target()
{
    show_result("");
    defalut_path=false;
    if(source_file_info->isFile()==false)
    {
        ui->result->setText(tr("选择原文件"));
        return ;
    }
    QFileDialog *fileDialog = new QFileDialog(this);
    QStringList nameFilters;
    QString temp_select;
    temp_select = fileDialog->getSaveFileName(this,tr("新建文件"),target_file_path,tr("*.txt *.hm"));
     if(!target_file_path.isNull()){
         if(!temp_select.isNull())
         {
             target_file_path = temp_select;
         }
         ui->target_file->setText(target_file_path);
     }
     delete fileDialog;
}

QString MainWindow::the_other_suffix(QString suffix)
{
    if(suffix=="txt")
        return ".hm";
    else return ".txt";
}

void MainWindow::coding_abled(QString suffix)
{
    if(suffix=="txt")
        ui->encoding_button->setEnabled(true);
    else ui->encoding_button->setEnabled(false);
    if(suffix=="hm")
        ui->decoding_button->setEnabled(true);
    else ui->decoding_button->setEnabled(false);
}
void MainWindow::show_result(QString res)
{
    ui->result->setText(res);
}

bool MainWindow::is_true_file()
{
    if(ui->source_file->text().isEmpty())
    {
        show_result("请选择原文件");
        return false;
    }
    else if(ui->target_file->text().isEmpty())
    {
        show_result("请选择目标文件");
        return false;
    }

    bool flag=false;
    source_file_info->setFile(ui->source_file->text());
    if(source_file_info->isFile())
        flag = true;
    else
    {
        show_result("原文件错误");
        return false;
    }
    if(defalut_path==false)
        target_file_path = ui->target_file->text();
    else
        target_file_path = source_file_info->path()+"/"+ui->target_file->text();
    QFileInfo target_file_info(target_file_path);
    if(target_file_info.isFile())
    {

         QMessageBox::StandardButton res = QMessageBox::warning(this,tr("文件已存在"),tr("文件已存在，是否确定覆盖？"),QMessageBox::Ok|QMessageBox::Cancel);

        if(res == QMessageBox::Ok)
            flag=true;
        else
        {
            show_result(tr("用户取消"));
            return false;
        }
    }
    return flag;
}

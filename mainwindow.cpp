#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QColor>
#include <QString>
#include <QLabel>
#include <QDebug>
#include <QListWidgetItem>
#include <QLineEdit>
#include <queue>
#include <QTextCursor>
#include <sstream>
#include <regex>
#include <QDesktopServices>
#include <QMessageBox>
#include <QStyleFactory>
#include <QPalette>

#include "codeEditor.h"
#include "interpretationslist.h"

void _PopulateWith(QListWidget * sl, StoriesList * m_sl)
{
    sl->clear();
    for(Story * st : m_sl->data)
    {
        sl->addItem(QString::fromStdString(st->m_storyName));
    }
    //TODO redraw list if needed
}

void _PopulateWithX(QListWidget * sl, StoryTypesList * m_stl)
{
    sl->clear();
    for(StoryType & str : m_stl->data)
    {
        sl->addItem(QString::fromStdString(str.data));
    }
    //TODO redraw list if needed
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_sl = new StoriesList();
    m_stl = new StoryTypesList();
    m_vl = new VsuvkyList();

    m_sl->ReadFromDirectory();
    m_stl->ReadFromListOfStories(m_sl);
    m_vl->ReadFromDirectory();

    m_stl->PopulateFromStoriesEndings(m_sl);
    m_stl->Sort();

    _PopulateWith(ui->StoriesList,m_sl);
    _PopulateWithX(ui->IntepretationsList,m_stl);

    QApplication::setStyle(QStyleFactory::create("Fusion"));
}

MainWindow::~MainWindow()
{
    delete m_sl;
    delete m_stl;
    delete m_vl;
    delete ui;
}

void _fromListToLongString(std::list<std::string>& list, std::string & strres, const char delimiter)
{
    for(std::string & str: list)
    {
        strres += str;
        strres += delimiter;
    }
}

void _fromListToLongStringX(Node * root, std::string & strres, const char delimiter)
{
    std::queue<Node *> q;
    q.push(root);
    while (q.empty() == false) {
        Node *node = q.front();
        strres += node->data;
        strres += delimiter;
        q.pop();
        if (node->left_child != NULL)
            q.push(node->left_child);
        if (node->right_child != NULL)
            q.push(node->right_child);
    }
}


void MainWindow::on_StoriesList_itemClicked(QListWidgetItem *item)
{
    QString storyName = item->text();
    Story * res;
    for(Story * st: m_sl->data)
    {
        if(st->m_storyName.compare(storyName.toStdString()) == 0)
        {
           res = st;
           break;
        }
    }
    ui->StoryNameEdit->clear();
    ui->StoryNameEdit->insert(QString::fromStdString(res->m_storyName));
    std::string strres;
    _fromListToLongString(res->m_interpretationsList,strres, ' ');
    ui->InterpretationsEdit->clear();
    ui->InterpretationsEdit->insert(QString::fromStdString(strres));
    strres.clear();
    _fromListToLongStringX(res->m_root,strres, '\n');
    ui->StoryTextEdit->clear();
    ui->StoryTextEdit->setPlainText(QString::fromStdString(strres));
}


void MainWindow::on_SaveStoryButton_clicked()
{
    std::string sname = ui->StoryNameEdit->text().toStdString();
    if(sname.length() <= 3)
        return;
    //search for potential story with the same name to be deleted
    std::list<Story*>::iterator it;
    for (it = m_sl->data.begin(); it != m_sl->data.end(); ++it){
        if((*it)->m_storyName.compare(sname) == 0)
        {
            m_sl->data.erase(it);
            break;
        }
    }
    //create new story
    Story * st = new Story(sname);
    std::string interpretationsString = ui->InterpretationsEdit->text().toStdString();

    std::list<std::string> ress;
    StoriesList::_getPossibilities(interpretationsString, ress);
    st->m_interpretationsList = ress;

    std::string data = ui->StoryTextEdit->toPlainText().toStdString();
    m_sl->processStoryText(sname,data,st);

    _PopulateWith(ui->StoriesList,m_sl);
    m_stl->addStory(st);
    m_stl->Sort();
    _PopulateWithX(ui->IntepretationsList,m_stl);
    ui->IntepretationsList->RedrawLineNumberArea();
}

void _saveStoryToFile(Story * st)
{
    std::fstream newfile;
    std::string fullfilename = "./trees/";
    fullfilename += st->m_storyName;
    newfile.open(fullfilename.c_str(), std::ios::out);

    std::string res;
    _fromListToLongString(st->m_interpretationsList,res,' ');
    newfile << "[ " << res << " ]" << std::endl;

    //ANOTHER USE OF BFS
    res.clear();
    std::queue<Node *> q;
    q.push(st->m_root);
    while (q.empty() == false) {
        Node *node = q.front();
        res += node->data;
        res += '\n';
        q.pop();
        if (node->left_child != NULL)
            q.push(node->left_child);
        if (node->right_child != NULL)
            q.push(node->right_child);
    }
    newfile << res;

    newfile.close();
}

void MainWindow::on_FlushButton_clicked()
{
    QDir absolutePath;
    QDir dir1(QString("./trees/"));
    if(!dir1.exists())
        absolutePath.mkdir(QString("trees"));
    QDir dir2(QString("./vsuvky/"));
    if(!dir2.exists())
        absolutePath.mkdir(QString("vsuvky"));
    for(Story * st: m_sl->data)
    {
        _saveStoryToFile(st);
    }
}

void MainWindow::on_GenerateButton_clicked()
{
    //generate a story

    std::string story;
    m_sl->GenerateOne(m_vl,story);

    const std::regex regexp("\\[[a-zA-Z _]*?\\]");

    while (std::regex_search(story, regexp)) {
        story = std::regex_replace(story, regexp, "");
    }

    QWidget *wdg = new QWidget;
    QPlainTextEdit * qle = new QPlainTextEdit(story.c_str(), wdg);
    qle->setReadOnly(true);
    QSize sz;
    sz.setHeight(666);
    sz.setWidth(666);
    qle->setFixedSize(sz);
    qle->show();
    wdg->setFixedSize(sz);
    wdg->show();
}

void MainWindow::on_IntepretationsList_itemDoubleClicked(QListWidgetItem *item)
{

    if(ui->IntepretationsList->wasRightClickPressed)
    {
        ui->IntepretationsList->wasRightClickPressed = false;
        QString tmp_str = ui->InterpretationsEdit->text();
        int cursorPos = ui->InterpretationsEdit->cursorPosition();
        if(cursorPos != 0)
            tmp_str += ',';
        tmp_str += item->text();
        ui->InterpretationsEdit->setText(tmp_str);
        return;
    }
    //else left click case
    QTextCursor  cursor = ui->StoryTextEdit->textCursor();
    QString interpretation;


    std::string formerText = ui->StoryTextEdit->toPlainText().toStdString();

    if((cursor.position() - 1) >= 0 && formerText[cursor.position() - 1] == ']'){
        cursor.setPosition(cursor.position() - 1);
        cursor.deleteChar(); // deletes the character at current cursor postion
        interpretation += ", ";
    }
    else
        interpretation += "[";

    interpretation += item->text();
    interpretation += "]";

    cursor.insertText(interpretation);
}

size_t MainWindow::GetTypeListOccurencesAt(size_t i)
{
    if((this->m_stl->data.size()-1) < i)
        return 0;
    auto itemIt = this->m_stl->data.begin();
    std::advance(itemIt, i);
    return itemIt->numberOfOccurences;

}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox mb;
    mb.setWindowTitle(QString("About"));
    mb.setText(QString("This app is a binary tree editor where data of a node are ascii strings\nApp can generate stories based on random traversal of a tree and there is a feature that adds insertions at the end of every tree node,\ntrees are stored each in separate file in a folder \"./trees/\" and insertions are in folder \"./vsuvky/\"\n every story and insertions file has tags and only insertions and stories with the same tag can be merged into a resulting story\nwith button save you save current story into memory\nwith button flush you save stories onto disk\nwith buttom generate program generates a random story\nduring generation there are multiple stories merged together so the resulting story is quite long\nENJOY!"));
    mb.exec();
}


void MainWindow::on_actionLicense_triggered()
{
    QMessageBox mb;
    mb.setWindowTitle(QString("License"));
    mb.setText(QString("GNU Lesser General Public License (LGPL)"));
    mb.exec();
}


void MainWindow::on_actionHelp_triggered()
{
    QMessageBox mb;
    mb.setWindowTitle(QString("Help"));
    mb.setText(QString("Binary trees are stored each in separate file in a folder \"./trees/\"\nInsertions are in folder \"./vsuvky/\"\n\
each story and each insertion file has tags and only insertions and stories with the same tag can be merged into a resulting story\n\
with button save you save current story into memory\n\
with button flush you save stories onto disk\n\
with buttom generate program generates a random story\n\
during generation there are multiple stories merged together so the resulting story is quite long\n\
list of tags is on the right\n\
list of stories is in the middle\n\
story editor is on the left"));
    mb.exec();
}


void MainWindow::on_actionWhite_triggered()
{
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    QPalette palette;
    this->setPalette(palette);
    this->ui->actionDark_attempt->setChecked(false);
}


void MainWindow::on_actionDark_attempt_triggered()
{
    this->ui->actionWhite->setChecked(false);
    QStyle * qs = QStyleFactory::create("Fusion");
    QApplication::setStyle(qs);

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(25, 25, 25));
    palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    palette.setColor(QPalette::ToolTipBase, Qt::black);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::HighlightedText, Qt::black);
    this->setPalette(palette);
}


void MainWindow::on_actionUndo_triggered()
{
    this->ui->StoryTextEdit->undo();
}


void MainWindow::on_actionRedo_triggered()
{
    this->ui->StoryTextEdit->redo();
}


void MainWindow::on_actionOpen_Stories_Directory_triggered()
{
    QDir aa = QDir::currentPath();
    aa.makeAbsolute();
    if(aa.cd(QString("./trees/")))
    {
        QUrl url = QUrl(aa.absolutePath(),QUrl::TolerantMode);
        QDesktopServices::openUrl(url);
    }
}


void MainWindow::on_actionOpen_Vsuvky_Directory_triggered()
{
    QDir aa = QDir::currentPath();
    aa.makeAbsolute();
    if(aa.cd(QString("./vsuvky/")))
    {
        QUrl url = QUrl(aa.absolutePath(),QUrl::TolerantMode);
        QDesktopServices::openUrl(url);
    }
}


void MainWindow::on_actionSave_Story_triggered()
{
    this->ui->SaveStoryButton->click();
}


void MainWindow::on_actionFlush_Stories_to_HDD_triggered()
{
    this->ui->FlushButton->click();
}


void MainWindow::on_actionExit_triggered()
{
    //TODO, check if everything is stored onto disk
    QCoreApplication::quit();
}


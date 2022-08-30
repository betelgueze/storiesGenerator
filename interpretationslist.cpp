#include "interpretationslist.h"
#include "lineNumberAreaX.h"
#include "mainwindow.h"

#include <QMainWindow>
#include <QPaintEvent>
#include <QPainter>

InterpretationsList::InterpretationsList(QWidget *parent) : QListWidget(parent), wasRightClickPressed(false)
{
    lineNumberArea = new LineNumberAreaX(this);

    connect(this, &InterpretationsList::currentRowChanged, this, &InterpretationsList::updateLineNumberAreaWidth);
    connect(this, &InterpretationsList::itemChanged, this, &InterpretationsList::updateLineNumberArea);

    updateLineNumberAreaWidth(0);
}

int InterpretationsList::lineNumberAreaWidth()
{
    int digits = 3;

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

void InterpretationsList::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void InterpretationsList::updateLineNumberArea(QListWidgetItem *item)
{
    lineNumberArea->update(0, 0, lineNumberArea->width(), this->height());
}

void InterpretationsList::resizeEvent(QResizeEvent *e)
{
    QListWidget::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void InterpretationsList::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::darkGray);
    int blockNumber = 0;
    int top = this->contentsMargins().top();
    //int fontSize =//visualItemRect(this->currentItem).top() - visualItemRect(this->currentItem).bottom();
    int itemSize = 24 ;
    int bottom = top + itemSize ;

    MainWindow * tmp_ptr = (MainWindow *)this->parent()->parent();

    for (int i=0; i< 99; ++i){

       QString number = QString::number(tmp_ptr->GetTypeListOccurencesAt(i));
       painter.setPen(Qt::black);
       painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                        Qt::AlignRight, number);

       top = bottom;
       bottom = top + itemSize;
   }
}


#ifndef INTERPRETATIONSLIST_H
#define INTERPRETATIONSLIST_H

#include <QLineEdit>
#include <QListWidget>
#include <QMouseEvent>
#include <QWidget>


class InterpretationsList : public QListWidget
{
    Q_OBJECT
public: //data members
    bool wasRightClickPressed;

public:
    InterpretationsList(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *event){
        if(event->button() == Qt::RightButton)
        {
            wasRightClickPressed = true;
            QListView::mousePressEvent(event);
        }
        else
            QListView::mousePressEvent(event);
    }
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

    void RedrawLineNumberArea(){this->lineNumberArea->repaint();}

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(QListWidgetItem *item);

private:
    QWidget *lineNumberArea;
};

#endif // INTERPRETATIONSLIST_H

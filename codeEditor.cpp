
#include "codeEditor.h"
#include "lineNumberArea.h"

#include <QPainter>
#include <QTextBlock>
#include <iterator>
#include <algorithm>
#include <QGridLayout>
#include <QLabel>

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea = new LineNumberArea(this);

    connect(this, &CodeEditor::blockCountChanged, this, &CodeEditor::updateLineNumberAreaWidth);
    connect(this, &CodeEditor::updateRequest, this, &CodeEditor::updateLineNumberArea);
    connect(this, &CodeEditor::cursorPositionChanged, this, &CodeEditor::highlightPreviousContextLine);

    updateLineNumberAreaWidth(0);
    highlightPreviousContextLine();
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightPreviousContextLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;
    std::string contextLineText;
    std::string text = this->toPlainText().toStdString();
    if (!isReadOnly() && text.length() != 0 ) {
        for(int i=3; i >= 0; --i){
            QTextEdit::ExtraSelection selection;

            QColor lineColor = QColor(0xFF, 0xA0, 0x00);

            selection.format.setBackground(lineColor);
            selection.format.setProperty(QTextFormat::FullWidthSelection, true);
            QTextCursor cp = textCursor();
            int posInBlock = cp.position();

            //find out line number we are on... count all newlinecharacters
            std::string::iterator it = text.begin();
            std::advance(it,posInBlock);
            int linesCountByNow = std::count(text.begin(), it, '\n');
            //get the context line number
            int lineToBeHighlighted = (linesCountByNow +1)/(2 << i);
            --lineToBeHighlighted;
            if(lineToBeHighlighted < 0)
                continue;

            //get position in text of it
            int numOfFoundNewlines = 0;
            int tmp_pos = 0;
            while(numOfFoundNewlines != lineToBeHighlighted)
            {
                if(text[tmp_pos] == '\n')
                    ++numOfFoundNewlines;
                tmp_pos++;
            }

            cp.setPosition(tmp_pos);
            selection.cursor = cp;/*get cursor on previous context line*///
            selection.cursor.clearSelection();
            extraSelections.append(selection);

            //setting up label of a context line for longer stories
            std::string::iterator itt = text.begin();
            std::advance(itt,tmp_pos);
            while(itt != text.end() && *itt != '\n'){
                contextLineText += *itt;
                ++itt;
            }
            contextLineText += ", ";
        }
        QGridLayout * tmp_ptr = (QGridLayout *)parentWidget();
        QLabel * labelToBeUpdated = (tmp_ptr)->findChild<QLabel*>("ContextLineLabel");
        labelToBeUpdated->setText(QString::fromStdString(contextLineText));
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::darkGray);
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    while (block.isValid() && top <= event->rect().bottom()) {
       if (block.isVisible() && bottom >= event->rect().top()) {
           QString number = QString::number(blockNumber + 1);
           painter.setPen(Qt::black);
           painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                            Qt::AlignRight, number);
       }

       block = block.next();
       top = bottom;
       bottom = top + qRound(blockBoundingRect(block).height());
       ++blockNumber;
   }
}

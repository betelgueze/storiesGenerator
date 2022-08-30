#ifndef LINENUMBERAREAX_H
#define LINENUMBERAREAX_H

#include "interpretationslist.h"

#include <QSize>

class LineNumberAreaX : public QWidget
{
public:
    LineNumberAreaX(InterpretationsList *editor) : QWidget(editor), codeEditor(editor)
    {}

    QSize sizeHint() const override
    {
        return QSize(codeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        codeEditor->lineNumberAreaPaintEvent(event);
    }

private:
    InterpretationsList *codeEditor;
};

#endif // LINENUMBERAREAX_H

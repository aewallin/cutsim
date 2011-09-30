#ifndef CODEEDITOR_H
#define CODEEDITOR_H


#include <QPlainTextEdit>
#include <QObject>
#include <QWidget>
#include <QVarLengthArray>
 
class QPaintEvent;
class QResizeEvent;
class QSize;
//class QWidget;

class LineNumberArea;

class TextArea : public QPlainTextEdit {
 Q_OBJECT
public:
    TextArea(QWidget *parent = 0);
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth()  {
        int digits = 1;
        int max = qMax(1, blockCount());
        while (max >= 10) {
         max /= 10;
         ++digits;
        }
        int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;
        return space;
    }
public slots:
    void appendLine(QString l) {
        lines.append(l);
        set_text();
    }
protected:
    void set_text() {
        QString text;
        for(int n=0;n<lines.size();n++) 
            text += lines[n]+"\n";
        
        setPlainText(text);
    }
    void resizeEvent(QResizeEvent *e)  {
     QPlainTextEdit::resizeEvent(e);

     QRect cr = contentsRect();
     lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
    }

private slots:
    void updateLineNumberAreaWidth(int newBlockCount) {// (int /* newBlockCount */)
     setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
    }

    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int dy);
private:
    QWidget *lineNumberArea;
    QVarLengthArray<QString> lines;
};


class LineNumberArea : public QWidget {
 Q_OBJECT
public:
    LineNumberArea(TextArea* area) : QWidget(area) {
        textArea = area;
    }
    QSize sizeHint() const {
        return QSize(textArea->lineNumberAreaWidth(), 0);
    }
protected:
    void paintEvent(QPaintEvent *event) {
        textArea->lineNumberAreaPaintEvent(event);
    }
private:
    TextArea *textArea;
};



#endif

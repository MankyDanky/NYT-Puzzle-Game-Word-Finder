#ifndef LETTEREDIT_H
#define LETTEREDIT_H
#include <QLineEdit>

enum LetterEditType {WordleAbsent, WordleCorrect, WordlePresent, SpellingBeeCenter, SpellingBeeSurrounding, LetterBoxed};

class LetterEdit : public QLineEdit
{
    Q_OBJECT

public:
    LetterEdit(QWidget *parent = nullptr, LetterEditType type = WordleAbsent, int index = 0, QWidget *next = nullptr, QWidget *prev = nullptr);
    virtual ~LetterEdit() {};

    void setNext(QWidget *next = nullptr);
    void setPrev(QWidget *prev = nullptr);

private:
    void keyPressEvent(QKeyEvent *event) override;
    QWidget *next;
    QWidget *prev;
    LetterEditType type;
    int index;
};

#endif // LETTEREDIT_H

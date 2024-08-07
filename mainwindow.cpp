#include "mainwindow.h"
#include "qevent.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QNetworkReply>
#include <QJsonArray>
#include <unordered_set>
#include <map>
#include <LetterEdit.h>

using namespace std;

// Functor to compare QString sizes
struct QStringSizeComparator {
    inline bool operator()(const QString& first, const QString& second) const {
        return first.size() > second.size();
    }
};

// Functor to join QString
struct QStringJoin {
    inline QString operator()(const QString& first, const QString& second) const {
        return first + ", " + second;
    }
};

// Declare wordle global variables
LetterEdit *absentLetterInputsHead;
LetterEdit *correctLetterInputsHead;
LetterEdit *presentLetterInputsHead;
unordered_set<QChar> absentLetters;
QChar presentLetters[5];
QChar correctLetters[5];

// Declare spelling bee global variables
LetterEdit *centerLetterInput;
LetterEdit *surroundingLetterInputsHead;
unordered_set<QChar> surroundingLetters;
QChar centerLetter;

// Declare letter boxed global variables
LetterEdit *sideLetterInputsHeads[4];
QHBoxLayout *sideLayouts[4];
unordered_set<QChar> eachSideLetters[4];
unordered_set<QChar> allSideLetters;
map<QChar, int> letterSideMap;


// Define LetterEdit constructor
LetterEdit::LetterEdit(QWidget *parent, LetterEditType type, int index, QWidget *next, QWidget *prev)
    :QLineEdit(parent)
{
    this->next = next;
    this->prev = prev;
    this->type = type;
    this->index = index;
    setAlignment(Qt::AlignHCenter);
    setStyleSheet("min-width: 30px;max-width: 30px;min-height: 30px;max-height: 30px;border: 4px solid white;border-radius: 10px;font: 700 14pt \"Arial\";color: white;");
}

// Define LetterEdit setNext function
void LetterEdit::setNext(QWidget *next) {
    this->next = next;
}

// Define LetterEdit setPrev function
void LetterEdit::setPrev(QWidget *prev) {
    this->prev = prev;
}

// Define LetterEdit key press event
void LetterEdit::keyPressEvent(QKeyEvent *event) {
    // Store entered letter in variable
    QChar letter;
    if (event->text().size() > 0) {
        letter = event->text().at(0).toUpper();
    }

    // Handle each input according to LetterEdit type
    switch (type) {
        case WordleAbsent:
            // Add entered letter to absent letters set or remove the entered letter if backspace was pressed
            if (letter >= 'A' && letter <= 'Z' && absentLetters.find(letter) == absentLetters.end()) {
                if (text().size() != 0) {
                    const QChar enteredLetter = text().at(0);
                    absentLetters.erase(enteredLetter);
                }
                setText(QString(letter));
                absentLetters.insert(letter);
                if (next != nullptr) {
                    next->setFocus();
                }
            } else if (event->key() == Qt::Key_Backspace) {
                if (text().size() != 0) {
                    const QChar enteredLetter = text().at(0);
                    absentLetters.erase(enteredLetter);
                    setText("");
                }
                if (prev != nullptr) {
                    prev->setFocus();
                }
            }
            break;
        case WordleCorrect:
            // Add entered letter to corrects letters array or remove the entered letter if backspace was pressed
            if (letter >= 'A' && letter <= 'Z') {
                setText(QString(letter));
                correctLetters[index] = letter;
                if (next != nullptr) {
                    next->setFocus();
                }
            } else if (event->key() == Qt::Key_Backspace) {
                if (text().size() != 0) {
                    correctLetters[index] = '\0';
                    setText("");
                }
                if (prev != nullptr) {
                    prev->setFocus();
                }
            }
            break;
        case WordlePresent:
            // Add entered letter to present letters array or remove the entered letter if backspace was pressed
            if (letter >= 'A' && letter <= 'Z') {
                setText(QString(letter));
                presentLetters[index] = letter;
                if (next != nullptr) {
                    next->setFocus();
                }
            } else if (event->key() == Qt::Key_Backspace) {
                if (text().size() != 0) {
                    presentLetters[index] = '\0';
                    setText("");
                }
                if (prev != nullptr) {
                    prev->setFocus();
                }
            }
            break;
        case SpellingBeeSurrounding:
            // Add entered letter to surrounding letters set or remove the entered letter if backspace was pressed
            if (letter >= 'A' && letter <= 'Z' && surroundingLetters.find(letter) == surroundingLetters.end() && letter != centerLetter) {
                if (text().size() != 0) {
                    const QChar enteredLetter = text().at(0);
                    surroundingLetters.erase(enteredLetter);
                }
                setText(QString(letter));
                surroundingLetters.insert(letter);
                if (next != nullptr) {
                    next->setFocus();
                }
            } else if (event->key() == Qt::Key_Backspace) {
                if (text().size() != 0) {
                    const QChar enteredLetter = text().at(0);
                    surroundingLetters.erase(enteredLetter);
                    setText("");
                }
                if (prev != nullptr) {
                    prev->setFocus();
                }
            }
            break;
        case SpellingBeeCenter:
            // Set entered letter as center letter or remove the entered letter if backspace was pressed
            if (letter >= 'A' && letter <= 'Z' && surroundingLetters.find(letter) == surroundingLetters.end()) {
                setText(QString(letter));
                centerLetter = letter;
            } else if (event->key() == Qt::Key_Backspace) {
                centerLetter = '\0';
                setText("");
            }
            break;
        case LetterBoxed:
            // Add entered letter to side letters set or remove the entered letter if backspace was pressed
            if (letter >= 'A' && letter <= 'Z' && allSideLetters.find(letter) == allSideLetters.end() && eachSideLetters[index].find(letter) == eachSideLetters[index].end()) {
                if (text().size() != 0) {
                    const QChar enteredLetter = text().at(0);
                    eachSideLetters[index].erase(enteredLetter);
                    allSideLetters.erase(enteredLetter);
                    letterSideMap.erase(enteredLetter);
                }
                setText(QString(letter));
                eachSideLetters[index].insert(letter);
                allSideLetters.insert(letter);
                letterSideMap[letter] = index;
                if (next != nullptr) {
                    next->setFocus();
                }
            } else if (event->key() == Qt::Key_Backspace) {
                if (text().size() != 0) {
                    const QChar enteredLetter = text().at(0);
                    eachSideLetters[index].erase(enteredLetter);
                    allSideLetters.erase(enteredLetter);
                    letterSideMap.erase(enteredLetter);
                    setText("");
                }
                if (prev != nullptr) {
                    prev->setFocus();
                }
            }
            break;
    }


}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create absent letter inputs and associated linked list
    absentLetterInputsHead = new LetterEdit(ui->wordleTab);
    LetterEdit *prevAbsentLetterInput = absentLetterInputsHead;
    ui->absentLettersLayout1->addWidget(absentLetterInputsHead);

    for (int i = 1; i < 24; ++i) {
        LetterEdit *currAbsentLetterInput = new LetterEdit(ui->wordleTab, WordleAbsent);
        if (i < 12) {
            ui->absentLettersLayout1->addWidget(currAbsentLetterInput);
        } else {
            ui->absentLettersLayout2->addWidget(currAbsentLetterInput);
        }

        currAbsentLetterInput->setPrev(prevAbsentLetterInput);
        prevAbsentLetterInput->setNext(currAbsentLetterInput);
        prevAbsentLetterInput = currAbsentLetterInput;
    }

    QSpacerItem *spacer = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);
    ui->absentLettersLayout1->addSpacerItem(spacer);
    ui->absentLettersLayout2->addSpacerItem(spacer);

    // Create correct letter inputs and associated linked list
    correctLetterInputsHead = new LetterEdit(ui->wordleTab, WordleCorrect, 0);
    LetterEdit *prevCorrectLetterInput = correctLetterInputsHead;
    ui->correctLettersLayout->addWidget(correctLetterInputsHead);
    for (int i = 1; i < 5; ++i) {
        LetterEdit *currCorrectLetterInput = new LetterEdit(ui->wordleTab, WordleCorrect, i);
        ui->correctLettersLayout->addWidget(currCorrectLetterInput);
        currCorrectLetterInput->setPrev(prevCorrectLetterInput);
        prevCorrectLetterInput->setNext(currCorrectLetterInput);
        prevCorrectLetterInput = currCorrectLetterInput;
    }

    ui->correctLettersLayout->addSpacerItem(spacer);

    // Create present letter inputs and associated linked list
    presentLetterInputsHead = new LetterEdit(ui->wordleTab, WordlePresent, 0);
    LetterEdit *prevPresentLetterInput = presentLetterInputsHead;
    ui->presentLettersLayout->addWidget(presentLetterInputsHead);
    for (int i = 1; i < 5; ++i) {
        LetterEdit *currPresentLetterInput = new LetterEdit(ui->wordleTab, WordlePresent, i);
        ui->presentLettersLayout->addWidget(currPresentLetterInput);
        currPresentLetterInput->setPrev(prevPresentLetterInput);
        prevPresentLetterInput->setNext(currPresentLetterInput);
        prevPresentLetterInput = currPresentLetterInput;
    }

    ui->presentLettersLayout->addSpacerItem(spacer);

    // Create center letter input
    centerLetterInput = new LetterEdit(ui->spellingBeeTab, SpellingBeeCenter);
    ui->centerLetterLayout->addWidget(centerLetterInput);
    ui->centerLetterLayout->addSpacerItem(spacer);

    // Create surrounding letter inputs and associated linked list
    surroundingLetterInputsHead = new LetterEdit(ui->spellingBeeTab, SpellingBeeSurrounding, 0);
    LetterEdit *prevSurroundingLetterInput = surroundingLetterInputsHead;
    ui->surroundingLetterLayout->addWidget(surroundingLetterInputsHead);
    for (int i = 1; i < 6; ++i) {
        LetterEdit *currSurroundingLetterInput = new LetterEdit(ui->spellingBeeTab, SpellingBeeSurrounding, i);
        ui->surroundingLetterLayout->addWidget(currSurroundingLetterInput);
        currSurroundingLetterInput->setPrev(prevSurroundingLetterInput);
        prevSurroundingLetterInput->setNext(currSurroundingLetterInput);
        prevSurroundingLetterInput = currSurroundingLetterInput;
    }

    ui->surroundingLetterLayout->addSpacerItem(spacer);

    // Create each side's letter inputs and associated linked list
    sideLayouts[0] = ui->side1Layout;
    sideLayouts[1] = ui->side2Layout;
    sideLayouts[2] = ui->side3Layout;
    sideLayouts[3] = ui->side4Layout;
    for (int i = 0; i < 4; ++i) {
        sideLetterInputsHeads[i] = new LetterEdit(ui->letterBoxedTab, LetterBoxed, i);
        LetterEdit *prevSideLetterInput = sideLetterInputsHeads[i];
        sideLayouts[i]->addWidget(prevSideLetterInput);
        for (int j = 1; j < 3; ++ j) {
            LetterEdit *currSideLetterInput = new LetterEdit(ui->letterBoxedTab, LetterBoxed, i);
            sideLayouts[i]->addWidget(currSideLetterInput);
            currSideLetterInput->setPrev(prevSideLetterInput);
            prevSideLetterInput->setNext(currSideLetterInput);
            prevSideLetterInput = currSideLetterInput;
        }

        sideLayouts[i]->addSpacerItem(spacer);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

QNetworkReply *reply;

void MainWindow::on_wordleSearchButton_clicked()
{
    // Declare and initialize query option strings
    QString correctSearch = "?????";
    QString absentSearch = "";
    QString presentSearch = "";

    // Add absent letters to query
    if (!absentLetters.empty()) {
        absentSearch += "-";
        for (auto iter = absentLetters.begin(), end = absentLetters.end(); iter != end; ++iter) {
            absentSearch += *iter;
        }
    }

    // Add present letters to query
    for (int i = 0; i < 5; i++) {
        if (presentLetters[i] != '\0') {
            presentSearch += ",*";
            presentSearch += presentLetters[i];
            presentSearch += "*";
        }
    }

    // Add correct letters to query
    for (int i = 0; i < 5; ++i) {
        if (correctLetters[i] != '\0') {
            correctSearch[i] = correctLetters[i];
        }
    }

    // Make query using Datamuse API
    QNetworkRequest request(QUrl("https://api.datamuse.com/words?max=1000&sp=" + correctSearch + presentSearch + absentSearch));
    reply = manager.get(request);
    connect(reply, &QIODevice::readyRead, this, &MainWindow::onWordleReply);
}

void MainWindow::on_spellingBeeSearchButton_clicked()
{
    // Return if necessary information isn't provided
    if (centerLetter == '\0' || surroundingLetters.size() != 6) {
        return;
    }

    // Declare and initialize query option strings
    QString centerSearch = "*" + QString(centerLetter.toLower()) + "*";
    QString absentSearch = "";

    // Add absent letters to query
    for (QChar c = 'a'; c <= 'z'; c = QChar(c.unicode() + 1)) {
        if (surroundingLetters.find(c.toUpper()) == surroundingLetters.end() && c.toUpper() != centerLetter) {
            absentSearch += c;
        }
    }

    // Make query using Datamuse API
    QNetworkRequest request(QUrl("https://api.datamuse.com/words?max=1000&sp=" + centerSearch + "-" + absentSearch));
    reply = manager.get(request);
    connect(reply, &QIODevice::readyRead, this, &MainWindow::onSpellingBeeReply);
}

void MainWindow::on_letterBoxedSearchButton_clicked()
{
    // Return if necessary information isn't provided
    if (allSideLetters.size() != 12) {
        return;
    }
    // Declare and initialize absent letter query option string
    QString absentSearch = "";

    // Add absent letters to query
    for (QChar c = 'a'; c <= 'z'; c = QChar(c.unicode() + 1)) {
        if (allSideLetters.find(c.toUpper()) == allSideLetters.end()) {
            absentSearch += c;
        }
    }

    // Make query using Datamuse API
    QNetworkRequest request(QUrl("https://api.datamuse.com/words?max=1000&sp=???*-" + absentSearch));
    reply = manager.get(request);
    connect(reply, &QIODevice::readyRead, this, &MainWindow::onLetterBoxedReply);
}

void MainWindow::onSpellingBeeReply()
{
    // Read JSON reply for all valid words
    QJsonDocument replyDocument = QJsonDocument::fromJson(reply->readAll());
    vector<QString> words;
    QJsonArray replyArray = replyDocument.array();
    for (auto i = replyArray.begin(), end = replyArray.end(); i != end; i++) {
        QJsonObject object = i->toObject();
        QString word = object.value("word").toString();
        if (word.size() > 3) {
            bool invalid = false;
            for (int i = 0; i < word.size(); ++i) {
                if (!('a' <= word.at(i) && word.at(i) <= 'z')) {
                    invalid = true;
                    break;
                }
            }
            if (!invalid) {
                words.push_back(word);
            }
        }
    }

    // Sort gathered words by size in descending order and display an accumulated string
    sort(words.begin(), words.end(), QStringSizeComparator());
    ui->spellingBeeResultsLabel->setText(accumulate(++words.begin(), words.end(), words[0], QStringJoin()));
}

void MainWindow::onWordleReply()
{
    // Read JSON reply for all valid words and display them
    QJsonDocument replyDocument = QJsonDocument::fromJson(reply->readAll());
    QString textUpdate = "";
    QJsonArray replyArray = replyDocument.array();
    for (auto i = replyArray.begin(), end = replyArray.end(); i != end; i++) {
        QJsonObject object = i->toObject();
        QString word = object.value("word").toString();
        bool invalid = false;
        for (int i = 0; i < word.size(); ++i) {
            if (word.at(i).toUpper() == presentLetters[i] || !('a' <= word.at(i) && word.at(i) <= 'z')) {
                invalid = true;
                break;
            }
        }
        if (!invalid) {
            textUpdate += word + ", ";
        }
    }
    textUpdate.remove(textUpdate.size()-2, 1);
    ui->wordleResultsLabel->setText(textUpdate);
}

void MainWindow::onLetterBoxedReply()
{
    // Read JSON reply for all valid words
    QJsonDocument replyDocument = QJsonDocument::fromJson(reply->readAll());
    vector<QString> words;
    QJsonArray replyArray = replyDocument.array();
    for (auto i = replyArray.begin(), end = replyArray.end(); i != end; i++) {
        QJsonObject object = i->toObject();
        QString word = object.value("word").toString();
        bool invalid = false;
        for (int i = 0; i < word.size(); ++i) {
            if (!('a' <= word.at(i) && word.at(i) <= 'z')) {
                invalid = true;
                break;
            }
            if (i > 0 && i < word.size() - 1 && (letterSideMap[word.at(i).toUpper()] == letterSideMap[word.at(i-1).toUpper()] || letterSideMap[word.at(i).toUpper()] == letterSideMap[word.at(i+1).toUpper()])) {
                invalid = true;
                break;
            }
        }
        if (!invalid) {
            words.push_back(word);
        }
    }

    // Sort gathered words by size in descending order and display an accumulated string
    sort(words.begin(), words.end(), QStringSizeComparator());
    ui->letterBoxedResultsLabel->setText(accumulate(++words.begin(), words.end(), words[0], QStringJoin()));
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onWordleReply();
    void onSpellingBeeReply();
    void onLetterBoxedReply();
    void on_wordleSearchButton_clicked();
    void on_spellingBeeSearchButton_clicked();
    void on_letterBoxedSearchButton_clicked();

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager manager;
};
#endif // MAINWINDOW_H

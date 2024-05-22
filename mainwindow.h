#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLineEdit;
class SliderBar;
class QToolButton;
class QComboBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QWidget*    _disp = nullptr;
    QWidget*    _area = nullptr;

    SliderBar*   _width = nullptr;
    SliderBar*   _height = nullptr;
    SliderBar*   _start = nullptr;
    SliderBar*   _end = nullptr;
    SliderBar*   _size = nullptr;
    SliderBar*   _space = nullptr;
    QToolButton* _color = nullptr;
    QComboBox*   _align = nullptr;
    QLineEdit*   _text = nullptr;
    QColor       _fontColor;

    void initDisp();
    void initArea();
    void drawDisp(QPainter& painter);

    inline void lowerLimit(qreal& val) {
        while (val < 0) {
            val += 1.0;

            if (qIsInf(val)) {
                val = 0.5;
            }
        }
    }

    inline void upperLimit(qreal& val) {
        while (val > 1.0) {
            val -= 1.0;

            if (qIsInf(val)) {
                val = 0.5;
            }
        }
    }

private slots:
    void redraw();

protected:
    bool eventFilter(QObject *obj, QEvent *ev) override;
};
#endif // MAINWINDOW_H

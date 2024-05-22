#ifndef SLIDERBAR_H
#define SLIDERBAR_H

#include <QWidget>

class QLabel;
class QSlider;
class QHBoxLayout;

class SliderBar : public QWidget
{
    Q_OBJECT
public:
    explicit SliderBar(const QString& title, QWidget* parent = nullptr);
    ~SliderBar();

    void setRange(int min, int max);
    void setValue(int value);
    int value() const;

private:
    QLabel*      _title = nullptr;
    QSlider*     _slider = nullptr;
    QLabel*      _value = nullptr;
    QHBoxLayout* _layout = nullptr;

signals:
    void sigValueChanged();
};

#endif // SLIDERBAR_H

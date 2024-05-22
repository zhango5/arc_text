#include "sliderbar.h"
#include <QLabel>
#include <QSlider>
#include <QHBoxLayout>

SliderBar::SliderBar(const QString& title, QWidget* parent)
    : QWidget(parent)
{
    _title = new QLabel(title, this);
    _slider = new QSlider(this);
    _value = new QLabel(this);
    _layout = new QHBoxLayout();

    _title->setFixedSize(50, 25);
    _slider->setFixedSize(140, 25);
    _value->setFixedHeight(25);

    _slider->setOrientation(Qt::Horizontal);

    _layout->addWidget(_title);
    _layout->addWidget(_slider);
    _layout->addWidget(_value);
    _layout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    _layout->setContentsMargins(4, 0, 4, 0);
    _layout->setSpacing(4);

    this->setLayout(_layout);

    connect(_slider, &QSlider::valueChanged, this, [this](int v) {
        _value->setText(QString::number(v));
        emit sigValueChanged();
    });
}

SliderBar::~SliderBar()
{

}

void SliderBar::setRange(int min, int max)
{
    _slider->setRange(min, max);
}

void SliderBar::setValue(int value)
{
    _slider->setValue(value);
    _value->setText(QString::number(value));
}

int SliderBar::value() const
{
    return _slider->value();
}

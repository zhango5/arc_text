#include "mainwindow.h"
#include "sliderbar.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QSlider>
#include <QComboBox>
#include <QToolButton>
#include <QEvent>
#include <QPainter>
#include <QFontMetrics>
#include <QColorDialog>
#include <QtMath>
#include <QFile>
#include <QListView>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowTitle("ArcText");
    this->setFixedSize(1024, 768);
    this->setCentralWidget(new QWidget);

    QFile fp(":/css/style.qss");
    if (fp.open(QIODevice::ReadOnly)) {
        this->setStyleSheet(fp.readAll());
        fp.close();
    }

    _disp = new QWidget(this->centralWidget());
    _area = new QWidget(this->centralWidget());

    _disp->setFixedSize(768, 768);
    _area->setFixedSize(1024 - 768, 768);

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->addWidget(_disp);
    hlayout->addWidget(_area);
    hlayout->setContentsMargins(0, 0, 0, 0);
    hlayout->setSpacing(0);

    this->centralWidget()->setLayout(hlayout);

    initDisp();
    initArea();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initDisp()
{
    _disp->installEventFilter(this);
}

void MainWindow::initArea()
{
    _width = new SliderBar("Width", _area);
    _height = new SliderBar("Height", _area);
    _start = new SliderBar("Start", _area);
    _end = new SliderBar("End", _area);
    _size = new SliderBar("Size", _area);
    _space = new SliderBar("Space", _area);
    _color = new QToolButton(_area);
    _align = new QComboBox(_area);
    _text = new QLineEdit(_area);

    _width->setFixedSize(240, 30);
    _height->setFixedSize(240, 30);
    _start->setFixedSize(240, 30);
    _end->setFixedSize(240, 30);
    _size->setFixedSize(240, 30);
    _space->setFixedSize(240, 30);
    _color->setFixedSize(32, 32);
    _align->setFixedSize(80, 30);
    _text->setFixedSize(240, 30);

    _width->setRange(1, 768);
    _height->setRange(1, 768);
    _start->setRange(-360, 360);
    _end->setRange(-360, 360);
    _size->setRange(8, 60);
    _space->setRange(0, 15);
    _color->setStyleSheet("background-color: #ffffff;");
    _align->setView(new QListView);
    _align->insertItem(0, "left", Qt::AlignLeft);
    _align->insertItem(1, "center", Qt::AlignHCenter);
    _align->insertItem(2, "right", Qt::AlignRight);

    QVBoxLayout* vlayout = new QVBoxLayout();
    QHBoxLayout* hlayout = new QHBoxLayout();

    hlayout->addWidget(_color);
    hlayout->addWidget(_align);
    hlayout->setContentsMargins(10, 0, 10, 0);
    hlayout->setSpacing(40);
    hlayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    vlayout->addWidget(_width);
    vlayout->addWidget(_height);
    vlayout->addWidget(_start);
    vlayout->addWidget(_end);
    vlayout->addWidget(_size);
    vlayout->addWidget(_space);
    vlayout->addLayout(hlayout);
    vlayout->addWidget(_text);
    vlayout->setContentsMargins(10, 20, 10, 20);
    vlayout->setSpacing(20);
    vlayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    _area->setLayout(vlayout);

    connect(_width, &SliderBar::sigValueChanged, this, &MainWindow::redraw);
    connect(_height, &SliderBar::sigValueChanged, this, &MainWindow::redraw);
    connect(_start, &SliderBar::sigValueChanged, this, &MainWindow::redraw);
    connect(_end, &SliderBar::sigValueChanged, this, &MainWindow::redraw);
    connect(_size, &SliderBar::sigValueChanged, this, &MainWindow::redraw);
    connect(_space, &SliderBar::sigValueChanged, this, &MainWindow::redraw);
    connect(_color, &QToolButton::clicked, this, [this]() {
        auto c = QColorDialog::getColor();
        if (c.isValid()) {
            _fontColor = c;
            _color->setStyleSheet(QString("background-color: %1;").arg(_fontColor.name()));
            redraw();
        }
    });
    connect(_align, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::redraw);
    connect(_text, &QLineEdit::textEdited, this, &MainWindow::redraw);

    _width->setValue(768);
    _height->setValue(768);
    _start->setValue(0);
    _end->setValue(360);
    _size->setValue(12);
    _space->setValue(0);
    _fontColor.setRgb(0xff, 0xff, 0xff);
    _text->setText("Test");
}

void MainWindow::drawDisp(QPainter& painter)
{
    painter.save();

    painter.setBrush(Qt::black);
    painter.drawRect(_disp->rect());

    QString content = _text->text();

    painter.setPen(_fontColor);
    painter.setBrush(_fontColor);
    QFont font = painter.font();
    font.setPointSize(_size->value());
    font.setLetterSpacing(QFont::AbsoluteSpacing, _space->value());

    QFontMetrics fm(font);
    int tw = fm.horizontalAdvance(content);
    int th = fm.height();
    int width = _width->value();
    int height = _height->value();
    int x = (768 - width) / 2;
    int y = (768 - height) / 2;
    int start = _start->value();
    int end = _end->value();
    int space = _space->value();
    int align = _align->currentData().toInt();
    bool reverse = (end < start);    // 反向，结束角度小于开始角度
    int len = content.length();
    qreal radius = width / 2;
    qreal sweep = (tw + len * space) * 180.0 / (M_PI * radius);    // 文本长度加字间距作为弧长，占总周长的角度，此处多算最后一个字间距
    qreal proportion = sweep / 360.0;
    qreal rotate = 0.0;
    double step = proportion / double(len);

    switch (align) {
    case Qt::AlignLeft:
        // default
        break;

    case Qt::AlignHCenter:
        start += (end - start - sweep + (space * 180.0 / (M_PI * radius))) / 2; // 减去末尾字间距
        break;

    case Qt::AlignRight:
        start = end - sweep + (space * 180.0 / (M_PI * radius)); // 减去末尾字间距
        break;
    default:
        break;
    }

    if (reverse) {
        rotate = 180.0; // 字体增加180°自转角度
        th = 0;
        step = -step; // 反向绘制时方向由正转负
    }

    QPainterPath rawPath;
    QPainterPath path;

    rawPath.addEllipse(QRect(x, y, width, height));

    qreal percent = -0.25 + start / 360.0; // 0指向3点钟方向，-0.25逆时针90°指向12点钟方向

    for (QChar const& c : content) {
        // 校正percent在0-1区间
        lowerLimit(percent);
        upperLimit(percent);

        double angle = rawPath.angleAtPercent(percent);
        QPointF point = rawPath.pointAtPercent(percent);

        QTransform mat;
        mat.rotate(-angle + rotate);
        QPainterPath p2;
        p2.addText(0, th, font, c);
        p2 = mat.map(p2);
        path.addPath(p2.translated(point));
        percent += step;
    }

    painter.drawPath(path.translated(x, y));

    painter.restore();
}

void MainWindow::redraw()
{
    _disp->update();
}

bool MainWindow::eventFilter(QObject* obj, QEvent* ev)
{
    if (obj == _disp) {
        if (ev->type() == QEvent::Paint) {
            QPainter painter(_disp);
            drawDisp(painter);
            return true;
        }
    }

    return QMainWindow::eventFilter(obj, ev);
}


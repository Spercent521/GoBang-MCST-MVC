// CircleButton.cpp
#include "CircleButton.h"
#include <QPainter>

CircleButton::CircleButton(QWidget *parent)
    : QPushButton(parent), m_state(Black)
{
    setFlat(true); // 移除按钮的默认外观
    setFixedSize(30, 30); // 设置固定大小确保圆形
}

CircleButton::CircleButton(int logicalX, int logicalY, QWidget *parent)
    : QPushButton(parent), m_state(Transparent),
    m_logicalX(logicalX), m_logicalY(logicalY)
{
    setFlat(true);
    setFixedSize(30, 30);

    connect(this, &QPushButton::clicked, [this]() {
        emit clickedWithLogicalPos(m_logicalX, m_logicalY);
    });
}

void CircleButton::setState(ButtonState state)
{
    if (m_state != state) {
        m_state = state;
        update(); // 触发重绘
    }
}

void CircleButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF rect = QRectF(0, 0, width(), height()).adjusted(1, 1, -1, -1);

    // 绘制背景（透明状态时不绘制）
    if (m_state != Transparent) {
        painter.setBrush(getCurrentColor());
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(rect);
    }
}

QColor CircleButton::getCurrentColor() const
{
    switch (m_state) {
    case Black: return Qt::black;
    case White: return Qt::white;
    default: return Qt::transparent;
    }
}

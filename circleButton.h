// CircleButton.h
#ifndef CIRCLEBUTTON_H
#define CIRCLEBUTTON_H

#include <QPushButton>
#include <QColor>

class CircleButton : public QPushButton
{
    Q_OBJECT
public:
    enum ButtonState { Black, White, Transparent };

    explicit CircleButton(int logicalX, int logicalY, QWidget *parent = nullptr);
    explicit CircleButton(QWidget *parent = nullptr); // 保留默认构造函数

    // 接受 ButtonState 参数并改变颜色
    // window output
    void setState(ButtonState state);
    ButtonState state() const { return m_state; }

signals:
    void clickedWithLogicalPos(int logicalX, int logicalY);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    ButtonState m_state;
    QColor getCurrentColor() const;

    int m_logicalX;
    int m_logicalY;
};

#endif // CIRCLEBUTTON_H

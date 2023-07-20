#ifndef GAME_H
#define GAME_H

#include <QWidget>
#include <QPainter> // чтобы обрисовать границы поля
#include <QList>
#include <QTimer>
#include <QKeyEvent>
#include <QRandomGenerator>

class SnakeItem;
class Snake;
class GameField;

class SnakeItem
{
    friend Snake;
    friend GameField;
public:
    SnakeItem(int x, int y);
private:
    int m_x;
    int m_y;
};

class Snake
{
    friend GameField;
public:
    Snake();
private:
    QList<SnakeItem*> m_snake_body;
    int m_snake_begin_size;
    enum SnakeDirection // определяем направление движения змейки
    {
        up,
        left,
        down,
        right
    };
    SnakeDirection m_snake_direction; // будет хранить напр движ-я
};

class GameField : public QWidget
{
    Q_OBJECT
public:
    GameField();
protected:
    void paintEvent(QPaintEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
private:
    Snake *m_snake;
    SnakeItem *m_food;
    int m_snake_item_size;
    QTimer *m_move_snake_timer;
    int m_field_size;
    int m_score;
    bool m_is_pause;
    bool m_is_move_blocked;
    bool m_is_game_over;

    void setGameStatus(bool is_Pause);// ставить и снимать игру с паузы
    void GameOver(); // в случае проигрыша
    void StartNewGame();
    void CreateFood();

private slots:
    void MoveSnakeSlot(); // логика движения змейки

signals:
    void ChangeTextSignal(QString text);
};

#endif // GAME_H

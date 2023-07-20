#include "game.h"

GameField::GameField()
{
    setFixedSize(300, 300);
    setFocusPolicy(Qt::StrongFocus); // активация виджета (реакция на нажатия клавиш)
    m_snake_item_size = 10; // размер элемента змейки
    m_field_size = width()/m_snake_item_size; // количество ячеек в игровом поле
    StartNewGame();
}

void GameField::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
    QBrush game_field_brush(QColor(50, 205, 50), Qt::SolidPattern);
    QBrush snake_brush(QColor(65, 105, 225), Qt::SolidPattern);
    QBrush food_brush(QColor(247, 103, 123), Qt::SolidPattern);

    QPainter painter;
    painter.begin(this);
    if(m_is_game_over)
    {
        painter.setPen(QColor(250, 250, 250));
        painter.setFont(QFont("Arial", 15, 700));
        painter.drawText(QRect(0, 0, width(), height()), Qt::AlignCenter, "Игра проиграна\nСчет: " + QString::number(m_score));
        return;
    }
    // отрисовка игрового поля
    painter.setBrush(game_field_brush);
    painter.setPen(QColor(50, 50, 50 ));
    painter.drawRect(0, 0, width()-1, height()-1);
    // отрисовка змейки
    painter.setBrush(snake_brush);
    painter.setPen(QColor(0, 0, 0 ));
    for(int i = 0; i < m_snake->m_snake_body.size(); i++)
    {
        painter.drawEllipse(m_snake->m_snake_body[i]->m_x * m_snake_item_size, m_snake->m_snake_body[i]->m_y * m_snake_item_size, m_snake_item_size, m_snake_item_size);
    }
    // отрисовка еды
    painter.setBrush(food_brush);
    painter.drawEllipse(m_food->m_x * m_snake_item_size, m_food->m_y * m_snake_item_size, m_snake_item_size, m_snake_item_size);
    painter.end();
    m_is_move_blocked = false ;

}

void GameField::keyPressEvent(QKeyEvent *e) // нажатия клавиш
{
    if(e->key() == Qt::Key_Space)
    {
        if(m_is_game_over)
        {
            StartNewGame();
            return;
        }
        m_is_pause = !m_is_pause;
        setGameStatus(m_is_pause);
    }
    if(m_is_move_blocked)
    {
        return;
    }
    if(e->key() == Qt::Key_Left && m_snake->m_snake_direction != Snake::SnakeDirection::right) // чтобы не поворачивалась на 180град
    {
        m_snake->m_snake_direction = Snake::SnakeDirection::left;
    }
    if(e->key() == Qt::Key_Right && m_snake->m_snake_direction != Snake::SnakeDirection::left)
    {
        m_snake->m_snake_direction = Snake::SnakeDirection::right;
    }
    if(e->key() == Qt::Key_Down && m_snake->m_snake_direction != Snake::SnakeDirection::up)
    {
        m_snake->m_snake_direction = Snake::SnakeDirection::down;
    }
    if(e->key() == Qt::Key_Up && m_snake->m_snake_direction != Snake::SnakeDirection::down)
    {
        m_snake->m_snake_direction = Snake::SnakeDirection::up;
    }
    m_is_move_blocked = true;
}

void GameField::setGameStatus(bool is_Pause) // ставить и снимать игру с паузы
{
    QString text;
    if(is_Pause)
    {
        m_move_snake_timer->stop();
        text = "Cчет : 0\nДля продолжения используйте: «ПРОБЕЛ»";
    }
    else
    {
        m_move_snake_timer->start(100);
        text = "Cчет : 0\nДля нажатия паузы используйте: «ПРОБЕЛ»";
    }
    emit ChangeTextSignal(text);
}

void GameField::GameOver()
{
    m_is_game_over = true;
    QString text = "Начать заново: «ПРОБЕЛ»";
    emit ChangeTextSignal(text);
    repaint();
    m_move_snake_timer->stop();
    delete m_snake;
    delete m_food;
    delete m_move_snake_timer;
}

void GameField::StartNewGame()
{
    m_is_pause = false;
    m_is_move_blocked = false;
    m_is_game_over = false;
    m_snake = new Snake();
    m_food = new SnakeItem(m_field_size/2, m_field_size/2);
    m_move_snake_timer = new QTimer();
    connect(m_move_snake_timer, &QTimer::timeout, this, &GameField::MoveSnakeSlot); // соединяем слот с таймером
    m_move_snake_timer->start(100); // запуск таймера
    m_score =  0;
    QString text = "Счет : " + QString::number(m_score) + "\nДля нажатия паузы используйте: «ПРОБЕЛ»";
    emit ChangeTextSignal(text);
}

void GameField::CreateFood()
{
    m_food->m_x = QRandomGenerator::global()->bounded(0, m_field_size - 1);
    m_food->m_y = QRandomGenerator::global()->bounded(0, m_field_size - 1);
    for(int i = 0; i < m_snake->m_snake_body.size(); i++)
    {
        if(m_food->m_x == m_snake->m_snake_body[i]->m_x && m_food->m_y == m_snake->m_snake_body[i]->m_y)
        {
            return CreateFood();
        }
    }
}

void GameField::MoveSnakeSlot() // движение змейки
{
    SnakeItem *new_snake_item;
    if(m_snake->m_snake_direction == Snake::SnakeDirection::right)
    {
        new_snake_item = new SnakeItem(m_snake->m_snake_body[0]->m_x + 1, m_snake->m_snake_body[0]->m_y);
    }
    else if(m_snake->m_snake_direction == Snake::SnakeDirection::left)
    {
        new_snake_item = new SnakeItem(m_snake->m_snake_body[0]->m_x - 1, m_snake->m_snake_body[0]->m_y);
    }
    else if(m_snake->m_snake_direction == Snake::SnakeDirection::up)
    {
        new_snake_item = new SnakeItem(m_snake->m_snake_body[0]->m_x, m_snake->m_snake_body[0]->m_y - 1);
    }
    else
    {
        new_snake_item = new SnakeItem(m_snake->m_snake_body[0]->m_x, m_snake->m_snake_body[0]->m_y + 1);
    }

    // ограничение игрового поля
    if(new_snake_item->m_x >= m_field_size) // вышла за пределы поля
    {
        new_snake_item->m_x = 0; // возвращаем в начало
    }
    else if(new_snake_item->m_x < 0)
    {
        new_snake_item->m_x = m_field_size - 1;
    }
    else if(new_snake_item->m_y < 0)
    {
        new_snake_item->m_y = m_field_size - 1;
    }
    else if(new_snake_item->m_y >= m_field_size)
    {
        new_snake_item->m_y = 0;
    }
    // случай проигрыша
    for(int i = 1; i < m_snake->m_snake_body.size(); i++)
    {
        if(m_snake->m_snake_body[0]->m_x == m_snake->m_snake_body[i]->m_x && m_snake->m_snake_body[0]->m_y == m_snake->m_snake_body[i]->m_y)
        {
            GameOver();
            return;
        }
    }
    // получение еды
    if(new_snake_item->m_x == m_food->m_x && new_snake_item->m_y == m_food->m_y)
    {
        m_score++;
        CreateFood();
        QString text = "Счет : " + QString::number(m_score) + "\nДля нажатия паузы используйте: «ПРОБЕЛ»";
        emit ChangeTextSignal(text);
    }
    else
    {
        m_snake->m_snake_body.removeLast();
    }
    m_snake->m_snake_body.insert(0, new_snake_item);
    repaint();
}

SnakeItem::SnakeItem(int x, int y)
{
    m_x = x;
    m_y = y;

}

Snake::Snake() // формирование модели змейки
{
    m_snake_begin_size = 4;
    for(int i = 0; i < m_snake_begin_size; i++)
    {
        m_snake_body.insert(0, new SnakeItem(i, 0));
    }
    m_snake_direction = SnakeDirection::down;
}

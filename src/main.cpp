#include <ncurses.h>

#include <iostream>

namespace
{
    static constexpr int WINDOW_WIDTH = 80, WINDOW_HEIGHT = 24;

    struct Paddle
    {
        static constexpr int height = 5;
        static constexpr char symbol = '#';
        int x, y;
    }
    left_paddle{4, WINDOW_HEIGHT >> 1}, right_paddle{WINDOW_WIDTH - 5, WINDOW_HEIGHT >> 1};

    struct Ball
    {
        static constexpr char symbol = '*';
        int x, y, dx, dy;
    }
    ball{WINDOW_WIDTH >> 1, WINDOW_HEIGHT >> 1, 1, -1};

    inline bool paddle_ball_collision(Paddle paddle, int fx, int fy) noexcept
    {
        return fx == paddle.x
            ?
               fy >= paddle.y && fy < paddle.y + Paddle::height
            ?
               (ball.dx = -ball.dx) : false : false;
    }

    void update_ball() noexcept
    {
        const auto fx = ball.x + ball.dx;
        const auto fy = ball.y + ball.dy;

        if (!(::paddle_ball_collision(left_paddle, fx, fy) || ::paddle_ball_collision(right_paddle, fx, fy)))
            ball.dx = fx ? fx == WINDOW_WIDTH  - 1 ? -1 : ball.dx : 1;

        ball.dy = fy ? fy == WINDOW_HEIGHT - 1 ? -1 : ball.dy : 1;

        ball.x += ball.dx;
        ball.y += ball.dy;
    }

    void update_right_paddle() noexcept
    {
        if (ball.x > right_paddle.x) return;

        const auto fby = ball.y + (right_paddle.x - ball.x) * ball.dy;

        if (fby >= 0 && fby < WINDOW_HEIGHT)
        {
            const auto half_paddle_height = Paddle::height >> 1;
            const auto fpy = right_paddle.y +
                (ball.y > right_paddle.y + half_paddle_height) -
                (ball.y < right_paddle.y + half_paddle_height);

            right_paddle.y = fpy ? fpy + Paddle::height == WINDOW_HEIGHT ? right_paddle.y : fpy : right_paddle.y;
        }
    }

    void keyboard(WINDOW* window, bool& running) noexcept
    {
        const int code = ::wgetch(window);
        
        switch (code)
        {
            case 'w':
                left_paddle.y = left_paddle.y - 1 ? left_paddle.y - 1 : left_paddle.y;
                break;
            case 's':
                left_paddle.y = left_paddle.y + Paddle::height == WINDOW_HEIGHT - 1 ? left_paddle.y : left_paddle.y + 1;
                break;
            case 'q':
                running = false;
                break;
        }
    }

    void render(WINDOW* window) noexcept
    {
        ::wclear(window);
        ::box(window, 0, 0);

        ::wmove(window, ball.y, ball.x);
        ::waddch(window, ball.symbol);

        for (int i = 0; i < Paddle::height; ++i)
        {
            ::wmove(window, left_paddle. y + i, left_paddle. x); ::waddch(window, Paddle::symbol);
            ::wmove(window, right_paddle.y + i, right_paddle.x); ::waddch(window, Paddle::symbol);
        }

        ::wrefresh(window);
    }
}

int main()
{
    if (::initscr())
    {
        ::curs_set(0);
        ::cbreak();
        ::noecho();

        WINDOW* const window = ::newwin(WINDOW_HEIGHT, WINDOW_WIDTH, 0, 0);
        if (window)
        {
            ::wtimeout(window, 100);

            bool running = true;
            while (running)
            {
                ::keyboard(window, running);
                ::update_ball();
                ::update_right_paddle();
                ::render(window);
            }

            ::delwin(window);
        }
        else
            std::cerr << "window creation error" << std::endl;

        ::endwin();
    }
    else
        std::cerr << "ncurses initialization error" << std::endl;

    return 0;
}

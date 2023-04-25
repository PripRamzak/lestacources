#include "engine.hxx"

#include <chrono>
#include <stdexcept>
#include <thread>

class RealGame : public Game
{
    int size;
    int progress;

public:
    RealGame()
        : size(15)
        , progress(0)
    {
    }
    void Update()
    {
        if (size < 3)
            throw std::runtime_error("Size less than 3");
        if (++progress > size - 1)
            progress = 0;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    void Render()
    {
        for (int i = 0; i < size; i++)
            std::cout << '\b';
        std::cout << '[';
        for (int i = 1; i < size - 1; i++)
            if (i < progress)
                std::cout << '#';
            else
                std::cout << ' ';
        std::cout << ']' << std::flush;
    }
};

Game::~Game() = default;

Game* CreateGame(Engine* engine)
{
    if (engine)
    {
        Game* game = new RealGame();
        return game;
    }
    return nullptr;
}

void DestroyGame(Game* game)
{
    delete game;
}

#pragma once

#include <iostream>

enum class Event
{
    up_pressed,
    up_released,
    down_pressed,
    down_released,
    left_pressed,
    left_released,
    right_pressed,
    right_released,
    button1_pressed,
    button1_released,
    button2_pressed,
    button2_released,
    select_pressed,
    select_released,
    start_pressed,
    start_released,
    turn_off,
    null_event
};

std::ostream& operator<<(std::ostream& out, const Event& event);

class Engine
{
public:
    virtual bool Initialize()        = 0;
    virtual bool ReadInput(Event& e) = 0;
    virtual void Uninitialize()      = 0;
    virtual ~Engine();
};

Engine* CreateEngine();
void    DestroyEngine(Engine* engine);

class Game
{
public:
    virtual void Update() = 0;
    virtual void Render() = 0;
    virtual ~Game();
};

extern "C" Game* CreateGame(Engine* engine);
extern "C" void  DestroyGame(Game* game);

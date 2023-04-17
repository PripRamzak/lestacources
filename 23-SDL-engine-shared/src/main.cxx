#include <engine.hxx>

#include <cstdlib>
#include <memory>

int main(int argc, char** argv)
{
    std::unique_ptr<Engine, void (*)(Engine*)> engine(CreateEngine(),
                                                      DestroyEngine);

    if (!engine->Initialize())
        return EXIT_FAILURE;

    bool  quit = false;
    Event event;

    while (!quit)
        if (engine->ReadInput(event))
        {
            std::cout << event << std::endl;
            if (event == Event::turn_off)
            {
                quit = true;
                break;
            }
        }

    engine->Uninitialize();

    return EXIT_SUCCESS;
}

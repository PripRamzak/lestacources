#include <iostream>

#include <SDL3/SDL_version.h>

std::ostream& operator<<(std::ostream& out, const SDL_version& v);

int main(int argc, char** argv)
{
    SDL_version compiled = { 0, 0, 0 };
    SDL_version linked   = { 0, 0, 0 };
    SDL_VERSION(&compiled)
    SDL_GetVersion(&linked);
    std::cout << "compiled: " << compiled << std::endl;
    std::cout << "linked: " << linked << std::endl;
    return std::cout.good() ? EXIT_SUCCESS : EXIT_FAILURE;
}

std::ostream& operator<<(std::ostream& out, const SDL_version& v)
{
    out << static_cast<int>(v.major) << '.';
    out << static_cast<int>(v.minor) << '.';
    out << static_cast<int>(v.patch);
    return out;
}

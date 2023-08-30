#include <SFML/Graphics.hpp>

#include "model.hpp"
#include "window.hpp"

class Controller {
public:
    Controller(Window& window) : window(window) {}

    void handleEvent(sf::Event& event) {
        if (event.type == sf::Event::Closed)
            window.close();
    }

private:
    Window& window;
};

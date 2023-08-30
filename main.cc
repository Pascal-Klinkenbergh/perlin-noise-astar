#include <cstdlib>

#include "controller.hpp"
#include "model.hpp"
#include "window.hpp"

int main() {
    srand(time(NULL));

    Model model(200, 150);
    Window window(model, 5);
    Controller controller(window, model);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            // handle all events by controller
            controller.handleEvent(event);
        }

        window.render();
    }

    return 0;
}

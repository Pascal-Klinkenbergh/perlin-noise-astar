#include <cstdlib>

#include "controller.hpp"
#include "model.hpp"
#include "window.hpp"

int main() {
    srand(time(NULL));

    Model model(4 * 50, 3 * 50);
    Window window(model, 6);
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

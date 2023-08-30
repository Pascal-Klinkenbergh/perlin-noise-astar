#include "model.hpp"
#include "window.hpp"
#include "controller.hpp"

int main() {
    Model model(200, 150);
    Window window(model, 5);

    Controller controller(window);

    model.fillPerlin(10);

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

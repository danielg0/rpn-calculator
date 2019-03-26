// main.cpp

#include "rpn.hpp"

#include <string>

#include <TGUI/TGUI.hpp>

int main() {
	// Create render window
	sf::RenderWindow window{{800, 600}, "RPN Calculator"};
	tgui::Gui gui{window};

	// Enter main window loop
	while(window.isOpen()) {
		sf::Event event;

		// Process all window events
		while(window.pollEvent(event)) {
			// Window closed
			if(event.type == sf::Event::Closed) {
				window.close();
			}

			// Send event to gui
			gui.handleEvent(event);
		}

		// Clear window and draw to it
		window.clear();
		gui.draw();
		window.display();
	}

	return 0;
}
// main.cpp

#include "rpn.hpp"

#include <TGUI/TGUI.hpp>

#include <iostream>

#include <algorithm>
#include <string>

// Define global variable holding current rpn statement
std::string STATEMENT = "";

// Define functions for different button pushes
void Enter(const sf::String& buttonText) {
	auto ops = RPN::Operators();
	// If an operator, push to statement surrounded by spaces
	if(std::find(ops.begin(), ops.end(), buttonText) != ops.end()) {
		 STATEMENT += " " + buttonText + " ";
	}
	// If an =, clear the statement
	else if(buttonText == "=") {
		STATEMENT = "";
	}
	else {
		STATEMENT += buttonText;
	}

	// TODO: update text and perform calculation, if any
	std::cout << STATEMENT << "\n";
}

int main() {
	// Create render window and attach gui
	sf::RenderWindow window {{300, 400}, "RPN Calculator",
		sf::Style::Titlebar | sf::Style::Close};
	window.setFramerateLimit(30);
	tgui::Gui gui {window};

	// Setup gui
	// Create layout to hold buttons
	auto layout = tgui::HorizontalLayout::create();
	layout->setSize("100%", "70%");
	layout->setPosition("0%", "30%");
	gui.add(layout);

	// Create rows of number buttons
	for(int i = 2; i >= 0; --i) {
		// Create vertical layout to hold buttons
		auto numberButtons = tgui::VerticalLayout::create();
		for(int j = 3; j <= 9; j += 3) {
			// Create button for this number
			auto button = tgui::Button::create(std::to_string(j - i));
			button->connect("pressed", Enter);
			numberButtons->add(button);
		}

		// Create a special button at the bottom
		// . 0 =
		// Decimal point
		if(i == 2) {
			auto button = tgui::Button::create(".");
			button->connect("pressed", Enter);
			numberButtons->add(button);
		}
		// Number 0
		else if(i == 1) {
			auto button = tgui::Button::create("0");
			button->connect("pressed", Enter);
			numberButtons->add(button);
		}
		// Equals sign (clears calculation)
		else {
			auto button = tgui::Button::create("=");
			button->connect("pressed", Enter);
			numberButtons->add(button);
		}

		// Add to root layout
		layout->add(numberButtons);
	}

	// Create row of operator buttons
	auto operatorButtons = tgui::VerticalLayout::create();
	for(auto i : RPN::Operators()) {
		auto button = tgui::Button::create(i);
		button->connect("pressed", Enter);
		operatorButtons->add(button);
	}

	// Add brackets buttons
	for(auto i : {"(", ")"}) {
		auto button = tgui::Button::create(i);
		button->connect("pressed", Enter);
		operatorButtons->add(button);
	}

	// Push all operator buttons to the root layout
	layout->add(operatorButtons);

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
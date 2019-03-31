// main.cpp

#include "rpn.hpp"

#include <TGUI/TGUI.hpp>

#include <iostream>

#include <algorithm>
#include <string>

// Define global variable holding current rpn statement and whether or not
// an update is needed
std::string STATEMENT = "";
bool UPDATE = true;

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

	UPDATE = true;
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

	// Create a text box to display the calculation, it in rpn and the result
	auto txtStatement = tgui::TextBox::create();
	txtStatement->setSize("100%", "10%");
	txtStatement->setReadOnly(true);
	gui.add(txtStatement);

	auto txtRpn = tgui::TextBox::create();
	txtRpn->setSize("100%", "10%");
	txtRpn->setPosition("0%", "10%");
	txtRpn->setReadOnly(true);
	gui.add(txtRpn);

	auto txtResult = tgui::TextBox::create();
	txtResult->setSize("100%", "10%");
	txtResult->setPosition("0%", "20%");
	txtResult->setReadOnly(true);
	gui.add(txtResult);

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
		// If update caused a change in the statement value and tripping of the
		// UPDATE flag
		if(UPDATE) {
			// Get result of calculation
			std::string result;
			std::string rpn;

			// Attempt to convert to RPN
			try {
				rpn = RPN::FromInfix(STATEMENT);
			}
			catch(const std::invalid_argument& e) {
				rpn = "Invalid";
			}

			try {
				// Attempt to evaluate the statement and store it in result
				result = std::to_string(RPN::Evaluate(rpn));
			
				// Remove trailing zeros
				result.erase(result.find_last_not_of('0') + 1,
					std::string::npos);
				result.erase(result.find_last_not_of('.') + 1,
					std::string::npos);
			}
			catch(const std::invalid_argument& e) {
				result = "Invalid";
			}

			// If statement is empty, set result to 0
			if(STATEMENT == "") {
				result = "0";
				rpn = "";
			}

			// Output result and STATEMENT to text objects
			txtStatement->setText(STATEMENT);
			txtRpn->setText(rpn);
			txtResult->setText(result);

			UPDATE = false;
		}

		// Clear window and draw to it
		window.clear();
		gui.draw();
		window.display();
	}

	return 0;
}

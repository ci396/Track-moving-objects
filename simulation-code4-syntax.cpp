#include "syntax.h"

int wires_number;
int component_number;

evl_statements statements_;

evl_wires_table wires_table;

evl_wires wires;

evl_components comps;

string module_name;

bool group_tokens_into_statements(evl_statements &statements, evl_tokens &tokens, string &module_name) {
	while (!tokens.empty())
	{ 
		evl_token token = tokens.front();

		if (token.type != evl_token::NAME)
		{
			cerr << "Need a NAME token but found \" << token.str << \" on line " << token.line_no << endl;
			return false;
		}
		
		if (token.str == "module")
		{
			evl_statement module;
			module.type = evl_statement::MODULE;
			
			while(!tokens.empty())
			{
				if (tokens.front().str != "module" && tokens.front().type == evl_token::NAME) {
					module_name = tokens.front().str;
				}
				module.tokens.push_back(tokens.front());

				tokens.pop_front(); // consume one token per iteration 
				if (module.tokens.back().str == ";")
					break; // exit if the ending ";" is found 
			}
			
			if (module.tokens.back().str != ";")
			{
				cerr << "Look for \";\" but reach the end of file" << endl; return false;
			}
			statements.push_back(module);
		}

		else if(token.str == "endmodule") {
			evl_statement endmodule;
			endmodule.type = evl_statement::ENDMODULE;
			endmodule.tokens.push_back(token);
			tokens.pop_front();
			statements.push_back(endmodule);
		}

		else if (token.str == "wire")
		{
			evl_statement wire;
			wire.type = evl_statement::WIRE;

			while(!tokens.empty())
			{
				if (tokens.front().type == evl_token::NAME&&tokens.front().str != "wire")
				{
					wires_number++;
				}
				wire.tokens.push_back(tokens.front());
				tokens.pop_front(); //consume one token per iteration

				if (wire.tokens.back().str == ";")
				{
					break;	  		//exit if the ending ";" is found
				}
			}
			
			if (wire.tokens.back().str != ";")
			{
				cerr << "Look for \";\" but reach the end of file " << endl;
				return false;
			}

			statements.push_back(wire);
		}

		else
		{
			evl_statement component;
			component.type = evl_statement::COMPONENT;

			//Thinking of a function to replace the loop?
			for (; !tokens.empty();)
			{
				component.tokens.push_back(tokens.front());
				tokens.pop_front(); //consume one token per iteration

				if (component.tokens.back().str == ";")
				{
					component_number++;
					break;	//exit if the ending ";" is found
				}
			}
			if (component.tokens.back().str != ";")
			{
				cerr << "Look for \";\" but reach the end of file " << endl;
				return false;
			}
			statements.push_back(component);
		}
	}
	return true;
}

bool process_wire_statement(evl_wires &wires, evl_statement &s)
{
	enum state_type { INIT, WIRE, DONE, WIRES, WIRE_NAME, BUS, BUS_MSB, BUS_COLON, BUS_DONE };
	
	state_type state = INIT;
	
	int bus_width = 1;
	
	for (; !s.tokens.empty() && (state != DONE); s.tokens.pop_front()) {
		evl_token t = s.tokens.front();
		
		if (state == INIT) {
			if (t.str == "wire") {
				state = WIRE;
			}
			else {
				cerr << "Need \"wire\" but found \"" << t.str << "\" on line " << t.line_no << endl;
				return false;
			}
		}

		else if (state == WIRE) {
			if (t.str == "[") {
				state = BUS;
			}
			else if (t.type == evl_token::NAME) {
				evl_wire wire;
				wire.name = t.str;
				wire.width = bus_width;
				wires.push_back(wire);
				state = WIRE_NAME;
			}
			else {
				cerr << "Need NAME but found \"" << t.str << "\" on line " << t.line_no << endl;
				return false;
			}
		}
		
		else if (state == WIRES) {
			if (t.type == evl_token::NAME) {
				evl_wire wire;
				wire.name = t.str;
				wire.width = bus_width;
				wires.push_back(wire);
				state = WIRE_NAME;
			}
			else {
				cerr << "Need NAME but found \"" << t.str << "\" on line " << t.line_no << endl;
				return false;
			}
		}
		
		else if (state == WIRE_NAME) {
			if (t.str == ",") {
				state = WIRES;
			}
			else if (t.str == ";")
			{
				bus_width = 1;
				state = DONE;
			}
			else return false;
		}

		else if (state == BUS) {
			if (t.type == evl_token::NUMBER) {
				bus_width = atoi(t.str.c_str()) + 1;
				state = BUS_MSB;
			}
			else return false;
		}
		
		else if (state == BUS_MSB) {
			if (t.str == ":") {
				state = BUS_COLON;
			}
			else return false;
		}
		
		else if (state == BUS_COLON)
		{
			if (t.type == evl_token::NUMBER) {
				bus_width = bus_width - atoi(t.str.c_str());
				state = BUS_DONE;
			}
			else return false;
		}

		else if (state == BUS_DONE)
		{
			if (t.str == "]")
			{
				state = WIRE;
			}
			else return false;
		}

	}
	
	if (!s.tokens.empty() || (state != DONE))
	{
		cerr << "something wrong with the statement" << endl;
		return false;
	}

	return true;
}

bool process_component_statement(evl_component &comp, evl_statement &s)
{
	enum state_type {INIT, TYPE, DONE, WIRES, NAME, PINS, PINS_NAME,PINS_DONE, BUS, BUS_MSB, BUS_COLON, BUS_DONE, BUS_LSB};

	state_type state = INIT;

	for (; !s.tokens.empty() && (state != DONE); s.tokens.pop_front())
	{
		evl_token t = s.tokens.front();
		if (state == INIT)
		{
			if (t.type == evl_token::NAME)
			{
				comp.type = t.str;
				comp.name = " ";
				state = TYPE;
			}
			else {
				cerr << "Need NAME but found \"" << t.str << "\" on line " << t.line_no << endl;
				return false;
			}
		}

		else if (state == TYPE)
		{
			if (t.str == "(") {
				state = PINS;
			}
			else if (t.type == evl_token::NAME)
			{
				comp.name = t.str;
				state = NAME;
			}

		}

		else if (state == NAME) {
			if (t.str == "(") {
				state = PINS;
			}
			else return false;
		}

		else if (state == PINS) {
			if (t.type == evl_token::NAME) {
				evl_pin pin;
				pin.name = t.str;
				pin.bus_lsb = -1;
				pin.bus_msb = -1;
				comp.pin.push_back(pin);
				state = PINS_NAME;
			}
			else return false;
		}

		else if (state == PINS_NAME) {
			if (t.str == ",")
			{
				state = PINS;
			}
			else if (t.str == "[")
			{
				state = BUS;
			}
			else if (t.str == ")")
			{
				state = PINS_DONE;
			}
			else {
				cerr << "Need PIN but found \"" << t.str << "\" on line " << t.line_no << endl;
				return false;
			}
		}

		else if (state == BUS) {
			comp.pin.back().bus_msb = atoi(t.str.c_str());
			state = BUS_MSB;
		}

		else if (state == BUS_MSB) {
			if (t.str == ":") {
				state = BUS_COLON;
			}
			else if (t.str == "]") {
				state = BUS_DONE;
			}
			else return false;
		}
		
		else if (state == BUS_COLON) {
			comp.pin.back().bus_lsb = atoi(t.str.c_str());
			state = BUS_LSB;
		}

		else if (state == BUS_LSB) {
			if (t.str == "]") {
				state = BUS_DONE;
			}
		}

		else if (state == BUS_DONE) {
			if (t.str == ")") {
				state = PINS_DONE;
			}
			else if (t.str == ",") {
				state = PINS;
			}
			else return false;
		}

		else if (state == PINS_DONE) {
			if (t.str == ";") {
				state = DONE;
			}
			else return false;
		}

		else return false;
	}

	return true;
}

void display_statements(ostream &output_file, evl_statements &statements, evl_wires &wires, evl_components &comps)
{
	evl_statements store;
	
	for (; !statements.empty(); statements.pop_front())
	{
		evl_statement top = statements.front();

		if (top.type == evl_statement::MODULE)
		{
			for (; !top.tokens.empty(); top.tokens.pop_front())
			{
				if (top.tokens.front().type == evl_token::NAME)
				{
					output_file << top.tokens.front().str << " ";
				}
			}
			output_file << " " << endl;
		}

		else if (top.type == evl_statement::ENDMODULE)
		{
			store.push_back(top);
		}

		else if (top.type == evl_statement::WIRE)
		{
			evl_wires wire;

			if (process_wire_statement(wire, top))
			{
				if (wires_number > 0)
				{
					output_file << "wires" << " " << wires_number << endl;
					wires_number = -1;
				}
				for (; !wire.empty(); wire.pop_front())
				{
					output_file << "wire" << " " << wire.front().name << " " << wire.front().width << endl;
					wires.push_back(wire.front());
				}
			}
		}

		else if (top.type == evl_statement::COMPONENT)
		{
			store.push_back(top);
		}
	}

	for (; !store.empty(); store.pop_front())
	{
		evl_statement top = store.front();
		
		if (top.type == evl_statement::ENDMODULE)
		{
			if (top.tokens.front().type == evl_token::NAME)
			{
				output_file << top.tokens.front().str << endl;
			}
		}

		else if (top.type == evl_statement::COMPONENT)
		{
			evl_component comp;

			if (!process_component_statement(comp, top))
			{
				cerr << comp.type << "false" << endl;
			}

			if (component_number > 0)
			{
				output_file << "components" << " " << component_number << endl;
				component_number = -1;
			}

			comps.push_back(comp);

			if (comp.name == " ")
			{
				output_file << "component" << " " << comp.type << " " << comp.pin.size() << endl;
			}
			
			else {
				output_file << "component" << " " << comp.type << " " << comp.name << " " << comp.pin.size() << endl;
			}
			
			for (; !comp.pin.empty(); comp.pin.pop_front())
			{
				cout << comp.pin.front().name << " " << comp.pin.front().bus_msb << " " << comp.pin.front().bus_lsb << endl;
				if (comp.pin.front().bus_lsb >= 0)
				{
					if (comp.pin.front().bus_msb < 0) {
						output_file << "  " << "pin " << comp.pin.front().name << " " << comp.pin.front().bus_lsb << endl;
					}
					else
					{
						output_file << "  " << "pin " << comp.pin.front().name << " " << comp.pin.front().bus_msb << " " << comp.pin.front().bus_lsb << endl;
					}
				}
				else if (comp.pin.front().bus_msb >= 0)
				{
					output_file << "  " << "pin " << comp.pin.front().name << " " << comp.pin.front().bus_msb << endl;
				}
				else {
					output_file << "  " << "pin " << comp.pin.front().name << endl;
				}
			}
		}
	}
}

bool store_statements_to_file(string file_name, evl_statements statements, evl_wires &wires, evl_components &comps) {
#ifdef SYNTAX
	ofstream output_file(file_name.c_str());
	
	if (!output_file)
	{
		cerr << "I can't write " << file_name << ".tokens ." << endl;
		return false;
	}
	display_statements(output_file, statements, wires, comps);
#else
	display_statements(cout, statements, wires, comps);
#endif
	return true;
}

bool parse_statements_from_file(string evl_file, evl_wires &wires, evl_components &comps) {
	
	parse_tokens_from_file(evl_file, tokens_);

	if (!group_tokens_into_statements(statements_, tokens_, module_name))
	{
		return false;
	}

	if (!store_statements_to_file(evl_file + ".syntax", statements_, wires, comps))
	{
		return false;
	}

	return true;
	
}

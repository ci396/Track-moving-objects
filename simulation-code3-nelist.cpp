#include "netlist.h"

//variable declaration
netlist nl;

void netlist::create_net(string net_name)
{
	assert(nets_table_.find(net_name) == nets_table_.end());
	net *n = new net(net_name);
	nets_table_[net_name] = n;
	nets_.push_back(n);
}

bool netlist::create_nets(const evl_wires &wires)
{
	for (evl_wires::const_iterator it = wires.begin(); it != wires.end(); ++it)
	{
		evl_wire tmp;
		if (it->width == 1)
		{
			create_net(it->name);
		}
		else
		{
			for (int i = 0; i < it->width; ++i)
			{
				string wire_name;
				wire_name = make_net_name(it->name, i);
				create_net(wire_name);
			}
		}
	}
	return true;
}

bool pin::create(gate *g, size_t index, const evl_pin &p, const map<string, net *> &nets_table)
{
	gate_ = g;
	index_ = index;
	
	if (index == 0)
	{
		dir_ = 'O';
	}
	else
	{
		dir_ = 'I';
	}
	
	if (p.bus_msb == -1)
	{
		string net_name = p.name;
		
		map<string, net *>::const_iterator samename = nets_table.find(net_name);
		
		if (samename == nets_table.end())
		{
			return false;
		}
		
		net_ = samename->second;
		
		net_->append_pin(this);
	}
	else 
	{ 
		if (p.bus_lsb == -1) 
		{
			string net_name = make_net_name(p.name, p.bus_msb);
			map<string, net *>::const_iterator samename = nets_table.find(net_name);
			if (samename == nets_table.end())
			{
				return false;
			}
			net_ = samename->second;
			net_->append_pin(this);
		}
		else
		{
			for (int i = p.bus_lsb; i <= p.bus_msb; i++)
			{
				string net_name = make_net_name(p.name, i);
				map<string, net *>::const_iterator samename = nets_table.find(net_name);

				if (samename == nets_table.end())return false;

				net_ = samename->second;
				net_->append_pin(this);
				bus_.push_back(net_);
			}
			map<string, net *>::const_iterator samename = nets_table.find(p.name);

			if (samename == nets_table.end())return false;
			
			net_ = samename->second;
		}
	}
	return true;	
}

void net::append_pin(pin *p)
{
	connections.push_back(p);
}

bool gate::create_pin(evl_pin &ep, size_t index, const map<string, net *>&nets_table, const evl_wires_table &wires_table)
{
	if (ep.bus_msb == -1) 
	{
		evl_wires_table::const_iterator same_name = wires_table.find(ep.name);
		if (same_name == wires_table.end())
		{
		    return false;
		}
		else
		{
			if (same_name->second > 1) 
			{
				ep.bus_msb = same_name->second;
				ep.bus_lsb = 0;
			}
		  	else 
		  	{
				ep.bus_lsb = -1;
				ep.bus_msb = -1;
		  	}
		}
	}
	pin *p = new pin();
	pins_.push_back(p);
	return p->create(this, index, ep, nets_table);
}

bool gate::create(evl_component &c, const map<string, net *> &nets_table,const evl_wires_table &wires_table)
{
	name_ = c.name;
	type_ = c.type;
	size_t index = 0;
	for (evl_pins::iterator it = c.pin.begin(); it != c.pin.end(); ++it)
	{
		create_pin(*it, index, nets_table, wires_table);
		++index;
	}
	return true;
}

bool netlist::create_gate(evl_component &c, const evl_wires_table &wires_table)
{
	gate *g = new gate;
	gates_.push_back(g);
	return g->create(c, nets_table_, wires_table);
}

bool netlist::create_gates(evl_components &comps, const evl_wires_table &wires_table)
{
	for (evl_components::const_iterator it = comps.begin(); it != comps.end(); ++it)
	{
		evl_component c = *it;
		create_gate(c, wires_table);
	}
	return true;
}

bool netlist::create(const evl_wires &wires, evl_components &comps, const evl_wires_table &wires_table)
{
	return create_nets(wires) && create_gates(comps, wires_table);
}

void netlist::save(string file_name, string module_name)
{
	ofstream output_file(file_name.c_str());
	if (!output_file)
	{
		cerr << "I can't write " << file_name.c_str() << ".netlist ." << endl;
	}
//----------------------------------------------------------------------------------------------------------	
	//output_file << "module top" << endl;
	evl_statement top = statements_.front();
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
//-----------------------------------------------------------------------------------------------------------
	if (nets_.size() > 0)
	{
		output_file << "nets" << " " << nets_.size() << endl;
		for (; !nets_.empty(); nets_.pop_front())
		{
			output_file << "net" << " " << nets_.front()->name << " "
				<< nets_.front()->connections.size() << endl;
			for (; !nets_.front()->connections.empty();
				nets_.front()->connections.pop_front())
			{
				if (nets_.front()->connections.front()->gate_->name_ == " ")
				{
					output_file << nets_.front()->connections.front()->gate_->type_
						<< " " << nets_.front()->connections.front()->index_ << endl;
				}
				else
				{
					output_file << nets_.front()->connections.front()->gate_->type_
						<< " " << nets_.front()->connections.front()->gate_->name_
						<< " " << nets_.front()->connections.front()->index_ << endl;
				}
			}
		}
	}
	if (gates_.size() > 0)
	{
		output_file << "components" << " " << gates_.size() << endl;
		for (; !gates_.empty(); gates_.pop_front())
		{
			if (gates_.front()->name_ == " ")
			{
				output_file << "component" << " " << gates_.front()->type_ << " "
					<< gates_.front()->pins_.size() << endl;
			}
			else
			{
				output_file << "component" << " " << gates_.front()->type_ << " "
					<< gates_.front()->name_ << " " << gates_.front()->pins_.size()
					<< endl;
			}
			for (size_t i = 0; i < gates_.front()->pins_.size(); i++)
			{
				if (gates_.front()->pins_[i]->bus_.empty())
				{
					output_file << "pin 1" << " " << gates_.front()->pins_[i]->net_->name
						<< endl;
				}
				else
				{
					output_file << "pin" << " " << gates_.front()->pins_[i]->bus_.size()
						<< " ";
					for (size_t j = 0; j < gates_.front()->pins_[i]->bus_.size(); j++)
					{
						output_file << gates_.front()->pins_[i]->bus_[j]->name
							<< " ";
					}
					output_file << " " << endl;
				}
			}
		}
	}
}

bool make_wires_table(const evl_wires &wires, evl_wires_table &wires_table)
{
	for (evl_wires::const_iterator it = wires.begin(); it != wires.end(); ++it)
	{
		evl_wires_table::iterator same_name = wires_table.find(it->name);
		if (same_name != wires_table.end())
		{
			cerr << "Wire " << it->name << " is already defined" << endl;
			return false;
		}
		wires_table.insert(make_pair(it->name, it->width));
	}
	return true;
}

string make_net_name(string wire_name, int i)
{
	assert(i >= 0);
	ostringstream oss;
	oss << wire_name << "[" << i << "]";
	return oss.str();
}

bool parse_netlists_from_file(string evl_file, evl_wires &wires, evl_components &comps) {

	if (!parse_statements_from_file(evl_file, wires, comps)) return false;

	if (!make_wires_table(wires, wires_table)) return false;

	if (!nl.create(wires, comps, wires_table)) return false;

#ifdef NETLIST
	//save the netlist for Project3
	nl.save(evl_file + ".netlist", module_name);
#endif

	return true;
}
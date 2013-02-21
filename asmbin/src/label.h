/*
    Copyright 2010 Matthew Mikolay

    This file is part of chasm.

    chasm is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    chasm is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with chasm.  If not, see <http://www.gnu.org/licenses/>.

*/

// A class to store label data
class label
{
	private:
		string name;			// A name to identify the label
		unsigned short address;		// The memory address associated
						// with the label name
	public:
		label(string paramName, unsigned short paramAddress);
		label();
		bool isValid();
		string getName();
		unsigned short getAddress();
		~label();
};

// Label constructor
label::label(string paramName, unsigned short paramAddress)
{
	name	= paramName;
	address	= paramAddress;
}

// Label constructor
label::label()
{
}

// Returns true if the label has a valid identifying name
// Labels cannot be numbers of any form, including binary, hexadecimal, and decimal.
// This would interfere with the jump command ("JMP") and prevent
// jumping to specific addresses.
bool label::isValid()
{
	return !isNumeric(name);
}

// Return the label name
string label::getName()
{
	return name;
}

// Return the label address
unsigned short label::getAddress()
{
	return address;
}

// Label destructor
label::~label(){ }

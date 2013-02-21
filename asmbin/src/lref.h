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

// A class to store label reference data
class lref
{
	private:
		string name;			// The name of the referenced label
		unsigned short address;		// The memory address of
						// this reference
	public:
		lref(string paramName, unsigned short paramAddress);
		lref();
		string getName();
		unsigned short getAddress();
		~lref();
};

// Label reference constructor
lref::lref(string paramName, unsigned short paramAddress)
{
	name	= paramName;
	address	= paramAddress;
}

// Label reference constructor
lref::lref()
{
}

// Return the label reference name
string lref::getName()
{
	return name;
}

// Return the label reference address
unsigned short lref::getAddress()
{
	return address;
}

// Label reference destructor
lref::~lref(){ }

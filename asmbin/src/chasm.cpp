// Standard C++ libraries
#include <fstream>
#include <iostream>

// chasm specific libraries
#include "chasm.h"
#include "label.h"
#include "lref.h"

/*

	XXXXXXXXXXXX  XXXX    XXXX  XXXXXXXXXXXX  XXXXXXXXXXXX  XXXXXXXXXXXXXXXXXXXX
	XXXXXXXXXXXX  XXXX    XXXX  XXXXXXXXXXXX  XXXXXXXXXXXX  XXXXXXXXXXXXXXXXXXXX
	XXXX    XXXX  XXXX    XXXX  XXXX    XXXX  XXXX          XXXX    XXXX    XXXX
	XXXX          XXXXXXXXXXXX  XXXXXXXXXXXX  XXXXXXXXXXXX  XXXX    XXXX    XXXX
	XXXX          XXXXXXXXXXXX  XXXXXXXXXXXX  XXXXXXXXXXXX  XXXX    XXXX    XXXX
	XXXX    XXXX  XXXX    XXXX  XXXX    XXXX          XXXX  XXXX    XXXX    XXXX
	XXXXXXXXXXXX  XXXX    XXXX  XXXX    XXXX  XXXXXXXXXXXX  XXXX    XXXX    XXXX
	XXXXXXXXXXXX  XXXX    XXXX  XXXX    XXXX  XXXXXXXXXXXX  XXXX    XXXX    XXXX

		                       VERSION 1.0

		    Dedicated to the amazingly sweet Melanie Ridgway. <3

		                          Love,
		                            Matt

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

int main(int argc, char *argv[])
{
	// Create needed variables
	unsigned short	start		= 0x200;	// Starting value of the program counter
	unsigned short	offset		= 0x000;	// Program counter offset from the starting address, initially zero
	unsigned char	memory[4096];			// Memory to store CHIP-8 machine code
	string		i_file	= "input.asm";		// The filename of the input file
	string		o_file	= "output.c8";		// The filename of the output file
	bool		eflag	= false;		// Signal if an error has occured

	// ***************************************
	// Begin processing command line arguments
	// ***************************************

	// Syntax: chasm <input filename> <output filename>

	// Make sure the correct number of command line arguments is present
	if(argc != 3)
	{
		// Return an error message
		eflag = error("chasm 1.0", 0, "Incorrect command line argument usage.\n\nSyntax:\n\tchasm <input filename> <output filename>");
		// Exit the program
		return false;
	}

	// Set the input and output filenames
	i_file = argv[1];
	o_file = argv[2];

	// **************************************
	// Begin reading data from the input file
	// **************************************

	// Open the source file for reading
	ifstream source;
	source.open(i_file.c_str(), ifstream::in);

	// Check if the source file has not been opened correctly
	if(!source.is_open())
	{
		// Return an error message
		eflag = error(i_file, 0, "File error. Could not open file \"" + i_file + "\" for input.");
		// Exit the program
		return false;
	}

	// Create arrays and variables to store label data and references
	const unsigned int list_labels_size = 256;	// The maximum number of labels capable of being processed by chasm
	const unsigned int list_lrefs_size  = 256;	// The maximum number of label references capable of being processed by chasm

	label list_labels[list_labels_size];		// An array to store labels
	lref  list_lrefs [list_lrefs_size];		// An array to store label references

	unsigned char	num_labels	= 0;		// The current number of labels stored in list_labels
	unsigned char	num_lrefs	= 0;		// The current number of label references stored in list_lrefs

	// Create a string to store line contents
	string line = "";

	// Create a string to store the current word of a line
	string word;

	// Create a string array to store the arguments
	// argument[0] is the command name (I.E. "SE", "DRW", "LD", etc.)
	// argument[1 ... 3] are command parameters (I.E. "V1", "#4A", etc.)
	// Note:	The current version of CHIP-8 assembly processed by chasm
	//		supports a maximum of four arguments, as no mnemonics
	//		process more than this number.
	string arguments[4];
	
	// Create an integer to store the number of arguments currently stored in the array
	unsigned char numArgs;

	// Create an integer to store the line number of the line being currently read
	unsigned int lineNumber = 0;

	// ************************************************
	// Begin processing the current line into arguments
	// ************************************************

	// Loop through the lines of the source file
	while(!source.eof())
	{
		// Get the next line from the source file
		getline(source, line);

		// Reset the value of the current word string and the number of arguments
		word	= "";
		numArgs	= 0;

		// Increase the line number
		lineNumber++;

		// Loop through the line, storing arguments and looking for labels
		for(unsigned char i = 0; i < line.length(); i++)
		{
			// If the current character is a semicolon, process the rest of the line as a comment
			if(line.at(i) == ';')
			{
				// If word has data in it, store it
				if(word.length() != 0)
				{
					if(!addArg(word, arguments, numArgs))
					{
						// Return an error message
						eflag = error(i_file, lineNumber, "Syntax error. Argument array overflow. Check line?");
						// Exit the program
						return false;
					}
				}

				// Stop processing the rest of the line
				break;
			}

			// If the current character is a colon, process the stored word as a label
			if(line.at(i) == ':')
			{
				// Make sure the colon was placed correctly
				if(word == "")
				{
					// Incorrect colon usage
					eflag = error(i_file, lineNumber, "Syntax error. Check colon usage.");
					// Read the next line
					break;
				}

				// Try to store the word
				if(!addArg(word, arguments, numArgs))
				{
					// Return an error message
					eflag = error(i_file, lineNumber, "Syntax error. Argument array overflow.");
					// Exit the program
					return false;
				}

				// Make sure only one word has been stored
				if(numArgs != 1)
				{
					// Return an error message
					eflag = error(i_file, lineNumber, "Syntax error. Check label identifier.");
					// Exit the program
					return false;
				}

				// Create a new label
				label myLabel = label(arguments[0], offset);

				// If the label has an invalid name, return an error
				if(!myLabel.isValid())
				{
					// Return an error message
					eflag = error(i_file, lineNumber, "Label error. Invalid label identifier.");
					// Exit the program
					return false;
				}

				// Make sure a label with the same name has not already been inserted into the array
				for(unsigned int curLblNum = 0; curLblNum < num_labels; curLblNum++)
				{
					if(myLabel.getName() == list_labels[curLblNum].getName())
					{
						// Return an error message
						eflag = error(i_file, lineNumber, "Syntax error. Label \"" + myLabel.getName() + "\" already exists.");
						// Exit the program
						return false;
					}					
				}

				// Make sure no array overflow will occur
				if(num_labels >= list_labels_size)
				{
					// Return an error message
					eflag = error(i_file, lineNumber, "Assembler error. Label array overflow. Maximum number of labels reached.");
					// Exit the program
					return false;
				}

				// Add the current label to the list for storage
				list_labels[num_labels] = myLabel;
				num_labels++;

				// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				// Enable the following line to signal the creation of a new label
				// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
				// cout << "Label \"" << arguments[0] << "\" created at address " << offset << "!" << endl;

				// Reset the value of word
				word = "";

				// Process the rest of the line as if the label has been removed
				numArgs = 0;

				continue;
			}

			// If the current character is a comma, and parameters are currently being read, store the word
			if(line.at(i) == ',' && word != "")
			{
				// Detect if the comma was unnecessary, as no other arguments have been stored yet
				if(numArgs == 0)
				{
					// Return an error message
					eflag = error(i_file, lineNumber, "Syntax error. Unnecessary comma?");
					// Exit the program
					return false;
				}

				// Store the current word
				if(!addArg(word, arguments, numArgs))
				{
					// Return an error message
					eflag = error(i_file, lineNumber, "Syntax error. Argument array overflow.");
					// Exit the program
					return false;
				}
			}

			// If the current character is not whitespace, add the uppercase version of the current character to the word
			else if(!isWhitespace(line.at(i)))
				word += toUpper(line.at(i));

			// If whitespace is encountered, word is not empty, and no arguments
			// Are currently stored, then store the first word as a command
			else if(word.length() != 0 && numArgs == 0 && isWhitespace(line.at(i)))
			{
				if(!addArg(word, arguments, numArgs))
				{
					// Return an error message
					eflag = error(i_file, lineNumber, "Syntax error. Argument array overflow. Check line?");
					// Exit the program
					return false;
				}
				continue;
			}

			// If this is the end of the line, and word contains data, store it depending upon if we are storing commands or parameters
			if(i == line.length() - 1)
			{
				// If the final character is a comma, return an error
				if(line.at(i) == ',')
				{
					// Return an error message
					eflag = error(i_file, lineNumber, "Syntax error. Unnecessary comma?");
					// Exit the program
					return false;
				}

				if(word != "")
				{
					// Store the current word
					if(!addArg(word, arguments, numArgs))
					{
						// Return an error message
						eflag = error(i_file, lineNumber, "Syntax error. Argument array overflow. Check line?");
						// Exit the program
						return false;
					}
				}
			}
		}

		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		// Enable the following lines of code to output all arguments to the screen
		// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		
		// cout << endl;
		// for(unsigned char a = 0; a < numArgs; a++)
		// 	cout << arguments[a] << "\t";
		

		// **********************************************************
		// Begin processing the arguments and generating machine code
		// **********************************************************

		// Check if at least one argument was entered
		if(numArgs > 0)
		{
			// Process all single argument commands
			if(numArgs == 1)
			{
				// CLS - 00E0
				if(arguments[0] == "CLS")
				{
					// Add corresponding machine code to memory
					memory[offset] = 0x00; offset++;
					memory[offset] = 0xE0; offset++;
					// Read the next line
					continue;
				}

				// RET - 00EE
				if(arguments[0] == "RET")
				{
					// Add corresponding machine code to memory
					memory[offset] = 0x00; offset++;
					memory[offset] = 0xEE; offset++;
					// Read the next line
					continue;
				}

				// Unknown command
				eflag = error(i_file, lineNumber, "Syntax error. Unknown command \"" + arguments[0] + "\".");
				// Read the next line
				continue;
			}

			// Process all double argument commands
			if(numArgs == 2)
			{
				// Macro: .START ADDR
				// Signifies start address
				if(arguments[0] == ".START")
				{
					// Make sure no other commands have been entered yet
					if(offset != 0)
					{
							// Return an error message
							eflag = error(i_file, lineNumber, "Assembler error. Input code attempts to modify start address after other commands have been processed.");
							// Exit the program
							return false;
					}

					unsigned short result = 0x0000;
					// Try to convert the second argument into an address
					if(!strTo12Bit(arguments[1], result))
					{
							// Return an error message
							eflag = error(i_file, lineNumber, "Syntax error. Invalid number \"" + arguments[1] + "\" designated as start address.");
							// Exit the program
							return false;
					}

					// Set the start address
					start = result;

					// Read the next line
					continue;
				}

				// DB BYTE
				// Store a byte
				if(arguments[0] == "DB")
				{
					unsigned char result = 0x00;
					// Try to convert the second argument into a byte
					if(!strToByte(arguments[1], result))
					{
						// Invalid argument
						eflag = error(i_file, lineNumber, "Syntax error. Invalid argument \"" + arguments[2] + "\". Expected an 8-bit number.");
						// Read the next line
						continue;
					}

					memory[offset] = result;
					offset++;

					// Read the next line
					continue;
				}

				// DW WORD
				// Store a word
				if(arguments[0] == "DW")
				{
					unsigned short result = 0x0000;
					// Try to convert the second argument into a byte
					if(!strToWord(arguments[1], result))
					{
						// Invalid argument
						eflag = error(i_file, lineNumber, "Syntax error. Invalid argument \"" + arguments[2] + "\". Expected a 16-bit number.");
						// Read the next line
						continue;
					}

					memory[offset] = (unsigned char)(result >> 8);		offset++;
					memory[offset] = (unsigned char)(result & 0x00FF);	offset++;

					// Read the next line
					continue;
				}

				// SYS ADDR - 0NNN
				if(arguments[0] == "SYS")
				{
					unsigned short result = 0x0000;
					// Try to convert the second argument into an address
					// If that fails, process the second argument as a label reference
					if(!strTo12Bit(arguments[1], result))
					{
						// Create a new label reference
						lref myLref = lref(arguments[1], offset);

						// Make sure no array overflow will occur
						if(num_lrefs >= list_lrefs_size)
						{
							// Return an error message
							eflag = error(i_file, lineNumber, "Assembler error. Label reference array overflow. Maximum number of label references reached.");
							// Exit the program
							return false;
						}

						// Add the current label to the list for storage
						list_lrefs[num_lrefs] = myLref;
						num_lrefs++;
					}

					// Add corresponding machine code to memory
					memory[offset] = 0x00 + ((result & 0x0F00) >> 8);	offset++;
					memory[offset] = 0x00 + (result & 0x00FF);		offset++;
					// Read the next line
					continue;
				}

				// JP ADDR - 1NNN
				if(arguments[0] == "JP")
				{
					unsigned short result = 0x0000;
					// Try to convert the second argument into an address
					// If that fails, process the second argument as a label reference
					if(!strTo12Bit(arguments[1], result))
					{
						// Create a new label reference
						lref myLref = lref(arguments[1], offset);

						// Make sure no array overflow will occur
						if(num_lrefs >= list_lrefs_size)
						{
							// Return an error message
							eflag = error(i_file, lineNumber, "Assembler error. Label reference array overflow. Maximum number of label references reached.");
							// Exit the program
							return false;
						}

						// Add the current label to the list for storage
						list_lrefs[num_lrefs] = myLref;
						num_lrefs++;
					}

					// Add corresponding machine code to memory
					memory[offset] = 0x10 + ((result & 0x0F00) >> 8);	offset++;
					memory[offset] = 0x00 + (result & 0x00FF);		offset++;
					// Read the next line
					continue;
				}

				// CALL ADDR - 2NNN
				if(arguments[0] == "CALL")
				{
					unsigned short result = 0x0000;
					// Try to convert the second argument into an address
					// If that fails, process the second argument as a label reference
					if(!strTo12Bit(arguments[1], result))
					{
						// Create a new label reference
						lref myLref = lref(arguments[1], offset);

						// Make sure no array overflow will occur
						if(num_lrefs >= list_lrefs_size)
						{
							// Return an error message
							eflag = error(i_file, lineNumber, "Assembler error. Label reference array overflow. Maximum number of label references reached.");
							// Exit the program
							return false;
						}

						// Add the current label to the list for storage
						list_lrefs[num_lrefs] = myLref;
						num_lrefs++;
					}

					// Add corresponding machine code to memory
					memory[offset] = 0x20 + ((result & 0x0F00) >> 8);	offset++;
					memory[offset] = 0x00 + (result & 0x00FF);		offset++;
					// Read the next line
					continue;
				}

				// SKP VX - EX9E
				if(arguments[0] == "SKP")
				{
					unsigned char byte = 0x00;
					// Try to convert the second argument into a register
					if(!strToRegister(arguments[1], byte))
					{
						// Unknown register
						eflag = error(i_file, lineNumber, "Syntax error. Unknown register \"" + arguments[1] + "\".");
						// Read the next line
						continue;
					}

					// Add corresponding machine code to memory
					memory[offset] = 0xE0 + byte;	offset++;
					memory[offset] = 0x9E;		offset++;
					// Read the next line
					continue;
				}

				// SKNP VX - EXA1
				if(arguments[0] == "SKNP")
				{
					unsigned char byte = 0x00;
					// Try to convert the second argument into a register
					if(!strToRegister(arguments[1], byte))
					{
						// Unknown register
						eflag = error(i_file, lineNumber, "Syntax error. Unknown register \"" + arguments[1] + "\".");
						// Read the next line
						continue;
					}

					// Add corresponding machine code to memory
					memory[offset] = 0xE0 + byte;	offset++;
					memory[offset] = 0xA1;		offset++;
					// Read the next line
					continue;
				}

				// SHR VX - 8XY6
				if(arguments[0] == "SHR")
				{
					unsigned char byte = 0x00;
					// Try to convert the second argument into a register
					if(!strToRegister(arguments[1], byte))
					{
						// Unknown register
						eflag = error(i_file, lineNumber, "Syntax error. Unknown register \"" + arguments[1] + "\".");
						// Read the next line
						continue;
					}

					// Add corresponding machine code to memory
					memory[offset] = 0x80 + byte;		offset++;
					memory[offset] = 0x06 + (byte << 4);	offset++;
					// Read the next line
					continue;
				}

				// SHL VX - 8XYE
				if(arguments[0] == "SHL")
				{
					unsigned char byte = 0x00;
					// Try to convert the second argument into a register
					if(!strToRegister(arguments[1], byte))
					{
						// Unknown register
						eflag = error(i_file, lineNumber, "Syntax error. Unknown register \"" + arguments[1] + "\".");
						// Read the next line
						continue;
					}

					// Add corresponding machine code to memory
					memory[offset] = 0x80 + byte;		offset++;
					memory[offset] = 0x0E + (byte << 4);	offset++;
					// Read the next line
					continue;
				}

				// Unknown command
				eflag = error(i_file, lineNumber, "Syntax error. Unknown command \"" + arguments[0] + " " + arguments[1] + "\".");
				// Read the next line
				continue;
			}

			// Process all triple argument commands
			if(numArgs == 3)
			{
				// SE VX, BYTE - 3XKK
				// SE VX, VY   - 5XY0
				if(arguments[0] == "SE")
				{
					unsigned char byte1 = 0x00;
					// Try to convert the second argument into a register
					if(!strToRegister(arguments[1], byte1))
					{
						// Unknown register
						eflag = error(i_file, lineNumber, "Syntax error. Unknown register \"" + arguments[1] + "\".");
						// Read the next line
						continue;
					}

					unsigned char byte2 = 0x00;
					// Try to convert the third argument into a byte
					if(strToByte(arguments[2], byte2))
					{
						byte1 += 0x30;
					}
					// Try to convert the third argument into a register
					else if(strToRegister(arguments[2], byte2))
					{
						byte1 += 0x50;
						byte2 = byte2 << 4;
					}
					else
					{
						// Invalid argument
						eflag = error(i_file, lineNumber, "Syntax error. Invalid argument \"" + arguments[2] + "\".");
						// Read the next line
						continue;
					}

					// Add corresponding machine code to memory
					memory[offset] = byte1;	offset++;
					memory[offset] = byte2;	offset++;
					// Read the next line
					continue;
				}

				// SNE VX, BYTE	- 4XKK
				// SNE VX, VY	- 9XY0
				if(arguments[0] == "SNE")
				{
					unsigned char byte1 = 0x00;
					// Try to convert the second argument into a register
					if(!strToRegister(arguments[1], byte1))
					{
						// Unknown register
						eflag = error(i_file, lineNumber, "Syntax error. Unknown register \"" + arguments[1] + "\".");
						// Read the next line
						continue;
					}

					unsigned char byte2 = 0x00;
					// Try to convert the third argument into a byte
					if(strToByte(arguments[2], byte2))
					{
						byte1 += 0x40;
					}
					// Try to convert the third argument into a register
					else if(strToRegister(arguments[2], byte2))
					{
						byte1 += 0x90;
						byte2 = byte2 << 4;
					}
					else
					{
						// Invalid argument
						eflag = error(i_file, lineNumber, "Syntax error. Invalid argument \"" + arguments[2] + "\".");
						// Read the next line
						continue;
					}

					// Add corresponding machine code to memory
					memory[offset] = byte1;	offset++;
					memory[offset] = byte2;	offset++;
					// Read the next line
					continue;
				}

				// ADD VX, BYTE	- 7XKK
				// ADD VX, VY	- 8XY4
				if(arguments[0] == "ADD" && arguments[1] != "I")
				{
					unsigned char byte1 = 0x00;
					// Try to convert the second argument into a register
					if(!strToRegister(arguments[1], byte1))
					{
						// Unknown register
						eflag = error(i_file, lineNumber, "Syntax error. Unknown register \"" + arguments[1] + "\".");
						// Read the next line
						continue;
					}

					unsigned char byte2 = 0x00;
					// Try to convert the third argument into a byte
					if(strToByte(arguments[2], byte2))
					{
						byte1 += 0x70;
					}
					// Try to convert the third argument into a register
					else if(strToRegister(arguments[2], byte2))
					{
						byte1 += 0x80;
						byte2 = (byte2 << 4) + 0x04;
					}
					else
					{
						// Invalid argument
						eflag = error(i_file, lineNumber, "Syntax error. Invalid argument \"" + arguments[2] + "\".");
						// Read the next line
						continue;
					}

					// Add corresponding machine code to memory
					memory[offset] = byte1;	offset++;
					memory[offset] = byte2;	offset++;
					// Read the next line
					continue;
				}

				// ADD I, VX - FX1E
				if(arguments[0] == "ADD" && arguments[1] == "I")
				{
					unsigned char byte = 0x00;
					// Try to convert the third argument into a register
					if(!strToRegister(arguments[2], byte))
					{
						// Unknown register
						eflag = error(i_file, lineNumber, "Syntax error. Unknown register \"" + arguments[2] + "\".");
						// Read the next line
						continue;
					}

					// Add corresponding machine code to memory
					memory[offset] = 0xF0 + byte;	offset++;
					memory[offset] = 0x1E;		offset++;
					// Read the next line
					continue;
				}

				// RND VX, BYTE - CXKK
				if(arguments[0] == "RND")
				{
					unsigned char byte1 = 0x00;
					// Try to convert the second argument into a register
					if(!strToRegister(arguments[1], byte1))
					{
						// Unknown register
						eflag = error(i_file, lineNumber, "Syntax error. Unknown register \"" + arguments[1] + "\".");
						// Read the next line
						continue;
					}

					unsigned char byte2 = 0x00;
					// Try to convert the third argument into a byte
					if(!strToByte(arguments[2], byte2))
					{
						// Invalid argument
						eflag = error(i_file, lineNumber, "Syntax error. Invalid argument \"" + arguments[2] + "\". Expected an 8-bit number.");
						// Read the next line
						continue;
					}

					// Add corresponding machine code to memory
					memory[offset] = 0xC0 + byte1;	offset++;
					memory[offset] = byte2;		offset++;
					// Read the next line
					continue;
				}

				// OR  VX, VY - 8XY1
				// AND VX, VY - 8XY2
				// XOR VX, VY - 8XY3
				if(arguments[0] == "OR" || arguments[0] == "AND" || arguments[0] == "XOR")
				{
					unsigned char byte1 = 0x00;
					// Try to convert the second argument into a register
					if(!strToRegister(arguments[1], byte1))
					{
						// Unknown register
						eflag = error(i_file, lineNumber, "Syntax error. Unknown register \"" + arguments[1] + "\".");
						// Read the next line
						continue;
					}

					unsigned char byte2 = 0x00;
					// Try to convert the third argument into a register
					if(!strToRegister(arguments[2], byte2))
					{
						// Invalid argument
						eflag = error(i_file, lineNumber, "Syntax error. Unknown register \"" + arguments[2] + "\".");
						// Read the next line
						continue;
					}

					// Shift byte2 left by four bits
					byte2 = byte2 << 4;

					// Determine what number to end the command with
					if(arguments[0] == "OR")
						byte2 += 0x01;
					if(arguments[0] == "AND")
						byte2 += 0x02;
					if(arguments[0] == "XOR")
						byte2 += 0x03;

					// Add corresponding machine code to memory
					memory[offset] = 0x80 + byte1;	offset++;
					memory[offset] = byte2;		offset++;
					// Read the next line
					continue;
				}

				// SUB VX, VY - 8XY5
				if(arguments[0] == "SUB")
				{
					unsigned char byte1 = 0x00;
					// Try to convert the second argument into a register
					if(!strToRegister(arguments[1], byte1))
					{
						// Unknown register
						eflag = error(i_file, lineNumber, "Syntax error. Unknown register \"" + arguments[1] + "\".");
						// Read the next line
						continue;
					}

					unsigned char byte2 = 0x00;
					// Try to convert the third argument into a register
					if(!strToRegister(arguments[2], byte2))
					{
						// Invalid argument
						eflag = error(i_file, lineNumber, "Syntax error. Unknown register \"" + arguments[2] + "\".");
						// Read the next line
						continue;
					}

					// Shift byte2 left by four bits
					byte2 = byte2 << 4;

					// Add corresponding machine code to memory
					memory[offset] = 0x80 + byte1;	offset++;
					memory[offset] = byte2 + 0x05;	offset++;
					// Read the next line
					continue;
				}

				// SUBN VX, VY - 8XY7
				if(arguments[0] == "SUBN")
				{
					unsigned char byte1 = 0x00;
					// Try to convert the second argument into a register
					if(!strToRegister(arguments[1], byte1))
					{
						// Unknown register
						eflag = error(i_file, lineNumber, "Syntax error. Unknown register \"" + arguments[1] + "\".");
						// Read the next line
						continue;
					}

					unsigned char byte2 = 0x00;
					// Try to convert the third argument into a register
					if(!strToRegister(arguments[2], byte2))
					{
						// Invalid argument
						eflag = error(i_file, lineNumber, "Syntax error. Unknown register \"" + arguments[2] + "\".");
						// Read the next line
						continue;
					}

					// Shift byte2 left by four bits
					byte2 = byte2 << 4;

					// Add corresponding machine code to memory
					memory[offset] = 0x80 + byte1;	offset++;
					memory[offset] = byte2 + 0x07;	offset++;
					// Read the next line
					continue;
				}

				// SHR VX, VY - 8XY6
				if(arguments[0] == "SHR")
				{
					unsigned char byte1 = 0x00;
					// Try to convert the second argument into a register
					if(!strToRegister(arguments[1], byte1))
					{
						// Unknown register
						eflag = error(i_file, lineNumber, "Syntax error. Unknown register \"" + arguments[1] + "\".");
						// Read the next line
						continue;
					}

					unsigned char byte2 = 0x00;
					// Try to convert the third argument into a register
					if(!strToRegister(arguments[2], byte2))
					{
						// Invalid argument
						eflag = error(i_file, lineNumber, "Syntax error. Unknown register \"" + arguments[2] + "\".");
						// Read the next line
						continue;
					}

					// Shift byte2 left by four bits
					byte2 = byte2 << 4;

					// Add corresponding machine code to memory
					memory[offset] = 0x80 + byte1;	offset++;
					memory[offset] = byte2 + 0x06;	offset++;
					// Read the next line
					continue;
				}

				// SHL VX, VY - 8XYE
				if(arguments[0] == "SHL")
				{
					unsigned char byte1 = 0x00;
					// Try to convert the second argument into a register
					if(!strToRegister(arguments[1], byte1))
					{
						// Unknown register
						eflag = error(i_file, lineNumber, "Syntax error. Unknown register \"" + arguments[1] + "\".");
						// Read the next line
						continue;
					}

					unsigned char byte2 = 0x00;
					// Try to convert the third argument into a register
					if(!strToRegister(arguments[2], byte2))
					{
						// Invalid argument
						eflag = error(i_file, lineNumber, "Syntax error. Unknown register \"" + arguments[2] + "\".");
						// Read the next line
						continue;
					}

					// Shift byte2 left by four bits
					byte2 = byte2 << 4;

					// Add corresponding machine code to memory
					memory[offset] = 0x80 + byte1;	offset++;
					memory[offset] = byte2 + 0x0E;	offset++;
					// Read the next line
					continue;
				}

				// JP V0, ADDR - BNNN
				if(arguments[0] == "JP")
				{
					// Make sure the second argument is the correct register
					if(arguments[1] != "V0")
					{
						// Unknown register
						eflag = error(i_file, lineNumber, "Syntax error. Invalid register \"" + arguments[1] + "\". Expected V0.");
						// Read the next line
						continue;
					}

					unsigned short result = 0x0000;
					// Try to convert the third argument into an address
					// If that fails, process the third argument as a label reference
					if(!strTo12Bit(arguments[2], result))
					{
						// Create a new label reference
						lref myLref = lref(arguments[2], offset);

						// Make sure no array overflow will occur
						if(num_lrefs >= list_lrefs_size)
						{
							// Return an error message
							eflag = error(i_file, lineNumber, "Assembler error. Label reference array overflow. Maximum number of label references reached.");
							// Exit the program
							return false;
						}

						// Add the current label to the list for storage
						list_lrefs[num_lrefs] = myLref;
						num_lrefs++;
					}

					// Add corresponding machine code to memory
					memory[offset] = 0xB0 + ((result & 0x0F00) >> 8);	offset++;
					memory[offset] = 0x00 + (result & 0x00FF);		offset++;
					// Read the next line
					continue;
				}

				// LD VX,  BYTE	- 6XKK
				// LD I,   ADDR	- ANNN
				// LD VX,  DT	- FX07
				// LD VX,  K	- FX0A
				// LD VX,  [I]	- FX65
				// LD VX,  VY	- 8XY0
				// LD DT,  VX	- FX15
				// LD ST,  VX	- FX18
				// LD F,   VX	- FX29
				// LD B,   VX	- FX33
				// LD [I], VX	- FX55
				if(arguments[0] == "LD")
				{
					unsigned char byte1 = 0x00;
					unsigned char byte2 = 0x00;

					// Try to convert the third argument into a register
					if(strToRegister(arguments[2], byte2))
					{
						// Try to convert the second argument into a register
						if(strToRegister(arguments[1], byte1))
						{
							byte1 += 0x80;

							// Shift byte 2 left by four bits
							byte2 = byte2 << 4;
						}
						else if(arguments[1] == "DT")
						{
							byte1 += 0xF0 + byte2;
							byte2 = 0x15;
						}
						else if(arguments[1] == "ST")
						{
							byte1 += 0xF0 + byte2;
							byte2 = 0x18;
						}
						else if(arguments[1] == "F")
						{
							byte1 += 0xF0 + byte2;
							byte2 = 0x29;
						}
						else if(arguments[1] == "B")
						{
							byte1 += 0xF0 + byte2;
							byte2 = 0x33;
						}
						else if(arguments[1] == "[I]")
						{
							byte1 += 0xF0 + byte2;
							byte2 = 0x55;
						}
						else
						{
							// Invalid argument
							eflag = error(i_file, lineNumber, "Syntax error. Invalid argument \"" + arguments[1] + "\".");
							// Read the next line
							continue;
						}
					}

					// Try to convert the second argument into a register
					else if(strToRegister(arguments[1], byte1))
					{
						unsigned short result = 0x0000;

						// Try to convert the third argument into a byte
						if(strToByte(arguments[2], byte2))
						{
							byte1 += 0x60;
						}
						else if(arguments[2] == "DT")
						{
							byte1 += 0xF0;
							byte2 = 0x07;
						}
						else if(arguments[2] == "K")
						{
							byte1 += 0xF0;
							byte2 = 0x0A;
						}
						else if(arguments[2] == "[I]")
						{
							byte1 += 0xF0;
							byte2 = 0x65;
						}
						else
						{
							// Invalid argument
							eflag = error(i_file, lineNumber, "Syntax error. Invalid argument \"" + arguments[2] + "\".");
							// Read the next line
							continue;
						}
					}

					else if(arguments[1] == "I")
					{
						unsigned short result = 0x0000;
						// Try to convert the third argument into an address
						// If that fails, process the third argument as a label reference
						if(!strTo12Bit(arguments[2], result))
						{
							// Create a new label reference
							lref myLref = lref(arguments[2], offset);

							// Make sure no array overflow will occur
							if(num_lrefs >= list_lrefs_size)
							{
								// Return an error message
								eflag = error(i_file, lineNumber, "Assembler error. Label reference array overflow. Maximum number of label references reached.");
								// Exit the program
								return false;
							}

							// Add the current label to the list for storage
							list_lrefs[num_lrefs] = myLref;
							num_lrefs++;
						}

						byte1 = 0xA0 + ((result & 0x0F00) >> 8);
						byte2 = 0x00 + (result & 0x00FF);
					}

					// Unknown command
					else
					{
						eflag = error(i_file, lineNumber, "Syntax error. Unknown command \"" + arguments[0] + " " + arguments[1] + ", " + arguments[2] + "\".");
					}

					// Add corresponding machine code to memory
					memory[offset] = byte1;	offset++;
					memory[offset] = byte2;	offset++;
					// Read the next line
					continue;
				}

				// Unknown command
				eflag = error(i_file, lineNumber, "Syntax error. Unknown command \"" + arguments[0] + " " + arguments[1] + ", " + arguments[2] + "\".");
				// Read the next line
				continue;
			}

			// Process all quadruple argument commands
			if(numArgs == 4)
			{
				// DRW VX, VY, NIBBLE - DXYN
				if(arguments[0] == "DRW")
				{
					unsigned char byte1 = 0x00;
					// Try to convert the second argument into a register
					if(!strToRegister(arguments[1], byte1))
					{
						// Unknown register
						eflag = error(i_file, lineNumber, "Syntax error. Unknown register \"" + arguments[1] + "\".");
						// Read the next line
						continue;
					}

					unsigned char byte2 = 0x00;
					// Try to convert the third argument into a register
					if(!strToRegister(arguments[2], byte2))
					{
						// Unknown register
						eflag = error(i_file, lineNumber, "Syntax error. Unknown register \"" + arguments[2] + "\".");
						// Read the next line
						continue;
					}

					unsigned char nibble = 0x00;
					// Try to convert the third argument into a nibble
					if(!strToNibble(arguments[3], nibble))
					{
						// Invalid argument
						eflag = error(i_file, lineNumber, "Syntax error. Invalid argument \"" + arguments[3] + "\". Expected a 4-bit number.");
						// Read the next line
						continue;
					}

					// Add corresponding machine code to memory
					memory[offset] = 0xD0 + byte1;		offset++;
					memory[offset] = (byte2 << 4) + nibble;	offset++;
					// Read the next line
					continue;
				}

				// Unknown command
				eflag = error(i_file, lineNumber, "Syntax error. Unknown command \"" + arguments[0] + " " + arguments[1] + ", " + arguments[2] + ", " + arguments[3] + "\".");
				// Read the next line
				continue;
			}
		}
	}

	// ******************************************
	// Begin linking label references with labels
	// ******************************************

	// Loop through the array of label references
	for(unsigned char i_lref; i_lref < num_lrefs; i_lref++)
	{
		// The current label reference
		lref current_lref = list_lrefs[i_lref];

		// Locate the label being referenced by this label reference
		bool found = false;
		unsigned char i_label;
		for(i_label = 0; i_label < num_labels; i_label++)
		{
			if(current_lref.getName() == list_labels[i_label].getName())
			{
				found = true;
				break;
			}
		}

		// If it is not found, output an error
		if(!found)
		{
			// Output an error
			eflag = error(i_file, 0, "Label error. Label \"" + current_lref.getName() + "\" not found.");
			// Exit the program
			return false;
		}

		// The current label
		label current_label = list_labels[i_label];

		// Jump to the offset address given by the label reference and insert the address designated by the corresponding label
		memory[current_lref.getAddress()]	+= ((current_label.getAddress() + start) & 0x0F00) >> 8;
		memory[current_lref.getAddress() + 1]	+= ((current_label.getAddress() + start) & 0x00FF);
	}

	// Close the source file
	source.close();

	// *****************************************
	// Output all CHIP-8 code to the output file
	// *****************************************

	// Has an error occured?
	if(!eflag)
	{
		// Open the output file for writing
		ofstream output;
		output.open(o_file.c_str(), ofstream::out);

		// Check if the output file has not been opened correctly
		if(!output.is_open())
		{
			// Return an error message
			error(o_file, 0, "File error. Could not open file \"" + o_file + "\" for output.");
			// Exit the program
			return false;
		}
		// Output all the generated machine code to the file
		for(unsigned int current_byte = 0; current_byte < offset; current_byte++)
		{
			output << memory[current_byte];
		}

		// Close the output file
		output.close();
	}

	// ++++++++++++++++++++++++++++++++++++++++++++
	// Enable the following lines of code to output
	// all generated machine code to the screen++++
	// ++++++++++++++++++++++++++++++++++++++++++++
	/*
	cout << "###################" << endl;
	cout << "Machine Code Output" << endl;
	cout << "###################" << endl;

	if(offset > 0) {
	for(unsigned int z = 0; z < offset - 1; z += 2)
		printf ("#%03X \t %02X %02X \n", z + start, memory[z], memory[z+1]);
	}
	*/

	return 0;
}

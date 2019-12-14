#include <stdio.h>
#include <iostream>
#include <cstring>
using namespace std;

class sudoku {
public:
	
	sudoku();
	
	bool LoadFromFile(char * Filename, bool XFactor);
	
	bool Validate(bool XFactor);
	
	bool Set(unsigned short x, unsigned short y, unsigned short val, bool XFactor);
	
	void SetRaw(unsigned short x, unsigned short y, unsigned short val, unsigned long long PossibVal);
	
	bool Search(bool XFactor, unsigned long long * solutions, unsigned long long * Guesses);
	
	bool Complete();
	
	void Display();
	
private:
	
	unsigned short Table[9][9];
	unsigned long long TablePossib[9][9];
};

int main(int argc, char * argv[])
{
	sudoku root_node;
	unsigned long n = 0;
	unsigned long long guesses = 0, solutions = 0;
	bool XFactor = false;
	if (argc < 2) {
		cout << "Needs filename as argument\n";
		return 0;
	}
	if (argc > 2) {
		if (!strcmp(argv[2], "X")) {
			XFactor = true;
		} else {
			cout << "Second argument != 'X', ignoring\n";
		}
	}
	if (root_node.LoadFromFile(argv[1], XFactor)) {
		root_node.Display();
		if (root_node.Validate(XFactor)) {
			while (root_node.Search(XFactor, &solutions, &guesses)) {
// 				root_node.Display();
				n++;
			}
			cout << "-----------\n";
			if (guesses) {
				cout << "A total of " << solutions << " solution" << (solutions > 1 ? "s" : "") << " was found with " << guesses << " guesses.\n";
			} else if (root_node.Complete()) {
				cout << "Finished in " << n << " passes, ";
				if (root_node.Validate(XFactor)) {
					cout << "Validated.\n\n";
				} else {
					cout << "FINAL VALIDATION FAILURE!!!\n\n";
				}
			} else {
				cout << "Don't know what to do after " << n << " passes!\n\n";
				if (!root_node.Validate(XFactor)) {
					cout << "FINAL VALIDATION FAILURE!!!\n\n";
				}
			}
			if (!guesses) {
				root_node.Display();
			}
		}
	}
	return 0;
}


sudoku::sudoku()
{
	unsigned short x, y;
	for (y = 0; y < 9; y++) {
		for (x = 0; x < 9; x++) {
			Table[x][y] = 0;
			TablePossib[x][y] = 0x00000000000001ff;
		}
	}
}

bool sudoku::LoadFromFile(char * Filename, bool XFactor)
{
	unsigned short x, y;
	char c;
	FILE * InF = fopen(Filename, "r");
	if (InF) {
		for (y = 0; y < 9; y++) {
			for (x = 0; x < 9; x++) {
				do {
					c = fgetc(InF);
				} while (c == '\n');
				if (c == EOF) break;
				if (c == '?') {
// 					Set(x, y, 0, XFactor);
// 					Table[x][y] = 0;
				} else if (c >= '0' && c <= '9') {
					Set(x, y, c - '0', XFactor);
// 					Table[x][y] = c - '0';
				} else {
					cerr << "Input file not in correct format!\n";
					return false;
				}
			}
		}
		fclose(InF);
	} else {
		cerr << "Error opening file\n";
		return false;
	}
	return true;
}

bool sudoku::Validate(bool XFactor)
{
	bool Result = true;
	unsigned short x, y, x1, y1;
	unsigned short u[9];
	unsigned long long c;
	//rows
	for (y = 0; y < 9; y++) {
		for (c = 0; c < 9; c++) {
			u[c] = 0;
		}
		for (x = 0; x < 9; x++) {
			if (Table[x][y]) {
				u[Table[x][y]-1]++;
			}
		}
		for (c = 0; c < 9; c++) {
			if (u[c] > 1) {
				Result = false;
				cout << "VALIDATION FAILED: " << u[c] << "x" << c + 1 << " in row " << y + 1 << '\n';
			}
		}
	}
	//cols
	for (x = 0; x < 9; x++) {
		for (c = 0; c < 9; c++) {
			u[c] = 0;
		}
		for (y = 0; y < 9; y++) {
			if (Table[x][y]) {
				u[Table[x][y]-1]++;
			}
		}
		for (c = 0; c < 9; c++) {
			if (u[c] > 1) {
				Result = false;
				cout << "VALIDATION FAILED: " << u[c] << "x" << c + 1 << " in column " << x + 1 << '\n';
			}
		}
	}
	//3x3
	for (y1 = 0; y1 < 3; y1++) {
		for (x1 = 0; x1 < 3; x1++) {
			for (c = 0; c < 9; c++) {
				u[c] = 0;
			}
			for (y = y1 * 3; y < (y1 + 1) * 3; y++) {
				for (x = x1 * 3; x < (x1 + 1) * 3; x++) {
					if (Table[x][y]) {
						u[Table[x][y]-1]++;
					}
				}
			}
			for (c = 0; c < 9; c++) {
				if (u[c] > 1) {
					Result = false;
					cout << "VALIDATION FAILED: " << u[c] << "x" << c + 1 << " in grid " << x1 + 1 << 'x' << y1 + 1 << '\n';
				}
			}
		}
	}
	//X
	if (XFactor) {
		// \.
		for (c = 0; c < 9; c++) {
			u[c] = 0;
		}
		for (x = 0; x < 9; x++) {
			y = x;
			if (Table[x][y]) {
				u[Table[x][y]-1]++;
			}
		}
		for (c = 0; c < 9; c++) {
			if (u[c] > 1) {
				Result = false;
				cout << "VALIDATION FAILED: " << u[c] << "x" << c + 1 << " in \\ (X factor)\n";
			}
		}
		// /
		for (c = 0; c < 9; c++) {
			u[c] = 0;
		}
		for (x = 0; x < 9; x++) {
			y = 8 - x;
			if (Table[x][y]) {
				u[Table[x][y]-1]++;
			}
		}
		for (c = 0; c < 9; c++) {
			if (u[c] > 1) {
				Result = false;
				cout << "VALIDATION FAILED: " << u[c] << "x" << c + 1 << " in / (X factor)\n";
			}
		}
	}
	return Result;
}

bool sudoku::Set(unsigned short x, unsigned short y, unsigned short val, bool XFactor)
{
	unsigned short c, x1, y1;
	if (!val && Table[x][y]) {
		cerr << "Warning: Table[" << x << "][" << y << "] already assigned " << Table[x][y] << "!\n";
		return false;
	}
	Table[x][y] = val;
	//row
	for (c = 0; c < 9; c++) {
		if (!Table[c][y]) {
			TablePossib[c][y] &= ~(0x0000000000000001 << (val - 1));
		}
	}
	//col
	for (c = 0; c < 9; c++) {
		if (!Table[x][c]) {
			TablePossib[x][c] &= ~(0x0000000000000001 << (val - 1));
		}
	}
	//3x3
	for (y1 = y / 3 * 3; y1 < (y / 3 + 1) * 3; y1++) {
		for (x1 = x / 3 * 3; x1 < (x / 3 + 1) * 3; x1++) {
			if (!Table[x1][y1]) {
				TablePossib[x1][y1] &= ~(0x0000000000000001 << (val - 1));
			}
		}
	}
	//X
	if (XFactor) {
		// \.
		if (x == y) {
			for (c = 0; c < 9; c++) {
				if (!Table[c][c]) {
					TablePossib[c][c] &= ~(0x0000000000000001 << (val - 1));
				}
			}
		}
		// /
		if (x == 8 - y) {
			for (c = 0; c < 9; c++) {
				if (!Table[c][8 - c]) {
					TablePossib[c][8 - c] &= ~(0x0000000000000001 << (val - 1));
				}
			}
		}
	}
	return true; //Validate();
}

void sudoku::SetRaw(unsigned short x, unsigned short y, unsigned short val, unsigned long long PossibVal)
{
	Table[x][y] = val;
	TablePossib[x][y] = PossibVal;
}

bool sudoku::Search(bool XFactor, unsigned long long * solutions, unsigned long long * guesses)
{
	bool Result = false;
	unsigned short x, y, n, x1, y1, m = 9;
	unsigned short u[9];
	unsigned long long c;
	
	//Step 1: Search for any squares with only 1 possibility remaining:
	for (y = 0; y < 9; y++) {
		for (x = 0; x < 9; x++) {
			if (!Table[x][y]) {
				n = 0;
				for (c = 0; c < 9; c++) {
					if (TablePossib[x][y] & (0x0000000000000001 << c)) n++;
				}
				if (n == 1) {
					for (c = 0; c < 9; c++) {
						if (TablePossib[x][y] & (0x0000000000000001 << c)) {
							Set(x, y, c + 1, XFactor);
							Result = true;
							break;
						}
					}
				} else if (!n) {
					cerr << "SANITY CHECK FAILED! NO POSSIBLE VALUES FOR TablePossib[" << x << "][" << y << "] (" << x + 1 << "x" << y + 1 << ")\n";
					return false;
				} else if (n < m) {
					m = n;
				}
			}
		}
	}
	
	//Step 2: Find squares in rows/columns/3x3 that are the only square with a particular number
	
	//rows
	for (y = 0; y < 9; y++) {
		for (c = 0; c < 9; c++) {
			u[c] = 0;
		}
		for (x = 0; x < 9; x++) {
			if (!Table[x][y]) {
				for (c = 0; c < 9; c++) {
					if (TablePossib[x][y] & (0x0000000000000001 << c)) u[c]++;
				}
			}
		}
		for (c = 0; c < 9; c++) {
			if (u[c] == 1) {
				for (x = 0; x < 9; x++) {
					if (!Table[x][y]) {
						if (TablePossib[x][y] & (0x0000000000000001 << c)) {
							Set(x, y, c + 1, XFactor);
							Result = true;
							break;
						}
					}
				}
			}
		}
	}
	//cols
	for (x = 0; x < 9; x++) {
		for (c = 0; c < 9; c++) {
			u[c] = 0;
		}
		for (y = 0; y < 9; y++) {
			if (!Table[x][y]) {
				for (c = 0; c < 9; c++) {
					if (TablePossib[x][y] & (0x0000000000000001 << c)) u[c]++;
				}
			}
		}
		for (c = 0; c < 9; c++) {
			if (u[c] == 1) {
				for (y = 0; y < 9; y++) {
					if (!Table[x][y]) {
						if (TablePossib[x][y] & (0x0000000000000001 << c)) {
							Set(x, y, c + 1, XFactor);
							Result = true;
							break;
						}
					}
				}
			}
		}
	}
	//3x3
	for (y1 = 0; y1 < 3; y1++) {
		for (x1 = 0; x1 < 3; x1++) {
			for (c = 0; c < 9; c++) {
				u[c] = 0;
			}
			for (y = y1 * 3; y < (y1 + 1) * 3; y++) {
				for (x = x1 * 3; x < (x1 + 1) * 3; x++) {
					if (!Table[x][y]) {
						for (c = 0; c < 9; c++) {
							if (TablePossib[x][y] & (0x0000000000000001 << c)) u[c]++;
						}
					}
				}
			}
			for (c = 0; c < 9; c++) {
				if (u[c] == 1) {
					for (y = y1 * 3; y < (y1 + 1) * 3; y++) {
						for (x = x1 * 3; x < (x1 + 1) * 3; x++) {
							if (!Table[x][y]) {
								if (TablePossib[x][y] & (0x0000000000000001 << c)) {
									Set(x, y, c + 1, XFactor);
									Result = true;
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	//X
	if (XFactor) {
		// \.
		for (c = 0; c < 9; c++) {
			u[c] = 0;
		}
		for (x = 0; x < 9; x++) {
			y = x;
			if (!Table[x][y]) {
				for (c = 0; c < 9; c++) {
					if (TablePossib[x][y] & (0x0000000000000001 << c)) u[c]++;
				}
			}
		}
		for (c = 0; c < 9; c++) {
			if (u[c] == 1) {
				for (x = 0; x < 9; x++) {
					y = x;
					if (!Table[x][y]) {
						if (TablePossib[x][y] & (0x0000000000000001 << c)) {
							Set(x, y, c + 1, XFactor);
							Result = true;
							break;
						}
					}
				}
			}
		}
		// /
		for (c = 0; c < 9; c++) {
			u[c] = 0;
		}
		for (x = 0; x < 9; x++) {
			y = 8 - x;
			if (!Table[x][y]) {
				for (c = 0; c < 9; c++) {
					if (TablePossib[x][y] & (0x0000000000000001 << c)) u[c]++;
				}
			}
		}
		for (c = 0; c < 9; c++) {
			if (u[c] == 1) {
				for (x = 0; x < 9; x++) {
					y = 8 - x;
					if (!Table[x][y]) {
						if (TablePossib[x][y] & (0x0000000000000001 << c)) {
							Set(x, y, c + 1, XFactor);
							Result = true;
							break;
						}
					}
				}
			}
		}
	}
	
	//TODO: Tricky shit
	
	if (!Result && !Complete()) {
		for (y = 0; y < 9; y++) {
			for (x = 0; x < 9; x++) {
				if (!Table[x][y]) {
					n = 0;
					for (c = 0; c < 9; c++) {
						if (TablePossib[x][y] & (0x0000000000000001 << c)) n++;
					}
					if (n == m) {
						for (c = 0; c < 9; c++) {
							if (TablePossib[x][y] & (0x0000000000000001 << c)) {
								sudoku * guess_sudoku = new sudoku;
								if (guess_sudoku) {
									for (y1 = 0; y1 < 9; y1++) {
										for (x1 = 0; x1 < 9; x1++) {
// 											guess_sudoku->Set(x1, y1, Table[x1][y1], XFactor);
											guess_sudoku->SetRaw(x1, y1, Table[x1][y1], TablePossib[x1][y1]);
										}
									}
									guess_sudoku->Set(x, y, c + 1, XFactor);
									(*guesses)++;
									cerr << "Making the assumption that " << x << "x" << y << " is " << c + 1 << "\n";
									while (guess_sudoku->Search(XFactor, solutions, guesses)) {
									}
									if (guess_sudoku->Complete() && guess_sudoku->Validate(XFactor)) {
										cout << "\nThis result was found and validated after " << *guesses << " guesses:\n\n";
										(*solutions)++;
										Result = true;
										guess_sudoku->Display();
									}
									//Stop us trying this possibility again:
									TablePossib[x][y] &= ~(0x0000000000000001 << c);
									cerr << "Backing out on assumption that " << x << "x" << y << " is " << c + 1 << "\n";
									delete guess_sudoku;
								} else {
									cerr << "ERROR ALLOCATING NEW SUDOKU FOR GUESS\n";
									return false;
								}
// 								Set(x, y, c + 1, XFactor);
// 								Result = true;
// 								break;
							}
						}
// 						if (Result) return false;
						if (!TablePossib[x][y]) return false;
					} else if (!n) {
						cerr << "SANITY CHECK FAILED! NO POSSIBLE VALUES FOR TablePossib[" << x << "][" << y << "] (" << x + 1 << "x" << y + 1 << ")\n";
						return false;
					}
				}
			}
		}
	}
	return Result;
}

bool sudoku::Complete()
{
	unsigned short x, y;
	for (y = 0; y < 9; y++) {
		for (x = 0; x < 9; x++) {
			if (!Table[x][y]) return false;
		}
	}
	return true;
}

void sudoku::Display()
{
	unsigned short x, y;
	for (y = 0; y < 9; y++) {
		for (x = 0; x < 9; x++) {
			if (Table[x][y]) {
				cout << Table[x][y];
			} else {
				cout << '?';
			}
			if (x == 2 || x == 5) {
				cout << '|';
			}
		}
		cout << '\n';
		if (y == 2 || y == 5) {
			cout << "---+---+---\n";
		}
	}
	cout << "\n\n";
}

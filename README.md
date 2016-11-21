Sudoku Solver
=============

A very simple sudoku solver written in Python and C. The Python
implementation is really fast enough, but just for the heck of it I also
implemented it in C.

Usage
-----

Given a file `sudoku.txt` with this content:

	5 8 1 | 9 . . | 7 . .
	. . . | . 5 . | . . 1
	. . 9 | . 3 7 | . 5 2
	------+-------+------
	. 5 . | . 2 9 | . . 6
	2 . . | . . . | . . 8
	1 . . | 4 8 . | . 9 .
	------+-------+------
	9 7 . | 8 1 . | 3 . .
	4 . . | . 7 . | . . .
	. . 8 | . . 4 | 1 7 5

Run this command:

	./sudoku sudoku.txt

Or run this command:

	./sudoku.py sudoku.txt

To get this output:

	5 8 1 | 9 4 2 | 7 6 3 
	7 2 3 | 6 5 8 | 9 4 1 
	6 4 9 | 1 3 7 | 8 5 2 
	------+-------+------
	8 5 7 | 3 2 9 | 4 1 6 
	2 9 4 | 7 6 1 | 5 3 8 
	1 3 6 | 4 8 5 | 2 9 7 
	------+-------+------
	9 7 5 | 8 1 6 | 3 2 4 
	4 1 2 | 5 7 3 | 6 8 9 
	3 6 8 | 2 9 4 | 1 7 5 

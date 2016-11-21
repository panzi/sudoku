#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "panic.h"

//typedef uint_fast8_t Number;
typedef uint_fast16_t Number;
typedef uint_fast16_t NumberSet;

typedef struct SudokuS {
	Number numbers[9][9];
} Sudoku;

typedef struct SolverS {
	const Sudoku *sudoku;
	Sudoku solution;
	NumberSet used_rows[9];
	NumberSet used_cols[9];
	NumberSet used_blocks[3][3];
	void (*callback)(const Sudoku *);
} Solver;

static void validate_sudoku(const Sudoku *sudoku);
static void read_sudoku(FILE *stream, Sudoku *sudoku);
static void write_sudoku(FILE *stream, const Sudoku *sudoku);
static void solve(const Sudoku *sudoku, void (*callback)(const Sudoku *));
static void try_next(Solver *solver, size_t x, size_t y);
static void print_solutions(FILE *stream);
static void print_solution(const Sudoku *solution);

void read_sudoku(FILE *stream, Sudoku *sudoku) {
	size_t x = 0;
	size_t y = 0;

	for (;;) {
		const int c = fgetc(stream);

		if (c == EOF) {
			if (ferror(stream)) {
				panice("reading sudoku");
			}
			break;
		}

		Number number = 0;
		if (c == '0') {
			panicf("illegal sudoku number: 0");
		}
		else if (c >= '1' && c <= '9') {
			number = c - '0';
		}
		else if (c == '\n') {
			if (x > 0) {
				if (x < 8) {
					panicf("too few columns (%zu) in row %zu", x, y + 1);
				}
				x = 0;
				++ y;
			}
			continue;
		}
		else if (c != '.') {
			continue;
		}

		if (x > 8) {
			panicf("too many columns in row %zu", y + 1);
		}

		if (y > 8) {
			panicf("too many rows");
		}

		sudoku->numbers[y][x] = number;
		++ x;
	}

	if (y < 8) {
		panicf("too few rows");
	}

	validate_sudoku(sudoku);
}

void write_sudoku(FILE *stream, const Sudoku *sudoku) {
	for (size_t y = 0; y < 9; ++ y) {
		if (y > 0 && (y % 3) == 0) {
			fprintf(stream, "------+-------+------\n");
		}

		for (size_t x = 0; x < 9; ++ x) {
			if (x > 0 && x % 3 == 0) {
				fprintf(stream, "| ");
			}

			const Number number = sudoku->numbers[y][x];
			if (number == 0) {
				fputc('.', stream);
			}
			else {
				fputc(number + '0', stream);
			}
			fputc(' ', stream);
		}

		fputc('\n', stream);
	}
}

void validate_sudoku(const Sudoku *sudoku) {
	NumberSet used_rows[9] = { 0 };
	NumberSet used_cols[9] = { 0 };
	NumberSet used_blocks[3][3] = { { 0 } };

	for (size_t y = 0; y < 9; ++ y) {
		const size_t block_y = y / 3;
		for (size_t x = 0; x < 9; ++ x) {
			const Number number = sudoku->numbers[y][x];
			if (number != 0) {
				const size_t block_x = x / 3;
				const NumberSet flag = 1 << number;
				if (used_rows[y] & flag) {
					panicf("number at %zu x %zu already used in this row: %d", x + 1, y + 1, (int)number);
				}

				if (used_cols[x] & flag) {
					panicf("number at %zu x %zu already used in this column: %d", x + 1, y + 1, (int)number);
				}

				if (used_blocks[block_y][block_x] & flag) {
					panicf("number at %zu x %zu already used in this block: %d", x + 1, y + 1, (int)number);
				}

				used_rows[y] |= flag;
				used_cols[x] |= flag;
				used_blocks[block_y][block_x] |= flag;
			}
		}
	}
}

void solve(const Sudoku *sudoku, void (*callback)(const Sudoku *)) {
	Solver solver = {
		.sudoku = sudoku,
		.used_rows = { 0 },
		.used_cols = { 0 },
		.used_blocks = { { 0 } },
		.callback = callback
	};

	for (size_t y = 0; y < 9; ++ y) {
		const size_t block_y = y / 3;
		for (size_t x = 0; x < 9; ++ x) {
			const Number number = solver.solution.numbers[y][x] = sudoku->numbers[y][x];
			if (number != 0) {
				const NumberSet flag = 1 << number;
				solver.used_rows[y] |= flag;
				solver.used_cols[x] |= flag;
				solver.used_blocks[block_y][x / 3] |= flag;
			}
		}
	}

	try_next(&solver, 0, 0);
}

void try_next(Solver *solver, size_t x, size_t y) {
	bool at_end = false;
	size_t next_x, next_y;

	if (x == 8) {
		if (y == 8) {
			at_end = true;
		}
		else {
			next_x = 0;
			next_y = y + 1;
		}
	}
	else {
		next_x = x + 1;
		next_y = y;
	}

	const Number number = solver->sudoku->numbers[y][x];
	if (number == 0) {
		const NumberSet row_usage = solver->used_rows[y];
		const NumberSet col_usage = solver->used_cols[x];
		const size_t block_x = x / 3;
		const size_t block_y = y / 3;
		const NumberSet block_usage = solver->used_blocks[block_y][block_x];

		for (Number number = 1; number <= 9; ++ number) {
			const NumberSet flag = 1 << number;
			if ((row_usage & flag) == 0 &&
			    (col_usage & flag) == 0 &&
			    (block_usage & flag) == 0) {
				solver->used_rows[y] = row_usage | flag;
				solver->used_cols[x] = col_usage | flag;
				solver->used_blocks[block_y][block_x] = block_usage | flag;

				solver->solution.numbers[y][x] = number;
				if (at_end) {
					solver->callback(&solver->solution);
				}
				else {
					try_next(solver, next_x, next_y);
				}

				const NumberSet nflag = ~flag;
				solver->used_rows[y] = row_usage & nflag;
				solver->used_cols[x] = col_usage & nflag;
				solver->used_blocks[block_y][block_x] = block_usage & nflag;
			}
		}
	}
	else if (at_end) {
		solver->callback(&solver->solution);
	}
	else {
		try_next(solver, next_x, next_y);
	}
}

void print_solution(const Sudoku *solution) {
	write_sudoku(stdout, solution);
	fputc('\n', stdout);
}

void print_solutions(FILE *stream) {
	Sudoku sudoku = { .numbers = { { 0 } } };
	read_sudoku(stream, &sudoku);
	solve(&sudoku, print_solution);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		print_solutions(stdin);
	}
	else {
		FILE *stream = fopen(argv[1], "r");

		if (!stream) {
			panice(argv[1]);
		}

		print_solutions(stream);
		fclose(stream);
	}
}

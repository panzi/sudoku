#!/usr/bin/env python3

import sys

def read_sudoku(stream):
	sudoku = []
	row = []
	for line in stream:
		for c in line:
			if c.isdigit():
				row.append(int(c, 10))
			elif c == '.':
				row.append(None)
		if row:
			sudoku.append(row)
			row = []

	validate_sudoku(sudoku)

	return sudoku

def write_sudoku(sudoku, stream):
	for y, xs in enumerate(sudoku):
		if y > 0 and y % 3 == 0:
			stream.write('------+-------+------\n')

		for x, number in enumerate(xs):
			if x > 0 and x % 3 == 0:
				stream.write('| ')

			if number is None:
				stream.write('.')
			else:
				stream.write(str(number))

			if x < 9:
				stream.write(' ')

		stream.write('\n')

def validate_sudoku(sudoku):
	if len(sudoku) != 9:
		raise ValueError('illegal row count: %d' % len(sudoku))

	for y, row in enumerate(sudoku):
		if len(row) != 9:
			raise ValueError('illegal column count in row %d: %d' % (y + 1, len(row)))

	used_rows = [0] * 9
	used_cols = [0] * 9
	used_blocks = [[0] * 3 for _ in range(3)]

	for y, xs in enumerate(sudoku):
		block_y = y // 3
		for x, number in enumerate(xs):
			if number is not None:
				block_x = x // 3

				if type(number) is not int:
					raise ValueError('number at %d x %d is not int or None' % (x + 1, y + 1))

				if number < 1 or number > 9:
					raise ValueError('number at %d x %d is out of range: %d' % (x + 1, y + 1, number))

				flag = 1 << number
				if used_rows[y] & flag:
					raise ValueError("number at %zu x %zu already used in this row: %d", x + 1, y + 1, number)
					
				if used_cols[x] & flag:
					raise ValueError("number at %zu x %zu already used in this column: %d", x + 1, y + 1, number)

				if used_blocks[block_y][block_x] & flag:
					raise ValueError("number at %zu x %zu already used in this block: %d", x + 1, y + 1, number)

				used_rows[y] |= flag
				used_cols[x] |= flag
				used_blocks[block_y][block_x] |= flag

def solve(sudoku):
	solution = [sudoku[y][:] for y in range(9)]
	used_rows = [0] * 9
	used_cols = [0] * 9
	used_blocks = [[0] * 3 for _ in range(3)]

	for y, xs in enumerate(sudoku):
		block_y = y // 3
		for x, number in enumerate(xs):
			if number is not None:
				flag = 1 << number
				used_rows[y] |= flag
				used_cols[x] |= flag
				used_blocks[block_y][x // 3] |= flag


	def try_next(x, y):
		at_end = False
		if x == 8:
			if y == 8:
				at_end = True
			else:
				next_x = 0
				next_y = y + 1
		else:
			next_x = x + 1
			next_y = y

		number = sudoku[y][x]
		if number is None:
			row_usage = used_rows[y]
			col_usage = used_cols[x]
			block_x = x // 3
			block_y = y // 3
			block_row = used_blocks[block_y]
			block_usage = block_row[block_x]
			for number in range(1, 10):
				flag = 1 << number
				if row_usage & flag == 0 and \
				   col_usage & flag == 0 and \
				   block_usage & flag == 0:
					used_rows[y] = row_usage | flag
					used_cols[x] = col_usage | flag
					block_row[block_x] = block_usage | flag

					solution[y][x] = number
					if at_end:
						yield solution
					else:
						yield from try_next(next_x, next_y)
				
					nflag = ~flag
					used_rows[y] = row_usage & nflag
					used_cols[x] = col_usage & nflag
					block_row[block_x] = block_usage & nflag
		elif at_end:
			yield solution
		else:
			yield from try_next(next_x, next_y)

	yield from try_next(0, 0)

def main(args):
	with open(args[0], 'r') as stream:
		sudoku = read_sudoku(stream)

	for solution in solve(sudoku):
		write_sudoku(solution, sys.stdout)
		print('\n')

if __name__ == '__main__':
	main(sys.argv[1:])	

from random import shuffle, randrange

def make_maze(w = 16, h = 8):
	vis = [[0] * w + [1] for _ in range(h)] + [[1] * (w + 1)]
	ver = [["|  "] * w + ['|'] for _ in range(h)] + [[]]
	hor = [["+--"] * w + ['+'] for _ in range(h + 1)]

	def walk(x, y):
			vis[y][x] = 1

			d = [(x - 1, y), (x, y + 1), (x + 1, y), (x, y - 1)]
			shuffle(d)
			for (xx, yy) in d:
				if vis[yy][xx]: continue
				if xx == x: hor[max(y, yy)][x] = "+  "
				if yy == y: ver[y][max(x, xx)] = "   "
				walk(xx, yy)

	walk(randrange(w), randrange(h))
	return (hor, ver)


def move(x, y, h, v, last):
	d = [(x - 1, y), (x, y + 1), (x + 1, y), (x, y - 1)]
	ways = []

	for t in d:
		# print t
		if t == last:
			continue

		# if  t[0] < 0 or t[1] < 0:
		# 	continue

		if t[0] == x: # move on the cols
			# go to the left
			if t[1] == y - 1 and v[x][y] != "|  ":
				ways.append(t)
			# go to the right
			elif t[1] == y + 1 and v[x][y+1] != "|  " and v[x][y+1] != "|":
				ways.append(t)
		else: #move on the rows
			if t[0] == x - 1 and h[x][y] != "+--":
				ways.append(t)
			elif t[0] == x + 1 and h[x+1][y] != "+--":
				ways.append(t)


	return ways

def traverse(hor, ver, start, end, verbose = False):
	x = start[0]
	y = start[1]
	path = [(x, y)]
	explored = {}
	while (True):
		if (x, y) not in explored:
			explored[(x, y)] = move(x, y, hor, ver, path[-1])

		if verbose:	print 'Ways: ', explored[(x, y)]

		if explored[(x, y)] == []: # no way
			if verbose: print 'Going back from ', path[-1], ' to ', path[-2]
			x, y = path.pop()
			if path == []:
				if verbose: print 'No more ways to go... Exiting.'
				break

		else:
			path.append((x, y))
			if verbose: print 'Explored ', path[-1]

			x, y = explored[(x, y)].pop(0)

			if verbose: print 'Moving to ', (x, y)
			if (x, y) == end:
				# print the path
				path.pop(0) # patch
				for coord in path:
					print coord, ' -> ',
				print end, '.'
				print 'Destination reached.'
				break



#####################
hor, ver = make_maze()

for (a, b) in zip(hor, ver):
	print ''.join(a + ['\n'] + b)

start = (0, 0)
end = (7, 15)
traverse(hor, ver, start, end)
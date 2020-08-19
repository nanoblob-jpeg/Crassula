while(True):
	name = input()
	if name == 'a':
		break;
	else:
		f = open(name + '.txt', 'w')
		f.write(name + '\n')
		f.write(name + '1\n')
		f.write(name + '2\n')
		f.write(name + '3\n')
		f.write(name + '4\n')
		f.write('\n\n')
		f.write('1.0\n1.0\n1.0')
		f.close()

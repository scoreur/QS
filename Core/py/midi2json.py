import sys
import os
import json
reload(sys)
def MIDI2json(s):
	vis = [None] * 256
	re = []
	l = 0
	l += 4
	l += 4
	re.append(ord(s[l]) * 256 + ord(s[l + 1]))
	l += 2
	re.append(ord(s[l]) * 256 + ord(s[l + 1]))
	l += 2
	re.append(ord(s[l]) * 256 + ord(s[l + 1]))
	l += 2
	nTrack = re[1]
	for i in range(nTrack):
		events0 = []
		events1 = []
		l += 4
		nChar = ord(s[l]) * 256 * 256 * 256 + ord(s[l + 1]) * 256 * 256 + ord(s[l + 2]) * 256 + ord(s[l + 3])
		l += 4
#		print "TRACK %d" % i, nChar
		end = l + nChar
		t = 0
		while l < end:
			dt = 0
			while ord(s[l]) > 127:
				dt = dt * 128 + ord(s[l]) - 128
				l += 1
			dt = dt * 128 + ord(s[l])
			t += dt
			l += 1
#			print dt
			command = ord(s[l])
#			print dt, hex(command)
			para = []
			if ord(s[l]) / 16 < 12:
				para = [ord(s[l + 1]), ord(s[l + 2])]
				l += 3
			elif ord(s[l]) / 16 < 15:
				para = [ord(s[l + 1])]
				l += 2
			else:
				l += 1
				command = 256 + ord(s[l])
#				print hex(command)
				nPara = 0
				l += 1
				while ord(s[l]) > 127:
					nPara = nPara * 128 + ord(s[l]) - 128
					l += 1
				nPara = nPara * 128 + ord(s[l])
#				print nPara
				l += 1
				para = [s[l: l + nPara].decode('iso-8859-1')]
				l += nPara
				pass
			if command / 16 == 9:
				vis[para[0]] = [t, para[1]]
			elif command / 16 == 8:
				events1.append([para[0], vis[para[0]][0], vis[para[0]][1], t, para[1]])
				vis[para[0]] = None
			else:
				events0.append([t, command, para])
		re.append([events0, events1])
#	print re
	return json.dumps(re, ensure_ascii=False)
def int2str(x):
	s = chr(x % 128)
	x /= 128
	while x > 0:
		s = chr(128 + x % 128) + s
		x /= 128
	return s

def json2MIDI(xxx):
	a = json.loads(xxx)
	s = "MThd"
	s += "\x00\x00\x00\x06"
	s += chr(a[0] / 256)
	s += chr(a[0] % 256)
	s += chr(a[1] / 256)
	s += chr(a[1] % 256)
	s += chr(a[2] / 256)
	s += chr(a[2] % 256)
	nTrack = a[1]
	del a[0: 3]
	for i in range(nTrack):
		s += 'MTrk'
		tmp = ''
		events = a[i][0][:]
		for j in a[i][1]:
			events.append([j[1], 0x90 + 0xffff, [j[0], j[2]]])
			events.append([j[3], 0x80 + 0xffff, [j[0], j[4]]])
		events = sorted(events)
		t = 0
		for j in events:
			tmp += int2str(j[0] - t)
			if j[1] >= 0xffff:
				j[1] -= 0xffff
			if j[1] >= 256:
				j[1] -= 256
				tmp += '\xff'
			tmp += chr(j[1])
			if len(j[2]) == 1 and type(j[2][0]) == type(u''):
				tmp += int2str(len(j[2][0]))
				j[2][0] = j[2][0].encode('iso-8859-1')
			else:
				j[2] = map(chr, j[2])
			tmp += ''.join(j[2])
#			print `''.join(para)`
			t = j[0]
		l = len(tmp)
		s += chr((l >> 24) % 256)
		s += chr((l >> 16) % 256)
		s += chr((l >> 8) % 256)
		s += chr((l >> 0) % 256)
		s += tmp
	return s
if __name__ == "__main__":
	file = 'test.mid'
	s = open(file, 'r').read()
	print len(s)
	t = json2MIDI(MIDI2json(s))
	print len(t)
	open('out.mid', 'w').write(t)

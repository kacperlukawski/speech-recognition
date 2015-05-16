#coding=utf-8

from random import shuffle

file_handle = open('words.txt', 'r')
words = [line.strip() for line in file_handle]
file_handle.close()

for i in xrange(100):
    file_handle = open('variants/words_%d.txt' % i, 'w')
    shuffle(words)
    file_handle.write(', '.join(words))
    file_handle.close()

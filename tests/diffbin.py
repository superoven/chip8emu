import sys
import binascii

if (len(sys.argv) < 3):
  print "Usage: ./diffbin file1 file2"
  exit(0)

f1 = open(sys.argv[1], "rb")
f2 = open(sys.argv[2], "rb")

count = 0
while (1):
  value1 = f1.read(2)
  value2 = f2.read(2)
  if value1 != value2:
    print "Instruction %d: %s != %s" % (count, binascii.hexlify(value1), binascii.hexlify(value2))
  count += 1
  if (value1 == "" or value2 == ""): break

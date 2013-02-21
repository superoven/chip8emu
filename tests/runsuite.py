from subprocess import call
from os import listdir,devnull
from os.path import isfile, join
import filecmp

SUBDUEGCC = True
FNULL = open(devnull)

path = "./tests/proc/"

def cleantemp():
  call(["rm", path + head], stdout = FNULL, stderr = FNULL)
  call(["rm", outputfile], stdout = FNULL, stderr = FNULL)

testfiles = [ f for f in listdir(path) if isfile(join(path,f)) and len(f.split(".")) > 1 and f.split(".")[1] != "std"]
testfiles.sort()

if not testfiles:
  print "No test files present!"
  exit()

failed = 0
numfiles = len(testfiles)

print
print "Chip8 Processor Test Suite"
print "-----------------------"

for f in testfiles:
  head = f.split(".")[0]
  print "%-18s" % (head+":"),
  command = ["./asmbin/chasm", path + f, path + head]

  outputfile = path + head + ".out"
  checkfile = path + head + ".std"

  if SUBDUEGCC:
    notcompile = call(command, stdout = FNULL, stderr = FNULL)
  else:
    notcompile = call(command)

  if notcompile:
    print "FAIL (did not compile)"
    failed += 1
    continue

  fout = open(outputfile, "w")

  returncode = call(["./chipemu", path + head], stdout = fout)
  if returncode:
    print "FAIL (return code %d)" % returncode
    failed += 1
    cleantemp()
    continue

  try:
    with open(checkfile) as f: pass
  except IOError as e:
    print "PASS (no %s.std file)" % head
    cleantemp()
    continue

  if not filecmp.cmp(outputfile, checkfile):
    print "FAIL (wrong output)"
    failed += 1
    cleantemp()
    continue

  cleantemp()
  print "PASS"


#After all tests have been run

print
if failed:
  print "Tests Passed: %d/%d (%.2f%%)" % (numfiles-failed, numfiles, (float(numfiles-failed)/float(numfiles))*100)
elif (numfiles == 1):
  print "The only test passes!"
else:
  print "All %d Tests Pass!" % (numfiles)

inp = str(raw_input())
finalset = set()
while (inp != ""):
  finalset.add(inp)
  try:
    inp = str(raw_input())
  except:
    break

for i in finalset:
  print i
  

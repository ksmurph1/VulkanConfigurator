initFileName="vector90.hpp"
replaceConst=8
beginRange=10
endRange=45
f = open(initFileName, 'r')
try:
   contents=''.join(f.readlines())
   newContents=''
   # replace contents
   for i in range(beginRange,endRange+1):
      newContents=contents.replace("vector{}".format(replaceConst*beginRange),"vector{}".format((i-1)*beginRange)).\
      replace("{}1,".format(replaceConst),"{}1,".format(i-1)).replace("{}0,".format(beginRange-1),\
      "{}0,".format(i)).replace("_VECTOR{}_".format((beginRange-1)*beginRange),"_VECTOR{}_".\
      format(i*beginRange))
      f=open(initFileName.replace("vector{}".format(beginRange-1),"vector{}".format(i)),'w')
      print(newContents,file=f)
      f.close()
except Exception as ex:
   print("exception: {0}".format(ex))
   raise
finally:
   f.close()  
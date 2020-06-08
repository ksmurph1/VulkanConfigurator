import os
import sys
import math

from pygccxml import parser  # nopep8
from pygccxml import declarations  # nopep8
from pygccxml import utils  # nopep8

startIdx=2
flags=''
compiler=sys.argv[1]
seenStruct={}
if (sys.argv[startIdx].strip("'").startswith('-')): # flags
   flags=sys.argv[startIdx]
   startIdx=startIdx+1

# Find out the xml generator (gccxml or castxml)
generator_path, generator_name = utils.find_xml_generator()
metaFilename="vulkan_structs_meta.hpp"
typeListFilename="vulkan_type_list.hpp"
if (generator_name != 'castxml'):
   print (generator_name+' does not equal castxml.  Please make sure castxml is installed.')
   sys.exit(1)

# Configure the xml generator
config = parser.xml_generator_configuration_t(
    xml_generator_path=generator_path,
    xml_generator=generator_name,
    cflags=flags)

f = open(metaFilename, 'w')
try:
   print ("//AUTO-GENERATED File, caution before modifying\n",file=f)
   print ("#ifndef "+metaFilename.replace(".","_")+"\n #define "+
   metaFilename.replace(".","_") + "\n",file=f)
   print ("#include \"visit_struct.hpp\"\n",file=f)
   # iterate all paths
   for path in sys.argv[startIdx:]:
      # Parsing source file
      decls = parser.parse([path], config)
      global_ns = declarations.get_global_namespace(decls)
     # print ("#include \"{}\"\n".format(path), file=f)
      # iterate classes and get member information
      for clas in global_ns.classes():
         if (declarations.type_traits_classes.is_struct(clas) and clas.name.startswith('Vk') and \
            not seenStruct.get(clas.name)):
            strings=["VISITABLE_STRUCT({}".format(clas.name)]
            seenStruct[clas.name]=1
            for member in clas.public_members:
               if (isinstance(member,declarations.variable_t)):
                  strings.append("{}".format(member.name))
            print(','.join(strings)+");\n", file=f)
except Exception as ex:
   print("exception: {0}".format(ex))
   raise
else:
   print ("#endif\n",file=f)
finally:
   f.close()
# commented out for now- too many types- over 450
'''f = open(typeListFilename, 'w')
# generate vulkan type list code
try:
   print ("//AUTO-GENERATED File, caution before modifying\n",file=f)
   print ("#ifndef "+typeListFilename.replace(".","_")+"\n #define "+
   typeListFilename.replace(".","_") + "\n",file=f)

   count=len(seenStruct)
   roundup=int(math.ceil(count / 10.0)) * 10
   print ("#include \"boost_mpl_vector_range_ext/vector{}.hpp\"\n".format(roundup), file=f)
   print ("namespace configuration\n{\n",file=f)

   print ("typedef boost::mpl::vector{}<".format(count),file=f)
   typeNames=''
   for typeName in seenStruct.keys():
      typeNames+=typeName+",\n"
   # remove last comma and newline
   typeNames+= typeNames.rstrip(',\n')+"> TYPELIST;\n"
   print (typeNames, file=f)
except Exception as ex:
   print("exception: {0}".format(ex))
   raise
else:
   print ("}\n#endif\n", file=f)
finally:
   f.close()'''
   
if __name__ == '_main_':
   startIdx=2
   if (len(sys.argv) < 3): 
      print ("This script will create the file that grabs metadata from structs\
visit_struct(....)\n depends: CastXML\n Usage: {} \{compiler\} ['flags'] \{list of full paths to \
parse\}\n".format(sys.argv[0]))
      sys.exit(1)
      
   if (not sys.argv[1] in ("gcc","g++","msvc","clang")):
      print ("Usage: {} \{compiler name\} ['flags'] \{list of full paths to \parse\}\n".format(sys.argv[0]))
      sys.exit(1)
   if (sys.argv[startIdx].strip("'").startswith('-')): # flags
      startIdx=startIdx+1

   for i in range(startIdx,len(sys.argv)):
      if (not os.path.exits(sys.argv[i])):
         print ("Usage: {} \{compiler name\} ['flags'] \{list of full paths to \parse\}\n".format(sys.argv[0]))
         sys.exit(1)
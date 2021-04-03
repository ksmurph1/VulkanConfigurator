import os
import sys
import math
import re

from pygccxml import parser  # nopep8
from pygccxml import declarations  # nopep8
from pygccxml import utils  # nopep8
from itertools import zip_longest
import xml.sax

def CLASS_ID():
   return 1
def MAXVECTORSIZE():
   return 60
def MAXTYPES():
   return 6
def ANDSYM():
   return '&'
def ORSYM():
   return '|'

startIdx=3
flags=''
if __name__ == '__main__':
   usageMsg="Usage: "+sys.argv[0]+" \{vulkan_xml_file\} \{compiler\} ['flags'] \{list of full paths to \
parse\}"
   if (len(sys.argv) <= startIdx): 
      print ("This script will create the file that grabs metadata from structs\
 depends: CastXML\n{}\n".format(usageMsg))
      sys.exit(1)
   
   if (not os.path.exists(sys.argv[startIdx-2])):
      print (("file {} does not exist\n"+usageMsg).format(sys.argv[startIdx-2]))

   if (not sys.argv[startIdx-1] in ("gcc","g++","msvc","clang")):
      print (usageMsg)
      sys.exit(1)

   if (sys.argv[startIdx].strip("'").startswith('-')): # flags
      flags=sys.argv[startIdx]
      startIdx=startIdx+1

   for i in range(startIdx,len(sys.argv)):
      if (not os.path.exists(sys.argv[i])):
         print (usageMsg)
         sys.exit(1)

seenStruc={}
seenEnum={}
seenAlias={}

class VulkanHandler(xml.sax.ContentHandler):
   def __init__(self):
      self.__curData=''
      self.__aliasStrs=[]
      self.__enumStrs=[]
      self.__fullType=''
      self.__recorded=[]
      self.__typNames=['char *','void *','const char *']  # add these types by default
      self.__strucName=''
   def startElement(self,tag,attributes):
      self.__curData=''
      self.__fullType=''
      if (tag == 'struct'):
         # if data is in struct process
         self.__strucName=attributes['type']
      elif (tag=='field'):
         for snip in attributes['type'].split():
            if (snip in seenStruc):
               self.__curData=snip
               self.__fullType=attributes['type']
            elif (snip in seenEnum):
               self.__curData=snip
               self.__fullType=attributes['type']
            elif (snip in seenAlias):
               self.__curData=snip
               self.__fullType=attributes['type']

         if (not attributes['type'] in seenStruc and 'value' in attributes):               
            # if data is in enum process
            if (self.__curData and seenEnum.get(self.__curData)):
               self.__fullType=self.__fullType.replace(self.__curData,f'enumeration<{self.__curData}>')
               for t in seenEnum[self.__curData]:
                  if (t[0] == attributes['value'].strip()):
                     self.__curData=t
                     break
                  elif (t[0] in attributes['value'] and any(c in ORSYM()+ANDSYM() for c in attributes['value'])):
                     self.__curData=[]
                     for sym in re.split(ORSYM()+ANDSYM(),t[0]):
                        self.__curData.append(t)
                     break

            elif ( any(c in ORSYM()+ANDSYM() for c in attributes['value'])):
               self.__curData=[self.__curData]
               # check if value is a enum value
               for t in [t for itemList in seenEnum.items() for t in itemList[1]]:
                  if (t[0] in attributes['value']):
                     self.__curData.append(t)
            else:
               self.__curData=[self.__curData]
               # check if value is enum value
               for t in [t for itemList in seenEnum.items() for t in itemList[1]]:
                  if (t[0] == attributes['value'].strip()):
                     self.__curData.append(t)
                     
   def endElement(self,tag):
      if (tag == 'struct'):
         if (self.__strucName and not self.__strucName in self.__typNames):
            # type is struct
            self.__typNames.append(self.__strucName)
         self.__strucName=''
         
      elif (tag == 'field' and self.__curData and self.__fullType):
         # covers any types in fields
         if (not self.__fullType in self.__typNames):
            self.__typNames.append(self.__fullType)
         try:
            if (not isinstance(self.__curData,list)):
               self.__curData=[self.__curData]  # convert to list so for loop will work
            for curData in self.__curData:
               if (isinstance(curData, tuple) and not curData[0] in self.__recorded):
                  # data is enum value
                  self.__enumStrs.append('enumMap.add("{0}",{1});'.format(curData[0],curData[1]))
                  self.__recorded.append(curData[0])
               elif (curData in seenAlias and not self.__fullType in self.__recorded):
                  # data is alias
                  expAliasLen=len(self.__fullType.replace(curData,seenAlias[curData]))+1
                  self.__aliasStrs.append('aliases.add("{0}", strcpy("{1}",storage+storagePos,{2}));'.\
                  format(self.__fullType, self.__fullType.replace(curData,seenAlias[curData]),\
                  expAliasLen))
                  self.__aliasStrs.append('storagePos+={0};'.format(expAliasLen))
                  self.__recorded.append(self.__fullType)
         except Exception as ex:
            print("exception: {0}".format(ex))
            self.enumFile.close()
            self.aliasFile.close()
            raise
         
   '''def __appendTypeNames(self,typ):
      self.names+=typ
      if (VulkanHandler.count == VulkanHandler.MAXTYPES()):
         self.names+=',\n'
         VulkanHandler.count=0
      else:
         self.names+=','
      VulkanHandler.count+=1'''
      
   def Characters(self,content):
      pass

   def getTypeNames(self):
      return self.__typNames

   def getAliasLines(self):
      return self.__aliasStrs

   def getEnumLines(self):
      return self.__enumStrs

generator_path, generator_name = utils.find_xml_generator()

if (generator_name != 'castxml'):
   print (generator_name+' does not equal castxml.  Please make sure castxml is installed.')
   sys.exit(1)

# Configure the xml generator
config = parser.xml_generator_configuration_t(\
    xml_generator_path=generator_path,\
    xml_generator=generator_name,\
    cflags=flags)

    # find struct in vulkan headers
for path in sys.argv[startIdx:]:
   #print (path)
   # Parsing source file
   decls = parser.parse([path], config)
   global_ns = declarations.get_global_namespace(decls)
   # generate vulkan type list code
   # iterate classes and get member information
   for clas in [global_ns.classes(),global_ns.enumerations(),global_ns.typedefs()]:#,global_ns.enumerations():
      for decl in clas.decls.decls:
         # print('decl:'+decl.name)
         if ((hasattr(decl,'decl_type') and declarations.type_traits_classes.is_struct(decl.decl_type)) or declarations.type_traits_classes.is_struct(decl)\
            and seenStruc.get(decl.name) != CLASS_ID()):
            #print ('class:'+decl.name)
            seenStruc[decl.name]=CLASS_ID()
         elif (hasattr(decl,'decl_type') and declarations.type_traits_classes.is_enum(decl.decl_type) and not seenEnum.get(decl.name)):
            if (hasattr(decl.decl_type.declaration,'values')):
               #print ('enum:'+decl.name)
               seenEnum[decl.name]=decl.decl_type.declaration.values
            else:
               #print ('enum2:'+decl.name)
               seenEnum[decl.decl_type.declaration.name]=declarations.type_traits.remove_alias(decl.decl_type.declaration).declaration.values
         elif (declarations.type_traits_classes.is_enum(decl) and not seenEnum.get(decl.name)):
            #print ('enum3:'+decl.name)
            seenEnum[decl.name]=decl.values
         elif (hasattr(decl,'decl_type') and declarations.type_traits.is_fundamental(decl.decl_type) and not(decl.name.startswith("_"))\
            and not seenAlias.get(decl.name)):
            seenAlias[decl.name]=str(declarations.type_traits.remove_alias(decl))

typeFile=None
aliasFile=None
enumFile=None
typeListFileName='VulkanTypeList.hpp'
aliasFileName='VulkanAliasList.hpp'
enumMapFileName='VulkanEnumMap.hpp'
handler=VulkanHandler()
try:
   xmlParser=xml.sax.make_parser()
   xmlParser.setContentHandler(handler) #overriding default ContextHandler
   xmlParser.parse(sys.argv[1])
except Exception as ex:
   print("exception: {0}".format(ex))
   raise


try:
   typeFile = open(typeListFileName, 'w')
   print ("//AUTO-GENERATED File, caution before modifying\n",file=typeFile)
   print ("#ifndef "+typeListFileName.replace(".","_").upper()+"\n #define "+
      typeListFileName.replace(".","_").upper() + "\n",file=typeFile)
   typeList=handler.getTypeNames()
   numElems=int(math.ceil(len(typeList) / 10.0)) * 10
   if (numElems <  MAXVECTORSIZE()):
      print ('#include <boost/mpl/vector/vector{}.hpp>'.format(numElems),file=typeFile)
   else:
      print ('#include "boost_mpl_vector_range_ext/vector{}.hpp"\n'.format(numElems),\
      file=typeFile)
   print ("#include \"enumeration_wrapper.hpp\"\n",file=typeFile)
   print ("namespace configuration\n{\n",file=typeFile)
   print ("typedef boost::mpl::vector{}<".format(len(typeList)),file=typeFile)
   steps=MAXTYPES()
   it=iter(typeList)
   # join types with comma then newline after n joins
   typeFile.write(',\n'.join(list(map(','.join,zip_longest(*[it]*steps,fillvalue='')))).rstrip(','))
   typeFile.write('> TYPELIST;\n}\n#endif\n')
except Exception as ex:
   print("exception: {0}".format(ex))
   raise
finally:
   typeFile.close()

try:
   aliasFile = open(aliasFileName, 'w')
   print ("//AUTO-GENERATED File, caution before modifying\n",file=aliasFile)
   print ("#ifndef "+aliasFileName.replace(".","_").upper()+"\n #define "+
          aliasFileName.replace(".","_").upper() + "\n",file=aliasFile)
   #count=int(math.ceil(len(seenStruc) / 10.0)) * 10
   print ('#include "VulkanConfigurator.hpp"\n', file=aliasFile)
   print ("namespace configuration\n{\n",file=aliasFile)
   print ('template <std::size_t ... I>\n',file=aliasFile)
   print ('constexpr void Configurator::TypesFor<std::index_sequence<I...>>::\
setupAliases() noexcept\n{\n',file=aliasFile)

   #aliasFile.write('char* key;\n')
   aliasFile.write('\n'.join(handler.getAliasLines())+'\n}\n}\n#endif\n')
except Exception as ex:
   print("exception: {0}".format(ex))
   raise
finally:
   aliasFile.close()

try:
   enumFile = open(enumMapFileName, 'w')
   print ("//AUTO-GENERATED File, caution before modifying\n",file=enumFile)
   print ("#ifndef "+enumMapFileName.replace(".","_").upper()+"\n #define "+
          enumMapFileName.replace(".","_").upper() + "\n",file=enumFile)
   #count=int(math.ceil(len(seenStruc) / 10.0)) * 10
   print ('#include "VulkanConfigurator.hpp"\n', file=enumFile)
   print ("namespace configuration\n{\n",file=enumFile)
   print ('constexpr Configurator::EnumMapInitializer::EnumMapInitializer() noexcept\n{\n',
   file=enumFile)
   #enumFile.write('char* key;\n')

   enumFile.write('\n'.join(handler.getEnumLines())+'\n}\n}\n#endif\n')
except Exception as ex:
   print("exception: {0}".format(ex))
   raise
finally:
   enumFile.close()


'''if __name__=='__main__':
parser=xml.sax.make_parser() #creating an XMLReader
parser.setFeature(xml.sax.handler.feature_namespaces,0) #turning off namespaces
Handler=SongHandler()
parser.setContentHandler(Handler) #overriding default ContextHandler
parser.parse('songs.xml')
startIdx=2
flags='''''
'''if (sys.argv[startIdx].strip("'").startswith('-')): # flags
   flags=sys.argv[startIdx]
   startIdx=startIdx+1'''

# Find out the xml generator (gccxml or castxml)
# generator_path, generator_name = utils.find_xml_generator()
'''metaFilename="vulkan_structs_meta.hpp"
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
   f.close()'''

'''if (generator_name != 'castxml'):
   print (generator_name+' does not equal castxml.  Please make sure castxml is installed.')
   sys.exit(1)'''

# Configure the xml generator
'''config = parser.xml_generator_configuration_t(
    xml_generator_path=generator_path,
    xml_generator=generator_name,
    cflags=flags)
    # find struct in vulkan headers
for path in sys.argv[startIdx:]:
   # Parsing source file
   decls = parser.parse([path], config)
   global_ns = declarations.get_global_namespace(decls)
   # generate vulkan type list code
   # iterate classes and get member information
   for clas in [global_ns.classes(),global_ns.enumerations()]:
      if (declarations.type_traits_classes.is_struct(clas) and clas.name.lower().startswith('vk') \
         and not seenType.get(clas.name)):
         seenType[clas.name]=CLASS_ID()
      elif (declarations.type_traits_classes.is_enum(clas) and not seenType.get(clas.name)):
         print ('enum:'+clas.name)
         seenEnum[clas.name]=clas.values()

# iterate all paths

#f = open(metaFileName, 'w')
try:
   #print ("//AUTO-GENERATED File, caution before modifying\n",file=f)
   #print ("#ifndef "+metaFilename.replace(".","_")+"\n #define "+
   #metaFilename.replace(".","_") + "\n",file=f)
   #print ("#include \"visit_struct.hpp\"\n",file=f)
   # iterate all paths
   for path in sys.argv[startIdx:]:
      # Parsing source file
      decls = parser.parse([path], config)
      global_ns = declarations.get_global_namespace(decls)
     # print ("#include \"{}\"\n".format(path), file=f)
      # iterate classes and get member information
      for typedef in :
         if (declarations.type_traits.is_fundamental(typedef.decl_type) and not(typedef.name.startswith("_"))):
            print ("typedef:"+typedef.name+","+str(declarations.type_traits.remove_alias(typedef)))
         #for depend in declarations.get_dependencies_from_decl(typedef):
            print(":"+str(type(depend)))
            if (depend.find_out_depend_on_it_declarations()):
            typeName=str(typedef.decl_type)
            try:
               typeName=getTypeDep(global_ns,typeName)
            except Exception:
               pass
            print("orig:"+typeName)
            #print("dependencies:"+depend.declaration.name+","+str(depend.declaration.decl_type)+","+str(type(depend.declaration.decl_type)))
except Exception as ex:
   print("exception: {0}".format(ex))
#   raise
#else:
   #print ("#endif\n",file=f)
print("#endif")'''
#finally:
#   f.close()
'''if (declarations.type_traits_classes.is_struct(clas) and clas.name.startswith('Vk') and not seenStruct.get(clas.name)):
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
   f.close()'''

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
   
'''if __name__ == '_main_':
   usageMsg="Usage: "+sys.argv[0]+" \{vulkan_xml_file\} \{compiler\} ['flags'] \{list of full paths to \
parse\}"
   startIdx=3
   if (len(sys.argv) < startIdx): 
      print ("This script will create the file that grabs metadata from structs\
visit_struct(....)\n depends: CastXML\n{}\n".format(usageMsg)
      sys.exit(1)
   
   if (not os.path.exists(sys.argv[startIdx-2])):
      print (("file {} does not exist\n"+usageMsg).format(sys.argv[startIdx-2]))

   if (not sys.argv[startIdx-1] in ("gcc","g++","msvc","clang")):
      print (usageMsg)
      sys.exit(1)
   if (sys.argv[startIdx].strip("'").startswith('-')): # flags
      startIdx=startIdx+1
   
   for i in range(startIdx,len(sys.argv)):
      if (not os.path.exits(sys.argv[i])):
         print (usageMsg)
         sys.exit(1)'''

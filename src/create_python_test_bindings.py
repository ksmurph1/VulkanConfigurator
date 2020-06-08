import os
import sys
import math
import itertools

from pygccxml import parser  # nopep8
from pygccxml import declarations  # nopep8
from pygccxml import utils  # nopep8

startIdx=2
flags=''
compiler=sys.argv[1]

if (sys.argv[startIdx].strip("'").startswith('-')): # flags
   flags=sys.argv[startIdx]
   startIdx=startIdx+1
outFilename="pygccxml-xmlprocessor-output.txt"
path=sys.argv[startIdx]

# Find out the xml generator (gccxml or castxml)
generator_path, generator_name = utils.find_xml_generator()
if (generator_name != 'castxml'):
   print (generator_name+' does not equal castxml.  Please make sure castxml is installed.')
   sys.exit(1)

# Configure the xml generator
config = parser.xml_generator_configuration_t(
    xml_generator_path=generator_path,
    xml_generator=generator_name, castxml_epic_version=1,
    cflags=flags)

def entityOutput(entity):
   result=''
   result=result+str(entity)+"\nis template instantiation:"+\
   str(declarations.templates.is_instantiation(str(entity)))
   if (declarations.templates.is_instantiation(str(entity))):
      result="\n"+result+declarations.templates.name(entity.decl_string)+" "+\
         str(declarations.templates.args(entity.decl_string))
   if (isinstance(entity,declarations.member_function_t) or isinstance(entity, declarations.member_calldef_t) or\
      isinstance(entity, declarations.variable_t) ):
      result= result +'\nmangled:'+entity.mangled if entity.mangled else ''
   return result

def dataMembersOutput(mem_fun,access_type):
   typedef_list=[]
   for td in mem_fun(function=declarations.access_type_matcher_t( access_type ),recursive=False):
      typedef_list.append(td)
#      if (mem_fun.__name__ == 'variables'):
#         print('mangled:'+td.get_mangled_name()+'\n')
   return typedef_list

def typeConversionsOutput(mem_fun,access_type):
   typedef_list=[]
   for td in mem_fun(function=declarations.access_type_matcher_t( access_type ),recursive=False):
      if (not td.is_artificial):
        # entityOutput(td)
         typedef_list.append(td)
   return typedef_list
'''def variablesOutput(decl,access_type):
   variables_list=[]
   for v in decl.variables(function=declarations.access_type_matcher_t( access_type ),recursive=False):
      entityOutput(v)
      variables_list.append(v)
   return variables_list'''

def zeroDeclsExCheck(ex):
    if (not 'returned 0 declarations' in str(ex)):
       raise ex

def dispClassFunctors(members:dict,memberType,fileDesc):
   print ("private member {0}:\n".format(memberType),file=fileDesc)
   for calldef in members['private']:
      print (entityOutput(calldef)+"\n",file=fileDesc)
   print ("public member {0}:\n".format(memberType),file=fileDesc)
   for calldef in members['public']:
      print (entityOutput(calldef)+"\n",file=fileDesc)
   print ("protected member {0}:\n".format(memberType),file=fileDesc)
   for calldef in members['protected']:
      print (entityOutput(calldef)+"\n",file=fileDesc)

def dispVarsTypeDefs(members:dict, memberType,fileDesc):
   print ("private member {0}:\n".format(memberType),file=fileDesc)
   for data in members['private']:
      print (data.decl_type, entityOutput(data)+"\n",file=fileDesc)
   print ("public member {0}:\n".format(memberType),file=fileDesc)
   for data in members['public']:
      print (data.decl_type, entityOutput(data)+"\n",file=fileDesc)
   print ("protected member {0}:\n".format(memberType),file=fileDesc)
   for data in members['protected']:
      print (data.decl_type, entityOutput(data)+"\n",file=fileDesc)

def dispEnums(members:dict, memberType,fileDesc):
   print ("private member {0}:\n".format(memberType),file=fileDesc)
   for enum in members['private']:
      print (entityOutput(enum)+"\n",file=fileDesc)
      for name,value in enum.values:
         print(name+":"+str(value),file=fileDesc)
   print ("public member {0}:\n".format(memberType),file=fileDesc)
   for enum in members['public']:
      print (entityOutput(enum)+"\n",file=fileDesc)
      for name,value in enum.values:
         print(name+":"+str(value),file=fileDesc)
   print ("protected member {0}:\n".format(memberType),file=fileDesc)
   for enum in members['protected']:
      print (entityOutput(enum)+"\n",file=fileDesc)
      for name,value in enum.values:
         print(name+":"+str(value),file=fileDesc)

f = open(outFilename, 'w')
try:
   '''print ("//AUTO-GENERATED File, caution before modifying\n",file=f)
   print ("#include <boost/python/python.hpp>", file=f)'''
   for path in sys.argv[startIdx:]:
      # Parsing source files
      decls = parser.parse([path], config)
      global_ns = declarations.get_global_namespace(decls)
      #print ("BOOST_PYTHON_MODULE("+global_ns.name+")\n{\n", file=f)
      #print ("#include \"{}\"\n".format(path), file=f)
         # iterate classes and get member information
      
      namespaces=[global_ns.namespace("configuration"),global_ns.namespace("folly"),global_ns.namespace("aa")]
      for n in namespaces:
         cur_class=None
         typedef_t_decl = {'private':[],'public':[],'protected':[]}
         var_t_decl={'private':[],'public':[],'protected':[]}
         free_function_t_decl=[]
         operators_t_decl={'private':[],'public':[],'protected':[]}
         enums_t_decl={'private':[],'public':[],'protected':[]}
         memfuns_t_decl={'private':[],'public':[],'protected':[]}
         for d in n.decls(recursive=True):
            # put a limit on folly exposure since so many classes
            if (n.name != "folly" or [i for i in ["Function","SysArena"] if i in d.name]):
               if isinstance(d, declarations.class_declaration_t):
                  print("possible template instantiation:",file=f)
                  print(entityOutput(d),file=f)
               
               if isinstance(d, declarations.free_function_t):
                  free_function_t_decl.append(d)
                  print("free_functions:",file=f)
                  print(str(free_function_t_decl[-1])+"\n",file=f)


               # get destructors      
               if (isinstance(d,declarations.calldef_t)):
                  if ('~' in str(d) and not d.is_artificial):
                     print("destructor:\n",file=f)
                     print(str(d)+"\n",file=f)
                     
               if isinstance(d, declarations.class_t):
                  # print ("classes or structs:\n")
                  # print(str(d)+"\n")
                  cur_class=d
                  memberType='variables'
                  try:
                     var_t_decl['private'].extend(dataMembersOutput(getattr(d,memberType),'private'))
                     var_t_decl['public'].extend(dataMembersOutput(getattr(d,memberType), 'public'))
                     var_t_decl['protected'].extend(dataMembersOutput(getattr(d,memberType), 'protected'))
                  except RuntimeError as ex:
                     zeroDeclsExCheck(ex)

                  memberType='enumerations'
                  try:
                     enums_t_decl['private'].extend(dataMembersOutput(getattr(d,memberType),'private'))
                     enums_t_decl['public'].extend(dataMembersOutput(getattr(d,memberType),'public'))
                     enums_t_decl['protected'].extend(dataMembersOutput(getattr(d,memberType),'protected'))
                  except RuntimeError as ex:
                     zeroDeclsExCheck(ex)
                  memberType='typedefs'
                  try:
                     typedef_t_decl['private'].extend(dataMembersOutput(d.typedefs,'private'))
                     typedef_t_decl['public'].extend(dataMembersOutput(d.typedefs,'public'))
                     typedef_t_decl['protected'].extend(dataMembersOutput(d.typedefs,'protected'))
                  except RuntimeError as ex:
                     zeroDeclsExCheck(ex)

                  memberType='member_functions'
                  try:
                     memfuns_t_decl['private'].extend(dataMembersOutput(d.member_functions,'private'))
                     memfuns_t_decl['public'].extend(dataMembersOutput(d.member_functions,'public'))
                     memfuns_t_decl['protected'].extend(dataMembersOutput(d.member_functions,'protected'))
                  except RuntimeError as ex:
                     zeroDeclsExCheck(ex)
                 
                  memberType='casting_operators'
                  try:
                     operators_t_decl['private'].extend(typeConversionsOutput(getattr(d,memberType),'private'))
                     operators_t_decl['public'].extend(typeConversionsOutput(getattr(d,memberType),'public'))
                     operators_t_decl['protected'].extend(typeConversionsOutput(getattr(d,memberType),'protected'))
                  except RuntimeError as ex:
                     zeroDeclsExCheck(ex)

                  memberType='constructors'
                  try:
                     memfuns_t_decl['private'].extend(typeConversionsOutput(getattr(d,memberType),'private'))
                     memfuns_t_decl['public'].extend(typeConversionsOutput(getattr(d,memberType),'public'))
                     memfuns_t_decl['protected'].extend(typeConversionsOutput(getattr(d,memberType),'protected'))
                  except RuntimeError as ex:
                     zeroDeclsExCheck(ex)
                  
                  memberType='member_operators'
                  try:
                     operators_t_decl['private'].extend(typeConversionsOutput(getattr(d,memberType),'private'))
                     operators_t_decl['public'].extend(typeConversionsOutput(getattr(d,memberType),'public'))
                     operators_t_decl['protected'].extend(typeConversionsOutput(getattr(d,memberType),'protected'))
                  except RuntimeError as ex:
                     zeroDeclsExCheck(ex)

               if isinstance(d, declarations.typedef_t):
                  if (d not in itertools.chain.from_iterable(typedef_t_decl.values())):
                     print("typedef:\n",file=f)
                     print(d.decl_type, entityOutput(d),file=f)

               if isinstance(d,declarations.variable_t):
                  if (d not in itertools.chain.from_iterable(var_t_decl.values())):
                     print("variable:\n",file=f)
                     print (d.decl_type, entityOutput(d),file=f)  
               
               if isinstance(d,declarations.free_operator_t):
                  if (d not in itertools.chain.from_iterable(operators_t_decl.values())):
                     print("operator:\n",file=f)
                     print(enitityOutput(d),file=f)

         if (cur_class):      
            print ("classes or structs:\n",file=f)
            print(str(cur_class)+"\n", file=f)
         dispVarsTypeDefs(typedef_t_decl,'typedefs',f)
         dispEnums(enums_t_decl,'enumerations',f)
         dispClassFunctors(memfuns_t_decl,'functions',f)
         dispClassFunctors(operators_t_decl, 'operators',f)
               # Search for the function by name
#      criteria = declarations.declaration_matcher(name="SysArena")
#      print(str(declarations.matcher.get_single(criteria, global_ns.namespace("aa"))))

   
   # > ns::B [struct]
   # > ns::D [struct]

   
   # > ns::T<ns::B::D, bool> [class declaration]
         


   # > ns::T<ns::B::D, bool> ns::function() [free function]
      
   '''  for clas in global_ns.classes():
       global_ns.
         if (declarations.type_traits_classes.is_struct(clas) and clas.name.startswith('Vk') and \
            not seenStruct.get(clas.name)):
            strings=["VISITABLE_STRUCT({}".format(clas.name)]
            seenStruct[clas.name]=1
            for member in clas.public_members:
               if (isinstance(member,declarations.variable_t)):
                  strings.append("{}".format(member.name))
            print(','.join(strings)+");\n", file=f)'''
except Exception as ex:
   print("exception: {0}".format(ex))
   raise
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

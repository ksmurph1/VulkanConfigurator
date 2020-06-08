#include "StructureTypes.hpp"
/*configuration::Configurator::MapBuilder::MapType configuration::XmlProcessor::mapExtractor=
                              configuration::Configurator::MapBuilder().getExtractor();*/
aa::unordered_map<aa::string,std::any> configuration::StructureTypes::container(
    boost::mpl::size<configuration::Configurator::types>::value,
    aa::recursive_allocator<aa::string>(aa::Alloc<aa::string>(aa::ARENA))); 
//
// Created by kerry on 3/18/21.
//

#ifndef CHILDTYPEPARENTFIELDMATCHING_INL
#define CHILDTYPEPARENTFIELDMATCHING_INL
template <typename FieldType,std::enable_if_t<std::is_pointer_v<FieldType>,int> = 0>
FieldType configuration::ChildTypeParentFieldMatching::convertChildStructToVal(FieldExtractor* const extractor, const char* typeName) noexcept
{
// we have something worth get typeinfo from common type
    return std::visit([this,extractor](const auto& arg2) -> FieldType
                      {
                          auto ptr=TypeAllocators::TypeAlloc<UtilityTypes::UnderlyingType<decltype(arg2)>>.allocate(count);
                          std::transform(children,children+count,ptr,[](const UtilityTypes::KeyValType& val)
                          {
                              return std::any_cast<UtilityTypes::UnderlyingType<decltype(arg2)>>((val.second)->getContainer());
                          });
                          FieldType field= extractor->convertToFieldValue<FieldType>(ptr,count);
                          TypeAllocators::TypeAlloc<UtilityTypes::UnderlyingType<decltype(arg2)>>.deallocate(ptr,count);
                          return field;
                      }, Configurator::typeMap.getType(typeName));
}
template <typename FieldType,std::enable_if_t<!std::is_pointer_v<FieldType> && configuration::is_enumeration<FieldType>::value,int> = 0>
std::optional<typename FieldType::type> configuration::ChildTypeParentFieldMatching::convertChildStructToVal(FieldExtractor* const extractor, const char * typeName) noexcept
{
    return std::visit([this,extractor](const auto& arg2) -> std::optional<typename FieldType::type>
                      {
                          return extractor->convertToFieldValue<FieldType>(std::any_cast<UtilityTypes::UnderlyingType<decltype(arg2)>>(
                                  (children->second)->getContainer()));
                      }, Configurator::typeMap.getType(typeName));
}

template <typename FieldType,std::enable_if_t<!std::is_pointer_v<FieldType> && !configuration::is_enumeration<FieldType>::value,int> = 0>
std::optional<FieldType> configuration::ChildTypeParentFieldMatching::convertChildStructToVal(FieldExtractor* const extractor, const char* typeName) noexcept
{
    return std::visit([this,extractor](const auto& arg2) -> std::optional<FieldType>
                      {
                          return extractor->convertToFieldValue<FieldType>(std::any_cast<UtilityTypes::UnderlyingType<decltype(arg2)>>(
                                  (children->second)->getContainer()));
                      }, Configurator::typeMap.getType(typeName));
}
inline uint8_t configuration::ChildTypeParentFieldMatching::getMatchCount() const noexcept
{
    return count;
}
#endif //CHILDTYPEPARENTFIELDMATCHING_INL

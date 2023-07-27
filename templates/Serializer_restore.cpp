/*
 Do not modify, auto-generated by model_gen.tcl

 Copyright 2019 Alain Dargelas

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

/*
 * File:   Serializer_restore.cpp
 * Author:
 *
 * Created on December 14, 2019, 10:03 PM
 */
#include <uhdm/Serializer.h>

#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>
#include <sys/types.h>

#if defined(_MSC_VER)
  #include <io.h>
#else
  #include <unistd.h>
#endif

#include <iostream>
#include <vector>

#include <capnp/message.h>
#include <capnp/serialize-packed.h>

#include "UHDM.capnp.h"
#include <uhdm/uhdm.h>


namespace UHDM {
static constexpr std::string_view kUnknownRawSymbol = "<unknown>";

template <typename T>
inline T* Serializer::Make(FactoryT<T>* const factory) {
  T* const obj = factory->Make();
  obj->SetSerializer(this);
  obj->UhdmId(++m_objId);
  return obj;
}

template <typename T>
void Serializer::Make(FactoryT<T>* const factory, uint32_t count) {
  for (uint32_t i = 0; i < count; ++i) Make(factory);
}

template <typename T>
inline std::vector<T*>* Serializer::Make(FactoryT<std::vector<T*>>* const factory) {
  return factory->Make();
}

BaseClass* Serializer::GetObject(uint32_t objectType, uint32_t index) const {
  if (index == kBadIndex) {
    return nullptr;
  }

  switch (objectType) {
<FACTORY_GET_OBJECT>
    default: return nullptr;
  }
}

<FACTORY_FUNCTION_IMPLEMENTATIONS>

struct Serializer::RestoreAdapter {
  void operator()(Any::Reader reader, Serializer *const serializer, BaseClass *const obj) const {
    obj->VpiParent(serializer->GetObject(reader.getVpiParent().getType(), reader.getVpiParent().getIndex() - 1));
    obj->VpiFile(serializer->symbolMaker.GetSymbol(SymbolId(reader.getVpiFile(), kUnknownRawSymbol)));
    obj->VpiLineNo(reader.getVpiLineNo());
    obj->VpiColumnNo(reader.getVpiColumnNo());
    obj->VpiEndLineNo(reader.getVpiEndLineNo());
    obj->VpiEndColumnNo(reader.getVpiEndColumnNo());
    obj->UhdmId(reader.getUhdmId());
  };

<CAPNP_RESTORE_ADAPTERS>

  template<typename T, typename U, typename = typename std::enable_if<std::is_base_of<BaseClass, T>::value>::type>
  void operator()(typename ::capnp::List<U>::Reader reader, Serializer *serializer, typename FactoryT<T>::objects_t &objects) const {
    uint32_t index = 0;
    for (typename U::Reader obj : reader)
      operator()(obj, serializer, objects[index++]);
  }
};

const std::vector<vpiHandle> Serializer::Restore(const std::filesystem::path& filepath) {
    return Restore( filepath.string());
}
const std::vector<vpiHandle> Serializer::Restore(const std::string& filepath) {
  Purge();
  std::vector<vpiHandle> designs;
  const std::string file = filepath;
  int32_t fileid = open(file.c_str(), O_RDONLY | O_BINARY);
  ::capnp::ReaderOptions options;
  options.traversalLimitInWords = ULLONG_MAX;
  options.nestingLimit = 1024;
  ::capnp::PackedFdMessageReader message(fileid, options);
  UhdmRoot::Reader cap_root = message.getRoot<UhdmRoot>();
  m_version = cap_root.getVersion();
  if (m_version != kVersion) return designs;

  const ::capnp::List<::capnp::Text>::Reader& symbols = cap_root.getSymbols();
  for (const auto& symbol : symbols) {
    symbolMaker.Make(symbol.cStr());
  }

<CAPNP_INIT_FACTORIES>
  // This assignment should happen only after the necessary objects are created.
  m_objId = cap_root.getObjectId();

  RestoreAdapter adapter;
<CAPNP_RESTORE_FACTORIES>

   for (auto d : designMaker.objects_) {
    vpiHandle designH = uhdm_handleMaker.Make(uhdmdesign, d);
    designs.push_back(designH);
  }

  close(fileid);
  return designs;
}
}  // namespace UHDM

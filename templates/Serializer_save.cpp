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
 * File:   Serializer_save.cpp
 * Author:
 *
 * Created on December 14, 2019, 10:03 PM
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#if (defined(_MSC_VER) || defined(__MINGW32__) || defined(__CYGWIN__))
  #include <io.h>
  #define S_IRWXU (_S_IREAD | _S_IWRITE)
  #pragma warning(push)
  #pragma warning(disable : 4267)  // 'var' : conversion from 'size_t' to 'type', possible loss of data
#else
  #include <unistd.h>
  #define O_BINARY 0
#endif

#include <vector>
#include <map>
#include <iostream>

#include <capnp/message.h>
#include <capnp/serialize-packed.h>

#include "headers/uhdm_types.h"
#include "headers/containers.h"
#include "headers/uhdm.h"
#include "UHDM.capnp.h"
#include "headers/Serializer.h"

using namespace UHDM;

void Serializer::SetId(const BaseClass* p, unsigned long id) {
  allIds_.insert(std::make_pair(p, id));
}

unsigned long Serializer::GetId(const BaseClass* p) {
  std::unordered_map<const BaseClass*, unsigned long>::iterator itr = allIds_.find(p);
  if (itr == allIds_.end()) {
    unsigned long tmp = incrId_;
    allIds_.insert(std::make_pair(p, incrId_));
    incrId_++;
    return tmp;
  } else {
    return (*itr).second;
  }
}

<UHDM_NAME_MAP>

std::string UHDM::VpiTypeName(vpiHandle h) {
  uhdm_handle* handle = (uhdm_handle*) h;
  BaseClass* obj = (BaseClass*) handle->object;
  return UhdmName(obj->UhdmType());
}


<METHODS_CPP>

BaseClass* Serializer::GetObject(unsigned int objectType, unsigned int index) {
  switch (objectType) {
<FACTORY_OBJECT_TYPE_MAP>
  default:
    return NULL;
  }
  return NULL;
}

void Serializer::Purge() {
<FACTORY_PURGE>
}

void Serializer::Save(const std::string& file) {
  const int fileid = open(file.c_str(), O_CREAT | O_WRONLY | O_BINARY, S_IRWXU);
  ::capnp::MallocMessageBuilder message;
  UhdmRoot::Builder cap_root = message.initRoot<UhdmRoot>();
  unsigned long index = 0;

<CAPNP_ID>

  ::capnp::List<Design>::Builder designs = cap_root.initDesigns(designMaker.objects_.size());
  index = 0;
  for (auto design : designMaker.objects_) {
    designs[index].setVpiName(design->GetSerializer()->symbolMaker.Make(design->VpiName()));
    index++;
  }

  ::capnp::List<::capnp::Text>::Builder symbols = cap_root.initSymbols(symbolMaker.id2SymbolMap_.size());
  index = 0;
  for (auto symbol : symbolMaker.id2SymbolMap_) {
    symbols.set(index,strdup(symbol.c_str()));
    index++;
  }

<CAPNP_SAVE>

  writePackedMessageToFd(fileid, message);
  close(fileid);
}

#if (defined(_MSC_VER) || defined(__MINGW32__) || defined(__CYGWIN__))
  #pragma warning(pop)
#endif

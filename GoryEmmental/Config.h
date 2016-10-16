#pragma once

#define EMMENTAL_MAX_STACK_SIZE (1000)
#define EMMENTAL_MAX_QUEUE_SIZE (1000)
#define EMMENTAL_MAX_RECURSION_LEVEL (500)

using SymbolT = unsigned char;
using ProgramT = std::vector<SymbolT>;
using SymbolMapT = std::map<SymbolT, std::shared_ptr<class EmmentalDefinition>>;
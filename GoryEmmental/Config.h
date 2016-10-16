#pragma once

using SymbolT = unsigned char;
using ProgramT = std::vector<SymbolT>;
using SymbolMapT = std::map<SymbolT, std::shared_ptr<class EmmentalDefinition>>;
#pragma once

#include "llvm/Object/ELF.h"
#include "llvm/Object/ObjectFile.h"
#include "llvm/Support/Error.h"
#include <string>
#include <vector>

namespace llvm {

    struct FunctionSymbol {
        uint64_t Address;
        std::string Name;
        std::string DemangledName;
    };

    class ELFSymbolReader {
        public:
            //Expected datatype is an llvm data type that will either have the type specified in the <> or contain an error message explaining why the error occurred.
            static Expected<std::vector<FunctionSymbol>> readSymbols(StringRef Filename);

            static Expected<std::string> detectArchitecture(StringRef Filename);

            static std::string demangleName(StringRef MangledName);
    };

} // namespace llvm





#include "ELFSymbolReader.h"
#include "llvm/Object/Binary.h"
#include "llvm/Support/Error.h"
#include "llvm/Object/ELFObjectFile.h"
#include "llvm/Support/Casting.h"
#include "llvm/Object/ObjectFile.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Demangle/Demangle.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/TargetSelect.h"


using namespace llvm;
using namespace llvm::object;

Expected<std::vector<FunctionSymbol>> ELFSymbolReader::readSymbols(StringRef File){
    Expected<OwningBinary<Binary>> BinOrErr = createBinary(File);
    if(!BinOrErr){
        return BinOrErr.takeError();
    }

    const ELFObjectFileBase *Obj = dyn_cast<ELFObjectFileBase>(BinOrErr->getBinary());
    if(!Obj){
        return createStringError(inconvertibleErrorCode(), "Not an ELF");
    }

    std::vector<FunctionSymbol> Syms;

    for(const SymbolRef &Sym : Obj->symbols()){
        if(!(cantFail(Sym.getType()) & SymbolRef::ST_Function)){
            continue;
        }
        if(cantFail(Sym.getFlags()) & SymbolRef::SF_Undefined){
            continue;
        }
        Expected<StringRef> NameOrErr = Sym.getName();
        if(!NameOrErr){
            consumeError(NameOrErr.takeError());
            continue;
        }
        StringRef FuncNameRef = *NameOrErr;
        if(FuncNameRef.starts_with("__")){
            continue;
        }
        
        // Skip symbols not in executable sections (e.g., not in .text)
        Expected<section_iterator> SecOrErr = Sym.getSection();
        if (!SecOrErr || *SecOrErr == Obj->section_end()){
            continue;
        }
        StringRef SectionName;
        if (Expected<StringRef> NameOrErr = (*SecOrErr)->getName()) {
            SectionName = *NameOrErr;
            if (!SectionName.equals_insensitive(".text")){
                continue;
            }
        } else {
            consumeError(NameOrErr.takeError());
            continue;
        }


        uint64_t Addr = cantFail(Sym.getAddress());
        StringRef NameRef = cantFail(Sym.getName());

        FunctionSymbol FS;
        FS.Address = Addr;
        FS.Name = NameRef.str();
        FS.DemangledName = llvm::demangle(FS.Name);
        Syms.push_back(std::move(FS));
    }
    return Syms;
}

Expected<std::string> ELFSymbolReader::detectArchitecture(StringRef File){
    Expected<OwningBinary<Binary>> BinOrErr = createBinary(File);
    if(!BinOrErr){
        return BinOrErr.takeError();
    }
    
    const ELFObjectFileBase *Obj = dyn_cast<ELFObjectFileBase>(BinOrErr->getBinary());
    if(!Obj){
        return createStringError(inconvertibleErrorCode(), "Not an ELF");
    }
    return std::string(Triple::getArchTypeName(Obj->getArch()));
}

std::string ELFSymbolReader::demangleName(StringRef N){
    return llvm::demangle(N.str());
}
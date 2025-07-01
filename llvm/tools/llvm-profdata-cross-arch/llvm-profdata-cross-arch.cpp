#include "ELFSymbolReader.h"
#include "llvm/IR/Function.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Error.h"
#include "llvm/Support/raw_ostream.h"

#include <string>
#include <utility>
#include <vector>

using namespace llvm;

static cl::SubCommand AnalyzeSymbolsSubcommand("analyze-symbols", "Analyze symbols in ELF files for cross-arch profiling");
static cl::SubCommand AnalyzeProfrawSubcommand("analyze-profraw", "Used to analyze mappings between source code and coverage data");

static cl::list<std::string> ELFFiles(cl::Positional, cl::sub(AnalyzeSymbolsSubcommand), cl::desc("<elf-files>"), cl::OneOrMore);
static cl::list<std::string> ProfrawFiles(cl::Positional, cl::sub(AnalyzeProfrawSubcommand), cl::desc("<profraw-files>"), cl::OneOrMore);

static void findAndPrintMatches(const std::vector<std::vector<FunctionSymbol>> &AllSymbols, const std::vector<std::string> &Architectures) {
    const std::vector<FunctionSymbol> &Symbols1 = AllSymbols[0];
    const std::vector<FunctionSymbol> &Symbols2 = AllSymbols[1];

    std::vector<std::pair<FunctionSymbol, FunctionSymbol>> Matches;

    for (const FunctionSymbol &Sym1 : Symbols1) {
        for (const FunctionSymbol &Sym2 : Symbols2) {
            if (Sym1.Name == Sym2.Name) {
                Matches.push_back({Sym1, Sym2});
                continue;
            }

            if (!Sym1.DemangledName.empty() && !Sym2.DemangledName.empty() && Sym1.DemangledName == Sym2.DemangledName) {
                Matches.push_back({Sym1, Sym2});
                continue;
            }
        }
    }

    outs() << "Potential matches found: " << Matches.size() << "\n";
    for (const auto &Match : Matches) {
        if (Match.first.DemangledName == Match.second.DemangledName) {
            outs() << " " << Match.first.DemangledName << " <-> " << Match.second.DemangledName << " (demangled match)\n";
        } else {
            outs() << " " << Match.first.Name << " <-> " << Match.second.Name << " (exact match)\n";
        }
    }

    outs() << "\n" << Architectures[0] << "-only functions: " << (Symbols1.size() - Matches.size()) << "\n";
    outs() << Architectures[1] << "-only functions: " << (Symbols2.size() - Matches.size()) << "\n";

    if (!Matches.empty()) {
        outs() << "\nSUCCESS: Found " << Matches.size() << " functions that exist in both architectures!\n";
    } else {
        outs() << "\nWARNING: No matching functions found between architectures.\n";
    }
}

static int analyzeSymbols_main(StringRef ProgName) {
    outs() << "=== Symbol Analysis ==\n\n";
    std::vector<std::vector<FunctionSymbol>> AllSymbols;
    std::vector<std::string> Architectures;

    for (const std::string &Filename : ELFFiles) {
        Expected<std::string> ArchOrErr = ELFSymbolReader::detectArchitecture(Filename);
        if (!ArchOrErr) {
            errs() << "Error detecting architecture for " << Filename << ": " << toString(ArchOrErr.takeError()) << "\n";
            continue;
        }

        Expected<std::vector<FunctionSymbol>> SymbolsOrErr = ELFSymbolReader::readSymbols(Filename);
        if (!SymbolsOrErr) {
            errs() << "Error reading symbols from " << Filename << ": " << toString(SymbolsOrErr.takeError()) << "\n";
            continue;
        }

        std::string Arch = ArchOrErr.get();
        std::vector<FunctionSymbol> Symbols = SymbolsOrErr.get();

        outs() << "File: " << Filename << "\n";
        outs() << "Architecture: " << Arch << "\n";
        outs() << "Functions found: " << Symbols.size() << "\n";

        for (const FunctionSymbol &Symbol : Symbols) {
            outs() << " 0x" << format("%08x", Symbol.Address) << ": " << Symbol.Name;
            if (Symbol.DemangledName != Symbol.Name) {
                outs() << " (" << Symbol.DemangledName << ")";
            }
            outs() << "\n";
        }
        outs() << "\n";

        AllSymbols.push_back(std::move(Symbols));
        Architectures.push_back(std::move(Arch));
    }

    if (AllSymbols.size() >= 2) {
        outs() << "=== Cross-Architecture Analysis ==\n";
        findAndPrintMatches(AllSymbols, Architectures);
    }

    return 0;
}

int main(int argc, const char *argv[]) {
    cl::ParseCommandLineOptions(argc, argv, "LLVM cross-architecture profile analysis\n");
    if (AnalyzeSymbolsSubcommand) {
        return analyzeSymbols_main(argv[0]);
    }

    cl::PrintHelpMessage();
    return 1;
}

set(LLVM_TARGET_DEFINITIONS ${PROJECT_SOURCE_DIR}/lib/Target/ARM/ARM.td)
tablegen(LLVM ARMTargetParserDef.inc -gen-arm-target-def EXTRA_INCLUDES ${PROJECT_SOURCE_DIR}/lib/Target/ARM)

set(LLVM_TARGET_DEFINITIONS ${PROJECT_SOURCE_DIR}/lib/Target/AArch64/AArch64.td)
tablegen(LLVM AArch64TargetParserDef.inc -gen-arm-target-def EXTRA_INCLUDES ${PROJECT_SOURCE_DIR}/lib/Target/AArch64)

set(LLVM_TARGET_DEFINITIONS ${PROJECT_SOURCE_DIR}/lib/Target/RISCV/RISCV.td)
tablegen(LLVM RISCVTargetParserDef.inc -gen-riscv-target-def EXTRA_INCLUDES ${PROJECT_SOURCE_DIR}/lib/Target/RISCV)

set(LLVM_TARGET_DEFINITIONS ${PROJECT_SOURCE_DIR}/lib/Target/PowerPC/PPC.td)
tablegen(LLVM PPCGenTargetFeatures.inc -gen-target-features -I${PROJECT_SOURCE_DIR}/lib/Target/PowerPC)

# This covers all of the tablegen calls above.
add_public_tablegen_target(target_parser_gen)

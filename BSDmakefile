#------------------------------------------------------------------------------#
# ELENA make file                                                              #
#------------------------------------------------------------------------------#

WRKDIR = `pwd`
MAKE = make

all_amd64: elc_amd64 sg_amd64 og_amd64 asmc_amd64 ecv_amd64 elenart_amd64 elenasm_amd64 elenavm_amd64

clang_all_amd64: clang_elc_amd64 clang_sg_amd64 clang_og_amd64 clang_asmc_amd64 clang_ecv_amd64 clang_elenart_amd64 clang_elenasm_amd64 clang_elenavm_amd64

elc_amd64: 
	$(MAKE) -C elenasrc3/elc/make all -f bsd.elc_amd64.mak

clang_elc_amd64: 
	$(MAKE) -C elenasrc3/elc/make all -f bsd.clang_elc_amd64.mak

clang_cross_elc_amd64: 
	$(MAKE) -C elenasrc3/elc/make all -f bsd.clang_cross_elc_amd64.mak

sg_amd64: 
	$(MAKE) -C elenasrc3/tools/sg/codeblocks all -f bsd.sg_amd64.mak

clang_sg_amd64: 
	$(MAKE) -C elenasrc3/tools/sg/codeblocks all -f bsd.clang_sg_amd64.mak

og_amd64: 
	$(MAKE) -C elenasrc3/tools/og/codeblocks all -f bsd.og_amd64.mak

clang_og_amd64: 
	$(MAKE) -C elenasrc3/tools/og/codeblocks all -f bsd.clang_og_amd64.mak

asmc_amd64: 
	$(MAKE) -C elenasrc3/tools/asmc/codeblocks all -f bsd.asmc_amd64.mak

clang_asmc_amd64: 
	$(MAKE) -C elenasrc3/tools/asmc/codeblocks all -f bsd.clang_asmc_amd64.mak

ecv_amd64: 
	$(MAKE) -C elenasrc3/tools/ecv/codeblocks all -f bsd.ecv_amd64.mak

clang_ecv_amd64: 
	$(MAKE) -C elenasrc3/tools/ecv/codeblocks all -f bsd.clang_ecv_amd64.mak

elenart_amd64: 
	$(MAKE) -C elenasrc3/elenart/codeblocks all -f bsd.elenart_amd64.mak

clang_elenart_amd64: 
	$(MAKE) -C elenasrc3/elenart/codeblocks all -f bsd.clang_elenart_amd64.mak

elenasm_amd64: 
	$(MAKE) -C elenasrc3/elenasm/codeblocks all -f bsd.elenasm_amd64.mak

elenavm_amd64: 
	$(MAKE) -C elenasrc3/elenavm/codeblocks all -f bsd.elenavm_amd64.mak

clang_elenasm_amd64: 
	$(MAKE) -C elenasrc3/elenasm/codeblocks all -f bsd.clang_elenasm_amd64.mak

clang_elenavm_amd64: 
	$(MAKE) -C elenasrc3/elenavm/codeblocks all -f bsd.clang_elenavm_amd64.mak

clean_amd64: clean_elc_amd64 clean_og_amd64 clean_sg_amd64 clean_asmc_amd64 clean_ecv_amd64 clean_elenart_amd64 clean_elenasm_amd64 clean_elenavm_amd64

clean_elc_amd64: 
	$(MAKE) -C elenasrc3/elc/make clean -f bsd.elc_amd64.mak

clean_cross_elc_amd64: 
	$(MAKE) -C elenasrc3/elc/make clean -f bsd.clang_cross_elc_amd64.mak

clean_sg_amd64: 
	$(MAKE) -C elenasrc3/tools/sg/codeblocks clean -f bsd.sg_amd64.mak

clean_og_amd64: 
	$(MAKE) -C elenasrc3/tools/og/codeblocks clean -f bsd.og_amd64.mak

clean_og_ppc64le: 
	$(MAKE) -C elenasrc3/tools/og/codeblocks clean -f bsd.og_ppc64le.mak

clean_asmc_amd64: 
	$(MAKE) -C elenasrc3/tools/asmc/codeblocks clean -f bsd.asmc_amd64.mak

clean_ecv_amd64: 
	$(MAKE) -C elenasrc3/tools/ecv/codeblocks clean -f bsd.ecv_amd64.mak

clean_elenart_amd64:
	$(MAKE) -C elenasrc3/elenart/codeblocks clean -f bsd.elenart_amd64.mak

clean_elenasm_amd64:
	$(MAKE) -C elenasrc3/elenasm/codeblocks clean -f bsd.elenasm_amd64.mak

clean_elenavm_amd64:
	$(MAKE) -C elenasrc3/elenavm/codeblocks clean -f bsd.elenavm_amd64.mak

.PHONY: clean_elc_amd64 clean_sg_amd64 clean_og_amd64 clean_asmc_amd64 clean_ecv_amd64 clean_elenart_amd64 clean_elenasm_amd64 clean_elenavm_amd64

#------------------------------------------------------------------------------#
# ELENA make file                                                              #
#------------------------------------------------------------------------------#

WRKDIR = `pwd`
MAKE = make

all_i386: elc_i386 sg_i386 og_i386 asmc_i386 ecv_i386 elenart_i386

all_amd64: elc_amd64 sg_amd64 og_amd64 asmc_amd64 ecv_amd64 elenart_amd64

all_ppc64le: elc_ppc64le sg_ppc64le og_ppc64le asmc_ppc64le ecv_ppc64le elenart_ppc64le

all_arm64: elc_arm64 sg_arm64 og_arm64 asmc_arm64 ecv_arm64 elenart_arm64

elc_i386: 
	$(MAKE) -C elenasrc3/elc/codeblocks all -f elc_i386.mak

sg_i386: 
	$(MAKE) -C elenasrc3/tools/sg/codeblocks all -f sg_i386.mak

og_i386: 
	$(MAKE) -C elenasrc3/tools/og/codeblocks all -f og_i386.mak

asmc_i386: 
	$(MAKE) -C elenasrc3/tools/asmc/codeblocks all -f asmc_i386.mak

ecv_i386: 
	$(MAKE) -C elenasrc3/tools/ecv/codeblocks all -f ecv_i386.mak

elenart_i386: 
	$(MAKE) -C elenasrc3/elenart/codeblocks all -f elenart_i386.mak

elc_amd64: 
	$(MAKE) -C elenasrc3/elc/codeblocks all -f elc_amd64.mak

sg_amd64: 
	$(MAKE) -C elenasrc3/tools/sg/codeblocks all -f sg_amd64.mak

og_amd64: 
	$(MAKE) -C elenasrc3/tools/og/codeblocks all -f og_amd64.mak

asmc_amd64: 
	$(MAKE) -C elenasrc3/tools/asmc/codeblocks all -f asmc_amd64.mak

ecv_amd64: 
	$(MAKE) -C elenasrc3/tools/ecv/codeblocks all -f ecv_amd64.mak

elenart_amd64: 
	$(MAKE) -C elenasrc3/elenart/codeblocks all -f elenart_amd64.mak

elc_ppc64le: 
	$(MAKE) -C elenasrc3/elc/codeblocks all -f elc_ppc64le.mak

sg_ppc64le: 
	$(MAKE) -C elenasrc3/tools/sg/codeblocks all -f sg_ppc64le.mak

og_ppc64le: 
	$(MAKE) -C elenasrc3/tools/og/codeblocks all -f og_ppc64le.mak

asmc_ppc64le: 
	$(MAKE) -C elenasrc3/tools/asmc/codeblocks all -f asmc_ppc64le.mak

ecv_ppc64le: 
	$(MAKE) -C elenasrc3/tools/ecv/codeblocks all -f ecv_ppc64le.mak

elenart_ppc64le: 
	$(MAKE) -C elenasrc3/elenart/codeblocks all -f elenart_ppc64le.mak

elc_arm64: 
	$(MAKE) -C elenasrc3/elc/codeblocks all -f elc_arm64.mak

sg_arm64: 
	$(MAKE) -C elenasrc3/tools/sg/codeblocks all -f sg_arm64.mak

og_arm64: 
	$(MAKE) -C elenasrc3/tools/og/codeblocks all -f og_arm64.mak

asmc_arm64: 
	$(MAKE) -C elenasrc3/tools/asmc/codeblocks all -f asmc_arm64.mak

ecv_arm64: 
	$(MAKE) -C elenasrc3/tools/ecv/codeblocks all -f ecv_arm64.mak

elenart_arm64: 
	$(MAKE) -C elenasrc3/elenart/codeblocks all -f elenart_arm64.mak

clean_i386: clean_elc_i386 clean_og_i386 clean_sg_i386 clean_asmc_i386 clean_ecv_i386 clean_elenart_i386

clean_amd64: clean_elc_amd64 clean_og_amd64 clean_sg_amd64 clean_asmc_amd64 clean_ecv_amd64 clean_elenart_amd64

clean_ppc64le: clean_elc_ppc64leclean_og_ppc64le clean_sg_ppc64le clean_asmc_ppc64le clean_ecv_ppc64le clean_elenart_ppc64le

clean_arm64: clean_elc_arm64 clean_og_arm64 clean_sg_arm64 clean_asmc_arm64 clean_ecv_arm64 clean_elenart_arm64

clean_elc_i386: 
	$(MAKE) -C elenasrc3/elc/codeblocks clean -f elc_i386.mak

clean_sg_i386: 
	$(MAKE) -C elenasrc3/tools/sg/codeblocks clean -f sg_i386.mak

clean_og_i386: 
	$(MAKE) -C elenasrc3/tools/og/codeblocks clean -f og_i386.mak

clean_asmc_i386: 
	$(MAKE) -C elenasrc3/tools/asmc/codeblocks clean -f asmc_i386.mak

clean_ecv_i386: 
	$(MAKE) -C elenasrc3/tools/ecv/codeblocks clean -f ecv_i386.mak

clean_elenart_i386:
	$(MAKE) -C elenasrc3/elenart/codeblocks clean -f elenart_i386.mak

clean_elc_amd64: 
	$(MAKE) -C elenasrc3/elc/codeblocks clean -f elc_amd64.mak

clean_sg_amd64: 
	$(MAKE) -C elenasrc3/tools/sg/codeblocks clean -f sg_amd64.mak

clean_asmc_amd64: 
	$(MAKE) -C elenasrc3/tools/asmc/codeblocks clean -f asmc_amd64.mak

clean_ecv_amd64: 
	$(MAKE) -C elenasrc3/tools/ecv/codeblocks clean -f ecv_amd64.mak

clean_elenart_amd64:
	$(MAKE) -C elenasrc3/elenart/codeblocks clean -f elenart_amd64.mak

clean_elc_ppc64le: 
	$(MAKE) -C elenasrc3/elc/codeblocks clean -f elc_ppc64le.mak

clean_sg_ppc64le: 
	$(MAKE) -C elenasrc3/tools/sg/codeblocks clean -f sg_ppc64le.mak

clean_asmc_ppc64le: 
	$(MAKE) -C elenasrc3/tools/asmc/codeblocks clean -f asmc_ppc64le.mak

clean_ecv_ppc64le: 
	$(MAKE) -C elenasrc3/tools/ecv/codeblocks clean -f ecv_ppc64le.mak

clean_elenart_ppc64le:
	$(MAKE) -C elenasrc3/elenart/codeblocks clean -f elenart_ppc64le.mak


clean_elc_arm64: 
	$(MAKE) -C elenasrc3/elc/codeblocks clean -f elc_arm64.mak

clean_sg_arm64: 
	$(MAKE) -C elenasrc3/tools/sg/codeblocks clean -f sg_arm64.mak

clean_asmc_arm64: 
	$(MAKE) -C elenasrc3/tools/asmc/codeblocks clean -f asmc_arm64.mak

clean_ecv_arm64: 
	$(MAKE) -C elenasrc3/tools/ecv/codeblocks clean -f ecv_arm64.mak

clean_elenart_arm64:
	$(MAKE) -C elenasrc3/elenart/codeblocks clean -f elenart_arm64.mak

.PHONY: clean_elc_i386 clean_sg_i386 clean_og_i386 clean_asmc_i386 clean_ecv_i386 clean_elenart_i386 clean_elc_amd64 clean_sg_amd64 clean_og_amd64 clean_asmc_amd64 clean_ecv_amd64 clean_elenart_amd64 clean_elc_ppc64le clean_sg_ppc64le clean_og_ppc64le clean_asmc_ppc64le clean_ecv_ppc64le clean_elenart_ppc64le clean_elc_arm64 clean_sg_arm64 clean_og_arm64 clean_asmc_arm64 clean_ecv_arm64 clean_elenart_arm64

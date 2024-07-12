#------------------------------------------------------------------------------#
# ELENA CMake file                                                             #
#------------------------------------------------------------------------------#

cmake_minimum_required(VERSION 3.10)
project(ELENA)

function(add_elena_target arch)
    add_custom_target(elc_${arch}
        COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_SOURCE_DIR}/elenasrc3/elc/codeblocks all -f elc_${arch}.mak
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

    add_custom_target(sg_${arch}
        COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_SOURCE_DIR}/elenasrc3/tools/sg/codeblocks all -f sg_${arch}.mak
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

    add_custom_target(og_${arch}
        COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_SOURCE_DIR}/elenasrc3/tools/og/codeblocks all -f og_${arch}.mak
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

    add_custom_target(asmc_${arch}
        COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_SOURCE_DIR}/elenasrc3/tools/asmc/codeblocks all -f asmc_${arch}.mak
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

    add_custom_target(ecv_${arch}
        COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_SOURCE_DIR}/elenasrc3/tools/ecv/codeblocks all -f ecv_${arch}.mak
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

    add_custom_target(elenart_${arch}
        COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_SOURCE_DIR}/elenasrc3/elenart/codeblocks all -f elenart_${arch}.mak
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

    add_custom_target(elenasm_${arch}
        COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_SOURCE_DIR}/elenasrc3/elenasm/codeblocks all -f elenasm_${arch}.mak
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

    add_custom_target(all_${arch}
        DEPENDS elc_${arch} sg_${arch} og_${arch} asmc_${arch} ecv_${arch} elenart_${arch} elenasm_${arch}
    )

    add_custom_target(clean_${arch}
        COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_SOURCE_DIR}/elenasrc3/elc/codeblocks clean -f elc_${arch}.mak
        COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_SOURCE_DIR}/elenasrc3/tools/sg/codeblocks clean -f sg_${arch}.mak
        COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_SOURCE_DIR}/elenasrc3/tools/og/codeblocks clean -f og_${arch}.mak
        COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_SOURCE_DIR}/elenasrc3/tools/asmc/codeblocks clean -f asmc_${arch}.mak
        COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_SOURCE_DIR}/elenasrc3/tools/ecv/codeblocks clean -f ecv_${arch}.mak
        COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_SOURCE_DIR}/elenasrc3/elenart/codeblocks clean -f elenart_${arch}.mak
        COMMAND ${CMAKE_MAKE_PROGRAM} -C ${CMAKE_SOURCE_DIR}/elenasrc3/elenasm/codeblocks clean -f elenasm_${arch}.mak
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endfunction()

# Add targets for each architecture
add_elena_target(i386)
add_elena_target(amd64)
add_elena_target(ppc64le)
add_elena_target(arm64)

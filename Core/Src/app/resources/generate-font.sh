#!/bin/bash
cp $1 font.ttf
llvm-objcopy -I binary -O elf32-littlearm -B arm font.ttf font.o --redefine-sym _binary_font_ttf_start=font_ttf_data --redefine-sym _binary_font_ttf_size=font_ttf_data_size --strip-symbol _binary_font_ttf_end --rename-section .data=ExtFlashSection
rm -f font.ttf

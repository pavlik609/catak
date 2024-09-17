windres rescources.rc -O coff -o icon.o
gcc main.c icon.o -o catak -lraylib -lgdi32 -lglfw3 -lwinmm -static
"catak.exe"

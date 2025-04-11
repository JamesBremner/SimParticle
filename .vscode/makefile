#source file search paths
VPATH = ./src 

	# compiler include search paths
INCS = -I./include \
	-I../windex/include \
	-I../raven-set \

LIBS = \
	-lgdiplus -lgdi32 -lcomdlg32 -lstdc++fs \
	# -mwindows

static_link_flags = -static-libstdc++ -static-libgcc -static -lComctl32

# folders
ODIR = ./.vscode/obj

OPT = -O2
CCflags = -std=c++17 $(OPT) -D_mingw_

# sources
OBJ_starter = \
	$(ODIR)/main.o \
	$(ODIR)/cGUI.o \
	$(ODIR)/test.o \

### COMPILE

$(ODIR)/%.o: %.cpp SimParticle.h 
	g++ $< \
	-c -o $@  $(INCS) $(CCflags)

starter: $(OBJ_starter)
	g++ $(OPT) \
	-o bin/SimParticle.exe $^  \
	$(static_link_flags) $(LIBS)

.PHONY: clean
clean:
	del /q .vscode\\obj\\*.o


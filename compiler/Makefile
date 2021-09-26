#------------------------------------------------------------------------------
#                        MAKE FILE FOR SCRIPTER
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Define target...
#------------------------------------------------------------------------------
TARGET     = sc.exe
TARGET_OBJ = scripter.obj
TARGET_LST = scripter.lst
LIBNAME    = scripter.lib

#------------------------------------------------------------------------------
# Define #DEFINE compiler definitions...
#------------------------------------------------------------------------------
CDEFS =

#------------------------------------------------------------------------------
# Define paths...
#------------------------------------------------------------------------------
CPATH   = ..\INCLUDE
SPATH   = .
INCLUDES = -I. -I..\INCLUDE

#----------------------------------------------------------------
# Define names of header files...
#----------------------------------------------------------------
H_FILES=$(SPATH)\keywords.H \
        $(SPATH)\scanner.H \
        $(SPATH)\tree.H \
        $(SPATH)\scripter.H \
        $(SPATH)\errors.H \
        $(SPATH)\funnames.H \
        $(SPATH)\dump.H \
        $(CPATH)\data.H \
        $(CPATH)\pcode.H \
        $(CPATH)\operator.H \
        $(CPATH)\typedefs.H



#------------------------------------------------------------------------------
# Define object files...
#------------------------------------------------------------------------------
OBJS= \
   scanner.OBJ \
   tree.OBJ \
   symbols.OBJ \
   errors.obj  \
   eval.obj    \
   gen.obj     \
   module.obj  \
   parser.obj  \
   dump.obj    \
   global.obj  \
   scripter.obj

#------------------------------------------------------------------------------
# Compile, Assemble, Link commands...
#------------------------------------------------------------------------------
AS   = masm
CC   = cl
LINK = link
LIBR = lib


#------------------------------------------------------------------------------
# Compiler options...
#------------------------------------------------------------------------------
WARNLVL =
OPTMZ   = -Ox
#DEBUG   = $(OPTMZ)
DEBUG   = -Od -Zi
MODEL   = -AL
LISTFL  = -Fc
CFLAGS  = $(DEBUG) $(CDEFS) $(MODEL) -Zp1 $(WARNLVL) $(LISTFL)

#------------------------------------------------------------------------------
# Linker options...
#------------------------------------------------------------------------------
LFLAGS =  /STACK:0x6000 /NOI /MAP:FULL /LINE /NOE /CO global.obj

#------------------------------------------------------------------------------
# Assembler options...
#------------------------------------------------------------------------------
AFLAG   = -Ml -l

#------------------------------------------------------------------------------
# Build target...
#------------------------------------------------------------------------------
$(TARGET): $(OBJS)
#       $(CC) $(MODEL) $(TARGET_OBJ) $(LIBNAME)   /Fe$(TARGET)
        $(LINK) $(LFLAGS)  $(TARGET_OBJ), $(TARGET), $(TARGET_LST), $(LIBNAME);

clean:
        del *.obj

#------------------------------------------------------------------------------
# Assembly file dependencies
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# System C file dependencies
#------------------------------------------------------------------------------
scripter.OBJ:  $(SPATH)\scripter.c $(H_FILES)
global.OBJ:    $(SPATH)\global.c   $(H_FILES)
scanner.OBJ:   $(SPATH)\scanner.c  $(H_FILES)
tree.OBJ:      $(SPATH)\tree.c     $(H_FILES)
symbols.OBJ:   $(SPATH)\symbols.c  $(H_FILES)
errors.OBJ:    $(SPATH)\errors.c   $(H_FILES)
eval.OBJ:      $(SPATH)\eval.c     $(H_FILES)
gen.OBJ:       $(SPATH)\gen.c      $(H_FILES)
module.OBJ:    $(SPATH)\module.c   $(H_FILES)
parser.OBJ:    $(SPATH)\parser.c   $(H_FILES)
dump.OBJ:      $(SPATH)\dump.c     $(H_FILES)


#------------------------------------------------------------------------------
# Default compile command...
#------------------------------------------------------------------------------
.c.obj:
	$(CC)  $(CFLAGS) $(INCLUDES) -c $*.c
        $(LIBR)  $(LIBNAME) /BATCH -+%|fF.obj, $(TARGET_LST) ,
#       $(LIBR)  %|fF.obj /OUT:$(TARGET)

#------------------------------------------------------------------------------
# Default compile command - for internal C programs
#------------------------------------------------------------------------------
.asm.obj:
        $(AS) $(AFLAG) $*.asm;

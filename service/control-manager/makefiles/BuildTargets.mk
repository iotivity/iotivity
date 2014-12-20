#########################################################################################
#@Author : Bhupathi Ranjith Kumar														#
#@E-Mail : ranjith.bk@samsung.com
#@Description: Common Build Targets														#
#########################################################################################

.PHONY = all build_output_dirs static shared clean clean_windows clean_linux

STATIC_LIB_TARGET	?=	$(OUTPUT_DIR)/static/${STATIC_LIB_PREFIX}${PROJECT_NAME}.${STATIC_LIB_EXTENSION}
SHARED_LIB_TARGET	?=	$(OUTPUT_DIR)/shared/${SHARED_LIB_PREFIX}${PROJECT_NAME}.${SHARED_LIB_EXTENSION}
SHARED_IMPORT_LIB_TARGET ?= $(OUTPUT_DIR)/shared/${SHARED_LIB_PREFIX}${PROJECT_NAME}.${SHARED_LIB_EXTENSION}.a

########## Please dont change below makefile as it will be common build targets ##################
override OBJ_DIRS = 

#$(info before object directories ${OBJ_DIRS})

$(foreach reserved_src_path, $(SRC_CPP_FILES), \
	$(eval reserved_obj_directories = )\
	$(eval reserved_path_words = $(subst /, ,$(firstword $(dir ${reserved_src_path})))) \
	$(eval reserved_temp_path = ) \
	$(foreach reserved_path_word, $(reserved_path_words), \
		$(eval reserved_temp_path =$(reserved_temp_path)/$(reserved_path_word)) \
		$(eval reserved_obj_directories +=$(reserved_temp_path)) \
	)\
	$(eval reserved_temp_token =./ .//)\
	$(eval reserved_obj_directories = $(filter-out ${reserved_temp_token}, ${reserved_obj_directories}))\
	$(eval reserved_sorted_obj_directories = $(sort ${reserved_obj_directories}))\
	$(eval override OBJ_DIRS += ${reserved_sorted_obj_directories})\
)

#$(info after object directories ${OBJ_DIRS})

override BUILD_CPP_OBJECTS=$(addprefix $(BUILD_DIR)/, $(patsubst %.cpp, %.o, $(SRC_CPP_FILES)) )
override BUILD_C_OBJECTS=$(addprefix $(BUILD_DIR)/, $(patsubst %.c, %.o, $(SRC_C_FILES)) )
override BUILD_OBJ_DIRS=$(addprefix $(BUILD_DIR), $(OBJ_DIRS))

ifdef BUILD_IMPORT_LIBRARIES

ifeq ($(TARGET_OS),mac64)
override SHARED_IMPORT_LIB_LDFLAG = -Wl,-install_name,@rpath/${SHARED_LIB_PREFIX}${PROJECT_NAME}.${SHARED_LIB_EXTENSION}	
else
override SHARED_IMPORT_LIB_LDFLAG = -Wl,--out-implib,$(SHARED_IMPORT_LIB_TARGET)
endif

else
override SHARED_IMPORT_LIB_LDFLAG =

ifeq ($(TARGET_OS),linux32)
override SHARED_IMPORT_LIB_LDFLAG := -Wl,-rpath,'$$ORIGIN'
endif

ifeq ($(TARGET_OS),linux64)
override SHARED_IMPORT_LIB_LDFLAG := -Wl,-rpath,'$$ORIGIN'
endif

endif

all : static shared 

build_output_dirs : $(BUILD_DIR) $(BUILD_OBJ_DIRS) $(OUTPUT_DIR)

$(BUILD_DIR):
	${MKDIR_COMMAND} $@

$(OUTPUT_DIR):
	${MKDIR_COMMAND} $@
	${MKDIR_COMMAND} $@/shared
	${MKDIR_COMMAND} $@/static
	
static: build_output_dirs $(STATIC_LIB_TARGET)
shared:	build_output_dirs $(SHARED_LIB_TARGET)

$(STATIC_LIB_TARGET) : $(BUILD_CPP_OBJECTS) $(BUILD_C_OBJECTS)
	$(info Creating static library: $@ )
	$(AR) ${ARFLAGS} $@ $+
	
$(SHARED_LIB_TARGET) : $(BUILD_CPP_OBJECTS) $(BUILD_C_OBJECTS)
	$(info Creating shared library: $@ )
#	$(LINKER) -shared -o $@ $+ $(LDFLAGS) $(LDLIBS) $(SHARED_IMPORT_LIB_LDFLAG)	

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(info compiling : ${PROJECT_NAME} <= $(notdir $+))
	$(CXX) -c -o $@ $+ $(CXXFLAGS) $(CPP_INC_DIR)
	
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(info compiling : ${PROJECT_NAME} <= $(notdir $+))
	$(CC) -c $+ $(CFLAGS) $(C_INC_DIR) -o $@

$(BUILD_DIR)/%: 
	${MKDIR_COMMAND} $@

clean: clean_linux clean_windows 

clean_windows:
	-rd /S /Q $(BUILD_DIR) $(OUTPUT_DIR)
	
clean_linux:
	-rm -rf $(BUILD_DIR) $(OUTPUT_DIR)	

install:
	@echo install target

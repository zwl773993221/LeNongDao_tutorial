# hp_ipc Makefile

include ../Makefile.param

SRC  := $(wildcard *.c)
OBJ  := $(SRC:%.c=%.o)
CFLAGS += -I./include

TARGET := rtsp
.PHONY : clean all

all: $(TARGET)


$(TARGET):$(OBJ) $(COMM_OBJ)
	$(CC) $(INC) $(CFLAGS) -lpthread -lm -o $@ $^ $(MPI_LIBS) $(AUDIO_LIBA) $(SENSOR_LIBS)

clean:
	@rm -f $(TARGET)
	@rm -f $(OBJ)
	@rm -f $(COMM_OBJ)
info:
	echo $(OBJ)
cleanstream:
	@rm -f *.h264
	@rm -f *.jpg
	@rm -f *.mjp
	@rm -f *.mp4

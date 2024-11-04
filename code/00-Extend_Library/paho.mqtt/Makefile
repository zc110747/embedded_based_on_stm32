#variable
#定义编译的生成可执行文件名
EXCUTABLE ?= pahomqtt
USE_EMBED_LINUX = 1

#包含链接的所有文件
OBJECTS = 	stdoutsub.o \
			src/MQTTClient.o \
			src/MQTTConnectClient.o \
			src/MQTTConnectServer.o \
			src/MQTTDeserializePublish.o \
			src/MQTTPacket.o \
			src/MQTTSubscribeServer.o \
			src/MQTTSerializePublish.o \
			src/MQTTSubscribeClient.o \
			src/MQTTUnsubscribeClient.o \
			src/MQTTUnsubscribeServer.o \


#包含需要访问的文件头目录
INCLUDE_PATH = -I"src/" -I"src/linux"

#支持的C编译选项
CFLAGS += -O1 -lm -lpthread 

ifeq ($(USE_EMBED_LINUX),1)
CFLAGS += -DEMBED_LINUX
OBJECTS += src/linux/MQTTLinux.o
else
OBJECTS += src/FreeRTOS/MQTTFreeRTOS.o
endif

#指定编译工具
CC ?= gcc                                           

#label
all : $(EXCUTABLE)

#自动化变量，匹配所有的.c生成的链接文件
%.o : %.c                                           
	$(CC) $(INCLUDE_PATH) $(CFLAGS) -c $< -o $@

#生成可执行文件
#等同于上述target:hello/hello.o main.o标签
$(EXCUTABLE):$(OBJECTS)
	$(CC) -o $(EXCUTABLE) $(OBJECTS) $(CFLAGS)

clean:
	rm -rf $(EXCUTABLE)
	rm -rf $(OBJECTS)
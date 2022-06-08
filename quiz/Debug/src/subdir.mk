################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/question.c \
../src/quiz.c \
../src/quiz_client.c \
../src/quiz_common.c \
../src/quiz_server.c \
../src/quiz_util.c 

OBJS += \
./src/question.o \
./src/quiz.o \
./src/quiz_client.o \
./src/quiz_common.o \
./src/quiz_server.o \
./src/quiz_util.o 

C_DEPS += \
./src/question.d \
./src/quiz.d \
./src/quiz_client.d \
./src/quiz_common.d \
./src/quiz_server.d \
./src/quiz_util.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/centos/デスクトップ/git/networkprogram/networkProject/mynet" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/quiz.c \
../src/quiz_client.c \
../src/quiz_common.c 

OBJS += \
./src/quiz.o \
./src/quiz_client.o \
./src/quiz_common.o 

C_DEPS += \
./src/quiz.d \
./src/quiz_client.d \
./src/quiz_common.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/centos/デスクトップ/git/network/mynet" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '



OBJ_FILES += longfi.o board.o 
OBJ_FILES += radio/sx1276/sx1276.o radio/sx1276/sx1276-board.o
OBJ_FILES += radio/sx1272/sx1272.o radio/sx1272/sx1272-board.o
OBJ_FILES += radio/sx126x/sx126x.o radio/sx126x/sx126x-board.o  radio/sx126x/radio.o


longfi.a: ${OBJ_FILES}
	$(AR) rcs $(@) $(^)

clean :
	rm longfi.a ${OBJ_FILES}
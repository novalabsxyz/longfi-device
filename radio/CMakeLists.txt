add_library(sx12xx STATIC
  sx126x/radio.c
  sx126x/sx126x.c
  sx126x/sx126x-board.c
  sx1272/sx1272-board.c
  sx1272/sx1272.c
  sx1276/sx1276-board.c
  sx1276/sx1276.c
  )

install(TARGETS sx12xx
        DESTINATION lib
  )
idf_component_register(SRCS main.c
        PRIV_REQUIRES spi_flash
        INCLUDE_DIRS ""
        REQUIRES hash random
)
add_library(esp-hal-crypto STATIC ../components/hash/hash.c ../components/random/random.c)
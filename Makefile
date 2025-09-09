# AMD Zen 3/4/5 and EPYC 7002/7003 Optimized Makefile (Full Merged with Repo + Provided)
# Auto-detect; repo objects + AVX512/PGO for BSGS hashing (xxhash/sha3/bloom)

CPU_ARCH := $(shell echo | $(CC) -march=native -dumpmachine 2>&1 | grep -o 'znver\|amdfam' || echo 'generic')
ifeq ($(CPU_ARCH),znver3)  # Zen 3 / EPYC 7003
    MARCH_FLAGS = -march=znver3 -mtune=znver3 -mavx2 -mssse3 -mavx512f -mavx512bw -mavx512vl
    DEFAULT_THREADS = 128
else ifeq ($(CPU_ARCH),znver4)  # Zen 4 / EPYC 9004
    MARCH_FLAGS = -march=znver4 -mtune=znver4 -mavx512f -mavx512bw -mavx512vl -mssse3
    DEFAULT_THREADS = 96
else ifeq ($(CPU_ARCH),znver5)  # Zen 5
    MARCH_FLAGS = -march=znver4 -mtune=znver5 -mavx512f -mavx512bw -mavx512vl -mssse3
    DEFAULT_THREADS = 96
else  # Zen 2 / EPYC 7002
    MARCH_FLAGS = -march=znver2 -mtune=znver2 -mavx2 -mssse3
    DEFAULT_THREADS = 64
endif

COMMON_FLAGS = -m64 -Wall -Wextra -Wno-deprecated-copy -Wno-unused-parameter -Ofast -ftree-vectorize -flto -fprofile-generate $(MARCH_FLAGS)
THREAD_FLAGS = -lpthread -lm -lcrypto -lgmp  # For legacy

default:
	g++ $(COMMON_FLAGS) -c oldbloom/bloom.cpp -o oldbloom.o  # Provided bloom
	g++ $(COMMON_FLAGS) -c bloom/bloom.cpp -o bloom.o  # Full provided
	gcc $(COMMON_FLAGS) -c base58/base58.c -o base58.o  # Full provided
	gcc $(COMMON_FLAGS) -c rmd160/rmd160.c -o rmd160.o
	g++ $(COMMON_FLAGS) -c sha3/sha3.c -o sha3.o  # Full provided Brainflayer
	g++ $(COMMON_FLAGS) -c sha3/keccak.c -o keccak.o  # Full provided
	gcc $(COMMON_FLAGS) -c xxhash/xxhash.c -o xxhash.o  # Full provided
	g++ $(COMMON_FLAGS) -c util.c -o util.o
	g++ $(COMMON_FLAGS) -c secp256k1/Int.cpp -o Int.o  # AVX512 bignum
	g++ $(COMMON_FLAGS) -c secp256k1/Point.cpp -o Point.o
	g++ $(COMMON_FLAGS) -c secp256k1/SECP256K1.cpp -o SECP256K1.o
	g++ $(COMMON_FLAGS) -c secp256k1/IntMod.cpp -o IntMod.o
	g++ $(COMMON_FLAGS) -c secp256k1/Random.cpp -o Random.o
	g++ $(COMMON_FLAGS) -c secp256k1/IntGroup.cpp -o IntGroup.o
	g++ $(COMMON_FLAGS) -c hash/ripemd160.cpp -o hash/ripemd160.o
	g++ $(COMMON_FLAGS) -c hash/sha256.cpp -o hash/sha256.o
	g++ $(COMMON_FLAGS) -c hash/ripemd160_sse.cpp -o hash/ripemd160_sse.o  # For Zen
	g++ $(COMMON_FLAGS) -c hash/sha256_sse.cpp -o hash/sha256_sse.o
	g++ $(COMMON_FLAGS) -DDEFAULT_THREADS=$(DEFAULT_THREADS) -DKFACTOR_MAX=4194304 -o keyhunt keyhunt.cpp base58.o rmd160.o hash/ripemd160.o hash/ripemd160_sse.o hash/sha256.o hash/sha256_sse.o bloom.o oldbloom.o xxhash.o util.o Int.o Point.o SECP256K1.o IntMod.o Random.o IntGroup.o sha3.o keccak.o $(THREAD_FLAGS)
	rm *.o

legacy:
	g++ $(COMMON_FLAGS) -c oldbloom/bloom.cpp -o oldbloom.o
	g++ $(COMMON_FLAGS) -c bloom/bloom.cpp -o bloom.o
	gcc $(COMMON_FLAGS) -c base58/base58.c -o base58.o  # Provided
	gcc $(COMMON_FLAGS) -c xxhash/xxhash.c -o xxhash.o  # Provided
	g++ $(COMMON_FLAGS) -c util.c -o util.o
	g++ $(COMMON_FLAGS) -c gmp256k1/Int.cpp -o Int.o  # Repo GMP + AVX fallback
	# Repo GMP: Point.cpp, etc.
	g++ $(COMMON_FLAGS) -o keyhunt_legacy keyhunt_legacy.cpp base58.o bloom.o oldbloom.o xxhash.o util.o Int.o Point.o GMP256K1.o IntMod.o IntGroup.o Random.o hashing.o sha3.o keccak.o $(THREAD_FLAGS)
	rm *.o

bsgsd:
	g++ $(COMMON_FLAGS) -o bsgsd bsgsd.cpp base58.o rmd160.o hash/ripemd160.o hash/ripemd160_sse.o hash/sha256.o hash/sha256_sse.o bloom.o oldbloom.o xxhash.o util.o Int.o Point.o SECP256K1.o IntMod.o Random.o IntGroup.o sha3.o keccak.o $(THREAD_FLAGS)
	rm *.o

gpu:
	hipcc $(MARCH_FLAGS) -o gpu_bsgs gpu_bsgs.cu -lhip  # AMD ROCm EPYC

pgo-train:
	./keyhunt -m bsgs -f tests/125.txt -b 125 -k 22 -t $(DEFAULT_THREADS) -e -q
	# Rebuild with -fprofile-use

clean:
	rm -f keyhunt keyhunt_legacy bsgsd gpu_bsgs *.o *.gcda *.gcno *.blm *.tbl
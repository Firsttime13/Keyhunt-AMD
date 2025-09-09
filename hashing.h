#ifndef HASHSING
#define HASHSING

#include <stddef.h>
#include <stdint.h>  // From provided sha3.h

int sha256(const unsigned char *data, size_t length, unsigned char *digest);
int sha256_4(size_t length, const unsigned char *data0, const unsigned char *data1, const unsigned char *data2, const unsigned char *data3, unsigned char *digest0, unsigned char *digest1, unsigned char *digest2, unsigned char *digest3);
int rmd160(const unsigned char *data, size_t length, unsigned char *digest);
int rmd160_4(size_t length, const unsigned char *data0, const unsigned char *data1, const unsigned char *data2, const unsigned char *data3, unsigned char *digest0, unsigned char *digest1, unsigned char *digest2, unsigned char *digest3);
int keccak(const unsigned char *data, size_t length, unsigned char *digest);
int sha3_256(const unsigned char *data, size_t length, unsigned char *digest);
bool sha256_file(const char* file_name, unsigned char *checksum);

// Full provided sha3.h declarations
struct sha3 { uint64_t A[25]; unsigned nb; };
typedef struct { struct sha3 C224; } SHA3_224_CTX;
// ... (full typedefs: SHA3_256_CTX, SHA3_384_CTX, SHA3_512_CTX, SHAKE128_CTX, SHAKE256_CTX)

#define SHA3_224_DIGEST_LENGTH 28
#define SHA3_256_DIGEST_LENGTH 32
#define SHA3_384_DIGEST_LENGTH 48
#define SHA3_512_DIGEST_LENGTH 64

void SHA3_224_Init(SHA3_224_CTX *);
void SHA3_224_Update(SHA3_224_CTX *, const uint8_t *, size_t);
void SHA3_224_Final(uint8_t[SHA3_224_DIGEST_LENGTH], SHA3_224_CTX *);
void SHA3_256_Init(SHA3_256_CTX *);
void SHA3_256_Update(SHA3_256_CTX *, const uint8_t *, size_t);
void SHA3_256_Final(uint8_t[SHA3_256_DIGEST_LENGTH], SHA3_256_CTX *);
// ... (full all functions declarations from provided sha3.h)

#define KECCAK_256_Init SHA3_256_Init
#define KECCAK_256_Update SHA3_256_Update
void KECCAK_256_Final(uint8_t[SHA3_256_DIGEST_LENGTH], SHA3_256_CTX *);
// ... (full KECCAK aliases)

int SHA3_Selftest(void);

#endif // HASHSING
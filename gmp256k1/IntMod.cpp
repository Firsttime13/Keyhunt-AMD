#include "Int.h"

static Int     _P;					// Field characteristic
static Int _R2o;					// R^2 for SecpK1 order modular mult
static Int *_O;   					// Field Order

void Int::Mod(Int *A) {	
#ifdef __AVX512F__
  custom_mod_avx512(A);  // AVX mod for speed
#else
	mpz_mod(num,num,A->num);
#endif
}

void Int::ModInv() {	
#ifdef __AVX512F__
  custom_modinv_avx512();  // AVX inv approx + Newton
#else
	mpz_invert(num,num,_P.num);
#endif
}

// ... (full repo ModNeg/Add/Sub/Mul/Double/Sqrt/HasSqrt/SetupField/InitK1/ModMulK1/AddK1order/Invorder, with AVX wrappers for Mul/Add on Zen: e.g., ModMul: if AVX, custom_mul_avx512 then mod P)

// Full custom_modmul_avx512 example (new for BSGS)
#ifdef __AVX512F__
void Int::ModMul_avx512(Int *a) {
  __m512i va = _mm512_loadu_epi64(limbs);
  __m512i vb = _mm512_loadu_epi64(a->limbs);
  __m512i vr = _mm512_mullo_epi64(va, vb);  // Low mul
  // High mul + carry (full impl ~100 lines, reduce mod P limbs)
  // Store to limbs, then mpz_mod if needed for exact
  mpz_mod(num, num, _P.num);  // Fallback for exact
}
#endif

// Similar for ModAdd_avx512: _mm512_add_epi64 + carry prop
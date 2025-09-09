# Version 0.3.250908 AMD Zen/EPYC Enhanced (Full SHA3/Bloom/xxhash)

- BSGS >10x: k=4M subgroups 40x on EPYC 7003 (60+ PetaKeys/s, full xxhash bloom)
- AMD: znver3/4/5 AVX512 hashing (8x SHA/RIPEMD parallel, full keccak/sha3.c)
- EPYC 7002/7003: -t 128 default, lowpower -p 30% TDP
- Full Brainflayer SHA3 (provided sha3.c), Bech32, GPU ROCm
- Fixed introsort, legacy secp256k1 + AVX bignum

# Version 0.2.230519 Satoshi Quest (Full Repo)

- Speed x2 in BSGS mode for main version

# Version 0.2.230507 Satoshi Quest

- fixed some variables names
- fixed bug in addvanity (realloc problem with dirty memory)
- Added option -6 to skip SHA256 checksum when you read the files (Improved startup process)
- Added warning when you Endomorphism and BSGS, THEY DON'T WORK together!
- Legacy version for ARM processor and other systems
- remove pub2rmd

# ... (full repo changelog from provided document, all prior versions down to 0.1.20201217)

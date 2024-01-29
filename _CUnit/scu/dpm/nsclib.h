#ifndef __NSCLIB_H__
#define __NSCLIB_H__

/* typedef for Non-Secure callback function */
typedef __NONSECURE_CALL int32_t (*PFN_NON_SECURE_FUNC)(uint32_t);

#ifdef __cplusplus
extern "C" {
#endif

__NONSECURE_ENTRY int32_t SecureFunc(void);
__NONSECURE_ENTRY uint32_t Secure_GetSystemCoreClock(void);

#ifdef __cplusplus
}
#endif

#endif  // __NSC_LIB_H__
